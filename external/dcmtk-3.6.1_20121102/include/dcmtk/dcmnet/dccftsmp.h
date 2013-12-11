/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmTransferSyntaxMap
 *
 */

#ifndef DCCFTSMP_H
#define DCCFTSMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"   /* for class OFList<> */
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/dcmnet/dcmsmap.h"  /* for class DcmSimpleMap<> */
#include "dcmtk/dcmnet/dccfuidh.h" /* for class DcmUIDHandler */


/** this helper class is a simple list of UID handler objects.
 *  Not intended for use by the end user.
 */
typedef OFList<DcmUIDHandler> DcmTransferSyntaxList;

/** this helper class maintains a map of transfer syntax lists.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmTransferSyntaxMap
{
public:
  /// constructor
  DcmTransferSyntaxMap();

  /// destructor
  ~DcmTransferSyntaxMap();

  /** add new entry to list within map.
   *  If key is new, new list is created. Otherwise transfer syntax
   *  is appended to existing list.
   *  @param key map key
   *  @param transferSyntaxUID transfer syntax UID
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition add(
    const char *key,
    const char *transferSyntaxUID);

  /** checks if the key is known
   *  @param key key name, must not be NULL
   *  @return true if key is known, false otherwise
   */
  OFBool isKnownKey(const char *key) const;

  /** returns the list of transfer syntaxes identified by the given key
   *  @param key transfer syntax list to search
   *  @return pointer to transfer syntax list if found, NULL otherwise
   */
  const DcmTransferSyntaxList *getTransferSyntaxList(const char *key) const;

private:
  /// private undefined copy constructor
  DcmTransferSyntaxMap(const DcmTransferSyntaxMap& arg);

  /// private undefined copy assignment operator
  DcmTransferSyntaxMap& operator=(const DcmTransferSyntaxMap& arg);

  /// map of transfer syntax lists
  DcmSimpleMap<DcmTransferSyntaxList *> map_;

};

#endif
