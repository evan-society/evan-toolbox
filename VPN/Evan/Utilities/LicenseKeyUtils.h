#ifndef LICENCEKEYUTILS_H
#define LICENCEKEYUTILS_H

#include "sha1.cpp"
#include <QString>

#ifdef USE_UNIX_LOCATIONS
extern QString licensepath;
#endif


#ifdef WIN32

#include <windows.h>
#include <iphlpapi.h>
#include <assert.h>

// Fetches all MAC addresses as a QStringList
QStringList GetMACaddresses()
{
    QString qmac;
    QStringList qmaclist;

    IP_ADAPTER_INFO AdapterInfo[16];			// Allocate information for up to 16 NICs
    DWORD dwBufLen = sizeof(AdapterInfo);		// Save the memory size of buffer

    DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo
                         AdapterInfo,							// [out] buffer to receive data
                         &dwBufLen);								// [in] size of receive data buffer
    assert(dwStatus == ERROR_SUCCESS);			// Verify return value is valid, no buffer overflow

    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info

    do
    {
        char mac[100];
        sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X",
                pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
        qmac = mac;
        pAdapterInfo = pAdapterInfo->Next;		// Progress through linked list
        if (qmac != "00-00-00-00-00-00")
            qmaclist.push_back(qmac);
    }
    while (pAdapterInfo);						// Terminate if last adapter

    return qmaclist;
}
#elif defined(__APPLE__)


#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOEthernetController.h>
#include <cstdio>

static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
{
    kern_return_t kernResult;
    CFMutableDictionaryRef matchingDict;
    CFMutableDictionaryRef propertyMatchDict;
    matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);

    if (NULL != matchingDict)
    {
        propertyMatchDict =
            CFDictionaryCreateMutable(kCFAllocatorDefault, 0,

                                      &kCFTypeDictionaryKeyCallBacks,

                                      &kCFTypeDictionaryValueCallBacks);

        if (NULL != propertyMatchDict)

        {

            CFDictionarySetValue(propertyMatchDict,

                                 CFSTR(kIOPrimaryInterface), kCFBooleanTrue);

            CFDictionarySetValue(matchingDict,

                                 CFSTR(kIOPropertyMatchKey), propertyMatchDict);

            CFRelease(propertyMatchDict);

        }

    }

    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault,

                 matchingDict, matchingServices);

    return kernResult;

}


static kern_return_t GetMACAddress(io_iterator_t intfIterator,

                                   UInt8 *MACAddress, UInt8 bufferSize)

{

    io_object_t intfService;

    io_object_t controllerService;

    kern_return_t kernResult = KERN_FAILURE;

    if (bufferSize < kIOEthernetAddressSize)
    {

        return kernResult;

    }

    bzero(MACAddress, bufferSize);

    while (intfService = IOIteratorNext(intfIterator))

    {

        CFTypeRef MACAddressAsCFData;

        // IONetworkControllers can't be found directly by the IOServiceGetMatchingServices call,

        // since they are hardware nubs and do not participate in driver matching. In other words,

        // registerService() is never called on them. So we've found the IONetworkInterface and will

        // get its parent controller by asking for it specifically.

        // IORegistryEntryGetParentEntry retains the returned object,

// so release it when we're done with it.

        kernResult =

            IORegistryEntryGetParentEntry(intfService,

                                          kIOServicePlane,

                                          &controllerService);

        if (KERN_SUCCESS != kernResult)
        {

            printf("IORegistryEntryGetParentEntry returned 0x%08x\n", kernResult);

        }

        else
        {

            // Retrieve the MAC address property from the I/O Registry in the form of a CFData

            MACAddressAsCFData =

                IORegistryEntryCreateCFProperty(controllerService,

                                                CFSTR(kIOMACAddress),

                                                kCFAllocatorDefault,

                                                0);

            if (MACAddressAsCFData)
            {

                CFShow(MACAddressAsCFData); // for display purposes only; output goes to stderr

                // Get the raw bytes of the MAC address from the CFData

                CFDataGetBytes((CFDataRef)MACAddressAsCFData,

                               CFRangeMake(0, kIOEthernetAddressSize), MACAddress);

                CFRelease(MACAddressAsCFData);

            }

            // Done with the parent Ethernet controller object so we release it.

            (void) IOObjectRelease(controllerService);

        }

        // Done with the Ethernet interface object so we release it.

        (void) IOObjectRelease(intfService);

    }

    return kernResult;

}


long GetMACAddressMAC(unsigned char * result)

{

    io_iterator_t intfIterator;

    kern_return_t kernResult = KERN_FAILURE;

    do

    {

        kernResult = ::FindEthernetInterfaces(&intfIterator);

        if (KERN_SUCCESS != kernResult) break;

        kernResult = ::GetMACAddress(intfIterator, (UInt8*)result, 6);

    }

    while (false);

    (void) IOObjectRelease(intfIterator);

}


QStringList GetMACaddresses()

{
	QString qmac;
    QStringList qmaclist;


    unsigned char result[6];

    if (GetMACAddressMAC(result) == 0)

    {

        char mac[100];

        sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X",

                (unsigned int)result[0],

                (unsigned int)result[1],

                (unsigned int)result[2],

                (unsigned int)result[3],

                (unsigned int)result[4],

                (unsigned int)result[5]

               );

        qmac = mac;
		qmaclist.push_back(qmac);
        return qmaclist;
    }

	qmac = "00-00-00-00-00-00";
	qmaclist.push_back(qmac);
    return qmaclist;

}

#else

#include <cstdio>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>

bool my_mac_addr_sys(u_char *addr)
{
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i;
    bool ok = false;
    bool result = false;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s==-1)
    {
        return result;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++)
    {
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0)
        {
            if (! (ifr.ifr_flags & IFF_LOOPBACK))
            {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
                {
                    ok = true;
                    break;
                }
            }
        }
    }

    close(s);
    if (ok)
    {
        bcopy( ifr.ifr_hwaddr.sa_data, addr, 6);
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}

QStringList GetMACaddresses()
{
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i;
    u_char addr[6];
    QStringList qmaclist;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s==-1)
    {
        return qmaclist;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++)
    {
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0)
        {
            if (! (ifr.ifr_flags & IFF_LOOPBACK))
            {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
                {
                    bcopy( ifr.ifr_hwaddr.sa_data, addr, 6);
                    char mac[100];
	            sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X",
	            addr[0],
	            addr[1],
	            addr[2],
	            addr[3],
	            addr[4],
	            addr[5]);
                    QString qmac = mac;
                    qmaclist.push_back(qmac);
                }
            }
        }
    }

    close(s);
    return qmaclist;
}

#endif // WIN32

QString GetEncryptedKey(QString macAddress)
{
    std::string secret = macAddress.toStdString();
    SHA1* sha1 = new SHA1();
    sha1->addBytes( secret.c_str(), strlen( secret.c_str() ) );
    unsigned char* digest = sha1->getDigest();
    unsigned char* digest_ptr = digest;

    QString licenseKey = "";
    for (int i=0;i<20;i++)
    {
        char ch[32];
        sprintf(ch, "%02x", *digest );
        licenseKey.append(ch);
        digest++;
    }

    delete sha1;
    free( digest_ptr );
    return licenseKey;
}

QStringList GetEncryptedKeyList(QStringList macAddresses)
{
    QStringList licenseKeyList;
    for (int i=0;i<macAddresses.size();i++)
    {
        licenseKeyList.push_back(GetEncryptedKey(macAddresses[i]));
    }
    return licenseKeyList;
}

// returns 1 if license is valid; returns 0 if license is invalid
int parseLicense( const QStringList &keysIn, QStringList &validKeys, QString &timekey, bool &isTrialLicense )
{
        QStringList keys;
        keys.reserve( keysIn.size() );

        //if(keys.size()>0)
        if ( keysIn.size() > 0 )
        {
            isTrialLicense = false;

            // special handling
            for ( int i = 0; i < keysIn.size(); i++ ) {
                keys.push_back( keysIn[i].trimmed() );
            }
            if ( keys.last().isEmpty() ) {
                keys.pop_back();
            }

            // special handling of trial licenses
            // NOTE: trial licenses worked OOTB in 1.63!
            // however, now it seems that we need this additional code:
            if ( keys.last() == QString( "trial" ) ) {
                isTrialLicense = true;
                keys.pop_back();
            }

            QStringList expDateStr = keys.last().split("x");
            if(expDateStr.size() == 3)
            {
                bool ok;
                int year = expDateStr[2].toInt(&ok,16);
                int month = expDateStr[1].toInt(&ok,16);
                int day = expDateStr[0].toInt(&ok,16);

                QDate date(year,month,day);
                if(date > date.currentDate())
                {
                    timekey = keys.last();
                    QString daysLeft;
                    daysLeft.setNum(date.currentDate().daysTo(date));

                    QStringList qmaclist = GetMACaddresses();
                    if ( isTrialLicense ) {
                        //qmaclist.clear();
                        if ( date.currentDate().daysTo(date) < 62 ) { // approximately 2 months max period for trial license
                            qmaclist.push_back( "99-99-99-99-99-99" );
                        } else {
                            QString warnMsg( "Trial license found, but a trial license is limited to a maximum of approximately 2 months, whereas this trial license is valid for " );
                            warnMsg += daysLeft;
                            warnMsg += " days.";
                            Logger::getInstance()->log( warnMsg, Logger::WARNING);
                        }
                    }
                    for(int i=0;i<qmaclist.size();i++)
                    {
                        qmaclist[i] += timekey;
                    }

                    bool isValid = false;
                    QStringList encryptedList = GetEncryptedKeyList(qmaclist);
                    for(int i=0;i<keys.size();i++)
                    {
                        if(encryptedList.contains(keys[i]))
                        {
                            isValid = true;
                            validKeys.push_back(keys[i]);
                            //return 1; // valid license found
                        }
                    }
                    if ( isValid ) {

                        if ( isTrialLicense ) {
                            Logger::getInstance()->log( "Trial License Key - " +
                                                        daysLeft +
                                                        " day" + ( ( date.currentDate().daysTo(date) > 1 ) ? "s" : "" ) +
                                                        " remaining.",
                                                        Logger::INFO );
                        } else {
                            Logger::getInstance()->log( "Valid License Key - " +
                                                        daysLeft +
                                                        " day" + ( ( date.currentDate().daysTo(date) > 1 ) ? "s" : "" ) +
                                                        " remaining.",
                                                        Logger::INFO );
                        }

                        return 1; // valid license found
                    }
                    Logger::getInstance()->log("No valid license key found in file: license.dat. Go to Help->Register to register your copy.", Logger::WARNING);
                }
                else
                {
                    Logger::getInstance()->log("Trial period expired. This software has now limited functionality. Go to Help->Register to register your copy.", Logger::WARNING);
                }
            }
            else
            {
                Logger::getInstance()->log("License file not in right format. Go to Help->Register to register your copy.", Logger::WARNING);
            }
        }
        else
        {
            Logger::getInstance()->log("License file contains no data. Go to Help->Register to register your copy.", Logger::WARNING);
        }

    return 0; // no valid license found
}

int checkForLicense()
{
    QDir qd;

    // check for license
#ifdef USE_UNIX_LOCATIONS
    if(qd.exists(licensepath))
#else
    if(qd.exists("license.dat"))
#endif
    {
        Logger::getInstance()->log( QString( "Opening " ) + qd.filePath( "license.dat" ) );

        QStringList keys;
        string str;
        ifstream infile;
#ifdef USE_UNIX_LOCATIONS
        infile.open((licensepath).toUtf8().data());
#else
        infile.open( "license.dat" );
#endif

        while( infile >> str )
        {
            if(str!="")
            {
                keys.push_back(str.c_str());
            }
        }

        infile.close();

        bool isTrialLicense;
        QStringList validKeys;
        QString timekey;
        int parseResult = parseLicense( keys, validKeys, timekey, isTrialLicense );

        return parseResult;
    }
    else
    {
        Logger::getInstance()->log("No license file found. Go to Help->Register to register your copy.", Logger::WARNING);
    }

    return 0;
}


#endif // LICENCEKEYUTILS_H
