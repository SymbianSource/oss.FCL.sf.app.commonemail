/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common base mailbox object
*
*/


#ifndef __CFSMAILBOXBASE_H
#define __CFSMAILBOXBASE_H

#include "CFSMailFolder.h"
#include "mmrinfoprocessor.h"
#include "cemailextensionbase.h"

/**
 *  base class for handling mailbox data
 *
 *  @lib FSFWCommonLib

 */

class CFSMailBoxBase : public CExtendableEmail
{
 public:
  
    /**
     * Two-phased constructor.
     *
     * @param aMailBoxId mailbox id in plugin containing mailbox
     */
     IMPORT_C static CFSMailBoxBase* NewL(const TFSMailMsgId aMailBoxId);
  	
    /**
     * Two-phased constructor.
     *
     * @param aMailBoxId mailbox id in plugin containing mailbox
     */
     IMPORT_C static CFSMailBoxBase* NewLC(const TFSMailMsgId aMailBoxId);

    /**
     * Destructor.
     */  
     IMPORT_C virtual ~CFSMailBoxBase();
  	
    /**
     * mailbox id accessor
     *
     * @return mailbox id
     */
     IMPORT_C TFSMailMsgId GetId( ) const;

    /**
     * mailbox name accessor
     *
     * @return mailbox name
     */
     IMPORT_C TDesC& GetName( ) const;

    /**
     * mailbox name mutator
     *
     * @param aMailBoxName mailbox name
     */
     IMPORT_C void SetName( const TDesC& aMailBoxName );
	 		
    /**
     * retuns mailbox status accessor
     *
     * @return mailbox status
     */
     IMPORT_C TFSMailBoxStatus GetStatus( ) const;

    /**
     * mailbox status mutator
     *
     * @param aStatus mailbox status
     */
     //<cmail>
     //function definition is empty, we need to remove it if not needed
     //</cmail>
     IMPORT_C void SetStatus( const TFSMailBoxStatus aStatus );

	/*
	 * Function to set context of Remote Contact Lookup service for
	 * this mailbox for protocol plugins.
	 *
	 * @param aProtocolUid specifies Ecom implementation UID of
	 *	      "Remote Contact Lookup Protocol Adaptation API" to use.
	 * @param aMailBoxId mailbox id in plugin
	 */
     IMPORT_C void SetRCLInfo(const TUid aProtocolUid, const TUint aMailBoxId);
 
	/*
     * Function to retrieve Remote Contact Lookup service context for
     * this mailbox.
     *
     * @param aProtocolUid specifies Ecom implementation UID of
     *        "Remote Contact Lookup Protocol Adaptation API" to use.
     * @param aMailBoxId mailbox id in plugin
     */
     IMPORT_C void GetRCLInfo(TUid& aProtocolUid, TUint& aAccountUid);

    /**
     * returns settings view id for mailbox
     *
     * @return settings view id
     */
     IMPORT_C const TUid GetSettingsUid();

    /**
     * sets settings view id for mailbox
     *
     * @param aUid settings view id
     */
     IMPORT_C void SetSettingsUid(const TUid aUid);

	/**
	 * Returns a meeting request info processor, which is used for replying
	 * to meeting request received as MMRInfoObjet
	 *
	 * @return meeting info Processor, NULL if not supported by plugin
	 *         ownership not transferred
	 */ 
     IMPORT_C MMRInfoProcessor& MRInfoProcessorL();

    /**
     * method to cheack if MRInfoProcessor is set
     *
     * @return
     */
	 IMPORT_C TBool IsMRInfoProcessorSet();

	/**
	 * Meeting request info processor mutator
	 *
	 * @param aMrInfoProcessor meeting info Processor, ownership transferred
	 */ 
	 IMPORT_C void SetMRInfoProcessorL(MMRInfoProcessor* aMrInfoProcessor);

    /**
     * mailbox own address accessor
     *
     * @return mailbox own address
     */
     IMPORT_C CFSMailAddress& OwnMailAddress( );

    /**
     * mailbox own address mutator
     *
     * @param aOwnMailAddress mailbox own address
     */
     IMPORT_C void SetOwnMailAddressL( CFSMailAddress* aOwnMailAddress);
	 
protected:

    /**
     * C++ default constructor.
     */
     CFSMailBoxBase();

     /**
      * branding id accessor
      */
      TDesC& BrandingId();

      /**
       * mailbox id mutator
       */
       void SetMailBoxId( TFSMailMsgId aMailBoxId );

protected: // data

protected: // <cmail>

  	/**
     * Two-phased constructor
     */
  	 void ConstructL( const TFSMailMsgId aMailBoxId );
  	 
private: // data
 	
	/**
     * mailbox name
     */
	 HBufC*	              iMailBoxName;

	 
	/**
     * remote contact lookup info
     */
     TUid                 iProtocolUid;
     TUint 	              iAccountUid;
	
	/**
     * settings uid
     */
     TUid                 iSettingsUid;

	/**
     * Meeting request info
     */
     MMRInfoProcessor*    iMRInfoProcessor;

	/**
     * mailbox own address
     */
	 CFSMailAddress*      iOwnMailAddress;

	 /**
	  * branding id
	  */
	 HBufC*                iBrId;

protected: // <cmail>

	 /**
	  * mailbox id
	  */
	 TFSMailMsgId         iMailBoxId;
};

#endif 	// __CFSMAILBOXBASE_H
