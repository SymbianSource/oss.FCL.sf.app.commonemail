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
* Description:  ESMR Tracking view UI Container class
*
*/

// <cmail> custom sw help files not avilable in Cmail
//#include <fscale.hlp.hrh> //for custom_sw helps
//#include <fsmr.hlp.hrh> //for custom_sw helps
// </cmail>
#include "cesmrtrackingview.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrinternaluid.h"
#include "cesmrtitlepane.h"
#include "cesmrfield.h"
#include "mesmrcalentry.h"
#include "nmrlayoutmanager.h"
#include "cesmrtrackingviewdialog.h"

#include <eiklabel.h>
#include <avkon.hrh>
#include <magnentryui.h>
#include <stringloader.h>
#include <gulcolor.h>
#include <eikimage.h>
#include <esmrgui.rsg>
#include <akniconutils.h>
#include <eikenv.h>
#include <aknsconstants.h>
#include <aknutils.h>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <hlplch.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <calentry.h>
#include <caluser.h>
#include <gulicon.h>

#include "emailtrace.h"


namespace{
// The string format of item in listbox.
_LIT(KListItemFormat, "%d\t%S"); 

static const TInt KIconArrayCount( 4 );
static const TInt KListboxItemGranularity( 3 );
static const TInt KMaxItemStringLen (128);
}
// <cmail> Removed profiling. </cmail>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTrackingView::CESMRTrackingView()
// ---------------------------------------------------------------------------
//
CESMRTrackingView::CESMRTrackingView( MESMRCalEntry& aEntry, 
        const TInt &aReqAttendee,
        const TInt &aOptAttendee) :
    iEntry(aEntry),iReqAttendeeCount(aReqAttendee),iOptAttendeeCount(aOptAttendee)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::NewL()
// ---------------------------------------------------------------------------
//
CESMRTrackingView* CESMRTrackingView::NewL(
        MESMRCalEntry& aEntry,
        const TRect& aRect,
        const TInt &aReqAttendee,
        const TInt &aOptAttendee)
    {
    FUNC_LOG;
    CESMRTrackingView* self = new (ELeave) CESMRTrackingView( aEntry ,aReqAttendee , aOptAttendee );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::ConstructL( const TRect& aRect )
    {
    FUNC_LOG;
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgAreaMain, aRect, ETrue);

    iTitle = CESMRTitlePane::NewL();
    // Default title text is set here.

    HBufC* title ;
    switch (iEntry.Type())
        {
        case MESMRCalEntry::EESMRCalEntryTodo:
            {
            title = StringLoader::LoadLC ( R_QTN_CALENDAR_TITLE_NEW_TODO );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryMemo:
            {
            title = StringLoader::LoadLC ( R_QTN_CALENDAR_TITLE_NEW_MEMO );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryAnniversary:
            {
            title = StringLoader::LoadLC(R_QTN_CALENDAR_TITLE_NEW_ANNIVERSARY);
            break;
            }
        case MESMRCalEntry::EESMRCalEntryMeetingRequest: // Fall through
        case MESMRCalEntry::EESMRCalEntryMeeting: // Fall through
        case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
        default:
            {
            title = StringLoader::LoadLC( R_QTN_MEET_REQ_TITLE );
            break;
            }
        }

    iTitle->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    
    iESMRStatic.ConnectL();
    iMenuHandler = &iESMRStatic.ContactMenuHandlerL();

    SetRect( aRect );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::~CESMRTrackingView()
// ---------------------------------------------------------------------------
//
CESMRTrackingView::~CESMRTrackingView()
    {
    FUNC_LOG;
    if ( iMenuHandler )
        {
        iMenuHandler->Reset();
        }
    iESMRStatic.Close();
    delete iTitle;
    iList->Reset();
    delete iList;
    delete iBgContext;
    delete iAddress;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::ExternalizeL( TBool /*aForceValidation*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::InternalizeL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::LaunchViewerHelpL
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::LaunchViewerHelpL()
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::LaunchHelpL
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::LaunchHelpL( const TDesC& aContext )
    {
    FUNC_LOG;
    TUid uid = { KUidCalendarApplication };
    CArrayFix<TCoeHelpContext>* cntx = 
        new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(cntx);
    cntx->AppendL( TCoeHelpContext( uid, aContext ) );
    CleanupStack::Pop(cntx);
    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), cntx );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::SetContainerWindowL(const CCoeControl& aContainer)
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    iTitle->SetContainerWindowL( *this );
    /**
     * Somehow if list is constructed before this SetContainerWindowL is called,
     * all redraws throws KERN-EXEC 3 (the CCoeControl's iWin variable is NULL)
     */
    
    CreateTrackingListL();
    InitTrackingListL();
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::CreateTrackingList
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::CreateTrackingListL()
    {
    iList = new(ELeave) CAknSingleGraphicStyleListBox();
    iList->ConstructL(this);     
    iList->SetContainerWindowL(*this); 
    iList->SetBackground( this );
    iList->CreateScrollBarFrameL(ETrue); 
    iList->ScrollBarFrame()->SetScrollBarVisibilityL( 
            CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );
    iList->SetListBoxObserver(this);
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::InitTrackingList
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::InitTrackingListL()
    {
    RPointerArray<CCalAttendee> attendees = iEntry.Entry().AttendeesL();
    
    // Init the array of item string and send ownership to listbox
    iItemArray = new (ELeave) CDesCArrayFlat( KListboxItemGranularity ); 
    iList->Model()->SetItemTextArray( iItemArray ); 
    iList->Model()->SetOwnershipType( ELbmOwnsItemArray ); 
    
    InitStatusIconArrayL();
    
    TBuf16<KMaxItemStringLen> itemString; 
      
    if(iReqAttendeeCount > 0)
        {
        // Add the "Required" item
        TBuf<10> textRequired(_L("Required"));
        itemString.Format( KListItemFormat, ENoneIcon, &textRequired);
        static_cast<CDesCArray*>(iItemArray)->AppendL( itemString ); 
    
        // Add required attendees
        for(TInt i = 0; i < iReqAttendeeCount; i++)
            {
            TPtrC text;
            text.Set(attendees[i]->CommonName());
            HBufC* name = text.AllocL();
            itemString.Format( KListItemFormat, EAcceptIcon, name);
            static_cast<CDesCArray*>(iItemArray)->AppendL( itemString ); 
            }
        
        iIndexOfOptLabel = iReqAttendeeCount + 1;
        }
   
    if(iOptAttendeeCount > 0)
        {
        // Add the "Optional" item
        TBuf<10> textOptioal(_L("Optional"));
        itemString.Format( KListItemFormat, ENoneIcon, &textOptioal);
        static_cast<CDesCArray*>(iItemArray)->AppendL( itemString ); 
            
        // Add Optional attendees
        for(TInt i = iReqAttendeeCount; i < iReqAttendeeCount + iOptAttendeeCount; i++)
            {
            TPtrC text;
            text.Set(attendees[i]->CommonName());
            HBufC* name = text.AllocL();
            itemString.Format( KListItemFormat, EAcceptIcon, name);
            static_cast<CDesCArray*>(iItemArray)->AppendL( itemString ); 
            }
        }
    
    iList->HandleItemAdditionL();
    
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::InitStatusIconArray
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::InitStatusIconArrayL()
    {
    // Set icon array for listbox
    CArrayPtr<CGulIcon>* iconArray = new (ELeave) CAknIconArray( KIconArrayCount ); 
    CleanupStack::PushL( iconArray );
    
    // Init status Icons
    CGulIcon* acceptIcon = InitStatusIconL(NMRBitmapManager::EMRBitmapTrackingAccept);
    CGulIcon* rejectIcon = InitStatusIconL(NMRBitmapManager::EMRBitmapTrackingReject);
    CGulIcon* tentativeIcon = InitStatusIconL(NMRBitmapManager::EMRBitmapTrackingTentative);
    CGulIcon* noneIcon = InitStatusIconL(NMRBitmapManager::EMRBitmapTrackingNone);
    
    // Append status icons
    iconArray->AppendL( acceptIcon );
    iconArray->AppendL( rejectIcon );
    iconArray->AppendL( tentativeIcon );
    iconArray->AppendL( noneIcon );
    
    // set icons array to list 
    iList->ItemDrawer()->ColumnData()->SetIconArray( iconArray ); 
    CleanupStack::Pop( iconArray );  
    
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::InitStatusIcon
// ---------------------------------------------------------------------------
//
CGulIcon* CESMRTrackingView::InitStatusIconL(NMRBitmapManager::TMRBitmapId aStatus)
    {
    CFbsBitmap* bitMap( NULL );
    CFbsBitmap* bitMapMask( NULL );    
    TSize sz(0,0);
    User::LeaveIfError( 
                  NMRBitmapManager::GetSkinBasedBitmap( 
                          aStatus, 
                          bitMap, bitMapMask, sz ) ); 
    CGulIcon* icon = CGulIcon::NewL(bitMap, bitMapMask);
    return icon;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRTrackingView::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );

    if ( iTitle )
        ++count;

    if ( iList )
        ++count;

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRTrackingView::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            {
            return iTitle;
            }
        case 1:
            {
            return iList;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::SizeChanged()
    {
    FUNC_LOG;
    
    TRect rect( Rect() );
    if ( iTitle )
        {
        TAknWindowComponentLayout titleLayout =
            NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutCtrlBar );
        AknLayoutUtils::LayoutControl( iTitle, rect, titleLayout );
        }
    if ( iList )
        {
        TAknWindowComponentLayout listLayout =
            NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutContentArea );
        AknLayoutUtils::LayoutControl( iList, rect, listLayout );
        }
    if( iBgContext )
        {
        iBgContext->SetRect( rect );
        if ( &Window() )
            {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::MinimumSize()
// Gets the main pane size needed by CEikDialog to layout itself correctly
// ---------------------------------------------------------------------------
//
TSize CESMRTrackingView::MinimumSize()
    {
    FUNC_LOG;
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    return rect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRTrackingView::OfferKeyEventL(
        const TKeyEvent &aKeyEvent, 
        TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( iList )
        {
        response = iList->OfferKeyEventL( aKeyEvent, aType );
        iList->DrawDeferred();
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::Draw
// This Draw is called when background for title pane (iTitle) is drawn
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();
      
    // Draw the background for iTitle using the current skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::Draw
// This draw method draws the dialog area
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::Draw(
        CWindowGc& aGc, 
        const CCoeControl& aControl, 
        const TRect& /*aRect*/) const
    {
    FUNC_LOG;
    // Draw the background for aControl using the current skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, &aControl, aGc, aControl.Rect() );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::GetTextDrawer
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::GetTextDrawer(
        CCoeTextDrawerBase*& /*aText*/, 
        const CCoeControl* /*aControl*/) const
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::SetTitleL()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::SetTitleL( const TDesC& aTitle )
    {
    FUNC_LOG;
    iTitle->SetTextL( aTitle );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::FontSizeSettingsChanged()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::FontSizeSettingsChanged()
    {
    FUNC_LOG;
//    iList->HandleFontChange();
    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    TInt error = KErrNone;
    
    switch ( aType )
        {
        case KEikDynamicLayoutVariantSwitch:
        case KAknLocalZoomLayoutSwitch:
            {
            SizeChanged();
            break;
            }
        default:
            {
            break;
            }
        }

    if ( error != KErrNone )
        {
        iCoeEnv->HandleError( error );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingView::MopSupplyObject()
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CESMRTrackingView::MopSupplyObject( TTypeUid aId )
    {
    if ( iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    if( EEventItemClicked == aEventType )
        {
        TInt currentFocusingIndex = aListBox->CurrentItemIndex();
        // Make sure the focus item is not the req/opt 
        if(currentFocusingIndex != 0 && currentFocusingIndex != iIndexOfOptLabel)
            {
            // Tap on the focused item 
            if(currentFocusingIndex == iFocusedItemIndex)
                {
                iParentDialog->HandleListEventL();
                }
            // Focus changed
            else
                {
                TInt attendeeIndex = currentFocusingIndex;
                
                if(currentFocusingIndex > 0 && currentFocusingIndex < iIndexOfOptLabel)
                    attendeeIndex--;
                if(currentFocusingIndex > iIndexOfOptLabel)
                    attendeeIndex -= 2;

                RPointerArray<CCalAttendee> attendees = iEntry.Entry().AttendeesL();
                iAddress = attendees[attendeeIndex]->Address().AllocL();
                
                iMenuHandler->SetValueL(*iAddress, CESMRContactMenuHandler::EValueTypeEmail);
                }
            }
        else
            {
            iMenuHandler->Reset();
            }
        // Record the focused item index
        iFocusedItemIndex = currentFocusingIndex;
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CESMRTrackingView::SetParentDialog(CCoeControl* aParentDialog)
    {
    iParentDialog = (CESMRTrackingViewDialog*)aParentDialog;
    }

// EOF


