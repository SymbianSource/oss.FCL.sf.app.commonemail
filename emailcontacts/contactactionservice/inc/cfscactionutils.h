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
* Description:  Declaration of CFscActionUtils.
 *
*/


#ifndef C_FSCACTIONUTILS_H
#define C_FSCACTIONUTILS_H

#include <e32base.h>
//<cmail>
#include "mfscactionutils.h"
//</cmail>
#include <RPbkViewResourceFile.h>

// FORWARD DECLARATIONS
class CPbkContactItem;
class CRepository;
class CRCSEProfileRegistry;
class CFscContactAction;
class MFscReasonCallback;
class CVPbkContactManager;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CFscAddressSelect;

/**
 *  Action utils.
 *  Implementation of a action utils.
 *
 *  @since S60 3.1
 */
class CFscActionUtils : public CBase, public MFscActionUtils
    {

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aContactManager Contact Manager
     * @return New instance of CFscActionUtils
     */
    static CFscActionUtils* NewL( CVPbkContactManager& aContactManager );

    /**
     * Two-phased constructor.
     *
     * @param aContactManager Contact Manager
     * @return New instance of CFscActionUtils
     */
    static CFscActionUtils* NewLC( CVPbkContactManager& aContactManager );

    /**
     * Destructor.
     */
    virtual ~CFscActionUtils();

public:
    // From base class MFscActionUtils

    /**
     * @see MFscActionUtils::ContactEngine
     */
    CVPbkContactManager& ContactManager();

    /**
     * @see MFscActionUtils::ShowInfoNoteL
     */
    void ShowInfoNoteL( TInt aNote, const HBufC* aContactName );

    /**
     * @see MFscActionUtils::ShowInfoNoteL
     */
    void ShowInfoNoteL( TInt aNote, TInt aCounter );
    
    /**
     * @see MFscActionUtils::ShowCnfNoteL
     */
    void ShowCnfNoteL( TInt aNote, const HBufC* aName );
        
    /**
     * @see MFscActionUtils::ShowNumberMissingDialogL
     */
    TInt ShowNumberMissingNoteL( TInt aMissingCount, TInt aTotalCount,
            TInt aNote, TInt aDialog );

    /**
     * @see MFscActionUtils::GetContactNameL
     */
    HBufC* GetContactNameL( MVPbkStoreContact& aStoreContact );

    /**
     * @see MFscActionUtils::GetVoiceCallNumberL
     */
    TInt GetVoiceCallNumberL( MVPbkStoreContact& aStoreContact, 
            TDes& aNumber );

    /**
     * @see MFscActionUtils::GetVideoCallNumberL
     */
    TInt GetVideoCallNumberL( MVPbkStoreContact& aStoreContact, 
            TDes& aNumber );

    /**
     * @see MFscActionUtils::GetVoipAddressL
     */
    TInt GetVoipAddressL( MVPbkStoreContact& aStoreContact, TDes& aNumber );

    /**
     * @see MFscActionUtils::GetConfInfoL
     */
    TInt GetConfInfoL( MVPbkStoreContact& aStoreContact, TDes& aConfNum,
            TDes& aConfId, TDes& aConfPin );

    /**
     * @see MFscActionUtils::GetPocAddressL
     */
    TInt GetPocAddressL( MVPbkStoreContact& aStoreContact, 
            TDes& aPocAddress );

    /**
     * @see MFscActionUtils::GetMessageAddressL
     */
    TInt GetMessageAddressL( MVPbkStoreContact& aStoreContact,
            TDes& aMsgAddress );
    
    /**
     * @see MFscActionUtils::GetEmailAddressL
     */
    TInt GetEmailAddressL( MVPbkStoreContact& aStoreContact,
            TDes& aEmailAddress );

    /**
     * @see MFscActionUtils::RemoveInvalidChar
     */
    void RemoveInvalidChar( const TDesC& aNumber, const TDesC& aValidChars,
            TDes& aOutNumber );

    /**
     * @see MFscActionUtils::IsContactNumberAvailableL
     */
    TBool IsContactNumberAvailableL( MVPbkStoreContact& aStoreContact,
            TContactNumberType aContactNumberType );

    /**
     * @see MFscActionUtils::Is3GNetworkAvailable
     */
    TBool Is3GNetworkAvailable();

    /**
     * @see MFscActionUtils::IsOfflineModeL
     */
    TBool IsOfflineModeL();

    /**
     * @see MFscActionUtils::IsPttConfiguredL
     */
    TBool IsPttConfiguredL();

    /**
     * @see MFscActionUtils::IsVoipConfiguredL
     */
    TBool IsVoipConfiguredL();

    /**
     * @see MFscActionUtils::SkinIconL
     */
    CGulIcon* SkinIconL( TAknsItemID aItemId, const TDesC& aFilename,
            TInt aFileBitmapId, TInt aFileMaskId );

    /**
     * @see MFscActionUtils::ActionPriorityL
     */
    TInt ActionPriority( TUint32 aActionPriorityCrKey, 
            TInt aDefaultPriority );

    /**
     * @see MFscActionUtils::CreateActionL
     */
    CFscContactAction* CreateActionL( MFscReasonCallback& aReasonCallback,
            TUid aContactActionUid, TUint64 aActionType,
            TInt aActionMenuTextResourceId, CGulIcon* aIcon );
    
    /**
     * Closes selection dialog.
     */
    void CloseSelectDialogL();
    
protected:
    
    /**
     * Opens selection dialog.
     * 
     * @param aResourceId resource identifying type of select dialog.
     * @param aStoreContact contact for which dialog is opened.
     * 
     * @return selected contact field or NULL if canceled. 
     */
    MVPbkStoreContactField* OpenSelectDialogL( TInt aResourceId, 
            MVPbkStoreContact& aStoreContact );    

private:
    // Private constructors

    /**
     * Constructor.
     *
     * @param aContactManager Contact Manager
     */
    CFscActionUtils( CVPbkContactManager& aContactManager );

    /**
     * Second phase constructor.
     */
    void ConstructL();
    
private:
    // Private methods

    /**
     * Returns full file path with drive.
     * 
     * @param aFilePath path to file with filename
     */
    TPtrC GetFileDirL( const TDesC& aFilePath );
    
    // Fix for EMZG-7M23KX
    /**
     * Find and add resource file to the list maintained by CCoeEnv.
     * Resource file must be deleted (remove from the list) after usage.
     * @return The offset value defined for this resource file
     */
    TInt FindAndAddResourceFileL();

private:
    // Private members

    /**
     * Contact Manager.
     */
    CVPbkContactManager& iContactManager;

    /**
     * Priority cenrep
     * Own.
     */
    CRepository* iRep;

    /**
     * CRCSEProfileRegistry for checking voip status
     */
    CRCSEProfileRegistry* iRCSEProfileRegistry;
    
    /**
     * Letter drive.
     */
    TFileName iLetterDrive;
    
    /**
     * Phone number/address selection dialog.
     * Own.
     */
    CFscAddressSelect* iAddressSelect;
    };

#endif // C_FSCACTIONUTILS_H
