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
 * Description:  ESMR titled field implementation
 *
 */
#include "cesmrattendeefield.h"
//<cmail>
#include "esmrcommands.h"
#include "CFSMailClient.h"
//</cmail>
#include "cesmrncspopuplistbox.h"
#include "cesmrncsemailaddressobject.h"
#include "mesmrlistobserver.h"
#include "cesmrcontacthandler.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrfieldbuilderdef.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrimage.h"
#include "cmrbutton.h"

#include <aknsbasicbackgroundcontrolcontext.h>
#include <calentry.h>
#include <stringloader.h>
#include <esmrgui.rsg>
#include <aknbutton.h>
#include <gulicon.h>
#include <ct/rcpointerarray.h>

// DEBUG
#include "emailtrace.h"


// unnamed namespace for local definitions
namespace { // codescanner::namespace
//CONSTANTS
const TInt KMaxAmountOfItems( 4 );
const TInt KMaxRemoteSearchResponseLength = 255;

// Panic code literal
_LIT( KESMRAttendeeField, "ESMRAttendeeField" );

/** Panic code enumeration */
enum TESMRAttendeeFieldPanic
    {
    EESMRAttendeeFieldNotMeetingRequestEntry // Passed entry is not MR
    };

void Panic( TESMRAttendeeFieldPanic aPanic )
    {
    User::Panic( KESMRAttendeeField, aPanic );
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAttendeeField::CESMRAttendeeField
// ---------------------------------------------------------------------------
//
CESMRAttendeeField::CESMRAttendeeField( CCalAttendee::TCalRole aRole )
:iRole( aRole )
    {
    FUNC_LOG;
    SetFocusType( EESMRHighlightFocus );    
    SetFieldId ( (iRole == CCalAttendee::EReqParticipant) ? 
            EESMRFieldAttendee : EESMRFieldOptAttendee );
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::~CESMRAttendeeField
// ---------------------------------------------------------------------------
//
CESMRAttendeeField::~CESMRAttendeeField( )
    {
    FUNC_LOG;
    delete iAacListBox;
    delete iContactHandler;
    delete iDefaultText;
    delete iBgCtrlContext;
    delete iFieldButton;
    delete iTitle;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::NewL
// ---------------------------------------------------------------------------
//
CESMRAttendeeField* CESMRAttendeeField::NewL( CCalAttendee::TCalRole aRole )
    {
    FUNC_LOG;
    CESMRAttendeeField* self = new (ELeave) CESMRAttendeeField( aRole );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ConstructL( )
    {
    FUNC_LOG;
    SetComponentsToInheritVisibility( ETrue );

    iContactHandler = CESMRContactHandler::NewL();

    iDefaultText = StringLoader::LoadL ( R_QTN_MEET_REQ_CONTACT_FIELD );

    iFieldButton = CMRButton::NewL( NMRBitmapManager::EMRBitmapSearchContacts );
    iFieldButton->SetParent( this );
    iFieldButton->SetObserver( this );

    iTitle = CMRLabel::NewL();
    iTitle->SetParent( this );
    
    HBufC* buf = NULL;
    if( FieldId() == EESMRFieldAttendee )
        {
        buf = StringLoader::LoadLC( R_QTN_MEET_REQ_LABEL_REQUIRED );
        }
    
    if( FieldId() == EESMRFieldOptAttendee )
        {
        buf = StringLoader::LoadLC( R_QTN_MEET_REQ_LABEL_OPT );
        }
    
    iTitle->SetTextL( *buf ); // ownership transferred
    CleanupStack::Pop( buf );

    
    iEditor = 
        new ( ELeave ) CESMRNcsAifEditor( *iContactHandler, iDefaultText );
    CESMRField::ConstructL( iEditor );
    iEditor->ConstructL (
            this,
            KMaxAddressFieldLines,
            0,
            CEikEdwin::EAvkonEditor | CEikEdwin::EResizable |
            CEikEdwin::ENoAutoSelection |CEikEdwin::EInclusiveSizeFixed |
            CEikEdwin::ENoHorizScrolling
            );    
    iEditor->SetEdwinSizeObserver( this );
    iEditor->SetParent( this );
    iEditor->SetPopupList( this );
    iEditor->SetAlignment ( EAknEditorAlignBidi | EAknEditorAlignCenter );
    iEditor->SetAknEditorInputMode ( EAknEditorTextInputMode );
    iEditor->SetAknEditorFlags ( 
            EAknEditorFlagNoT9| EAknEditorFlagUseSCTNumericCharmap );
    iEditor->SetAknEditorCurrentCase ( EAknEditorLowerCase );

    // Draw bg instead of using skin bg
    TRect tempRect( 0, 0, 0, 0 );
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );
    iBgCtrlContext = CAknsBasicBackgroundControlContext::NewL( 
                bitmapStruct.iItemId, 
                tempRect, 
                EFalse );        
    iEditor->SetSkinBackgroundControlContextL( iBgCtrlContext );
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRAttendeeField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );
    
    TRect richTextRect = 
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect textRect( NMRLayoutManager::GetLayoutText( 
            richTextRect, 
       NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );
    
    // Adjust field size so that there's room for expandable editor.
    richTextRect.Resize( 0, iSize.iHeight - textRect.Height() );
    
    // Add title area to the required size
    TSize titleSize( CESMRField::MinimumSize() );
    
    TSize completeFieldSize( titleSize );
    completeFieldSize.iHeight += richTextRect.Height();
    
    return completeFieldSize;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::InitializeL()
    {
    FUNC_LOG;
    // No Implementation
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    iEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL ( 
            CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EOff );
    
    TInt fieldAttendeeCount( 0 );
    
    CCalEntry& entry = aEntry.Entry ( );
    RPointerArray< CCalAttendee > attendees = entry.AttendeesL ( );
    RCPointerArray< CESMRNcsEmailAddressObject > addressList;
    CleanupClosePushL( addressList );    

    for (TInt i(0); i < attendees.Count(); i++ )
        {
        if ( attendees[i]->RoleL() == iRole )
            {
            fieldAttendeeCount++;
            
            const TDesC& addr = attendees[i]->Address ( );
            const TDesC& commonName = attendees[i]->CommonName ( );

            CESMRNcsEmailAddressObject* obj = 
                CESMRNcsEmailAddressObject::NewL ( commonName, addr );
            CleanupStack::PushL (obj );
            addressList.AppendL( obj );
            CleanupStack::Pop( obj );
            }
        }

    if ( fieldAttendeeCount > 0 )
        {
        if ( !iObserver->IsControlVisible(iFieldId ) )
            {
            iObserver->ShowControl( iFieldId );
            }
        iEditor->SetAddressesL( addressList );
        }
    else
        {
        AppendDefaultTextL();
        //during initialisation, SelectAllL() in setoutlinefocus gets overridden so this is needed
        iEditor->SelectAllL();
        }

    //don't show optional attendees field on initialisation if there are none
	if ( ( EESMRFieldModeEdit == FieldMode() ) && ( iRole == CCalAttendee::EOptParticipant ) )
        {
        if ( fieldAttendeeCount == 0 )
            {
            this->MakeVisible(EFalse);
            }
        }

    CleanupStack::PopAndDestroy( &addressList );
    // this is needed to be re-called here, otherwise the CEikEdwin
    // does not get correctly instantiated with default text
    iEditor->FocusChanged(EDrawNow);
    
    UpdateSendOptionL();
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRMeetingRequestEntry* mrEntry = NULL;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == aEntry.Type() )
        {
        mrEntry = static_cast<MESMRMeetingRequestEntry*>(&aEntry);
        }

    // This should always be called for MR entries
    __ASSERT_ALWAYS( mrEntry, Panic( EESMRAttendeeFieldNotMeetingRequestEntry ));

    if( iEditor->HasDefaultText() )
        {
        ClearDefaultTextL();
        }

    UpdateAttendeesL( *mrEntry );

    HBufC* text = iEditor->GetTextInHBufL();
        
    //If there is no attendees, put back the default text
    if( !text )
        {
    	AppendDefaultTextL();
    	//during initialisation, SelectAllL() in setoutlinefocus gets overridden so this is needed
    	iEditor->SelectAllL();
    	}
    delete text;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRAttendeeField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if( iFieldButton )
        {
        ++count;
        }
    
    if ( iTitle )
        {
        ++count;
        }

    if( iEditor )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRAttendeeField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            return iFieldButton;
        case 1:
            return iTitle;
        case 2:
            return iEditor;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect( Rect() );
       
    TAknLayoutRect firstRowLayoutRect( 
           NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect firstRowRect( firstRowLayoutRect.Rect() );
    
    TRect secondRowRect( firstRowRect );
    secondRowRect.Move( 0, firstRowRect.Height() );
    
    // Layout field button
    if( iFieldButton )
       {
       TAknWindowComponentLayout buttonLayout( 
               NMRLayoutManager::GetWindowComponentLayout(
                   NMRLayoutManager::EMRLayoutTextEditorIcon ) );
       AknLayoutUtils::LayoutControl( 
               iFieldButton, firstRowRect, buttonLayout );
       }
    
    // Layout field title
    if( iTitle )
       {
       TAknLayoutText labelLayout( 
               NMRLayoutManager::GetLayoutText(
                       firstRowRect, 
                           NMRLayoutManager::EMRTextLayoutTextEditor ) );
    
       iTitle->SetRect( labelLayout.TextRect() );
    
       // Setting font also for the label. Failures are ignored.
       iTitle->SetFont( labelLayout.Font() );
       }
    
    // Layout field editor
    if( iEditor )
        {
        TAknLayoutText editorLayoutText = NMRLayoutManager::GetLayoutText( 
                  secondRowRect, 
                  NMRLayoutManager::EMRTextLayoutTextEditor );

        TRect editorRect = editorLayoutText.TextRect();

        // Resize height according to actual height required by edwin.
        editorRect.Resize( 0, iSize.iHeight - editorRect.Height() );
        
        iEditor->SetRect( editorRect );
        
        // Try setting font. Failures are ignored.
        TRAP_IGNORE( iEditor->SetFontL( editorLayoutText.Font() ) );    
        }
    
    // Layout field focus
    if( iEditor )
       {
       // Layouting focus for rich text editor area
       TRect bgRect( iEditor->Rect() );
    
       // Move focus rect so that it's relative to field's position.
       bgRect.Move( -Position() );
       SetFocusRect( bgRect );   
       }

    if ( iAacListBox && iAacListBox->IsVisible ( ) )
        {
        TRAPD( error, iAacListBox->SetPopupMaxRectL( ResolvePopupRectL()) )
        if ( error )
            {
            iCoeEnv->HandleError( error );
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRAttendeeField::ResolvePopupRectL
// -----------------------------------------------------------------------------
//
TRect CESMRAttendeeField::ResolvePopupRectL( )
    {
    FUNC_LOG;
    if ( !iAacListBox )
        {
        return TRect( 0, 0, 0, 0 );
        }
    
    TRect popupRect( 0, 0, 0, 0 );
    
    // Let's determine Popup's maximum height
    TInt requiredPopupHeight = iAacListBox->CalcHeightBasedOnNumOfItems( 
            iAacListBox->Model()->NumberOfItems() );
    TInt numberOfItemsInPopUp = iAacListBox->Model()->NumberOfItems();
    
    if( numberOfItemsInPopUp > KMaxAmountOfItems )
        {
        // reducing popup item count shown at once to maximum value
        requiredPopupHeight = iAacListBox->CalcHeightBasedOnNumOfItems( 
                KMaxAmountOfItems );
        }
    
    // Popup x-coords are always the same
    TInt topLeftX = iEditor->Rect().iTl.iX;
    TInt bottomRightX = iEditor->Rect().iBr.iX;
    
    // Popup y-coords need to be calculated
    TInt topLeftY( 0 );
    TInt bottomRightY( 0 );
           
    // Next we have to resolve if popup needs to be drawn below or 
    // above the cursor (space requirement)
    
    TRect editorLineRect;
    iEditor->GetLineRectL( editorLineRect );
    TInt editorLineTopLeftY = editorLineRect.iTl.iY;
    TInt diff = iEditor->Rect().iTl.iY;
    TInt editorLineTopLeftYRelativeToParent = editorLineTopLeftY + diff;
    
    TInt listPaneHeight = Parent()->Rect().Height();
    
    // Popup will be drawn above the cursor
    if( editorLineTopLeftYRelativeToParent > TReal( listPaneHeight / 2 ) )
        {
        topLeftY = editorLineTopLeftYRelativeToParent - requiredPopupHeight;
        bottomRightY = editorLineTopLeftYRelativeToParent;
        }
    // Popup will be drawn below the cursor
    else
        {
        topLeftY = editorLineTopLeftYRelativeToParent + iEditor->GetLineHeightL();
        bottomRightY = topLeftY + requiredPopupHeight;
        }

    popupRect.SetRect( topLeftX, topLeftY, bottomRightX, bottomRightY );
    
    return popupRect;
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField:::AppendDefaultTextL()
// -----------------------------------------------------------------------------
//
void CESMRAttendeeField::AppendDefaultTextL()
    {
    FUNC_LOG;
    iEditor->SetTextL( iDefaultText );
    iEditor->HandleTextChangedL();
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::ClearDefaultTextL()
// -----------------------------------------------------------------------------
//
void CESMRAttendeeField::ClearDefaultTextL()
    {
    FUNC_LOG;
    iEditor->SetTextL( &KNullDesC );
    iEditor->HandleTextChangedL();
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRAttendeeField::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    FUNC_LOG;

    TKeyResponse ret( EKeyWasNotConsumed);

    if ( aKeyEvent.iCode == EKeyEnter || 
         aKeyEvent.iScanCode == EStdKeyEnter ||
         aKeyEvent.iCode == EKeyYes ||
         aKeyEvent.iScanCode == EStdKeyYes )
        {//prevent these keys from being used in any attendee field editor
        return EKeyWasConsumed;
        }

    if ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        if( iAacListBox && iAacListBox->IsVisible() && !iAacListBox->IsPopupEmpty() )
            {
            return iAacListBox->OfferKeyEventL( aKeyEvent, aType );
            }
        else
            {
            ret = iEditor->OfferKeyEventL (aKeyEvent, aType );
            }
        }
    else
        {
        if ( iAacListBox && iAacListBox->IsVisible ( ) )
            {
            if( aKeyEvent.iCode == EKeyDevice3 || aKeyEvent.iCode == EKeyDevice4 ||
                aKeyEvent.iCode == EKeyOK )
                {
                DoPopupSelectL();
                ret = EKeyWasConsumed;
                }
            }
        }

    if ( ret == EKeyWasNotConsumed )
        {
        ret = iEditor->OfferKeyEventL (aKeyEvent, aType );
        UpdateSendOptionL();
        }

    // Scroll editor visible, if for example cursor is out of the 
    // viewable area when user enters more text.
    if ( iObserver && aType == EEventKey )
        {
        CTextLayout* textLayout = iEditor->TextLayout();
        TInt lineHeight = iEditor->GetLineHeightL();

        TPoint cursorPos( 0,0 );
        textLayout->DocPosToXyPosL( iEditor->CursorPos(), cursorPos );

        TInt editorTlY = iEditor->Position().iY;
        TInt listHeight = iObserver->ViewableAreaRect().Height();
        
        TInt cursorTopY = cursorPos.iY - lineHeight + editorTlY;
        TInt cursorBottomY = cursorPos.iY + lineHeight + editorTlY;

        // If cursor is below visible area
        if ( cursorBottomY > listHeight )
            {
            iObserver->RePositionFields( -( cursorBottomY - listHeight ) );
            }
        // If cursor is over the top of visible area
        else if ( cursorTopY < 0 )
            {
            iObserver->RePositionFields( -cursorTopY );
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::SetContainerWindowL()
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::SetContainerWindowL( const CCoeControl& aControl )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aControl );

    iFieldButton->SetContainerWindowL( aControl );
    iFieldButton->SetParent( this );
	
    iTitle->SetContainerWindowL( aControl );
    iTitle->SetParent( this );
    
    iEditor->SetContainerWindowL( aControl );
    iEditor->SetParent( this );
        
    iAacListBox = CESMRNcsPopupListBox::NewL ( this, *iContactHandler );
    iAacListBox->MakeVisible ( EFalse );
    iAacListBox->SetListBoxObserver( this );
    iAacListBox->SetObserver( this );
    iAacListBox->ActivateL();
    
    iButtonGroupContainer = CEikButtonGroupContainer::Current();
	}

// ---------------------------------------------------------------------------
// CESMRAttendeeField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRAttendeeField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    TBool reDraw( EFalse );
    
    if( iSize != aSize )
        {
        // Let's save the required size for the iEditor
        iSize = aSize;
    
        if ( iObserver && aEdwin == iEditor )
            {
            iObserver->ControlSizeChanged ( this );
            reDraw = ETrue;
            }
        }

    return reDraw;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::UpdatePopupContactListL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::UpdatePopupContactListL( 
        const TDesC& aMatchString, 
        TBool /*iListAll*/)
    {
    FUNC_LOG;
    if ( aMatchString.CompareC( KNullDesC ) == 0 ||
            aMatchString.CompareC( *iDefaultText ) == 0 )
        {
        ClosePopupContactListL();
        }
    else if ( iAacListBox )
        {
        // Popup will be shown when async HandleControlEventL is received
        iAacListBoxEnabled = ETrue;

        if ( iAacListBox->IsVisible() )
            {
            iAacListBox->SetSearchTextL( aMatchString );
            }
        else
            {
            iAacListBox->InitAndSearchL( aMatchString );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ShowPopupCbaL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ShowPopupCbaL( TBool aShow )
    {
    if( aShow )
        {
        iButtonGroupContainer->SetCommandSetL( 
                R_CONTACT_POPUP_SOFTKEYS_SELECT_CANCEL);
        }
    else
        {
        iButtonGroupContainer->SetCommandSetL( 
                R_CONTACT_EDITOR_SOFTKEYS_OPTIONS_DONE__ADD);       
        }
    
    iButtonGroupContainer->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CESMRAttendeeField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    switch ( aCommand )
        {
        case EESMRCmdAttendeeInsertContact:
            {
    		HandleTactileFeedbackL();
            iContactHandler->GetAddressesFromPhonebookL( this );
            isUsed = ETrue;
            break;
            }
         case EESMRCmdAttendeeSoftkeySelect:
             {
             DoPopupSelectL();
             isUsed = ETrue;
             break;
             }
         case EESMRCmdAttendeeSoftkeyCancel:
             {
             ClosePopupContactListL();
             isUsed = ETrue;
             break;
             }
         case EESMRCmdLongtapDetected:
             {
             if(iEditor->IsFocused())
                 NotifyEventL(EESMRCmdLongtapDetected);
             isUsed = ETrue;
             
     		HandleTactileFeedbackL();
             break;
             }
         default:
             break;
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );

    
    if (aFocus) //Focus is gained on the field
        {
        if ( iEditor->HasDefaultText() )
            {
            iEditor->SelectAllL();
            }
        ChangeMiddleSoftKeyL( R_MR_ADD_SOFTKEY );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::OperationComplete()
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::OperationCompleteL(
        TContactHandlerCmd /*aCmd*/,
        const RPointerArray<CESMRClsItem>* aContacts )
    {
    FUNC_LOG;
    if ( aContacts )
        {
        if( aContacts->Count() > 0 )
            {
            RCPointerArray<CESMRNcsEmailAddressObject> ncsObjects;  // codescanner::resourcenotoncleanupstack        
            CleanupClosePushL( ncsObjects );
            for ( int i = 0; i < aContacts->Count(); i++ )
                {
                CESMRNcsEmailAddressObject* object =
                    CESMRNcsEmailAddressObject::NewL(
                        ( *aContacts )[i]->DisplayName(),
                        ( *aContacts )[i]->EmailAddress() );
                object->SetDisplayFull( ( *aContacts)[i]->MultipleEmails() );
                ncsObjects.Append( object );
                }

            iEditor->AppendAddressesL( ncsObjects );
            CleanupStack::PopAndDestroy( &ncsObjects );
            UpdateSendOptionL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::OperationError
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::OperationErrorL( TContactHandlerCmd /*aCmd*/,
                                          TInt /*aError*/ )
    {
    FUNC_LOG;
    //Nothing to do
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::HandleControlEventL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::HandleControlEventL(
        CCoeControl *aControl, 
        TCoeEvent aEventType )
    {
    FUNC_LOG;
    if ( aControl == iAacListBox &&
         aEventType == MCoeControlObserver::EEventStateChanged &&
         iAacListBoxEnabled )
        {
        if ( iAacListBox && iAacListBox->IsVisible() && iAacListBox->IsPopupEmpty() )
            {
            ClosePopupContactListL();
            }
        else if ( iAacListBox  && !iAacListBox->IsPopupEmpty() )
            {
            iAacListBox->SetPopupMaxRectL( ResolvePopupRectL() );
            iAacListBox->MakeVisible ( ETrue );
            ShowPopupCbaL( ETrue );
            }
        else
            {
            UpdateSendOptionL();
            }
        }
	}

// -----------------------------------------------------------------------------
// CESMRAttendeeField::DoPopupSelect
// -----------------------------------------------------------------------------
//
void CESMRAttendeeField::DoPopupSelectL()
    {
    FUNC_LOG;
    if( iAacListBox && iAacListBox->IsRemoteLookupItemSelected() )
        {
        // Launch remote lookup
        HBufC* lookupText = iEditor->GetLookupTextLC();
        CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason exitReason;
        CESMRNcsEmailAddressObject* address = ExecuteRemoteSearchL(
            exitReason,
            *lookupText );
        if ( address )
            {
            CleanupStack::PushL( address );
            iEditor->AddAddressL( *address );
            CleanupStack::PopAndDestroy( address );
            }
        CleanupStack::PopAndDestroy( lookupText );
        }
    else if( iAacListBox && !iAacListBox->IsPopupEmpty() )
        {
        CESMRNcsEmailAddressObject* emailAddress  = 
                iAacListBox->ReturnCurrentEmailAddressLC();
        if( emailAddress )
            {
            if ( emailAddress->EmailAddress().Compare( KNullDesC ))
                {
                iEditor->AddAddressL( *emailAddress );
                }
            else
                {
                // selected contact doesn't have email address, launch remote contact lookup
                // rcl must be usable, since otherwise there couldn't be any items
                // without email addresses
                CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason exitReason;
                CESMRNcsEmailAddressObject* remAddress = ExecuteRemoteSearchL(
                    exitReason,
                    emailAddress->DisplayName() );
                if ( remAddress )
                    {
                    CleanupStack::PushL( remAddress );
                    iEditor->AddAddressL( *remAddress );
                    CleanupStack::PopAndDestroy( remAddress );
                    }
                }
            CleanupStack::PopAndDestroy( emailAddress );
            }
        }
    ClosePopupContactListL();
    }

// -----------------------------------------------------------------------------
// CESMRAttendeeField::ClosePopupContactListL()
// -----------------------------------------------------------------------------
//
void CESMRAttendeeField::ClosePopupContactListL()
    {
    FUNC_LOG;
    // Disable popup if we get async HandleControlEventL later
    iAacListBoxEnabled = EFalse;

    if ( iAacListBox && iAacListBox->IsVisible() )
        {
        iAacListBox->MakeVisible( EFalse );
        AknsUtils::SetAvkonSkinEnabledL( ETrue );
        }
    
    ShowPopupCbaL( EFalse );    
    UpdateSendOptionL();
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ExecuteRemoteSearchL
// ---------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject* CESMRAttendeeField::ExecuteRemoteSearchL(
    CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason& /*aExitReason*/,
    const TDesC& aSearchText )
    {
    FUNC_LOG;
    RBuf displayname;  // codescanner::resourcenotoncleanupstack
    displayname.CreateL( KMaxRemoteSearchResponseLength );
    displayname.CleanupClosePushL();
    RBuf emailAddress;  // codescanner::resourcenotoncleanupstack
    emailAddress.CreateL( KMaxRemoteSearchResponseLength );
    emailAddress.CleanupClosePushL();
    
    // Pop-up needs to be closed before executing remote lookup with 
    // query dialog, because combination of this pop-up and any query dialog
    // causes background drawing problems with CEikMfne editors. 
    // Reason unknown.
    ClosePopupContactListL();
    
    TBool contactSelected = iContactHandler->LaunchRemoteLookupL( aSearchText,
                                                                  displayname,
                                                                  emailAddress );

    CESMRNcsEmailAddressObject* address = NULL;
    if ( contactSelected )
        {
        if ( !displayname.Length() )
            {
            address = CESMRNcsEmailAddressObject::NewL( emailAddress, emailAddress );
            }
        else
            {
            address = CESMRNcsEmailAddressObject::NewL( displayname, emailAddress );
            }
        }

    CleanupStack::PopAndDestroy( &emailAddress );
    CleanupStack::PopAndDestroy( &displayname );

    return address;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::UpdateSendOptionL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::UpdateSendOptionL()
    {
    FUNC_LOG;
    // Check if editor has text and it is different from default text.
    HBufC *text = iEditor->GetTextInHBufL();
    TBool enable = text && text->Length() > 0 && text->Compare( *iDefaultText ) != 0;
    if( enable )
    	{
        TPtr ptr = text->Des();
        ptr.Trim();        	
    	}
    enable = text && text->Length() > 0 && text->Compare( *iDefaultText ) != 0;
    delete text;

    // Send proper command to CESMREditorDialog::ProcessCommandL
    TInt command = 0;
    if ( iRole == CCalAttendee::EReqParticipant )
        {
        if ( enable )
            {
            command = EESMRCmdAttendeeRequiredEnabled;
            }
        else
            {
            command = EESMRCmdAttendeeRequiredDisabled;
            }
        }
    else if ( iRole == CCalAttendee::EOptParticipant )
        {
        if ( enable )
            {
            command = EESMRCmdAttendeeOptionalEnabled;
            }
        else
            {
            command = EESMRCmdAttendeeOptionalDisabled;
            }
        }
    
    NotifyEventL( command );
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::HandleListBoxEventL( CEikListBox* aListBox, 
                                              TListBoxEvent aEventType )
    {
    if( aEventType == EEventEnterKeyPressed || aEventType == EEventItemClicked 
            || aEventType == EEventItemDoubleClicked || aEventType == EEventItemSingleClicked )
        {
        TInt newIndex = aListBox->CurrentItemIndex();
        
        // if item is already highlighted and then clicked, 
        // it is considered that it has been selected
        if( newIndex == iPreviousIndex )
            {
			HandleTactileFeedbackL();
            DoPopupSelectL();
            
            // Item selected, index reseted
            iPreviousIndex = 0;
            }
        else
            {
            iPreviousIndex = newIndex;
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRAttendeeField::GetCursorLineVerticalPos
// -----------------------------------------------------------------------------
//
void CESMRAttendeeField::GetCursorLineVerticalPos(
        TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    TRect rect;

    TRAPD( err, iEditor->GetLineRectL( rect ) );

    if ( err == KErrNone )
        {
		rect.Move( 0, iFieldButton->Rect().Size().iHeight );
        aUpper = rect.iTl.iY;
        aLower = rect.iBr.iY;
        }
    else
        {
        // This isn't expected to happen in any situation.
        // but if it does at least there will be no zero size
        CESMRField::GetCursorLineVerticalPos( aUpper, aLower );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::HandleSingletapEventL
// ---------------------------------------------------------------------------
//
TBool CESMRAttendeeField::HandleSingletapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;
    TBool ret( EFalse );
    
    if( iTitle->Rect().Contains( aPosition ) ||
            iFieldButton->Rect().Contains( aPosition ) )
        {
        iContactHandler->GetAddressesFromPhonebookL( this );
        ret = ETrue;
        
		HandleTactileFeedbackL();
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::HandleLongtapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;
           
    if( iTitle->Rect().Contains( aPosition ) ||
            iFieldButton->Rect().Contains( aPosition ) )
        {
        NotifyEventL( EAknSoftkeyContextOptions );
		HandleTactileFeedbackL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::UpdateAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::UpdateAttendeesL( MESMRMeetingRequestEntry& aMREntry )
    {
    FUNC_LOG;
	/*
	 * Compare editor attendees to existing attendees and add / remove 
	 * when necessary.
	 */
	iEditor->CheckAddressWhenFocusLostL();
	
	CCalEntry& entry( aMREntry.Entry() );
	
	// Get editor's attendees
	RPointerArray<CESMRNcsEmailAddressObject> editorAttendees = 
			iEditor->GetAddressesL();
	TInt editorAttendeesCount( editorAttendees.Count() );
	
	// Get existing attendees
	RPointerArray<CCalAttendee>& existingAttendees = entry.AttendeesL();
	TInt existingAttendeesCount( existingAttendees.Count() );
	
	// Remove removed attendees from entry
	for( TInt i( existingAttendeesCount - 1 ); i >= 0 ; --i )
		{
		// existing address and role
		const TDesC& address = existingAttendees[i]->Address();
		CCalAttendee::TCalRole role = existingAttendees[i]->RoleL();
		
		// Let's try to find them amongst editor attendees
		TBool matchFound( EFalse );
		for( TInt j( 0 ); j < editorAttendeesCount ; ++j )
			{
			// if address match is found ...
			if( editorAttendees[j]->EmailAddress().Compare( address ) == 0 )
				{
				// ... And roles match also
				if( role == iRole )
					{
					// This attendee is left to the entry
					matchFound = ETrue;
					break;
					}
				}
			}
		if( !matchFound )
			{
			// Existing attendee not found from editor -> Let's delete that
			// from entry if roles match
			if ( existingAttendees[i]->RoleL( )== iRole )
				{
				entry.DeleteAttendeeL( i );
				}
			}
		}
	
	// Update existing attendees count, because some attendees might
	// have been removed
	existingAttendees.Reset();
	existingAttendees = entry.AttendeesL();
	existingAttendeesCount = existingAttendees.Count();
	
	// Add new attendees to entry
	for( TInt i( 0 ); i < editorAttendeesCount ; ++i )
		{
		CESMRNcsEmailAddressObject* obj = editorAttendees[i];
		CCalAttendee* attendee = CCalAttendee::NewL( obj->EmailAddress() );
		attendee->SetRoleL( iRole );
		attendee->SetCommonNameL( obj->DisplayName() );
	
		if ( EESMRRoleOrganizer == aMREntry.RoleL() || 
			 aMREntry.IsForwardedL() )
			{
			attendee->SetResponseRequested( ETrue );
			}
		
		TBool isNewAttendee( ETrue );		
		for( TInt i( 0 ); i < existingAttendeesCount; ++i )
			{
			if( existingAttendees[i]->Address().Compare( attendee->Address() ) == 0 )
				{
				if( existingAttendees[i]->RoleL() == iRole )
					{
					// Match found, this is not a new attendee
					isNewAttendee = EFalse;
					break;
					}
				}
			}
		// If this is new attendee, let's add it to entry
		if( isNewAttendee ) 
			{
			entry.AddAttendeeL( attendee );
			}
		}
    }


//EOF

