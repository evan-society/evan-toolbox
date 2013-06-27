/*
 *
 *  Copyright (C) 2001-2007, OFFIS
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
 *  Author:  Michael Onken
 *
 *  Purpose: Class for conversion of image file into new DICOM SC Image IODs
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2009-01-16 09:51:55 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2DPLNSC_H
#define I2DPLNSC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

class I2DOutputPlugNewSC : public I2DOutputPlug
{

public:

  /** Constructor, initializes member variables with standard values
   *  @return none
   */
  I2DOutputPlugNewSC();

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident();

  /** Overwrites function from base class. Returns the Storage SOP class
   *  written by this plugin
   *  @param suppSOPs - [out] List of UIDs representing the supported SOP 
   *                    classes supported by this plugin.
   *  @return none
   */
  virtual void supportedSOPClassUIDs(OFList<OFString> suppSOPs);

  /** Outputs SOP class specific information into dataset
   * @param dataset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string otherwise
   */
  virtual OFString isValid(DcmDataset& dataset) const;

  /** Virtual Destructor, clean up memory
   *  @return none
   */
  virtual ~I2DOutputPlugNewSC();

protected:

  /** Inserts attributes for Multi-frame Module
   *  @param targetDataset - [in/out] The dataset to write to
   *  @return EC_Normal if insertion was successfull, error code otherwise
   */
  virtual OFCondition insertMultiFrameAttribs(DcmDataset* targetDataset) const;

  /** Inserts attributes Rescale Slope/Intercept/Type, which have to be
   *  written (1C) if color model is MONOCHROME2 and BitsStored > 1.
   *  @param targetDataset - [out] The dataset to write to
   *  @return EC_Normal if insertion was successfull, error code otherwise
   */
  virtual OFCondition insertSCMultiFrameAttribs(DcmDataset *targetDataset) const;

  /** Checks whether Image Pixel module attributes conform to the
   *  specification of a a 1 bit Secondary Capture object.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal, if everything is fine, error code otherwise
   */
  virtual OFCondition handle1BitSC(DcmDataset *dataset) const;

  /** Checks whether Image Pixel module attributes conform to the
   *  specification of a a 8 bit Secondary Capture object.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal, if everything is fine, error code otherwise
   */
  virtual OFCondition handle8BitSC(DcmDataset *dataset) const;

  /** Checks whether Image Pixel module attributes conform to the
   *  specification of a a 16 bit Secondary Capture object.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal, if everything is fine, error code otherwise
   */
  virtual OFCondition handle16BitSC(DcmDataset *dataset) const;


};

#endif // I2DPLNSC_H

/*
 * CVS/RCS Log:
 * $Log: i2dplnsc.h,v $
 * Revision 1.2  2009-01-16 09:51:55  onken
 * Completed doxygen documentation for libi2d.
 *
 * Revision 1.1  2008-01-16 15:11:44  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.1  2008-01-11 14:17:53  onken
 * Added various options to i2dlib. Changed logging to use a configurable
 * logstream. Added output plugin for the new Multiframe Secondary Capture SOP
 * Classes. Added mode for JPEG plugin to copy exsiting APPn markers (except
 * JFIF). Changed img2dcm default behaviour to invent type1/type2 attributes (no
 * need for templates any more). Added some bug fixes.
 *
 */
