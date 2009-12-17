/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  common email address object
*
*/



#ifndef CFSMAILADDRESS_H
#define CFSMAILADDRESS_H

#include <e32std.h>
#include <e32base.h>

//<cmail>
#include "cfsmailcommon.h"
//</cmail>

/**
 *  email address handling
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */

class CFSMailAddress : public CBase
{
    public:

    /**
     * Two-phased constructor.
     *
     */
   	 IMPORT_C static CFSMailAddress* NewL();

    /**
     * Two-phased constructor.
     *
     */
  	 IMPORT_C static CFSMailAddress* NewLC();

    /**
     * destructor
     */
  	 IMPORT_C ~CFSMailAddress();
      	
    /**
     * email address accessor
     *
     * @return email address owned by class
     */
  	 IMPORT_C TDesC& GetEmailAddress() const;
        
    /**
     * email address mutator
     *
     * @param aAddress email address to be stored
     */
  	 IMPORT_C void SetEmailAddress(const TDesC& aAddress);
  	 
    /**
     * display name accessor
     *
     * @return stored display name owned by class
     */
  	 IMPORT_C TDesC& GetDisplayName() const;
        
    /**
     * display name mutator
     *
     * @param aDisplayName display name to be stored
     */
  	 IMPORT_C void SetDisplayName(const TDesC& aDisplayName);

    /**
     * constructor
     */
  	 CFSMailAddress();

 private:
 

 	/**
     * Two-phased constructor
     */
  	 void ConstructL();

 private: // data

  	/**
     * email address
     */
 	 HBufC 	*iEmailAddress;

  	/**
     * display name
     */
 	 HBufC 	*iDisplayName;

};

#endif
