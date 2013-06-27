/*
 *
 *  Copyright (C) 1994-2007, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: file cache facility for DcmElement::getPartialValue
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2007-07-11 08:50:23 $
 *  Source File:      $Source: /share/dicom/cvs-depot/dcmtk/dcmdata/include/dcmtk/dcmdata/dcfcache.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCFCACHE_H
#define DCFCACHE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/offile.h"     /* for offile_off_t */
#include "dcmtk/dcmdata/dcistrma.h" /* for class DcmInputStream */

/** This class implements a simple container that stores an input stream,
 *  the position of that input stream corresponding to the start of a DICOM 
 *  element value field contained in the stream and the current "owner" object.
 *  It is used to keep a single file handle open during multiple calls to
 *  DcmElement::getPartialValue(), thus speeding up the reading.
 */
class DcmFileCache
{
public:
	
  /// default constructor
  DcmFileCache()
  : stream_(NULL)
  , offset_(0)
  , user_(NULL)
  {
  }

  /// destructor
  ~DcmFileCache()
  {
    delete stream_;
  }

  /** checks if the given object is the current user of this cache
   *  returns true if so, false otherwise
   *  @param object pointer to "user object"
   */
  OFBool isUser(void *object) const
  {
    return object == user_;
  }

  /// returns object to default constructed state  
  void clear()
  {
    delete stream_;
    stream_ = NULL;
    offset_ = 0;
    user_ = NULL;
  }
  
  /** initializes the file cache with the given stream object
   *  and user.
   *  @param stream stream object
   *  @param user user object
   */
  void init(DcmInputStream *stream, void *user)
  {
    clear();
    stream_ = stream;
    user_ = user;
    if (stream_) offset_ = stream_->tell();
  }
  
  /// return input stream
  DcmInputStream *getStream()
  {
    return stream_;
  }

  /// return initial stream offset  
  offile_off_t getOffset() const { return offset_; }
  
private:

  /// private undefined copy constructor
  DcmFileCache(const DcmFileCache& arg);

  /// private undefined copy assignment operator
  DcmFileCache& operator=(const DcmFileCache& arg);

  /// input stream, may be NULL
  DcmInputStream *stream_;
  
  /// offset within stream for initial position
  offile_off_t offset_;

  /// object that currently uses the file cache, i.e. has created the stream
  const void *user_;  
};

#endif

/*
 * CVS/RCS Log:
 * $Log: dcfcache.h,v $
 * Revision 1.1  2007-07-11 08:50:23  meichel
 * Initial release of new method DcmElement::getPartialValue which gives access
 *   to partial attribute values without loading the complete attribute value
 *   into memory, if kept in file.
 *
 *
 */
