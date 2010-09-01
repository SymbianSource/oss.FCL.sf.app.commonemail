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
* Description:  Implementation of CFsFastAddressingList class
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)
/*#include <fsconfig.h>
#ifdef __FS_ALFRED_SUPPORT*/
#include "emailtrace.h"
#include <alf/alfevent.h>
/*#else // !__FS_ALFRED_SUPPORT
#include <hitchcock.h>
#endif // __FS_ALFRED_SUPPORT*/
//<cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)
#include <txtrich.h>

#include "fsfastaddressinglist.h"
#include "fsfastaddressinglistobserver.h"
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstextinputfield.h"
#include "fsfastaddressingvisualizer.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreeitemdata.h"
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFsFastAddressingList::CFsFastAddressingList( CAlfEnv& /*aEnv*/,
    MFsFastAddressingListObserver& aObserver ) :
    iObserver( aObserver )
    {
    FUNC_LOG;
    //No implementation nedeed
    }


// ---------------------------------------------------------------------------
// Two phased constructor
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );
    iTextInputField = CFsTextInputField::NewL( *this );
    iTextInputField->ActivateL();
    iVisualizer = CFsFastAddressingVisualizer::NewL( *this, *iTextInputField );
/*
    iTreeVisualizer = CFsTreeVisualizerBase::NewL( this,
                                  *iVisualizer->GetMainLayout() );

    iTreeList = CFsTreeList::NewL( *iTreeVisualizer,
                                   Env() );
*/
//    iTreeList->SetLooping( ETrue );

    iTextInputField->SetFocus( ETrue );
    iVisualizer->SetTextFieldVisible( ETrue );
    iTextInputField->MakeVisible( ETrue );
    }

// ---------------------------------------------------------------------------
// Two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CFsFastAddressingList* CFsFastAddressingList::NewL( CAlfEnv& aEnv,
    MFsFastAddressingListObserver& aObserver )
    {
    FUNC_LOG;
    CFsFastAddressingList* self =
             new ( ELeave ) CFsFastAddressingList( aEnv, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFsFastAddressingList::~CFsFastAddressingList()
    {
    FUNC_LOG;
    delete iVisualizer;
    delete iTreeList;
    delete iTextInputField;

    iPlainItemDataArr.ResetAndDestroy();
    iPlainItemDataArr.Close();
    iPlainItemVisualizerArr.ResetAndDestroy();
    iPlainItemVisualizerArr.Close();
    }

// ---------------------------------------------------------------------------
// Loads image from file and sets texture for icon
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetIconL( const TIconID aIconID,
    const TDesC& aFilename )
    {
    FUNC_LOG;
    if ( aIconID == EIconA )
        {
#pragma message( "Unigue number needed" )
        const TInt KFsUniqueId = 12345; // Number should be unique inside the application.
        iIconA.SetTexture( Env().TextureManager().LoadTextureL( 
            aFilename, TSize( 0, 0 ),  EAlfTextureFlagDefault, KFsUniqueId ) );
        iVisualizer->SetIcon( aIconID, iIconA );
        }
    else if ( aIconID == EIconB )
        {
#pragma message( "Unigue number needed" )
        const TInt KFsUniqueId = 123456; // Number should be unique inside the application.
        iIconB.SetTexture( Env().TextureManager().LoadTextureL( 
            aFilename, TSize( 0, 0 ),  EAlfTextureFlagDefault, KFsUniqueId ) );
        iVisualizer->SetIcon( aIconID, iIconB );
        }
    }


// ---------------------------------------------------------------------------
// Creates texture from bitmap and sets icon.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetIconL( const TIconID aIconID,
    const CFbsBitmap* aBitmap )
    {
    FUNC_LOG;
    if (aIconID == EIconA)
        {
        CAlfTexture& texture = Env().TextureManager().BlankTexture();
#pragma message( "Investigation needed" )
        //texture.UploadL( *aBitmap );
        iIconA.SetTexture( texture );
        iVisualizer->SetIcon( aIconID, iIconA );
        }
    else if ( aIconID == EIconB )
        {
        CAlfTexture& texture = Env().TextureManager().BlankTexture();
#pragma message( "Investigation needed" )
        //texture.UploadL( *aBitmap );
        iIconB.SetTexture( texture );
        iVisualizer->SetIcon( aIconID, iIconB );
        }
    }

// ---------------------------------------------------------------------------
// Controls visibility of the icon
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetIconVisible( const TIconID aIconID,
    const TBool aVisible )
    {
    FUNC_LOG;
    iVisualizer->SetIconVisible( aIconID, aVisible );
    }

// ---------------------------------------------------------------------------
// Returns visibility of the icon
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsFastAddressingList::IconVisible(
    const TIconID /*aIconID */) const
    {
    FUNC_LOG;
   
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Controls atomic mode of text input field.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetTextInputFieldAtomic(
    const TBool aAtomic)
    {
    FUNC_LOG;
    if (aAtomic)
        {
        iTextInputField->SetTextInputMode(
            CFsTextInputField::EAtomicInput );
        }
    else
        {
        iTextInputField->SetTextInputMode(
            CFsTextInputField::ENotAtomicInput );
        }
    }

// ---------------------------------------------------------------------------
// Resturns state of atomic mode of the text input field.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsFastAddressingList::TextInputFieldAtomic() const
    {
    FUNC_LOG;
    return (iTextInputField->TextInputMode() ==
        CFsTextInputField::EAtomicInput);
    }

// ---------------------------------------------------------------------------
// Controls read only mode of text input field.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetTextInputFieldReadOnly(
    const TBool aReadOnly)
    {
    FUNC_LOG;
    iTextInputField->SetReadOnly( aReadOnly );
    }

// ---------------------------------------------------------------------------
// Returns status of the read only mode of the text input field
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsFastAddressingList::TextInputFieldReadOnly() const
    {
    FUNC_LOG;
    return iTextInputField->IsReadOnly();
    }

// ---------------------------------------------------------------------------
// Sets content of the text input field
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetTextFieldContentsL(
    const TDesC& aText )
    {
    FUNC_LOG;
    iTextInputField->SetCursorPosL(
     0, EFalse );
    iTextInputField->SetTextL( &aText );
    iTextInputField->SetCursorPosL(
     aText.Length(), EFalse );
    }

// ---------------------------------------------------------------------------
// Returns content of the text input field
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::TextFieldContents( TDes& aText ) const
    {
    FUNC_LOG;
    iTextInputField->GetText( aText );
    }

// ---------------------------------------------------------------------------
// Controls visibility of the text input field
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetTextFieldVisibile(
    const TBool aVisibility )
    {
    FUNC_LOG;
    iVisualizer->SetTextFieldVisible( aVisibility );
    iTextInputField->MakeVisible( aVisibility );
    }

// ---------------------------------------------------------------------------
// Returns status of the visibility of the text input field.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsFastAddressingList::IsTextFieldVisible() const
    {
    FUNC_LOG;
    return iVisualizer->IsTextFieldVisible();
    }

// ---------------------------------------------------------------------------
// Controls visility of the tree list
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetListVisibility(
    const TBool /*aVisible*/)
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// Returns visibility status of the tree list
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsFastAddressingList::ListVisibile() const
    {
    FUNC_LOG;
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Copies items from array to the list. Previous itemset of the list is
// cleared
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetListItemsL(
    const RPointerArray<HBufC>& aItemTexts)
    {
    FUNC_LOG;
    //====== Uncomment these lines when the method RemoveAll
    //             will be implemented
    //iTreeList->RemoveAll();
    //iPlainItemDataArr.ResetAndDestroy();
    //iPlainItemVisualizerArr.ResetAndDestroy();
    for ( TInt item = 0 ; item < aItemTexts.Count() ; item++ )
        {
        CFsTreePlainOneLineItemData* plainItemData;
        MFsTreeItemVisualizer* plainItemVisualizer;
        plainItemData = CFsTreePlainOneLineItemData::NewL( );
        plainItemData->SetDataL( *aItemTexts[item] );
        plainItemVisualizer = CFsTreePlainOneLineItemVisualizer::NewL(
            *iTreeList->TreeControl() );
        iTreeList->InsertItemL( *plainItemData,
                                *plainItemVisualizer,
                                KFsTreeRootID );
        iPlainItemDataArr.AppendL( plainItemData );
        iPlainItemVisualizerArr.AppendL( plainItemVisualizer );
        }
    }

// ---------------------------------------------------------------------------
// Add passed item to the List.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::AddListItemL( const TDesC& aItemText )
    {
    FUNC_LOG;
    CFsTreePlainOneLineItemData* plainItemData;
    MFsTreeItemVisualizer* plainItemVisualizer;
    plainItemData = CFsTreePlainOneLineItemData::NewL( );
    plainItemData->SetDataL( aItemText );
    plainItemVisualizer = CFsTreePlainOneLineItemVisualizer::NewL(
        *iTreeList->TreeControl() );
    iTreeList->InsertItemL( *plainItemData,
                            *plainItemVisualizer,
                            KFsTreeRootID );
    iPlainItemDataArr.AppendL( plainItemData );
    iPlainItemVisualizerArr.AppendL( plainItemVisualizer );
    }

// ---------------------------------------------------------------------------
// Finds the passed text in itemset and highlight it
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::HighlightText( const TDesC& /*aText*/)
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// Controls position of the list (above the text input field or below)
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetListPosition( TFsListPosition aPos )
    {
    FUNC_LOG;
    iVisualizer->SetListPosition( aPos );
    }

// ---------------------------------------------------------------------------
// Returns the list position (above the text input field or below)
// ---------------------------------------------------------------------------
//
EXPORT_C CFsFastAddressingList::TFsListPosition
    CFsFastAddressingList::ListPosition() const
    {
    FUNC_LOG;
    return iVisualizer->ListPosition();
    }

// ---------------------------------------------------------------------------
// Controls the visibility of the scrollbar
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsFastAddressingList::SetScrollbarVisibility(
    const TBool /*aVisible */)
    {
    FUNC_LOG;
 
    }

// ---------------------------------------------------------------------------
// Returns status of the visibility of the scrollbar
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsFastAddressingList::ScrollbarVisibility() const
    {
    FUNC_LOG;

    return ETrue;
    }

// ---------------------------------------------------------------------------
// Returns the reference to the tree list component.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList& CFsFastAddressingList::List() const
    {
    FUNC_LOG;
    return *iTreeList;
    }

// ---------------------------------------------------------------------------
// From class CHuiControl
// Handling of the key events and passing to the text input field.
// ---------------------------------------------------------------------------
//
TBool CFsFastAddressingList::OfferEventL( const TAlfEvent &aEvent )
    {
    FUNC_LOG;
    TBool keyHandled( EFalse );

    if ( ( aEvent.IsKeyEvent() ) && ( aEvent.Code() ==  EEventKeyDown ) )
        {
        switch ( aEvent.KeyEvent().iScanCode )
            {
            case EStdKeyLeftArrow:
                {
                iTextInputField->MoveCursorL( TCursorPosition::EFLeft,
                                              EFalse );
                keyHandled = ETrue;
                break;
                }

            case EStdKeyRightArrow:
                {
                iTextInputField->MoveCursorL( TCursorPosition::EFRight,
                                              EFalse );
                keyHandled = ETrue;
                break;
                }

            default:
                {
                keyHandled = ( EKeyWasConsumed ==
                             iTextInputField->OfferKeyEventL( aEvent.KeyEvent(),
                                                              aEvent.Code() ) );
                break;
                }
            }
        }
     else
        {
        keyHandled = ( EKeyWasConsumed ==
                         iTextInputField->OfferKeyEventL( aEvent.KeyEvent(),
                                                          aEvent.Code() ) );
        }

    return keyHandled;
    }

// ---------------------------------------------------------------------------
// From class CHuiControl
// Invoked when layouts are updated
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::VisualLayoutUpdated( CAlfVisual& /*aVisual */)
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// From class CHuiControl
// Invoked when visibility of the control is changing
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::NotifyControlVisibility( TBool /*aIsVisible*/,
        CAlfDisplay& /*aDisplay */)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From class MFsTextInputFieldObserver
// From text input field observer. Invoked when changes in the text are made
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::HandleTextInputEventL(
    CFsTextInputField* aTextInputField,
    MFsTextInputFieldObserver::TFsTextInputEventType /*aEventType */)
    {
    FUNC_LOG;
    HBufC16* text = HBufC16::NewL(
        aTextInputField->RichText()->DocumentLength() );
    TPtr16 ptr = text->Des();
    aTextInputField->RichText()->Extract( ptr );
    iObserver.TextInputFieldUpdated( ptr );
    delete text;
    }

// ---------------------------------------------------------------------------
// Copies highlighted text to the text input field
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::MakeSelection()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// Highlights next item in the tree list
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::MoveSelectionDown()
    {
    FUNC_LOG;
  
    }

// ---------------------------------------------------------------------------
// Highlights previous item in the tree list
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::MoveSelectionUp()
    {
    FUNC_LOG;
 
    }

// ---------------------------------------------------------------------------
// Controls text input mode
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::SetTextInputMode()
    {
    FUNC_LOG;
  
    }

// ---------------------------------------------------------------------------
// Invoked when text has been modified.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingList::TextInputFieldModified()
    {
    FUNC_LOG;

    }

