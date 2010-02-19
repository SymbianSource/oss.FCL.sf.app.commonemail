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
* Description:  Meeting Request viewer response field implementation
*
*/

#include "cesmrresponsefield.h"
#include "cesmrresponseitem.h"
#include "mesmrresponseobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrconflictpopup.h"
#include "cesmrlayoutmgr.h"
#include "nmrlayoutmanager.h"
#include "esmrhelper.h"
#include "cfsmailbox.h"

#include <esmrgui.rsg>
#include <StringLoader.h>
#include <calentry.h>
#include <caluser.h>

// DEBUG
#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace // codescanner::namespace 
    { 
    const TInt KSelectionTopic(0);
    const TInt KOnlyTwoLines( 2 );
    const TInt KFirstCheckboxRow( 2 );
    const TInt KFirstControlItemIndex( 1 );
    const TInt KTopicFieldCount( 1 );
    const TInt KFirstSelectedItem( 1 );
    }  // namespace

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//                       CESMRResponseField
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRResponseField::NewL
// -----------------------------------------------------------------------------
//
CESMRResponseField* CESMRResponseField::NewL(
        MESMRResponseObserver* aResponseObserver )
    {
    FUNC_LOG;
    CESMRResponseField* self = new (ELeave) CESMRResponseField(aResponseObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::~CESMRResponseField
// -----------------------------------------------------------------------------
//
CESMRResponseField::~CESMRResponseField()
    {
    FUNC_LOG;
    iResponseItemArray.ResetAndDestroy();
    delete iConfPopup;
    
    if( iESMRStaticAccessed ) 
        {
        iESMRStatic.Close();
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::CESMRResponseField
// -----------------------------------------------------------------------------
//
CESMRResponseField::CESMRResponseField(MESMRResponseObserver* aResponseObserver)
: iResponseObserver(aResponseObserver), iSelectionIndex( KFirstSelectedItem ),
  iESMRStaticAccessed(EFalse)
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldResponseArea );
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::ConstructL()
    {
    FUNC_LOG;
    // Construct the topic for response area
    // Topic line is without icon and text. Text is set later
    CESMRResponseItem* responseItem =
        CESMRResponseItem::NewLC( EESMRCmdUndefined, KNullDesC, EFalse );
    iResponseItemArray.AppendL( responseItem );
    CleanupStack::Pop( responseItem );

    // Create tick boxes only in non touch environment
    if ( !AknLayoutUtils::PenEnabled() )
    	{
		// Constuct the first item (Accept)
		HBufC* stringholder = StringLoader::LoadLC( R_QTN_MEET_REQ_RESPONSE_ACCEPT ,
													iEikonEnv );
		responseItem = CESMRResponseItem::NewLC( EESMRCmdAcceptMR, *stringholder, ETrue );
		iResponseItemArray.AppendL( responseItem );
		CleanupStack::Pop( responseItem );
		CleanupStack::PopAndDestroy( stringholder );
			
		// Constuct the second item (Tentative)
		stringholder = StringLoader::LoadLC( R_QTN_MEET_REQ_RESPONSE_TENTATIVE , iEikonEnv );
		responseItem = CESMRResponseItem::NewLC( EESMRCmdTentativeMR, *stringholder, ETrue );
		iResponseItemArray.AppendL( responseItem );
		CleanupStack::Pop( responseItem );
		CleanupStack::PopAndDestroy( stringholder );
	
		// Constuct the third item (Decline)
		stringholder = StringLoader::LoadLC( R_QTN_MEET_REQ_RESPONSE_DECLINE , iEikonEnv );
		responseItem = CESMRResponseItem::NewLC( EESMRCmdDeclineMR, *stringholder, ETrue );
		iResponseItemArray.AppendL( responseItem );
		CleanupStack::Pop( responseItem );
		CleanupStack::PopAndDestroy( stringholder );
    	}
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::InitializeL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::InitializeL()
    {
    FUNC_LOG;
    TAknLayoutText layout = 
    NMRLayoutManager::GetLayoutText( 
            Rect(), NMRLayoutManager::EMRTextLayoutCheckboxEditor );    
    SetFont( layout.Font() );
    if( iSelectionIndex < iResponseItemArray.Count() )
        {
        iResponseItemArray[iSelectionIndex]->SetHighlight();
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CESMRResponseField::CountComponentControls() const
    {
    FUNC_LOG;
    return iResponseItemArray.Count();
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CESMRResponseField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    return iResponseItemArray[aInd];
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::Draw
// -----------------------------------------------------------------------------
//
void CESMRResponseField::Draw(
    const TRect& /*aRect*/ ) const
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::SizeChanged
// -----------------------------------------------------------------------------
//
void CESMRResponseField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );

    // Topic field
    TAknLayoutRect topicLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );  
    AknLayoutUtils::LayoutControl(
            ControlItem( KSelectionTopic ),
            rect,
            topicLayoutRect.Color().Value(), 
            topicLayoutRect.Rect().iTl.iX, 
            topicLayoutRect.Rect().iTl.iY, 
            topicLayoutRect.Rect().iBr.iX, 
            topicLayoutRect.Rect().iBr.iY,
            topicLayoutRect.Rect().Width(), 
            topicLayoutRect.Rect().Height());    

    // Items
    // Remove one for the topic field
    TInt count( iResponseItemArray.Count() - KTopicFieldCount );
    TInt row( KFirstCheckboxRow );
    TInt controlIndex( KFirstControlItemIndex );
    TRect parentRect( Rect() );
    for( TInt i = 0; i < count; ++i )
        {        
        TAknLayoutRect choiceLayoutRect =
            NMRLayoutManager::GetFieldRowLayoutRect( parentRect, row + i );
        rect = parentRect;
        // Move the row down then it will be drawn to correct position.
        // controlIndex+1 tells the fields index.
        TInt movement = choiceLayoutRect.Rect().Height();
        rect.Move( 0, movement * ( controlIndex + i ) );
        
        AknLayoutUtils::LayoutControl(
                ControlItem( controlIndex + i ),
                rect,
                choiceLayoutRect.Color().Value(), 
                choiceLayoutRect.Rect().iTl.iX, 
                choiceLayoutRect.Rect().iTl.iY, 
                choiceLayoutRect.Rect().iBr.iX, 
                choiceLayoutRect.Rect().iBr.iY,
                choiceLayoutRect.Rect().Width(), 
                choiceLayoutRect.Rect().Height() );          
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::MinimumSize
// -----------------------------------------------------------------------------
//
TSize CESMRResponseField::MinimumSize()
    {
    TRect rect( Rect() );

    // Topic field
    TAknLayoutRect row1LayoutRect(
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    rect = row1LayoutRect.Rect();
    
    TInt rowCount( iResponseItemArray.Count() );
    TInt height( rect.Height() * rowCount );
    return TSize( Parent()->Size().iWidth, height );
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRResponseField::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse consumed( EKeyWasNotConsumed );    

    if ( aType == EEventKey && iSelectionIndex < iResponseItemArray.Count() )
        {
        if ( aKeyEvent.iScanCode == EStdKeyUpArrow  && iSelectionIndex > 1)
            {
            iResponseItemArray[iSelectionIndex]->RemoveHighlight();
            iSelectionIndex--;
            iResponseItemArray[iSelectionIndex]->SetHighlight();
            consumed = EKeyWasConsumed;
            }
         if ( aKeyEvent.iScanCode == EStdKeyDownArrow  &&
              iSelectionIndex < iResponseItemArray.Count() - 1 )
            {
            iResponseItemArray[iSelectionIndex]->RemoveHighlight();
            iSelectionIndex++;
            iResponseItemArray[iSelectionIndex]->SetHighlight();
            consumed = EKeyWasConsumed;
            }
         // If ok button is pressed and highlight is not in the topic item
         if ( aKeyEvent.iScanCode == EStdKeyDevice3
              && ItemSelectedL() )
            {
            consumed = EKeyWasConsumed;
            }
        }

    if ( consumed == EKeyWasConsumed )
        {
        DrawDeferred();
        }

    return consumed;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::FocusChanged
// -----------------------------------------------------------------------------
//
void CESMRResponseField::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    FUNC_LOG;
    // Focus received
    if ( IsFocused() && iSelectionIndex < iResponseItemArray.Count() )
        {
        // By default, highlight the first item
        iResponseItemArray[iSelectionIndex]->SetHighlight();
        DrawDeferred();
        if (iConfPopup)
            {
            iConfPopup->ShowPopup();
            }
        }
    else // Focus lost remove highlight
        {
        for( TInt i = 0; i < iResponseItemArray.Count() ; i++ )
            {
            iResponseItemArray[i]->RemoveHighlight();
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::InternalizeL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    MESMRMeetingRequestEntry* entry = NULL;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == aEntry.Type() )
        {
        entry = static_cast<MESMRMeetingRequestEntry*>(&aEntry);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    MESMRMeetingRequestEntry::TESMREntryInfo attendeeInfo = entry->EntryAttendeeInfoL();
    HBufC* stringholder;
    switch( attendeeInfo )
        {
        case MESMRMeetingRequestEntry::EESMREntryInfoOutOfDate:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_OUT_OF_DATE , iEikonEnv );
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoCancelled:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_CANCELLED , iEikonEnv );
            HandleCancelledEventItemsL( aEntry );
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoAccepted:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPOND_ACCEPTED , iEikonEnv );
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoTentativelyAccepted:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPOND_TENTATIVE , iEikonEnv );
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoDeclined:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPOND_DECLINED , iEikonEnv );
            break;
            }

       case MESMRMeetingRequestEntry::EESMREntryInfoOccursInPast:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_OCCURS_PAST , iEikonEnv );
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoConflicts:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_CONFLICT , iEikonEnv );
            iConfPopup = CESMRConflictPopup::NewL(aEntry);
            iConfPopup->ShowPopup();
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoNormal:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND , iEikonEnv );
            break;
            }

        default:
        stringholder = StringLoader::LoadLC(
                       R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND , iEikonEnv );
        break;
        }

    iResponseItemArray[0]->SetTextL( stringholder->Des() );
    // Check do we need to expand because of the long topic text ( two lines max )
    if ( iResponseItemArray[0]->ItemTextLineCount() > 1  ||
            iResponseItemArray.Count() == KOnlyTwoLines )
        {
        SizeChanged();
        }
    CleanupStack::PopAndDestroy( stringholder );
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::AddObserver
// -----------------------------------------------------------------------------
//
void CESMRResponseField::AddObserver( MESMRResponseObserver* aResponseObserver )
    {
    FUNC_LOG;
    iResponseObserver = aResponseObserver;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::SetFont
// -----------------------------------------------------------------------------
//
void CESMRResponseField::SetFont( const CFont* aFont )
    {
    FUNC_LOG;
    iFont = aFont;
    // Set font for the response items (Tpoic, Accept, Tentative, Decline )
    TInt itemCount = iResponseItemArray.Count();
    for( TInt i = 0; i < itemCount; i++ )
        {
        iResponseItemArray[i]->SetFont( aFont );
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::SetLayoutManager
// -----------------------------------------------------------------------------
//
void CESMRResponseField::SetLayoutManager( CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    iLayout = aLayout;

    // Set layoutmanager for the response items
    TInt itemCount = iResponseItemArray.Count();
    for( TInt i = 0; i < itemCount; i++ )
        {
        iResponseItemArray[i]->SetLayoutManager( aLayout );
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::ItemSelectedL
// -----------------------------------------------------------------------------
//
TBool CESMRResponseField::ItemSelectedL()
    {
    FUNC_LOG;
    TBool selected( EFalse );
    
    if ( iSelectionIndex > KSelectionTopic )
        {
        if( iSelectionIndex < iResponseItemArray.Count() )
            {
            iResponseItemArray[iSelectionIndex]->ChangeIconL( ETrue );
            }
        // Redraw to enable new checked icon in the screen
        DrawDeferred();
                
        TBool response( EFalse );
        
        TInt cmd( 0 );
        if( iSelectionIndex < iResponseItemArray.Count() )
            {
            cmd = static_cast< TInt >( iResponseItemArray[iSelectionIndex]->CommandId() );
            }
        response = iResponseObserver->Response( cmd );
        
        if ( !response )
            {
            // Cancel was selected, update the icon
            if( iSelectionIndex < iResponseItemArray.Count() )
                {
                iResponseItemArray[iSelectionIndex]->ChangeIconL( EFalse );
                }
            }
        
        selected = ETrue;
        }
    
    return selected;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::HandleCancelledEventItemsL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::HandleCancelledEventItemsL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    // Remove all other but title items from response array. Only 'Remove from Cal'
    // should be shown.
    TInt count = iResponseItemArray.Count();
    for ( TInt i = count - 1; i > 0; --i )
        {
        CESMRResponseItem* item = iResponseItemArray[i];
        iResponseItemArray.Remove( i );
        delete item;
        item = NULL;
        }

    CCalEntry& entry = aEntry.Entry();
    CCalUser* calUser = entry.PhoneOwnerL();
    TPtrC addr = ESMRHelper::AddressWithoutMailtoPrefix( calUser->Address() );
    
    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;
    
    if( iESMRStatic.MailBoxL( addr ).HasCapability( 
            EFSMBoxCapaRemoveFromCalendar ) &&
            aEntry.IsStoredL() )
        {
        // Construct the 'Remove from Calendar' item
        HBufC* stringholder = StringLoader::LoadLC(
                R_QTN_MEET_REQ_RESPONSE_REMOVE_CALENDAR_EVENT, iEikonEnv );
        CESMRResponseItem* responseItem =
            CESMRResponseItem::NewL( EESMRCmdRemoveFromCalendar, *stringholder, ETrue );
        CleanupStack::PopAndDestroy( stringholder );
        CleanupStack::PushL( responseItem );
        // Set layout manager and font for the item
        responseItem->SetLayoutManager( iLayout );
        responseItem->SetFont( iFont );
        responseItem->SetHighlight();
        User::LeaveIfError( iResponseItemArray.Append( responseItem ) );
        CleanupStack::Pop( responseItem );
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::ExecuteGenericCommandL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if ( aCommand == EAknSoftkeySelect )
        {
        ItemSelectedL();
        }
    else
        {
        CESMRField::ExecuteGenericCommandL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::SetOutlineFocusL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );
    
    if ( aFocus )
        {
        SetMiddleSoftKeyVisible( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::ControlItem
// -----------------------------------------------------------------------------
//
CCoeControl* CESMRResponseField::ControlItem( TInt aIndex )
    {
    CCoeControl* control = NULL;
    if( aIndex < iResponseItemArray.Count() )
        {
        control = static_cast<CCoeControl*>( iResponseItemArray[aIndex] );
        }
        
    return control;
    }

// EOF

