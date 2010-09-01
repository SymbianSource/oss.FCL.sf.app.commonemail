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
* Description:  Declaration of interface MFscActionUtils.
 *
*/


#ifndef M_FSCACTIONUTILS_H
#define M_FSCACTIONUTILS_H

#include <e32base.h>
#include <AknsItemID.h> 

// FORWARD DECLARATIONS
class CPbkContactItem;
class CFscContactAction;
class MFscReasonCallback;
class CGulIcon;
class CVPbkContactManager;
class MVPbkStoreContact;

// ENUM DECLARATIONS

/**
 * Enumeration for contact's types
 */
enum TContactNumberType
    {
    ETypeVoiceCallNumber,
    ETypeVideoCallNumber,
    ETypeConfNumber,
    ETypeInternetTel,
    ETypePttNumber,
    ETypeAudioAddress,
    ETypeEmailAddress,
    ETypeMsgAddress,
    ETypeInvalid
    };

/**
 *   Utility method interface for contact actions
 *
 *  @since S60 3.1
 */
class MFscActionUtils
    {

public:
    // Public methods

    /**
     * Reference to virtual phonebook's contact manager
     *
     * @return contact manager
     */
    virtual CVPbkContactManager& ContactManager() = 0;

    /**
     * Shows information note
     *
     * @param aNote Resource ID of text to be shown
     * @param aContactName contact's name to be displayed in note text
     */ 
    virtual void ShowInfoNoteL( TInt aNote, 
            const HBufC* aContactName = NULL ) = 0;

    /**
     * Shows information note
     *
     * @param aNote Resource ID of text to be shown
     * @param aCounter Count of contacts displayed in note
     */
    virtual void ShowInfoNoteL( TInt aNote, 
                TInt aCounter ) = 0;
    
    /**
     * Shows confirmation note
     *
     * @param aNote Resource ID of text to be shown
     * @param aName name to be displayed in note text
     */ 
    virtual void ShowCnfNoteL( TInt aNote, const HBufC* aName = NULL ) = 0;
            
    /**
     * Show number missing dialog
     *
     * @param aMissingCount number missing count
     * @param aTotalCount total contact count
     * @param aNote note to be displaed
     * @param aDialog dialog
     * @return return Ok or Cancel based on user selection
     */
    virtual TInt ShowNumberMissingNoteL( TInt aMissingCount, TInt aTotalCount,
            TInt aNote, TInt aDialog ) = 0;

    /**
     * Get contacts name from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook store.
     * @return HBufC* contact's name or company name
     */
    virtual HBufC* GetContactNameL( MVPbkStoreContact& aStoreContact ) = 0;

    /**
     * Get phone number for voice call from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook store contact.
     * @param aNumber Default or selected phone number from number 
     * selection dialog.
     * @return KErrNone if successfully, KErrNotFound if there is no phone
     * number field, KErrCancel if user cancel number selection.
     */
    virtual TInt GetVoiceCallNumberL( MVPbkStoreContact& aStoreContact,
            TDes& aNumber ) = 0;

    /**
     * Get phone number for video call from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook contact store.
     * @param aNumber Default or selected phone number from number 
     * selection dialog.
     * @return KErrNone if successfully, KErrNotFound if there is no phone
     * number field, KErrCancel if user cancel number selection.
     */
     virtual TInt GetVideoCallNumberL( MVPbkStoreContact& aStoreContact,
             TDes& aNumber ) = 0;
     
    /**
     * Get Voip address from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook contact store.
     * @param aNumber Default or selected voip phone number from number 
     * selection dialog.
     * @return KErrNone if successfully, KErrNotFound if there is no phone
     * number field, KErrCancel if user cancel number selection.
     */
    virtual TInt GetVoipAddressL( MVPbkStoreContact& aStoreContact, 
            TDes& aNumber ) = 0;

    /**
     * Get Conference Number, Id and PIN from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook contact store.
     * @param aConfNum conference number
     * @param aConfId conference ID
     * @param aConfPin conference PIN
     * @return KErrNone if successful, KErrNotFound if there is no conference 
     * number set.
     */
    virtual TInt GetConfInfoL( MVPbkStoreContact& aStoreContact, 
            TDes& aConfNum, TDes& aConfId, TDes& aConfPin ) = 0;

    /**
     * Get PoC (PTT over Cellular) Address from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook contact store.
     * @param aPocAddress will be updated with selected address from address 
     * selection dialog.
     * @return KErrNone if successful, KErrNotFound if there is no address
     * field, KErrCancel if user cancel address selection.
     */
    virtual TInt GetPocAddressL( MVPbkStoreContact& aStoreContact,
            TDes& aPocAddress ) = 0;

    /**
     * Get Message Address from MVPbkStoreContact
     *
     * @param aStoreContact virtual phonebook contact store.
     * @param aMsgAddress will be updated with selected address from address 
     * selection dialog.
     * @return KErrNone if successful, KErrNotFound if there is no address
     * field, KErrCancel if user cancel address selection.
     */
    virtual TInt GetMessageAddressL( MVPbkStoreContact& aStoreContact,
            TDes& aMmsAddress ) = 0;
    
    /**
     * Get Email Address from MVPbkStoreContact
     *
     * @param  aStoreContact virtual phonebook contact store.
     * @param aEmailAddress will be updated with selected address from address
     * selection dialog.
     * @return KErrNone if successful, KErrNotFound if there is no address
     * field, KErrCancel if user cancel address selection.
     */
    virtual TInt GetEmailAddressL( MVPbkStoreContact& aStoreContact, 
            TDes& aMmsAddress ) = 0;
    
    /**
     * Removes Invalid Char from aNumber
     *
     * @param aNumber number to be updated
     * @param aValidChars is valid phone number characters
     * @param aOutNumber returned with valid ph number chars 
     */
    virtual void RemoveInvalidChar( const TDesC& aNumber, 
            const TDesC& aValidChars, TDes& aOutNumber ) = 0;

    /**
     * Finds specified contact number from store contact
     *
     * @param aStoreContact virtual phonebook store contact
     * @param aContactNumberType specifies contact type
     * @return ETrue if specified contact number available
     *   otherwise EFalse
     */                
    virtual TBool IsContactNumberAvailableL( MVPbkStoreContact& aStoreContact,
            TContactNumberType aContactNumberType ) = 0;
        
    /**
     * Finds availability of 3G network
     *
     * @return ETrue if 3G network available, otherwise EFalse
     */  
    virtual TBool Is3GNetworkAvailable() = 0;

    /**
     * Checks if phone is in offline mode
     *
     * @return ETrue if phone is in offline mode
     */
    virtual TBool IsOfflineModeL() = 0;

    /**
     * Finds availability of PTT settings
     *
     * @return ETrue if PTT is configured, otherwise EFalse
     */ 
    virtual TBool IsPttConfiguredL() = 0;

    /**
     * Finds availability of VOIP settings
     *
     * @return ETrue if voip configured, otherwise EFalse
     */ 
    virtual TBool IsVoipConfiguredL() = 0;

    /**
     * Creates icon from skin
     *
     * @param aItemId TAknsItemID
     * @param aFilename icon file's filename
     * @param aFileBitmapId containc bitmap id for from .miffile
     * @param aFileMaskId contains mask id from .mif file
     * @return CGulIcon* if icon is created successfully, 
     *  leaves if there is error in icon creation
     */
    virtual CGulIcon* SkinIconL( TAknsItemID aItemId, const TDesC& aFilename,
            TInt aFileBitmapId, TInt aFileMaskId ) = 0;

    /**
     * Reads action priority from central repositary
     *
     * @param aActionPriorityCrKey central repository key
     * @param aDefaultPriority default priority
     * @return Priority
     */
    virtual TInt ActionPriority( TUint32 aActionPriorityCrKey, 
            TInt aDefaultPriority ) = 0;

    /**
     * creates and returns pointer to CFscContactAction instance
     * Ownership is transferred
     *
     * @param aReasonCallback reference to MFscReasonCallback interface
     * @param aContactActionUid is Uid of action
     * @param aActionType specifies action type
     * @param aActionMenuTextResourceId action menu text
     * @param aIcon specifies icon for the action
     * @return CFscContactAction* pointer to contact action
     */
    virtual CFscContactAction* CreateActionL( 
            MFscReasonCallback& aReasonCallback, TUid aContactActionUid,
            TUint64 aActionType, TInt aActionMenuTextResourceId, 
            CGulIcon* aIcon ) = 0;
    
    /**
     * Closes selection dialog.
     */
    virtual void CloseSelectDialogL() = 0;
    
public:
    
    /**
     * Destructor.
     */
    virtual ~MFscActionUtils() {}
        
    };

#endif // M_FSCACTIONUTILS_H
