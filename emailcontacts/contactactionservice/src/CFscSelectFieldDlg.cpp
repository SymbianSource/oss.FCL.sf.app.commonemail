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
* Description:  Implementation of field selection dialog.
 *
*/


#include "emailtrace.h"
#include "CFscSelectFieldDlg.h"

// Phonebook 2
#include "CFscFieldListBoxModel.h"
#include "MFscControlKeyObserver.h"
#include "MFscClipListBoxText.h"
#include <CPbk2IconArray.h>
#include "CFscPresentationContactFieldCollection.h"
#include <CPbk2StorePropertyArray.h>
#include <FscActionUtils.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>

// System includes
#include <aknPopup.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <calslbs.h>
#include <barsread.h>

/// Unnamed namespace for local definitions
namespace
    {

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPostCond_Constructor = 1,
        EPanicPreCond_ExecuteLD,
        EPanicPreCond_AttemptExitL,
        EPanicPreCond_ResetWhenDestroyed
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CPbkSelectFieldDlg");
        User::Panic(KPanicText, aReason);
        }
#endif // _DEBUG
    /**
     * Creates and returns an icon array with Phonebook 2
     * default and extension icons.
     *
     * @return  An array filled with icons.
     */
    CPbk2IconArray* CreateListBoxIconArrayL()
        {
        FUNC_LOG;
        // Create icon array
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC(reader, R_FSC_FIELDTYPE_ICONS);
        CPbk2IconArray* iconArray = CPbk2IconArray::NewL(reader);
        CleanupStack::PopAndDestroy(); // reader

        return iconArray;
        }

    } /// namespace


/**
 * Specialized popup list class.
 */
NONSHARABLE_CLASS( CFscSelectFieldDlg::CPopupList ) :
public CAknPopupList
    {
public: // Interface

    /**
     * Like CAknPopupList::NewL.
     * @see CAknPopupList::NewL
     */
    static CPopupList* NewL(
            CEikListBox* aListBox,
            TInt aCbaResource,
            AknPopupLayouts::TAknPopupLayouts aType =
            AknPopupLayouts::EMenuWindow );

    /**
     * Sets the listbox item to focus initially.
     * By default focus is on the first item.
     *
     * @param aFocusIndex  Index of the listbox item to focus initially.
     */
    inline void SetInitialFocus(
            TInt aFocusIndex );

    /**
     * Like CAknPopupList::AttemptExitL.
     * @see CAknPopupList::AttemptExitL
     */
    inline void AttemptExitL(
            TBool aAccept );

private: // From CAknPopupList
    void SetupWindowLayout(
            AknPopupLayouts::TAknPopupLayouts aType );

private: // Implementation
    inline CPopupList();

private: // Data
    /// Own: Focus index
    TInt iFocusIndex;
    };

/**
 * Specialized listbox class.
 */
NONSHARABLE_CLASS( CFscSelectFieldDlg::CListBox ) :
public CAknDoubleGraphicPopupMenuStyleListBox,
public MFscClipListBoxText
    {
public: // Interface

    /**
     * Constructor.
     */
    inline CListBox();

    /**
     * Sets the observer.
     *
     * @param aKeyObserver      The observer to set.
     */
    inline void SetObserver(
            MFscControlKeyObserver* aKeyObserver );

    /**
     * Returns CONE environment.
     *
     * @return  CONE environment.
     */
    inline CCoeEnv* CoeEnv();

private: // From CAknDoubleGraphicPopupMenuStyleListBox
    TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

private: // From MFscClipListBoxText
    TBool ClipFromBeginning(
            TDes& aBuffer,
            TInt aItemIndex,
            TInt aSubCellNumber );

private: // Data
    /// Ref: Observer
    MFscControlKeyObserver* iKeyObserver;
    };

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::CPopupList
// --------------------------------------------------------------------------
//
inline CFscSelectFieldDlg::CPopupList::CPopupList() :
    iFocusIndex(KErrNotFound)
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::NewL
// --------------------------------------------------------------------------
//
CFscSelectFieldDlg::CPopupList* CFscSelectFieldDlg::CPopupList::NewL(
        CEikListBox* aListBox, TInt aCbaResource,
        AknPopupLayouts::TAknPopupLayouts aType)
    {
    FUNC_LOG;
    CPopupList* self = new ( ELeave ) CPopupList();
    CleanupStack::PushL(self);
    self->ConstructL(aListBox, aCbaResource, aType);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::SetInitialFocus
// --------------------------------------------------------------------------
//
inline void CFscSelectFieldDlg::CPopupList::SetInitialFocus(TInt aFocusIndex)
    {
    FUNC_LOG;
    iFocusIndex = aFocusIndex;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CPopupList::AttemptExitL
// --------------------------------------------------------------------------
//
inline void CFscSelectFieldDlg::CPopupList::AttemptExitL(TBool aAccept)
    {
    FUNC_LOG;
    CAknPopupList::AttemptExitL(aAccept);
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::SetupWindowLayout
// --------------------------------------------------------------------------
//
void CFscSelectFieldDlg::CPopupList::SetupWindowLayout(
        AknPopupLayouts::TAknPopupLayouts aType)
    {
    FUNC_LOG;
    // Call base class
    CAknPopupList::SetupWindowLayout(aType);
    // Set initial focus
    if (iFocusIndex >= 0)
        {
        ListBox()->SetCurrentItemIndex(iFocusIndex);
        }
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::CListBox
// --------------------------------------------------------------------------
//
inline CFscSelectFieldDlg::CListBox::CListBox()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::SetObserver
// --------------------------------------------------------------------------
//
inline void CFscSelectFieldDlg::CListBox::SetObserver(
        MFscControlKeyObserver* aKeyObserver)
    {
    FUNC_LOG;
    iKeyObserver = aKeyObserver;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::CoeEnv
// --------------------------------------------------------------------------
//
inline CCoeEnv* CFscSelectFieldDlg::CListBox::CoeEnv()
    {
    FUNC_LOG;
    return iCoeEnv;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CFscSelectFieldDlg::CListBox::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse ret = EKeyWasNotConsumed;

    if (iKeyObserver && iKeyObserver->FscControlKeyEventL(aKeyEvent, aType)
            == EKeyWasConsumed)
        {
        ret = EKeyWasConsumed;
        }
    else
        {
        ret = CAknDoubleGraphicPopupMenuStyleListBox::OfferKeyEventL(
                aKeyEvent, aType);
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CListBox::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CFscSelectFieldDlg::CListBox::ClipFromBeginning(TDes& aBuffer,
        TInt aItemIndex, TInt aSubCellNumber)
    {
    FUNC_LOG;
    return AknTextUtils::ClipToFit(aBuffer, AknTextUtils::EClipFromBeginning,
            this, aItemIndex, aSubCellNumber);
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CFscSelectFieldDlg
// --------------------------------------------------------------------------
//
CFscSelectFieldDlg::CFscSelectFieldDlg()
    {
    FUNC_LOG;
    __ASSERT_DEBUG(
            !iListBox && !iPopupList && !iKeyObserver && !iDestroyedPtr,
            Panic(EPanicPostCond_Constructor));
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::~CFscSelectFieldDlg
// --------------------------------------------------------------------------
//
CFscSelectFieldDlg::~CFscSelectFieldDlg()
    {
    FUNC_LOG;
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }
    if (iPopupList)
        {
        iPopupList->CancelPopup();
        }

    delete iListBox;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::ExecuteLD
// <cmail> Changed to ExecuteLD to prevent Codescanner warnings.
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CFscSelectFieldDlg::ExecuteLD( // <cmail>
        MVPbkStoreContactFieldCollection& aFieldCollection,
        const CVPbkContactManager& aContactManager,
        const MPbk2FieldPropertyArray& aFieldProperties, TInt aCbaResourceId,
        const TDesC& aHeading /*=KNullDesC*/, TInt aFocusIndex /*=KErrNotFound*/)
    {
    FUNC_LOG;
    __ASSERT_DEBUG(!iListBox && !iPopupList, Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL( this);
    TBool thisDestroyed = EFalse;
    // Destructor will set thisDestroyed to ETrue if this object is destroyed
    iDestroyedPtr = &thisDestroyed;

    CFscPresentationContactFieldCollection* fields =
            CFscPresentationContactFieldCollection::NewL(aFieldProperties,
                    aFieldCollection, aFieldCollection.ParentStoreContact() );
    CleanupStack::PushL(fields);

    if (aFocusIndex != KErrNotFound)
        {
        // The focus index is from store contact field order and
        // we have to map it to presentation contact field order
        MVPbkStoreContactField* initiallyFocusedField =
                aFieldCollection.FieldAtLC(aFocusIndex);
        for (TInt i( 0); i < fields->FieldCount(); ++i)
            {
            if (fields->FieldAt( i ).IsSame( *initiallyFocusedField) )
                {
                // Fix the focus index to match with
                // presentation contact fields
                aFocusIndex = i;
                break;
                }
            }
        CleanupStack::PopAndDestroy(); // initiallyFocusedField
        }

    // Create list box
    CreateListBoxL( *fields, aContactManager, aFieldProperties,
            aCbaResourceId, aHeading, aFocusIndex);

    // Show the list query
    TInt dlgResult = 0;
    TRAPD( err, dlgResult = iPopupList->ExecuteLD() )
    ;
    if ( !thisDestroyed)
        {
        iPopupList = NULL;
        }
    if (err != KErrNone)
        {
        User::Leave(err);
        }

    // Get the return value
    MVPbkStoreContactField* result= NULL;
    if (thisDestroyed)
        {
        // This object has been destroyed
        CleanupStack::PopAndDestroy(fields);
        CleanupStack::Pop(); // this
        }
    else
        {
        if (dlgResult)
            {
            TInt index = iListBox->CurrentItemIndex();
            result = fields->FieldAt( index ).CloneLC();
            CleanupStack::Pop();
            }
        CleanupStack::PopAndDestroy(fields);
        CleanupStack::PopAndDestroy(); // this
        }

    // <cmail>
    //CleanupDeletePushL(result);
    // </cmail>
    
    return result;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::SetObserver
// --------------------------------------------------------------------------
//
void CFscSelectFieldDlg::SetObserver(MFscControlKeyObserver* aKeyObserver)
    {
    FUNC_LOG;
    iKeyObserver = aKeyObserver;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::AttemptExitL
// --------------------------------------------------------------------------
//
void CFscSelectFieldDlg::AttemptExitL(TBool aAccept)
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iPopupList, Panic( EPanicPreCond_AttemptExitL ) );
    iPopupList->AttemptExitL(aAccept);
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CFscSelectFieldDlg::ResetWhenDestroyed(CFscSelectFieldDlg** aSelfPtr)
    {
    FUNC_LOG;
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this,
            Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CFscSelectFieldDlg::CreateListBoxL
// --------------------------------------------------------------------------
//
void CFscSelectFieldDlg::CreateListBoxL(
        const CFscPresentationContactFieldCollection& aFieldCollection,
        const CVPbkContactManager& aContactManager,
        const MPbk2FieldPropertyArray& aFieldProperties, TInt aCbaResourceId,
        const TDesC& aHeading, TInt aFocusIndex)
    {
    FUNC_LOG;
    // Create a list box
    iListBox = new(ELeave) CListBox;

    // Create a popup list
    CPopupList* popupList = CPopupList::NewL(iListBox, aCbaResourceId,
            AknPopupLayouts::EMenuDoubleLargeGraphicWindow);
    CleanupStack::PushL(popupList);

    // Init list box
    iListBox->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    iListBox->SetObserver(iKeyObserver);

    // Set title of popuplist
    if (aHeading.Length() > 0)
        {
        popupList->SetTitleL(aHeading);
        }

    // Create icon array
    CPbk2IconArray* iconArray = CreateListBoxIconArrayL();
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    // Create list box model
    HBufC* timeFormat = iListBox->CoeEnv()->AllocReadResourceLC( R_QTN_DATE_USUAL);

    CFscFieldListBoxModel::TParams params(aFieldCollection, aContactManager,
            aFieldProperties, *timeFormat, *iconArray, 
            NULL, 
            NULL);
    CFscFieldListBoxModel* listBoxModel = CFscFieldListBoxModel::NewL(params);
    CleanupStack::PushL(listBoxModel);
    listBoxModel->FormatFieldsL();
    CleanupStack::Pop(listBoxModel);
    CleanupStack::PopAndDestroy(); // timeFormat
    listBoxModel->SetClipper( *iListBox);
    iListBox->Model()->SetItemTextArray(listBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->Reset();

    if (aFocusIndex >= 0 && aFocusIndex < iListBox->Model()->NumberOfItems() )
        {
        popupList->SetInitialFocus(aFocusIndex);
        }

    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto);

    CleanupStack::Pop(); // listBox
    iPopupList = popupList;
    }

// End of File

