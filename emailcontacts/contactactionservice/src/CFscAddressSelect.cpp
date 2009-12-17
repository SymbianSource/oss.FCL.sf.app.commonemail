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
* Description:  Implementation of the class CFscAddressSelect.
 *
*/


#include "emailtrace.h"
#include "CFscAddressSelect.h"

// Phonebook 2
#include "CFscSelectFieldDlg.h"
#include "MFscControlKeyObserver.h"
#include <MPbk2ContactNameFormatter.h>
#include "CFscFieldPropertyArray.h"
#include "TFscAddressSelectParams.h"
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkFieldFilter.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <CVPbkContactManager.h>
#include <CVPbkDefaultAttribute.h>

// System includes
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <StringLoader.h>

#include <CVPbkContactManager.h>

/// Unnamed namespace for local definitions
namespace
    {

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPostCond_Constructor = 1,
        EPanicPreCond_ExecuteLD
        };

    static void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CPbk2AddressSelect");
        User::Panic(KPanicText, aReason);
        }
#endif // _DEBUG
    const TInt KFirstField = 0;
    const TInt KDefaultTitleFormat = MPbk2ContactNameFormatter::EUseSeparator;

    /**
     * Returns index of given field in store contact field collection.
     *
     * @param aCollection   Store contact field collection.
     * @param aField        Store contact field to search for.
     * @return  Index of the given field.
     */
    inline TInt IndexOfField(
            const MVPbkStoreContactFieldCollection& aCollection,
            const MVPbkStoreContactField& aField)
        {
        TInt ret = KErrNotFound;
        const TInt count = aCollection.FieldCount();

        for (TInt i = 0; i < count; ++i)
            {
            MVPbkStoreContactField* field = aCollection.FieldAtLC(i);
            if (aField.IsSame( *field) )
                {
                ret = i;
                CleanupStack::PopAndDestroy(); // field
                break;
                }
            CleanupStack::PopAndDestroy(); // field
            }

        return ret;
        }

    } /// namespace

// MODULE DATA STRUCTURES

/**
 * Special field selection dialog class for CFscAddressSelect.
 * The main purpose of this class is to
 * get #include of MFscControlKeyObserver
 * away from public header cfscaddressselect.h.
 */
NONSHARABLE_CLASS(CFscAddressSelect::CSelectFieldDlg) :
public CFscSelectFieldDlg,
private MFscControlKeyObserver
    {
public: // Construction

    /**
     * Constructor.
     *
     * @param aParent   Parent.
     */
    CSelectFieldDlg( CFscAddressSelect& aParent ) :
    iParent( aParent )
        {
        SetObserver( this );
        }

private: // From MFscControlKeyObserver
    TKeyResponse FscControlKeyEventL
    ( const TKeyEvent& aKeyEvent, TEventCode aType );

private: // Data
    /// Ref: Parent
    CFscAddressSelect& iParent;
    };

// --------------------------------------------------------------------------
// CFscAddressSelect::CSelectFieldDlg::FscControlKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CFscAddressSelect::CSelectFieldDlg::FscControlKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    FUNC_LOG;
    // Forward call to virtual function in CFscAddressSelect interface
    return iParent.FscControlKeyEventL(aKeyEvent, aType);
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::CFscAddressSelect
// --------------------------------------------------------------------------
//
CFscAddressSelect::CFscAddressSelect(TFscAddressSelectParams& aParams) :
    iParams(aParams)
    {
    FUNC_LOG;
    __ASSERT_DEBUG
    ( !iFieldDlg && !iDestroyedPtr,
            Panic( EPanicPostCond_Constructor ) );
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::~CFscAddressSelect
// --------------------------------------------------------------------------
//
CFscAddressSelect::~CFscAddressSelect()
    {
    FUNC_LOG;
    // Tell ExecuteLD this object is already destroyed
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }

    // Set eliminator pointer to NULL
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }

    delete iFieldDlg;
    delete iFieldFilter;
    delete iFieldTypeSelector;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::NewL
// --------------------------------------------------------------------------
//
CFscAddressSelect* CFscAddressSelect::NewL
( TFscAddressSelectParams& aParams )
    {
    FUNC_LOG;
    CFscAddressSelect* self = new ( ELeave ) CFscAddressSelect( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::ConstructL
// --------------------------------------------------------------------------
//
void CFscAddressSelect::ConstructL()
    {
    FUNC_LOG;
    // Read the resource referenced in address select resource struct
    const TInt fieldTypeSelectorRes = iParams.iResReader.ReadInt32();
    TResourceReader selectorReader;
    CCoeEnv::Static()->CreateResourceReaderLC(selectorReader, fieldTypeSelectorRes);
    // Give that resource reader to the field type selector
    iFieldTypeSelector = CVPbkFieldTypeSelector::NewL(selectorReader,
            iParams.iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // selectorReader

    iNoAddressesForNamePromptResource = iParams.iResReader.ReadInt32();
    iNoAddressesPromptResource = iParams.iResReader.ReadInt32();
    iSoftKeyResource = iParams.iResReader.ReadInt32();

    const CVPbkFieldFilter::TConfig
            config(
                    const_cast<MVPbkStoreContactFieldCollection&> (iParams.iContact.Fields() ),
                    iFieldTypeSelector, NULL);

    iFieldFilter = CVPbkFieldFilter::NewL(config);

    CVPbkContactManager& iContactManager =
            const_cast<CVPbkContactManager&>(iParams.iContactManager);
    iAttributeManager = &iContactManager.ContactAttributeManagerL();
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::ExecuteLD
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CFscAddressSelect::ExecuteLD()
    {
    FUNC_LOG;
    __ASSERT_DEBUG( !iFieldDlg, Panic( EPanicPreCond_ExecuteLD ) );

    // "D" function semantics
    CleanupStack::PushL( this );
    TBool thisDestroyed = EFalse;
    // Ensure that thisDestroyed will be ETrue if this object is destroyed.
    // See in destructor how this is done.
    iDestroyedPtr = &thisDestroyed;

    SelectFieldL();

    MVPbkStoreContactField* returnedField = NULL;
    if ( iSelectedField )
        {
        // We cannot return iSelectedField directly since if its NULL,
        // it's value changes to 0xdedede before it is returned.
        // Therefore we must test iSelectedField before assigning it
        // to returnedField.
        returnedField = iSelectedField;
        }

    if ( thisDestroyed )
        {
        // This object has already been destroyed
        CleanupStack::Pop( this );
        returnedField = NULL;
        }
    else
        {
        CleanupStack::PopAndDestroy( this );
        }

    return returnedField;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::AttemptExitL
// --------------------------------------------------------------------------
//
void CFscAddressSelect::AttemptExitL(TBool aAccept)
    {
    FUNC_LOG;
    if (iFieldDlg)
        {
        iFieldDlg->AttemptExitL(aAccept);
        }
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::FscControlKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CFscAddressSelect::FscControlKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret = EKeyWasNotConsumed;

    if ( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend )
        {
        // Event is Send key, tell field selection dialog to accept
        // current selection
        AttemptExitL( ETrue );
        ret = EKeyWasConsumed;
        }
    else
    	if ( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneEnd )
            {
            ForceExit();
            ret = EKeyWasConsumed;
            }

    return ret;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::RequestExitL
// --------------------------------------------------------------------------
//
void CFscAddressSelect::RequestExitL(TInt aCommandId)
    {
    FUNC_LOG;
    if (aCommandId == EEikBidCancel)
        {
        AttemptExitL(EFalse);
        }
    else
        if (aCommandId == EEikBidOk)
            {
            AttemptExitL(ETrue);
            }
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::ForceExit
// --------------------------------------------------------------------------
//
void CFscAddressSelect::ForceExit()
    {
    FUNC_LOG;
    TRAPD( err, AttemptExitL( EFalse ) )
    ;
    if (err != KErrNone)
        {
        // If not nicely then use the force
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CFscAddressSelect::ResetWhenDestroyed(MFscDialogEliminator** aSelfPtr)
    {
    FUNC_LOG;
    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::AddressField
// Returns true if aField is an applicable address field.
// --------------------------------------------------------------------------
//
TBool CFscAddressSelect::AddressField(const MVPbkStoreContactField& aField) const
    {
    FUNC_LOG;
    TBool ret = EFalse;

    // Return true if field belongs to the selector
    ret = (iFieldFilter->FindField(aField) == KErrNotFound ) ? EFalse : ETrue;

    return ret;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::NoAddressesL
// Called if there are no applicable address fields in a contact passed
// to ExecuteLD.
// --------------------------------------------------------------------------
//
void CFscAddressSelect::NoAddressesL(TFscAddressSelectParams aParams) const
    {
    FUNC_LOG;
    if ( !aParams.iSuppressWarnings)
        {
        HBufC* prompt= NULL;
        HBufC* name = aParams.iNameFormatter.GetContactTitleOrNullL(
                iParams.iContact.Fields(), KDefaultTitleFormat);

        if (name)
            {
            CleanupStack::PushL(name);
            prompt = StringLoader::LoadL(iNoAddressesForNamePromptResource,
                    *name);
            CleanupStack::PopAndDestroy(); // name
            }
        else
            {
            prompt = StringLoader::LoadL(iNoAddressesPromptResource);
            }

        if (prompt)
            {
            CleanupStack::PushL(prompt);
            // This is a waiting dialog because the address select might be
            // used from the application server and the information note will
            // disappear if the application server closes before the
            // note timeout has expired, thus causing blinking
            CAknInformationNote* noteDlg = new ( ELeave ) CAknInformationNote( ETrue );
            noteDlg->ExecuteLD( *prompt);
            CleanupStack::PopAndDestroy(); // prompt
            }
        }
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::SelectFieldL
// --------------------------------------------------------------------------
//
inline void CFscAddressSelect::SelectFieldL()
    {
    FUNC_LOG;
    // If currently focused field is an applicable field
    if (iParams.iFocusedField && AddressField( *iParams.iFocusedField) )
        {
        // Applicable field was focused -> return field
        SetSelectedFieldL(iParams.iFocusedField);
        }
    else
        {
        TBool found = EFalse;
        // Focus is in on some other field, first check default field
        if (iParams.iDefaultPriorities && iParams.iUseDefaultDirectly
                && iParams.iDefaultPriorities->Count() > 0)
            {
            found = SelectFromDefaultFieldsL();
            }

        if ( !found)
            {
            // No direct call to focused or default number, we have to
            // select from applicable fields
            SelectFromApplicableFieldsL();
            }
        }
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::SelectFromApplicableFieldsL
// --------------------------------------------------------------------------
//
inline void CFscAddressSelect::SelectFromApplicableFieldsL()
    {
    FUNC_LOG;
    TInt indexOfDefault = IndexOfDefaultFieldL();
    const TInt fieldCount = iFieldFilter->FieldCount();

    // Different actions for different number of applicable fields found
    if (fieldCount == 0)
        {
        // No applicable addresses found
        NoAddressesL(iParams);
        }
    else
        if (fieldCount == 1)
            {
            // Exactly one applicable address found, just return it
            SetSelectedFieldL( &iFieldFilter->FieldAt(KFirstField) );
            }
        else
            if (fieldCount > 1)
                {
                HBufC* entryTitle = iParams.iNameFormatter.GetContactTitleL(
                        iParams.iContact.Fields(), KDefaultTitleFormat);
                CleanupStack::PushL(entryTitle);
                HBufC* title= NULL;
                if (iParams.iTitleResId)
                    {
                    if (iParams.iIncludeContactNameInPrompt)
                        {
                        title = StringLoader::LoadL(iParams.iTitleResId,
                                *entryTitle);
                        }
                    else
                        {
                        title = StringLoader::LoadL(iParams.iTitleResId);
                        }
                    CleanupStack::PopAndDestroy(entryTitle);
                    }
                else
                    {
                    title = entryTitle; // takes ownership of entryTitle
                    CleanupStack::Pop(entryTitle);
                    entryTitle = NULL;
                    }

                // Run the address selection dialog
                CleanupStack::PushL(title);
                iFieldDlg = new ( ELeave ) CSelectFieldDlg( *this );
                iFieldDlg->ResetWhenDestroyed( &iFieldDlg);
                MVPbkStoreContactField* field= NULL;
                //<cmail>
                field = iFieldDlg->ExecuteLD( *iFieldFilter,
                        iParams.iContactManager, iParams.iFieldPropertyArray,
                        iSoftKeyResource, *title, indexOfDefault);
                CleanupDeletePushL( field );
                //</cmail>

                SetSelectedFieldL(field);

                CleanupStack::PopAndDestroy( 2); // field, title
                }
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::IndexOfDefaultFieldL
// --------------------------------------------------------------------------
//
inline TInt CFscAddressSelect::IndexOfDefaultFieldL()
    {
    FUNC_LOG;
    TInt ret = KErrNotFound;

    const MVPbkStoreContactField* field = FindDefaultFieldLC();
    if (field)
        {
        ret = iFieldFilter->FindField( *field);
        }
    CleanupStack::PopAndDestroy(); // field

    return ret;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::SelectFromDefaultFieldsL
// --------------------------------------------------------------------------
//
inline TBool CFscAddressSelect::SelectFromDefaultFieldsL()
    {
    FUNC_LOG;
    TBool found = EFalse;

    const MVPbkStoreContactField* field = FindDefaultFieldLC();
    if (field)
        {
        SetSelectedFieldL(field);
        found = ETrue;
        }
    CleanupStack::PopAndDestroy(); // field

    return found;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::FindDefaultFieldLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CFscAddressSelect::FindDefaultFieldLC()
    {
    FUNC_LOG;
    MVPbkStoreContactField* ret= NULL;

    // Go through the default priorities array, and
    // check does a specific default exist
    if (iParams.iDefaultPriorities)
        {
        const TInt defaultCount = iParams.iDefaultPriorities->Count();

        for (TInt i=0; i<defaultCount; ++i)
            {
            TVPbkDefaultType defaultType =
                    (TVPbkDefaultType) iParams.iDefaultPriorities->At(i);

            // Create an attribute prototype out of the attribute identifier
            CVPbkDefaultAttribute* attr =
                    CVPbkDefaultAttribute::NewL(defaultType);
            CleanupStack::PushL(attr);

            if (iAttributeManager->HasContactAttributeL( *attr,
                    iParams.iContact) )
                {
                const MVPbkStoreContactFieldCollection& fields =
                        iParams.iContact.Fields();

                // Get the field with attribute
                const TInt fieldCount = fields.FieldCount();
                for (TInt j = 0; j < fieldCount; ++j)
                    {
                    if (iAttributeManager->HasFieldAttributeL( *attr,
                            fields.FieldAt(j) ) )
                        {
                        ret = fields.FieldAtLC(j);
                        CleanupStack::Pop(); // ret
                        break;
                        }
                    }
                }

            CleanupStack::PopAndDestroy(attr);
            }
        }

    CleanupDeletePushL(ret);
    return ret;
    }

// --------------------------------------------------------------------------
// CFscAddressSelect::SetSelectedFieldL
// --------------------------------------------------------------------------
//
inline void CFscAddressSelect::SetSelectedFieldL(
        const MVPbkStoreContactField* aField)
    {
    FUNC_LOG;
    if (aField)
        {
        const MVPbkStoreContactFieldCollection& fields =
                iParams.iContact.Fields();
        TInt index = IndexOfField(fields, *aField);
        iSelectedField = fields.FieldAtLC(index);
        CleanupStack::Pop();
        }
    }

// End of File

