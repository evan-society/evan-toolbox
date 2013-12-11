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
 *    class DcmProfileEntry
 *    class DcmProfileMap
 *
 */

#ifndef DCCFPRMP_H
#define DCCFPRMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h" /* for class OFString */
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/dcmnet/dcmsmap.h"  /* for class DcmSimpleMap<> */
#include "dcmtk/dcmnet/dndefine.h"


/** this helper class is a profile list entry.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmProfileEntry
{
public:
  /** constructor
   *  @param presentationContextGroup symbolic identifier of the presentation context list
   *  @param roleSelectionGroup symbolic identifier of the role selection list, may be empty
   *  @param extendedNegotiationGroup symbolic identifier of the extended negotiation list, may be empty
   */
  DcmProfileEntry(
    const OFString& presentationContextGroup,
    const OFString& roleSelectionGroup,
    const OFString& extendedNegotiationGroup);

  /// copy constructor
  DcmProfileEntry(const DcmProfileEntry& arg);
 
  /// destructor
  ~DcmProfileEntry();

  /** returns the presentation context key
   *  @return presentation context key
   */
  const char *getPresentationContextKey() const;

  /** returns the role selection key
   *  @return role selection key, NULL if empty
   */
  const char *getRoleSelectionKey() const;

  /** returns the extended negotiation key
   *  @return extended negotiation key, NULL if empty
   */
  const char *getExtendedNegotiationKey() const;

  /** comparison operator.
   *  @param arg object to compare with
   *  @return true if equal
   */
  OFBool operator==(const DcmProfileEntry& arg) const
  {
    return (presentationContextGroup_ == arg.presentationContextGroup_) 
        && (roleSelectionGroup_ == arg.roleSelectionGroup_)
        && (extendedNegotiationGroup_ == arg.extendedNegotiationGroup_); 
  }

private:

  /// private undefined copy assignment operator
  DcmProfileEntry& operator=(const DcmProfileEntry& arg);

  /// symbolic identifier of the presentation context list
  OFString presentationContextGroup_;

  /// symbolic identifier of the role selection list, may be empty
  OFString roleSelectionGroup_;

  /// symbolic identifier of the extended negotiation list, may be empty
  OFString extendedNegotiationGroup_;
};


/** this helper class maintains a map of association negotiation profile keys.
 *  Not intended for use by the end user.
 */
class DCMTK_DCMNET_EXPORT DcmProfileMap
{
public:
  /// constructor
  DcmProfileMap();

  /// destructor
  ~DcmProfileMap();

  /** add new entry to list within map.
   *  @param key map key, must not exist in map
   *  @param presentationContextKey symbolic identifier of the presentation context list, must not be NULL
   *  @param roleSelectionKey symbolic identifier of the role selection list, may be NULL
   *  @param extendedNegotiationKey symbolic identifier of the extended negotiation list, may be NULL
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition add(
    const char *key,
    const char *presentationContextKey,
    const char *roleSelectionKey,
    const char *extendedNegotiationKey);

  /** checks if the key is known
   *  @param key key name, must not be NULL
   *  @return true if key is known, false otherwise
   */
  OFBool isKnownKey(const char *key) const;

  /** returns the presentation context key for the given profile
   *  @param key key name, must not be NULL
   *  @return presentation context key, NULL if not found
   */
  const char *getPresentationContextKey(const char *key) const;

  /** returns the role selection key for the given profile
   *  @param key key name, must not be NULL
   *  @return role selection key, NULL if not found or empty
   */
  const char *getRoleSelectionKey(const char *key) const;

  /** returns the extended negotiation key for the given profile
   *  @param key key name, must not be NULL
   *  @return extended negotiation key, NULL if not found or empty
   */
  const char *getExtendedNegotiationKey(const char *key) const;

private:
  /// private undefined copy constructor
  DcmProfileMap(const DcmProfileMap& arg);

  /// private undefined copy assignment operator
  DcmProfileMap& operator=(const DcmProfileMap& arg);

  /// map of profiles
  DcmSimpleMap<DcmProfileEntry *> map_;

};


#endif
