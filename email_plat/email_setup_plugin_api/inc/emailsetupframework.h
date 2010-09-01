/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Interface definition for EmailSetupFramework
*     		   Interface is used by the Email Setup Plugin Interface.
*
*/


#ifndef _EMAIL_SETUP_FRAMEWORK__
#define _EMAIL_SETUP_FRAMEWORK__

#include <e32base.h>
#include <etelmm.h>
#include <badesca.h>


//CONSTANTS
//Maximum length of the Service Provider (operator) name
const TInt KMaxSPNameLength = 32;

//FORWARD DECLARATIONS
class CGulIcon;
/**
 * Container class SIM and Device information.
 * Contains country code, network id, service provider name, imsi, for
 * the current SIM card and ImeiId and DeviceId of the current device.
 */
class TSIMAndDeviceData 
{
public:
	TSIMAndDeviceData()
	{
	}
	TSIMAndDeviceData(const TDesC& aName, const TDesC& aCountryCode, const TDesC& aNetworkId):
					iSPName(aName),
					iCountryCode(aCountryCode),
					iNetworkId(aNetworkId)
					{}
									
	
	TBuf<KMaxSPNameLength>                       iSPName;
   	RMobilePhone::TMobilePhoneNetworkCountryCode iCountryCode;
    RMobilePhone::TMobilePhoneNetworkIdentity    iNetworkId;
	RMobilePhone::TMobilePhoneSubscriberId       iImsi;
    TBuf<RMobilePhone::KPhoneSerialNumberSize>   iImeiId;
    TBuf<RMobilePhone::KPhoneModelIdSize>        iPhoneModel;
	
};    

/**
 * Interface class passed to the email setup plugin trough its constructor.
 * APIs of this interface can be used to query some information from the 
 * Email Setup Framework which calles the plugin.
 */
class MEmailSetupFramework
	{
	public:
		/** 
         * Queries the supported services (received from the server) in format of descriptor array.
         * For example: {"email"} if only pop/imap is supported
         * @return - reference to array of descriptors with the supported services, 
         *					      array is empty if there are no supported services
         */
        virtual const CDesC8Array& SupportedServices() = 0;
        
        /** 
         * Queries the AccessPoint uid(IAP) proposed to be set to the e-mail account.
         * @param aAPUid - uid of AccessPoint
         * @return  -  error code: KErrNotFound if no AP is to be offered, KErrNone otherwise
         */
		virtual TInt GetAccessPoint( TUint32& aAPUid ) = 0;
		
		/** 
         * Returnes the SIM information (country code, network id, 
         * service provider name, imsi)
         * and device info (imeiId and device Id).
         * @return  -  a TSIMAndDeviceData reference 
         *             to the object containing the information
         */
        virtual TSIMAndDeviceData& SIMAndDeviceData() = 0;
        
        /** 
         * DEPRECATED - Should not be used, always return KErrNotSupported.
         * 
         */
        virtual TInt CreateAccessPoint( const TDesC& aAccessPointName ) = 0;
        
        /**
         * Shows an advanced POP/IMAP settings query dialog. The initial settings are 
         * provisioned in the aXmlBuf parameter and the settings entered/changed 
         * by the user are returned in  the same aXmlBuf parameter. 
         * The format of the xml is the provider element format of the determineAccount query 
         * (CCDS protocol).
         * 
         * @param aXmlBuf - in/out parameter containning the advanced POP/IMAP settings
         * 
         */
        virtual void QueryPopImapSettingsL( RBuf8& aXmlBuf ) = 0;
        
        /**
         * Returns a branded graphical element.
         *
         * @param aBrandId is the domain.
         *
         * @return A pointer to a CGulIcon object. The caller of this method is 
         *         responsible of destroying the object. Returns NULL if the 
         *         brand doesn't contain the requested branding element.
         */
         virtual CGulIcon* GetIconL( const TDesC& aBrandId ) = 0;

		
	};
	
#endif //_EMAIL_SETUP_FRAMEWORK__
