/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*  Description: This file defines class CIpsSetWizardAdapter.
*
*/


#ifndef IPSSETWIZARDADAPTER_H
#define IPSSETWIZARDADAPTER_H


#include "ipssetdata.h"
#include "ipssetdatactrlcenrep.h"
#include "ipssetutilsconsts.h"
#include "ipssetwizardsettingscenrepkeys.h"

class CRepository;
class CMsvSession;

struct TPortValue
    {
    TUint inport;
    TUint outport;
    };

struct TSecurityValue
    {
    TUint inSecurity;
    TUint outSecurity;
    };

class CIpsSetWizardAdapter : public CBase
    {
    public:  // Constructors and destructor
        
        

        /**
         * Creates object from CIpsSetWizardAdapter and 
         * leaves it to cleanup stack
         * @since FS v1.0
         * @return, Constructed object
         */
        static CIpsSetWizardAdapter* NewLC( 
            CRepository& aRepository, 
            CMsvSession& aSession );

        /**
         * Create object from CIpsSetWizardAdapter
         * @since FS v1.0
         * @return, Constructed object
         */
        static CIpsSetWizardAdapter* NewL( 
            CRepository& aRepository,
            CMsvSession& aSession );

        /**
         * Destructor
         * @since FS v1.0
         */
        virtual ~CIpsSetWizardAdapter();
        
        /**
         * handle commands to create new mailbox from wizard outputs
         * @since FS v1.0
         */
        void HandleWizardDataL();
        
    private:
        
        /**
         * Read email settings from cenrep
         * @since FS v1.0
         */
        void ReadWizardSettignsL();
         
        /**
         * Read email settings from cenrep
         * to member variable
         * @return error code
         * @since FS v2.0
         */
        TInt GetIntFromCenRepL( TIpsSetWizardSettings aSetting, 
                                TInt& aVariable );
        
        
         /**
         * Read email settings from cenrep
         * to member variable
         * @return error code
         * @since FS v2.0
         */
        TInt GetTextFromCenRepL( TIpsSetWizardSettings aSetting, 
                                 TIpsSetUtilsTextPlain& aVariable );
         
        /**
         * Create settings object
         * @param, aSetData. Setting object
         * @since FS v1.0
         */
        void SetSettingsL( CIpsSetData& aSetData );

        /**
         * Sets wizard data fields to KErrNotFound of KNullDesC
         * @since FSEmail 2.0
         */
        void EmptyWizardCenRep();

        /**
         * Create mailbox to centrep
         * @param, aSetData. Setting object
         * @since FS v1.0
         */
        void CreateEmailAccountL( CIpsSetData& aSetData );

        /**
         * Clear CIpsSetWizardAdapter data fields
         * @since FS v1.0
         */
        void ClearFields();

         /**
         * Check if given mailbox name already exists and 
         * append number behind mailbox.
         * @param, aName. Name of mailbox
         * @return, Mailbox name. Appends number if necessary
         * @since FS v1.0
         */
        TDes& CheckMailboxNamesL( TDes& aName );

        /**
         * Searches access point by name and returns corresponding structure 
         * @param, aIAPName. Access point name
         * @return, iap structure. Set to "always ask" if not found.
         * @since FS v1.0
         */
        TImIAPChoice SetIAPByNameL( TDes& aIAPName );
        
        /**
         * Sets port values to defult if not given by wizard.
         * @param, aProtocol. Protocol setting
         * @param, aInport. POP/IMAP -port setting from cenrep
         * @param, aOutport. SMTP -port setting from cenrep
         * @param, aSecurity. TSecurityValue structure 
         * @return, port value structure.
         * @since FS v1.0
         */
        TPortValue CheckPortValues( 
                const TInt aProtocol, 
                const TInt aInport, 
                const TInt aOutport,
                const TSecurityValue aSecurity );

        /**
         * Function for setting security values.
         * @param, aInSecurity. Security setting from cenrep
         * @param, aOutSecurity. Security setting from cenrep
         * @return, port value structure.
         * @since FS 2.0
         */
        TSecurityValue SetSecurityValues( 
                const TInt aInSecurity, 
                const TInt aOutSecurity );

        /**
         * 
         * @since FS 2.0
         */
        void ChangeAsciiPrintable(
                const TDesC16& aOrgText, 
                TDes& aValText );
        
        /**
         * 
         * @since FS 2.0
         */
        void ChangeUnicodePrintable(
                const TDesC16& aOrgText, 
                TDes& aValText );


    protected:  // Constructors

        /**
         * Default constructor for CIpsSetWizardAdapter
         * @since FS v1.0
         * @return, Constructed object
         */
        CIpsSetWizardAdapter( 
            CRepository& aRepository,
            CMsvSession& aSession );

        /**
         * Symbian 2-phase constructor
         * @since FS v1.0
         */
        void ConstructL();

    private: // Data

        // amount of accounts configured 
        TInt iAccountNumber;
        // maximum accounts reached 1 = yes, 0 = no
        TInt iMaxAccounts;
        // POP/IMAP data
        TIpsSetUtilsTextPlain iEmailAddress;
        TIpsSetUtilsTextPlain iUsername;
        TIpsSetUtilsTextPlain iPassword;
        // Incoming Server
        TIpsSetUtilsTextPlain iIncomingServer;
        // protocol 0= POP, 1= IMAP
        TInt iProtocol;
        // security auth 0 = no, 1 = yes
        TInt iInSecurityAuth;
        // Security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
        TInt iInSecurityProtocol;
        // Incoming port number, optional if 0/empty/missing key -> use default ports
        TInt iInIncomingPort;
        // Email-Providers Outgoing server
        TIpsSetUtilsTextPlain iProviderOutgoingServer;
        // Email-Providers security auth 0 = no, 1 = yes
        TInt iProviderOutSecurityAuth;
        // Email-Providers Security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
        TInt iProviderOutSecurityProtocol;
        // Email-Providers outgoing port number, 
        // optional if 0/empty/missing key -> use default ports
        TInt iProviderOutgoingPort;
        // Operators Outgoing Server
        TIpsSetUtilsTextPlain iOperatorOutgoingServer;
        // Operators security auth 0 = no, 1 = yes
        TInt iOperatorOutSecurityAuth;
        // Operators Security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
        TInt iOperatorOutSecurityProtocol;
        // Operators outgoing port number
        TInt iOperatorOutgoingPort;
        
        //<cmail>
        // ETrue if there are some field that should be hidden from ui
        TBool iHiddenData;
        //</cmail>
        
        // Recomended accesspoint to use
        TIpsSetUtilsTextPlain iRecomendedAP;
        // Mailbox name
        TIpsSetUtilsTextPlain iMailboxName;
        
        // Central Repository Session   
        CRepository& iWizardCenRep;
        // Cenrep controller
        CIpsSetDataCtrlCenrep* iCenRep;
        //
        CMsvSession& iSession;

    };


#endif //IPSSETWIZARDADAPTER_H

//End of file
