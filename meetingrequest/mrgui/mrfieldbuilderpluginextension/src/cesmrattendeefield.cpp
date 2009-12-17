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
* Description:  ESMR titled field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrattendeefield.h"

#include <eiklabel.h>
#include <avkon.hrh>
#include <caluser.h>
#include <calentry.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <AknsFrameBackgroundControlContext.h>
//<cmail>
#include "esmrcommands.h"
#include "CFSMailClient.h"
//</cmail>
#include "cesmrncspopuplistbox.h"
#include "cesmrncsemailaddressobject.h"
#include "mesmrlistobserver.h"
#include "cesmrborderlayer.h"
#include "cesmrcontacthandler.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrlayoutmgr.h"
// <cmail> Removed profiling. </cmail>

using namespace ESMRLayout;

//CONSTANTS

// unnamed namespace for local definitions
namespace { // codescanner::namespace

const TInt KMaxRemoteSearchResponseLength = 255;

static void RPointerArrayResetAndDestroy( TAny* aPtr )
    {
    static_cast<RPointerArray<CESMRNcsEmailAddressObject>*>( aPtr )->ResetAndDestroy();
    }

#ifdef _DEBUG

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

#endif // _DEBUG

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAttendeeField::CESMRAttendeeField
// ---------------------------------------------------------------------------
//
CESMRAttendeeField::CESMRAttendeeField( CCalAttendee::TCalRole aRole ) :
    iRole( aRole)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::~CESMRAttendeeField
// ---------------------------------------------------------------------------
//
CESMRAttendeeField::~CESMRAttendeeField( )
    {
    FUNC_LOG;
    delete iAacListBox;
    delete iTitle;
    delete iContactHandler;
    delete iDefaultText;
    delete iFrameBgContext;
    delete iPhoneOwnerAddr;
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
    SetExpandable ( );

    SetFieldId ( (iRole == CCalAttendee::EReqParticipant) ? EESMRFieldAttendee
                                                          : EESMRFieldOptAttendee );

    iTitle = new (ELeave) CEikLabel();
    iTitle->SetLabelAlignment ( CESMRLayoutManager::IsMirrored ( ) ? ELayoutAlignRight
                                                                   : ELayoutAlignLeft );

    iContactHandler = CESMRContactHandler::NewL();

    iDefaultText = StringLoader::LoadL ( R_QTN_MEET_REQ_CONTACT_FIELD );
    iEditor = new ( ELeave ) CESMRNcsAifEditor( *iContactHandler, iDefaultText );

    iEditor->SetPopupList(this );

    HBufC* label = StringLoader::LoadLC (iRole == CCalAttendee::EReqParticipant ?
            R_QTN_MEET_REQ_LABEL_REQUIRED:R_QTN_MEET_REQ_LABEL_OPT );

    iTitle->SetTextL ( *label );
    CleanupStack::PopAndDestroy (label );
    
    iBackground = AknsDrawUtils::ControlContext( this );
    
    CESMRField::ConstructL ( iEditor );
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::InitializeL()
    {
    FUNC_LOG;
    iEditor->SetFontL( iLayout->Font(iCoeEnv, iFieldId), iLayout );
    iEditor->SetAlignment ( EAknEditorAlignBidi | EAknEditorAlignCenter );
    iEditor->SetAknEditorInputMode ( EAknEditorTextInputMode );
    iEditor->SetAknEditorFlags ( EAknEditorFlagNoT9| EAknEditorFlagUseSCTNumericCharmap );
    iEditor->SetAknEditorCurrentCase ( EAknEditorLowerCase );
    iEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL ( CEikScrollBarFrame::EOff,
                                                                CEikScrollBarFrame::EOff );
    iEditor->SetEdwinSizeObserver ( this );

    iTitleSize = iLayout->FieldSize ( EESMRFieldAttendeeLabel );
    iTitle->SetFont( iLayout->Font(iCoeEnv, EESMRFieldAttendeeLabel) );
    
    iAacListBox->Initialise(iLayout);
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry ( );
    RPointerArray< CCalAttendee > attendees = entry.AttendeesL ( );
    RPointerArray< CESMRNcsEmailAddressObject > addressList; // codescanner::resourcenotoncleanupstack
    TCleanupItem arrayCleanup( RPointerArrayResetAndDestroy, &addressList );
    CleanupStack::PushL( arrayCleanup );

    for (TInt i(0); i < attendees.Count ( ); i++ )
        {
        if ( attendees[i]->RoleL() == iRole )
            {
            const TDesC& addr = attendees[i]->Address ( );
            const TDesC& commonName = attendees[i]->CommonName ( );

            CESMRNcsEmailAddressObject* obj = CESMRNcsEmailAddressObject::NewL ( commonName, addr );
            CleanupStack::PushL (obj );
            addressList.AppendL( obj );
            CleanupStack::Pop( obj );
            }
        }

    if (attendees.Count() > 0 )
        {
        if ( !iObserver->IsControlVisible(iFieldId ) )
            {
            iObserver->InsertControl(iFieldId );
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
    if ( iRole == CCalAttendee::EOptParticipant )
        {
        if ( addressList.Count() == 0 )
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
    __ASSERT_DEBUG( mrEntry, Panic(EESMRAttendeeFieldNotMeetingRequestEntry ) );

    if(iEditor->HasDefaultText())
        {
        ClearDefaultTextL();
        }

    // remove all previous attendees from the attendee list
    CCalEntry& entry = mrEntry->Entry();
    
    if ( !iPhoneOwnerAddr )
    	{
		CCalUser* phoneOwner = entry.PhoneOwnerL();
		if (phoneOwner)
			{
			iPhoneOwnerAddr = phoneOwner->Address().AllocL();
			}
    	}
    
    ClearAttendeeListL ( entry );
    ParseAttendeesL ( *mrEntry );
    
    HBufC *text = iEditor->GetTextInHBufL();
        
    //If there is no added attendees put back the default text
    if(!text)
        {
    	AppendDefaultTextL();
    	//during initialisation, SelectAllL() in setoutlinefocus gets overridden so this is needed
    	iEditor->SelectAllL();
    	}
    delete text;
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ExpandedHeight
// ---------------------------------------------------------------------------
//
TInt CESMRAttendeeField::ExpandedHeight( ) const
    {
    FUNC_LOG;
    return iTitleSize.iHeight + iExpandedSize.iHeight;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::GetMinimumVisibleVerticalArea
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    TRect rect;

    TRAPD( err, iEditor->GetLineRectL( rect ) );

    if ( err == KErrNone )
        {
        rect.Move( 0, iTitleSize.iHeight );
        aUpper = rect.iTl.iY;
        aLower = rect.iBr.iY;
        }
    else
        {
        // This isn't expected to happen in any situation.
        // but if it does at least there will be no zero size
        CESMRField::GetMinimumVisibleVerticalArea( aUpper, aLower );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRAttendeeField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = CESMRField::CountComponentControls ( );

    if ( iTitle )
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
    if ( aInd == 0 )
        {
        return iTitle;
        }
    return CESMRField::ComponentControl ( aInd );
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect = Rect ( );

    if ( CESMRLayoutManager::IsMirrored ( ) )
        {
        TPoint titlePos( rect.iBr.iX - iTitleSize.iWidth, rect.iTl.iY );
        iTitle->SetExtent ( titlePos, iTitleSize );

        TSize textSize( rect.Width ( ) - ( KIconSize.iWidth + KIconBorderMargin ),
                iExpandedSize.iHeight );
        TPoint textPos( rect.iBr.iX - KIconSize.iWidth- textSize.iWidth,
                rect.iTl.iY + iTitleSize.iHeight );
        iBorder->SetExtent ( textPos, textSize );
        }
    else
        {
        // title
        iTitle->SetExtent ( rect.iTl, iTitleSize );

        // editor (editor is wrapped inside the 'iBorder' member)
        TRect borderRect(TPoint (
                rect.iTl.iX + KIconSize.iWidth + KIconBorderMargin,
                rect.iTl.iY + iTitleSize.iHeight ), TSize (
                rect.Width ( )- KIconSize.iWidth + KIconBorderMargin - KFieldEndMargin,
                iExpandedSize.iHeight ));

        iBorder->SetRect (borderRect );
        }

    if ( iAacListBox && iAacListBox->IsVisible ( ) )
        {
        TRAPD( error, iAacListBox->SetPopupMaxRectL( ResolvePopupRectL()) )
        if ( error )
            {
            CEikonEnv::Static()->// codescanner::eikonenvstatic
                HandleError( error );
            }
        }
    
    if( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iEditor->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }    
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::PositionChanged
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::PositionChanged( )
    {
    FUNC_LOG;
    if ( iAacListBox && iAacListBox->IsVisible ( ) )
        {
        TRAPD( error, iAacListBox->SetPopupMaxRectL( ResolvePopupRectL()) )
        if ( error )
            {
            CEikonEnv::Static()->// codescanner::eikonenvstatic
                HandleError( error );
            }
        }
    
    if( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iEditor->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
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
        return TRect(0,0,0,0);
        }

    TRect popupRect = iBorder->Rect();
    TRect lineRect;
    iEditor->GetLineRectL( lineRect );
    lineRect.Move( 0, popupRect.iTl.iY );
    TRect parentRect = Parent()->Rect();

    TInt listBoxBorderHeight = iAacListBox->CalcHeightBasedOnNumOfItems( 0 );
    TInt listBoxItemHeight = iAacListBox->CalcHeightBasedOnNumOfItems( 1 )
                                - listBoxBorderHeight;

    if ( lineRect.iTl.iY > ( parentRect.Height() >> 1 ) )
        {
        // Draw popup list on top of cursor
        popupRect.iTl.iY = parentRect.iTl.iY;
        popupRect.iBr.iY = lineRect.iTl.iY;

        TInt diff = popupRect.Height() % listBoxItemHeight;
        popupRect.iTl.iY += diff - listBoxBorderHeight;
        }
    else
        {
        // Draw popup list below cursor
        popupRect.iTl.iY = lineRect.iBr.iY;
        popupRect.iBr.iY = parentRect.iBr.iY;

        TInt diff = popupRect.Height() % listBoxItemHeight;
        popupRect.iBr.iY -= diff - listBoxBorderHeight;
        }

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

    // Scroll editor.
    if ( iObserver && aType == EEventKey )
        {
        CTextLayout* textLayout = iEditor->TextLayout();
        TInt lineHeight = iEditor->GetLineHeightL();

        TPoint cursorPos( 0,0 );
        textLayout->DocPosToXyPosL( iEditor->CursorPos(), cursorPos );

        TInt editorTlY = iEditor->Position().iY;
        TInt listHeight = iObserver->ListHeight();
        
        TInt cursorTopY = cursorPos.iY - lineHeight + editorTlY;
        TInt cursorBottomY = cursorPos.iY + lineHeight + editorTlY;

        // If cursor is below visible area
        if ( cursorBottomY > listHeight )
            {
            iObserver->MoveListAreaUpL( cursorBottomY - listHeight );
            }
        // If cursor is over the top of visible area
        else if ( cursorTopY < 0 )
            {
            iObserver->MoveListAreaDownL( -cursorTopY );
            }
        // Latch on top of the screen
        else if ( textLayout->GetLineNumber( iEditor->CursorPos() ) < 2 )
            {
            iObserver->MoveListAreaDownL( iTitleSize.iHeight );
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
    CESMRField::SetContainerWindowL ( aControl );

    iEditor->ConstructL (
            this,
            KMaxAddressFieldLines,
            0,
            CEikEdwin::EAvkonEditor | CEikEdwin::EResizable |
            CEikEdwin::ENoAutoSelection |CEikEdwin::EInclusiveSizeFixed |
            CEikEdwin::ENoHorizScrolling 
            );

    iAacListBox = CESMRNcsPopupListBox::NewL ( this, *iContactHandler );
    iAacListBox->MakeVisible ( EFalse );
    iAacListBox->SetObserver( this );
    iAacListBox->ActivateL();
    iButtonGroupContainer = CEikButtonGroupContainer::Current();

    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ActivateL()
    {
    CCoeControl::ActivateL();
    
    TRect editorRect = iEditor->Rect();
    
    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrInput, editorRect, editorRect, EFalse ) ;

    iFrameBgContext->SetParentContext( iBackground );
    iEditor->SetSkinBackgroundControlContextL( iFrameBgContext );
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRAttendeeField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    iExpandedSize = aSize;
    if ( iObserver )
        {
        iObserver->ControlSizeChanged ( this );
        }
    
    if ( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iEditor->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }
    
    return ETrue;
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
    if ( aMatchString.CompareC ( KNullDesC )== 0 )
        {
        ClosePopupContactListL ( );
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
            iAacListBox->InitAndSearchL ( aMatchString );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRHeaderContainer:ShowPopupCbaL
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
// CESMRHeaderContainer:ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case EESMRCmdAttendeeInsertContact:
            {
            iContactHandler->GetAddressesFromPhonebookL( this );
            }
            break;
         case EESMRCmdAttendeeSoftkeySelect:
             {
             DoPopupSelectL();
             }
             break;
         case EESMRCmdAttendeeSoftkeyCancel:
             {
             ClosePopupContactListL();
             }
             break;
         default:
             break;
        }
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::SetOutlineFocusL
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
        RPointerArray<CESMRNcsEmailAddressObject> ncsObjects;  // codescanner::resourcenotoncleanupstack
        TCleanupItem arrayCleanup( RPointerArrayResetAndDestroy, &ncsObjects );
        CleanupStack::PushL( arrayCleanup );
        for ( int i = 0; i < aContacts->Count(); i++ )
            {
            CESMRNcsEmailAddressObject* object =
                CESMRNcsEmailAddressObject::NewL(
                    (*aContacts)[i]->DisplayName(),
                    (*aContacts)[i]->EmailAddress() );
            object->SetDisplayFull( (*aContacts)[i]->MultipleEmails() );
            ncsObjects.Append( object );
            }

        iEditor->AppendAddressesL(ncsObjects);
        CleanupStack::PopAndDestroy( &ncsObjects );
        UpdateSendOptionL();
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
        else if ( iAacListBox && !iAacListBox->IsVisible() && !iAacListBox->IsPopupEmpty() )
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

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ClearAttendeeListL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ClearAttendeeListL( CCalEntry& aEntry )
    {
    FUNC_LOG;
    RPointerArray< CCalAttendee > & att = aEntry.AttendeesL( );
    for (TInt i(0); i < att.Count( ); )
        {
        // if there is an attendee with this fields role, remove it
        if ( att[i]->RoleL( )== iRole )
            {
            aEntry.DeleteAttendeeL( i );
            }
        else
            {
            ++i;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRAttendeeField::ParseAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRAttendeeField::ParseAttendeesL(
		MESMRMeetingRequestEntry& aMREntry )
    {
    FUNC_LOG;
    iEditor->CheckAddressWhenFocusLostL ( );

    RPointerArray<CESMRNcsEmailAddressObject> attendees = iEditor->GetAddressesL ( );
    CCalEntry& entry( aMREntry.Entry() );
    TInt count = attendees.Count ( );
    
    for (TInt i=0; i<count; i++ )
        {
        CESMRNcsEmailAddressObject* obj = attendees[i];
        CCalAttendee* attendee = CCalAttendee::NewL ( obj->EmailAddress ( ) );
        attendee->SetRoleL ( iRole );
        attendee->SetCommonNameL ( obj->DisplayName ( ) );

        if ( EESMRRoleOrganizer == aMREntry.RoleL() || 
        	 aMREntry.IsForwardedL() )
            {
            attendee->SetResponseRequested( ETrue );
            }
        
        entry.AddAttendeeL ( attendee );
        if (iPhoneOwnerAddr && attendee->Address().CompareF(*iPhoneOwnerAddr) == 0)
        	{
        	entry.SetPhoneOwnerL(static_cast<CCalUser*>(attendee));
        	}
        }
        
    }

// -----------------------------------------------------------------------------
// CESMRHeaderContainer::DoPopupSelect
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
// CNcsHeaderContainer::ClosePopupContactListL()
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
        ShowPopupCbaL( EFalse );
        }
    
    UpdateSendOptionL();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::ExecuteRemoteSearchL
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

//EOF

