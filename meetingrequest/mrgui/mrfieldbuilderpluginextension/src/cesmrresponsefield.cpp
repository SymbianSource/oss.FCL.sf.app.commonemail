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
* Description:  Meeting Request viewer response field implementation
*
*/
#include "cesmrresponsefield.h"
#include "cesmrresponseitem.h"
#include "mesmrresponseobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrconflictpopup.h"
#include "nmrlayoutmanager.h"
#include "esmrhelper.h"
#include "cfsmailcommon.h"
#include "tesmrinputparams.h"
#include "cfsmailcommon.h"
#include "cfsmailbox.h"
#include "cfsmailclient.h"

#include "cesmrlistquery.h"
#include <esmrgui.rsg>
#include <StringLoader.h>
#include <caluser.h>

// DEBUG
#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace
    {
    const TInt KConflictItemIndex( 0 );
    const TInt KExtraInfoItemIndex( 1 ); //"Request is out of date" "Please respond", ect
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
: iResponseObserver(aResponseObserver), iSelectionIndex( 0 ),
  iESMRStaticAccessed(EFalse), iConflicted(EFalse)
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

    CESMRField::ConstructL( NULL );

    // Add pls resopond item to this field.
    CESMRResponseItem* responseItem =
        CESMRResponseItem::NewLC( EESMRCmdUndefined, KNullDesC, EFalse );
    responseItem->SetContainerWindowL( *this );
    iResponseItemArray.AppendL( responseItem );
    CleanupStack::Pop( responseItem );

    SetFocusType( EESMRHighlightFocus );
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
// CESMRResponseField::SizeChanged
// -----------------------------------------------------------------------------
//
void CESMRResponseField::SizeChanged()
    {
    FUNC_LOG;
    TInt count( iResponseItemArray.Count() );
    TRect parentRect( Rect() );
    TRect rect;
    for( TInt i = 0; i < count; ++i )
        {
        TAknLayoutRect choiceLayoutRect =
            NMRLayoutManager::GetFieldRowLayoutRect( parentRect, i + 1 );
        rect = parentRect;
        // Move the row down then it will be drawn to correct position.
        // controlIndex+1 tells the fields index.
        TInt movement = choiceLayoutRect.Rect().Height();
        rect.Move( 0, movement * i );

        TInt leftMargin = choiceLayoutRect.Rect().iTl.iX;
        TInt rightMargin = choiceLayoutRect.Rect().iBr.iX;
        leftMargin -= parentRect.iTl.iX;
        rightMargin -= parentRect.iTl.iX;

        AknLayoutUtils::LayoutControl(
                ComponentControl( i ),
                rect,
                choiceLayoutRect.Color().Value(),
                leftMargin,
                0,
                rightMargin,
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

    if ( aType == EEventKey )
        {
        if ( iConflicted )
            {
            if ( aKeyEvent.iScanCode == EStdKeyUpArrow  && iSelectionIndex > 0 )
                {
                iResponseItemArray[KExtraInfoItemIndex]->RemoveHighlight();
                iResponseItemArray[KConflictItemIndex]->SetHighlight();
                iSelectionIndex = KConflictItemIndex;
                consumed = EKeyWasConsumed;
                }

            if ( aKeyEvent.iScanCode == EStdKeyDownArrow  && iSelectionIndex < iResponseItemArray.Count()-1 )
                {
                iResponseItemArray[KConflictItemIndex]->RemoveHighlight();
                iResponseItemArray[KExtraInfoItemIndex]->SetHighlight();
                iSelectionIndex = KExtraInfoItemIndex;
                consumed = EKeyWasConsumed;
                }
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
    if ( iConflicted && !iPointEvent )
    	{
        // set the default value for iSelectionIndex, depend on iPreItemIndex
    	if ( PreItemIndex() <= CurrentItemIndex() )
    		{
            iSelectionIndex = 0;
    		}
    	else
    		{
            iSelectionIndex = 1;
    		}
    	}
    // Focus received
    if ( IsFocused() && iSelectionIndex < iResponseItemArray.Count() )
        {
        if( !iPointEvent )
        	{
            // By default, highlight the first item
            iResponseItemArray[iSelectionIndex]->SetHighlight();
            DrawDeferred();
        	}
        }
    else // Focus lost remove highlight
        {
        for( TInt i = 0; i < iResponseItemArray.Count() ; i++ )
            {
            iResponseItemArray[i]->RemoveHighlight();
            }
        }
    iPointEvent = EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::InternalizeL
// -----------------------------------------------------------------------------
//
void CESMRResponseField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    iEntry = &aEntry;
    iNormalResponse = EFalse;
    iRemoveResponse = EFalse;
    MESMRMeetingRequestEntry* entry = NULL;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == aEntry.Type() )
        {
        entry = static_cast<MESMRMeetingRequestEntry*>(&aEntry);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    // Judge if there is any conflicts with other existed event.
    iConflicted = entry->Conflicts();
    if ( iConflicted )
        {
        // Construct the item to show conflicts info, and insert it to the beginning of array
        CESMRResponseItem* conflictItem =
            CESMRResponseItem::NewLC( EESMRCmdUndefined, KNullDesC, EFalse );
        conflictItem->SetContainerWindowL( *this );
        iResponseItemArray.Insert( conflictItem, 0 );
        CleanupStack::Pop( conflictItem );

        HBufC* conflictString;
        conflictString = StringLoader::LoadLC(
                       R_QTN_MEET_REQ_RESPONSE_CONFLICT,
                       iCoeEnv );
        // Show prompt conflict dialog
        iConfPopup = CESMRConflictPopup::NewL(aEntry);

        if ( iResponseItemArray.Count() > 0 )
            {
            iResponseItemArray[KConflictItemIndex]->SetUnderlineL( ETrue );
            iResponseItemArray[KConflictItemIndex]->SetTextL( conflictString->Des() );
            }
        CleanupStack::PopAndDestroy( conflictString );
        }


    // Set other info, for example, "please respond", "Request is out of date",
    // "Meeting has been canceled" ......
    MESMRMeetingRequestEntry::TESMREntryInfo attendeeInfo = entry->EntryAttendeeInfoL();
    HBufC* stringholder;
    switch( attendeeInfo )
        {
        case MESMRMeetingRequestEntry::EESMREntryInfoOutOfDate:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_OUT_OF_DATE,
                           iCoeEnv );
            if( entry->IsOpenedFromMail() )
            	{
                iRemoveResponse = ETrue;
            	}
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoCancelled:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_CANCELLED,
                           iCoeEnv );
            HandleCancelledEventItemsL( aEntry );
            if( entry->IsOpenedFromMail() )
            	{
            	iRemoveResponse = ETrue;
            	}
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoAccepted:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPOND_ACCEPTED,
                           iCoeEnv );
            iNormalResponse = ETrue;
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoTentativelyAccepted:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPOND_TENTATIVE,
                           iCoeEnv );
            iNormalResponse = ETrue;
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoDeclined:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPOND_DECLINED,
                           iCoeEnv );
            iNormalResponse = ETrue;
            break;
            }

       case MESMRMeetingRequestEntry::EESMREntryInfoOccursInPast:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_OCCURS_PAST,
                           iCoeEnv );
            if( entry->IsOpenedFromMail() )
            	{
            	iRemoveResponse = ETrue;
            	}
            break;
            }

        case MESMRMeetingRequestEntry::EESMREntryInfoNormal:
            {
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND,
                           iCoeEnv );
            iNormalResponse = ETrue;
            break;
            }

        default:
            stringholder = StringLoader::LoadLC(
                           R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND,
                           iCoeEnv );
            iNormalResponse = ETrue;
        break;
        }

    TInt repondItemIndex( 0 );
    if ( iConflicted && iResponseItemArray.Count() == 2 )
        {
        repondItemIndex = 1;
        }

    iResponseItemArray[repondItemIndex]->SetUnderlineL( ETrue );
    iResponseItemArray[repondItemIndex]->SetTextL( *stringholder );

    SizeChanged();
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
    // Set font for the response items (conflict and extrainfo item )
    TInt itemCount = iResponseItemArray.Count();
    for( TInt i = 0; i < itemCount; i++ )
        {
        iResponseItemArray[i]->SetFont( aFont );
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
    TInt ret( KErrCancel );

    if ( iConflicted )
        {
        if ( iSelectionIndex == 0 )
            {
            if ( iConfPopup )
                {
                iConfPopup->ShowPopup();
                }
            }

        if ( KExtraInfoItemIndex == iSelectionIndex )
            {
            if ( iNormalResponse )
                {
                // Try to send response
            	ret = ShowQueryL( CESMRListQuery::EESMRNormalResponseQuery );
                }
            if ( iRemoveResponse )
                {
                if ( AllowedRemoveMrFromCalL() )
                    {
                    ret = ShowQueryL( CESMRListQuery::EESMRRemoveResponseQuery );
                    }
                else
                    {
                    ret = ShowQueryL( CESMRListQuery::EESMRDeleteEventResponseQuery );
                    }
                }
            }
        }
    else
        {
        if ( KConflictItemIndex == iSelectionIndex )
            {
            if ( iNormalResponse )
                {
                // Try to send response
				ret = ShowQueryL(
						CESMRListQuery::EESMRNormalResponseQuery );
                }
            if ( iRemoveResponse )
                {
                if ( AllowedRemoveMrFromCalL() )
                    {
                    ret = ShowQueryL( CESMRListQuery::EESMRRemoveResponseQuery );
                    }
                else
                    {
                    ret = ShowQueryL( CESMRListQuery::EESMRDeleteEventResponseQuery );
                    }
                }
            }
        }

    if ( KErrCancel != ret )
        {
        NotifyEventAsyncL( ret );
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
    CCalEntry& entry = aEntry.Entry();
    CCalUser* calUser = entry.PhoneOwnerL();
    TPtrC addr = ESMRHelper::AddressWithoutMailtoPrefix( calUser->Address() );

    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;
    }

// -----------------------------------------------------------------------------
// CESMRResponseField::ExecuteGenericCommandL
// -----------------------------------------------------------------------------
//
TBool CESMRResponseField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );

    switch ( aCommand )
        {
        case EAknSoftkeySelect:
        case EAknCmdOpen:
            {
            ItemSelectedL();
            isUsed = ETrue;
            break;
            }
        default:
            {
            isUsed = CESMRField::ExecuteGenericCommandL( aCommand );
            break;
            }
        }

    return isUsed;
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
// CESMRResponseField::HandleSingletapEventL
// -----------------------------------------------------------------------------
//
TBool CESMRResponseField::HandleSingletapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;

    TBool handled( EFalse );
    if( !iLongTapEventConsumed )
        {
        handled = HandleTapEventL( aPosition );
        }
    iLongTapEventConsumed = EFalse;

    return handled;
    }

// ---------------------------------------------------------------------------
// CESMRResponseField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CESMRResponseField::HandleLongtapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;
    HandleTapEventL( aPosition );
    }

// ---------------------------------------------------------------------------
// CESMRResponseField::HandletapEventL
// ---------------------------------------------------------------------------
//
TBool CESMRResponseField::HandleTapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;

    TBool handled( EFalse );
    TInt ret( KErrCancel );

    if ( Rect().Contains( aPosition ) )
        {
		HandleTactileFeedbackL();
        if ( iConflicted )
            {
            TRect conflictItemRect = iResponseItemArray[KConflictItemIndex]->Rect();
            if ( conflictItemRect.Contains( aPosition ) )
                {
                iResponseItemArray[KExtraInfoItemIndex]->RemoveHighlight();
                iResponseItemArray[KConflictItemIndex]->RemoveHighlight();
                iSelectionIndex = KConflictItemIndex;
                 if ( iConfPopup )
                    {
                    iLongTapEventConsumed = ETrue;
                    iConfPopup->ShowPopup();
                    }
                }
            else
                {
                iResponseItemArray[KConflictItemIndex]->RemoveHighlight();
                iResponseItemArray[KExtraInfoItemIndex]->RemoveHighlight();
                iSelectionIndex = KExtraInfoItemIndex;
                DrawDeferred();
                if ( iNormalResponse )
                    {
					ret = ShowQueryL(
							CESMRListQuery::EESMRNormalResponseQuery );
                    // Use this flag to avoid the same event be handled by
                    // HandleSingletapEventL() when HandleLongtapEventL().
                    iLongTapEventConsumed = ETrue;
                    }
                if ( iRemoveResponse )
                    {
                    if ( AllowedRemoveMrFromCalL() )
                        {
                        ret = ShowQueryL( CESMRListQuery::EESMRRemoveResponseQuery );
                        }
                    else
                        {
                        ret = ShowQueryL( CESMRListQuery::EESMRDeleteEventResponseQuery );
                        }
                    iLongTapEventConsumed = ETrue;
                    }
                }
            }
        else
            {
            iResponseItemArray[0]->RemoveHighlight();
            if ( iNormalResponse )
                {
				ret = ShowQueryL(
						CESMRListQuery::EESMRNormalResponseQuery );
                iLongTapEventConsumed = ETrue;
                }

            if ( iRemoveResponse )
                {
                if ( AllowedRemoveMrFromCalL() )
                    {
                    ret = ShowQueryL( CESMRListQuery::EESMRRemoveResponseQuery );
                    }
                else
                    {
                    ret = ShowQueryL( CESMRListQuery::EESMRDeleteEventResponseQuery );
                    }

                iLongTapEventConsumed = ETrue;
                }
            }

        DrawDeferred();

        handled = ETrue;
        iPointEvent = ETrue;

        if ( KErrCancel != ret )
            {
            NotifyEventAsyncL( ret );
            }

        }

    return handled;
    }

// ---------------------------------------------------------------------------
// CESMRResponseField::HandleRawPointerEventL
// ---------------------------------------------------------------------------
//
TBool CESMRResponseField::HandleRawPointerEventL(
        const TPointerEvent& aPointerEvent )
	{
    FUNC_LOG;

    if( aPointerEvent.iType == TPointerEvent::EButton1Down )
    	{
    	TInt count( iResponseItemArray.Count() );

    	for( TInt i = 0; i < count; ++i )
    		{
    		if( iResponseItemArray[i]->Rect().Contains(
    				aPointerEvent.iPosition ) )
    			{
    			iResponseItemIndex = i;
    			iResponseItemArray[i]->SetHighlight();
    			DrawDeferred();
    			}
    		}
    	}

    // If iResponseItemIndex is valid
    if(  iResponseItemIndex >= 0 &&
    		iResponseItemIndex <= iResponseItemArray.Count() )
    	{
    	// If event type is Up or drag outside response items rect ...
		if( ( aPointerEvent.iType == TPointerEvent::EDrag &&
				!( iResponseItemArray[iResponseItemIndex]->Rect().Contains(
						aPointerEvent.iPosition ) ) ) ||
				aPointerEvent.iType == TPointerEvent::EButton1Up )
			{
			// ... highlight is removed.
			iResponseItemArray[iResponseItemIndex]->RemoveHighlight();
			DrawDeferred();
			}
    	}

    // This function does not consume the pointer event
    return EFalse;
	}

// ---------------------------------------------------------------------------
// CESMRResponseField::ShowQueryL
// ---------------------------------------------------------------------------
//
TInt CESMRResponseField::ShowQueryL(
		const CESMRListQuery::TESMRListQueryType aType ) const
    {
    FUNC_LOG;
    TInt ret( KErrCancel );

    CESMRListQuery* query =
            CESMRListQuery::NewL( aType );
    CleanupStack::PushL( query );
    ret = query->ExecuteLD();
    CleanupStack::Pop( query );

    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRResponseField::AllowedRemoveMrFromCalL
// ---------------------------------------------------------------------------
//
TBool CESMRResponseField::AllowedRemoveMrFromCalL()
	{
    TBool syncEnable( EFalse );

    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry->Type() )
        {
        MESMRMeetingRequestEntry* mrEntry =
                static_cast<MESMRMeetingRequestEntry*>( iEntry );

        TESMRInputParams esmrParams;
        if ( mrEntry->IsOpenedFromMail() &&
                mrEntry->StartupParameters( esmrParams ) )
            {
            TFSMailMsgId mailboxId(
                    esmrParams.iMailMessage->GetMailBoxId() );

            CFSMailBox* mailbox =
                    esmrParams.iMailClient->GetMailBoxByUidL( mailboxId );
            CleanupStack::PushL( mailbox );

            if ( mailbox->HasCapability( EFSMBoxCapaSupportsSync ) )
                {
                syncEnable = ETrue;
                }
            CleanupStack::PopAndDestroy( mailbox );
            mailbox = NULL;
            }
        }

    return syncEnable && iEntry->IsStoredL();
	}
// EOF

