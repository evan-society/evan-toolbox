#include "LicenseKeyUtils.h"

#include "Generator.h"


void Generator::enterMacAddress()
{
    m_macAddress = textEdit->toPlainText();
}

void Generator::generate()
{
    if(m_macAddress == "")
    {
        return;
    }

/* put this block at the top */
    QString day,month,year;
    day.setNum(m_date.day(),16);
    month.setNum(m_date.month(),16);
    year.setNum(m_date.year(),16);
    QString timekey = day + "x" + month + "x" + year;
/* /block */

    QString key;

    QStringList addresses = m_macAddress.split("\n");
    for(int j=0;j<addresses.size();j++)
    {
		if(addresses[j] != "")
		{
		    if(addresses[j].length() != 17)
		    {
		        key.append("Please enter Mac addresses in the form 'xx-xx-xx-xx-xx-xx'\n");
		        continue;
		    }
		    for (int i=2;i<14;i+=3)
		    {
		        if(addresses[j].at(i) != '-')
		        {
			        key.append("Please enter Mac addresses in the form 'xx-xx-xx-xx-xx-xx'\n");
			        break;
		        }
			}
			key.append(GetEncryptedKey(addresses[j] + timekey)); // incorporate the key into the mac address
			key.append("\n");
		}
	}
    textEdit_2->setText(key + timekey);
}
