/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MBUtils settings definition
*
*/

#ifndef C_MRMBUTILSSETTINGS_H
#define C_MRMBUTILSSETTINGS_H

#include <e32base.h>
#include <mmrmbutilssettings.h>

class CFSMailClient;
class MMRMailboxUtilsObserver;

/**
 * CMRMBUtilsSettings is class accessing CMAIL specific
 * mailboxutils settins.
 * 
 * Settings include:
 *   - key to S60 MR Enabler framework settings
 *   - CMAIL mailbox plug-in UID
 *   - CMAIL mailbox ID
 *   
 *   @see MMRMBUtilsSettings
 */
NONSHARABLE_CLASS( CMRMBUtilsSettings ) : 
        public CBase,
        public MMRMBUtilsSettings
    {
public: // Constructors and destructors

    /**
    * Symbian two-phased constructor.
    * @param aMailClient Reference to mail client object
    * @return instantiated object
    */
    static CMRMBUtilsSettings* NewL(
            CFSMailClient& aMailClient );        

    /**
    * Destructor.
    */
    ~CMRMBUtilsSettings();

public: // From MMRMBUtilsSettings
    void SetDefaultMailboxSettingL( 
            TInt aId );
    void GetDefaultMailboxSettingsL( TInt& aId );
    
public: // New functions

    /**
    * Gets the default CMAIL information from central repository.
    * @param aDefaultMailbox Link to S60 MR framework setting
    * @param aMailboxPluginUID CMAIL Mailbox plugin ID
    * @param aMailboxId CMAIL mailbox ID  
    */
    TInt GetDefaultCMailMRMailBoxL( 
            TInt& aDefaultBox,
            TUid& aMailboxPluginUID,
            TInt& aMailboxId );

    /**
    * Sets the default CMAIL information into central repository.
    * @param aDefaultMailbox Link to S60 MR framework setting
    * @param aMailboxPluginUID CMAIL Mailbox plugin ID
    * @param aMailboxId CMAIL mailbox ID  
    */       
    TInt SetDefaultCMailMRMailBoxL( 
            TInt aDefaultBox,
            TUid aMailboxPluginUID,
            TInt aMailboxId );
    
private: // Constructors and destructors
    CMRMBUtilsSettings(
            CFSMailClient& aMailClient );
    void ConstructL();
    
private:
    /// Ref: Reference to mail client object
    CFSMailClient& iMailClient;
    };

#endif // C_MRMBUTILSSETTINGS_H

// EOF
