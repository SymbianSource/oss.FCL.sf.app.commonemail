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
* Description:  ESMR UI Container class
*
*/

#include "cesmrview.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrinternaluid.h"
#include "cesmrtitlepane.h"
#include "cesmrfield.h"
#include "mesmrcalentry.h"
#include "nmrlayoutmanager.h"
#include "mesmrfieldstorage.h"
#include "FreestyleEmailUiConstants.h"
#include "cesmrcaldbmgr.h"
#include "mmrfocusstrategy.h"
#include "cmrnaviarrow.h"
#include "mesmrlistobserver.h"
#include "cmrtoolbar.h"
#include "cmrattachmentindicator.h"
#include "esmrdef.h"
#include "mesmrfieldevent.h"
#include "cesmrgenericfieldevent.h"

#include <eiklabel.h>
#include <avkon.hrh>
#include <MAgnEntryUi.h>
#include <StringLoader.h>
#include <gulcolor.h>
#include <eikimage.h>
#include <esmrgui.rsg>
#include <AknIconUtils.h>
#include <eikenv.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <hlplch.h>
#include <csxhelp/cmail.hlp.hrh>
#include <touchfeedback.h>
#include <akntoolbar.h>
#include <eikcolib.h>
#include <featdiscovery.h>
#include <bldvariant.hrh>
#include <layoutmetadata.cdl.h>

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRView::CESMRView()
// ---------------------------------------------------------------------------
//
CESMRView::CESMRView( MESMRFieldStorage* aStorage,
        MESMRCalEntry& aEntry,
        MMRFocusStrategy& aFocusStrategy,
        MESMRNaviArrowEventObserver* aObserver, 
        CMRToolbar& aToolbar )
    : iStorage( aStorage ),
      iEntry( &aEntry ),
	  iFocusStrategy( aFocusStrategy ),
	  iObserver( aObserver ),
	  iToolbar( aToolbar )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRView::NewL()
// ---------------------------------------------------------------------------
//
CESMRView* CESMRView::NewL(
        MESMRFieldStorage* aStorage,
        MESMRCalEntry& aEntry,
        const TRect& aRect,
        MMRFocusStrategy& aFocusStrategy,
        CMRToolbar& aToolbar,
        MESMRNaviArrowEventObserver* aObserver )
    {
    FUNC_LOG;
    CESMRView* self = new (ELeave) CESMRView( 
            aStorage, aEntry, aFocusStrategy, aObserver, aToolbar );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRView::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRView::ConstructL( const TRect& aRect )
    {
    FUNC_LOG;
    CreateWindowL();
    SetRect( aRect );
    }

// ---------------------------------------------------------------------------
// CESMRView::~CESMRView()
// ---------------------------------------------------------------------------
//
CESMRView::~CESMRView()
    {
    FUNC_LOG;
    delete iListPane;
    delete iBgContext;
    delete iNaviArrowLeft;
    delete iNaviArrowRight;
    delete iScrollBar;
    delete iStorage;
    }

// ---------------------------------------------------------------------------
// CESMRView::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRView::ExternalizeL( TBool aForceValidation )
    {
    FUNC_LOG;
    // externalize all fields:
    iListPane->ExternalizeL( *iEntry, aForceValidation );
    }

// ---------------------------------------------------------------------------
// CESMRView::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRView::InternalizeL()
    {
    FUNC_LOG;
    iListPane->DisableSizeChange( ETrue );
    // internalize all fields:
    iListPane->InternalizeL( *iEntry );
    iListPane->DisableSizeChange( EFalse );
    iListPane->SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRView::IsControlVisible()
// ---------------------------------------------------------------------------
//
TBool CESMRView::IsControlVisible( TESMREntryFieldId aFieldId ) const
    {
    FUNC_LOG;
    return iListPane->IsControlVisible( aFieldId );
    }

// ---------------------------------------------------------------------------
// CESMRView::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRView::ProcessEditorCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case EESMRCmdPriorityHigh: // Fall through
        case EESMRCmdPriorityNormal: // Fall through
        case EESMRCmdPriorityLow:
           {
           CESMRField* field = iStorage->FieldById( EESMRFieldSubject );
           if ( field )
               {
               field->ExecuteGenericCommandL(
                       aCommand );
               iListPane->SizeChanged();
               }
           break;
           }
        case EESMRCmdAddOptAttendee:
        	{
            iListPane->ShowControl( EESMRFieldOptAttendee );
            SetControlFocusedL( EESMRFieldOptAttendee );
            break;
        	}
        case EAknCmdHelp:
        	{
        	break;
        	}
        case EMRCmdHideAttachmentIndicator:
            {
            if ( iAttachmentIndicator )
                {
                TInt pos = iControls.Find( iAttachmentIndicator );
                if (KErrNotFound != pos )
                    {
                    iControls.Remove( pos );
                    }
                    
                delete iAttachmentIndicator;
                iAttachmentIndicator = NULL;
                
                SizeChanged();
                }            
            }
            break;
        default://forward to fields
        	{
            iListPane->FocusedField()->ExecuteGenericCommandL( aCommand );
            break;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::LaunchEditorHelpL
// ---------------------------------------------------------------------------
//
void CESMRView::LaunchEditorHelpL()
    {
    FUNC_LOG;

    switch ( iEntry->Type() )
        {
        case MESMRCalEntry::EESMRCalEntryTodo:
            {
            LaunchHelpL( KFSCA_HLP_TODO_EDITOR );
        	break;
            }
        case MESMRCalEntry::EESMRCalEntryMemo:
            {
            LaunchHelpL( KFSCA_HLP_MEMO_EDITOR );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryAnniversary:
            {
            LaunchHelpL( KFSCA_HLP_ANNIVERSARY_EDITOR );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryMeeting:
            {
            LaunchHelpL( KFSCA_HLP_CALE_MEETING_EDITOR );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryMeetingRequest:
            {
            LaunchHelpL( KMEET_REQ_HLP_EDITOR );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
        default:
            {
            break;
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMRView::LaunchViewerHelpL
// ---------------------------------------------------------------------------
//
void CESMRView::LaunchViewerHelpL()
    {
    FUNC_LOG;


    switch ( iEntry->Type() )
        {
    	case MESMRCalEntry::EESMRCalEntryTodo:
    	    {
            LaunchHelpL( KFSCA_HLP_TODO_VIEWER );
            break;
    	    }
        case MESMRCalEntry::EESMRCalEntryMemo:
            {
            LaunchHelpL( KFSCA_HLP_MEMO_VIEWER );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryAnniversary:
            {
            LaunchHelpL( KFSCA_HLP_ANNI_VIEWER );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryMeeting:
            {
            LaunchHelpL( KFSCA_HLP_CALE_MEETING_VIEWER );
            break;
            }
        case MESMRCalEntry::EESMRCalEntryMeetingRequest:
            {
            MESMRMeetingRequestEntry* mrEntry =
               static_cast<MESMRMeetingRequestEntry*>( iEntry );
            
            if ( mrEntry->IsOpenedFromMail() )
                {
                if ( CFeatureDiscovery::IsFeatureSupportedL(
                        TUid::Uid( KFeatureIdFfCalMeetingRequestUi ) ) )
                    {
                    // Full functionality
                    LaunchHelpL( KFS_VIEWER_HLP_MEETING_REQ_VIEW );
                    }
                else
                    {
                    // Only viewer from Mail supported
                    LaunchHelpL( KFS_VIEWER_HLP_MR_RECEIVED );
                    }
                }
            else if ( mrEntry->RoleL() == EESMRRoleOrganizer )
                {
                LaunchHelpL( KFSCA_HLP_VIEWER_ORGANIZER );
                }
            else 
                {
                LaunchHelpL( KFSCA_HLP_VIEWER );
                }
            break;
            }
        case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
        default:
            {
            break;
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMRView::IsComponentfocused
// ---------------------------------------------------------------------------
//
TBool CESMRView::IsComponentFocused( TESMREntryFieldId aField ) const
	{
    FUNC_LOG;
	return 	FocusedField() == aField;
	}

// ---------------------------------------------------------------------------
// CESMRView::FocusedField
// ---------------------------------------------------------------------------
//
TESMREntryFieldId CESMRView::FocusedField() const
    {
    FUNC_LOG;
    TESMREntryFieldId fieldId( ( TESMREntryFieldId ) 0 );

	CESMRField* field = iListPane->FocusedField();

    if ( field )
        {
        fieldId = field->FieldId();
        }

    return fieldId;
	}

// ---------------------------------------------------------------------------
// CESMRView::ClickedOrFocusedField
// ---------------------------------------------------------------------------
//
TESMREntryFieldId CESMRView::ClickedOrFocusedField() const
    {
    FUNC_LOG;
    TESMREntryFieldId fieldId( ( TESMREntryFieldId ) 0 );

    // Get clicked item if exists
    CESMRField* field = iListPane->ClickedField();

    if ( field )
        {
        fieldId = field->FieldId();
        }
    // If doesn't, try getting focused item
    else
        {
        field = iListPane->FocusedField();
        if( field )
            {
            fieldId = FocusedField();
            }
        }

    return fieldId;
    }

// ---------------------------------------------------------------------------
// CESMRView::AdjustViewL
// ---------------------------------------------------------------------------
//
void CESMRView::AdjustViewL(
            MESMRCalEntry* aNewEntry,
            const CESMRPolicy& aNewPolicy )
    {
    iStorage->ChangePolicyL( aNewPolicy,*aNewEntry );
    iEntry = aNewEntry;
    iListPane->InitializeL();
    iListPane->ReActivateL();
    InternalizeL();
    SetControlFocusedL( EESMRFieldUnifiedEditor );
    SetTitlePaneObserver();
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRView::LaunchHelpL
// ---------------------------------------------------------------------------
//
void CESMRView::LaunchHelpL( const TDesC& aContext )
    {
    FUNC_LOG;

    CArrayFix<TCoeHelpContext>* cntx =
		new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(cntx);
    cntx->AppendL( TCoeHelpContext( KFSEmailUiUid, aContext ) );
    CleanupStack::Pop(cntx);
    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), cntx );
    }


// ---------------------------------------------------------------------------
// CESMRView::SetTitlePaneObserver
// ---------------------------------------------------------------------------
//
void CESMRView::SetTitlePaneObserver()
    {
    CESMRField* field = NULL;
    TInt fieldCount( iStorage->Count() );
    for ( TInt i(0); i < fieldCount; ++i )
        {
        field = iStorage->Field( i );
        TESMREntryFieldId fieldId( field->FieldId() );

        switch( fieldId )
            {
            case EESMRFieldSubject: //Fall through
            case EESMRFieldOccasion://Fall through
            case EESMRFieldPriority://Fall through
            case EESMRFieldDetailedSubject:
                {
                field->SetTitlePaneObserver( this );
                break;
                }
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::UpdateTitlePaneTextL
// ---------------------------------------------------------------------------
//
void CESMRView::UpdateTitlePaneTextL( const TDesC& aText )
    {
    FUNC_LOG;
    SetTitleL( aText );
    }

// ---------------------------------------------------------------------------
// CESMRView::HandleScrollEventL
// ---------------------------------------------------------------------------
//
void CESMRView::HandleScrollEventL(
    CEikScrollBar* aScrollBar,
    TEikScrollEvent aEventType )
    {
    FUNC_LOG;

    if ( !aScrollBar )
       {
       return;
       }

    TInt newPosition = aScrollBar->ThumbPosition();

    switch( aEventType )
       {
       case EEikScrollThumbDragVert: // Drag started
       case EEikScrollThumbReleaseVert: // Drag released
           {
           if( newPosition < iScrollBarThumbPosition )
               {
               iListPane->ScrollFieldsDown(
                       iScrollBarThumbPosition - newPosition );

               iScrollBarThumbPosition = newPosition;
               }

           else if( newPosition > iScrollBarThumbPosition )
               {
               iListPane->ScrollFieldsUp(
                       newPosition - iScrollBarThumbPosition );

               iScrollBarThumbPosition = newPosition;
               }
           break;
           }
       case EEikScrollPageUp:
           {
           if( newPosition < iScrollBarThumbPosition )
               {
               iListPane->ScrollFieldsDown(
                       iScrollBarThumbPosition - newPosition );

               iScrollBarThumbPosition = newPosition;
               }
           break;
           }
       case EEikScrollPageDown:
           {
           if( newPosition > iScrollBarThumbPosition )
               {
               iListPane->ScrollFieldsUp(
                       newPosition - iScrollBarThumbPosition );

               iScrollBarThumbPosition = newPosition;
               }
           break;
           }
       default:
           {
           break;
           }
       }

    DrawNow();
    }

// ---------------------------------------------------------------------------
// CESMRView::ScrollBarPositionChanged()
// ---------------------------------------------------------------------------
//
void CESMRView::ScrollBarPositionChanged( TInt aThumbPosition )
    {
    FUNC_LOG;
    iScrollBarThumbPosition = aThumbPosition;
    }

// ---------------------------------------------------------------------------
// CESMRView::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRView::SetContainerWindowL( const CCoeControl& aContainer )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );

    /**
     * Somehow if list is constructed before this SetContainerWindowL is called,
     * all redraws throws KERN-EXEC 3 (the CCoeControl's iWin variable is NULL)
     */

    iScrollBar = new (ELeave) CAknDoubleSpanScrollBar( this );
    iScrollBar->ConstructL( EFalse, this, this, CEikScrollBar::EVertical, 0 );
    iScrollBar->SetModel( &iScrollBarModel );
    iScrollBar->MakeVisible( ETrue );

    iControls.AppendL( iScrollBar );
    
    iListPane = CMRListPane::NewL(
            *this,
            *iStorage,
            iScrollBarModel,
            *iScrollBar,
            *this );

    iControls.AppendL( iListPane );
    
    iListPane->SetBackground( this );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaMain, Rect(), ETrue );

    if( iEntry )
        {
        MESMRCalDbMgr& dbMgr = iEntry->GetDBMgr();
        dbMgr.SetCurCalendarByEntryL( *iEntry );
        }

    // Physics: enable needed pointer events
    EnableDragEvents();
    ClaimPointerGrab();

    // Initialize the focus
    iFocusStrategy.InitializeFocus();

    if( iEntry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        // Static cast is safe here. We know for sure that entry is MR
        MESMRMeetingRequestEntry* mrEntry =
                static_cast<MESMRMeetingRequestEntry*>( iEntry );
        if ( mrEntry->IsOpenedFromMail() )
            {
            TAknLayoutRect naviArrowRightLayoutRect =
                        NMRLayoutManager::GetLayoutRect( Rect(),
                                NMRLayoutManager::EMRLayoutMRNaviArrowRight );
            TRect naviArrowRect( naviArrowRightLayoutRect.Rect() );
            
            iNaviArrowLeft = CMRNaviArrow::NewL(
                    this, 
                    CMRNaviArrow::EMRArrowLeft, 
            		iObserver, 
            		naviArrowRect.Size()  );
            
            iNaviArrowRight = CMRNaviArrow::NewL( 
                    this, 
                    CMRNaviArrow::EMRArrowRight, 
            		iObserver, 
            		naviArrowRect.Size() );
            
            iControls.AppendL( iNaviArrowLeft );
            iControls.AppendL( iNaviArrowRight );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRView::CountComponentControls() const
    {
    FUNC_LOG;
    return iControls.Count();
    }

// ---------------------------------------------------------------------------
// CESMRView::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRView::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    return iControls[aInd];
    }

// ---------------------------------------------------------------------------
// CESMRView::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRView::SizeChanged()
    {
    FUNC_LOG;
    
    // If list pane is not yet constructed, no point in layouting anything
    if( !iListPane )
        {
        return;
        }
    
    // Remove possible intersection with toolbar from container rect
    TRect containerRect( ContainerRect() );
    
    // Now toolbar does not intersect with view, so it must not be taken
    // into account when calculating other component layouts.
    containerRect.SetRect( TPoint( 0, 0 ), containerRect.Size() );
    
    // Get the rect of stripe.
    TAknLayoutRect stripeLayoutRect =
        NMRLayoutManager::GetLayoutRect( containerRect,
                NMRLayoutManager::EMRLayoutStripe );
    iStripeRect = stripeLayoutRect.Rect();
    
    // Layouting listpane, scroll bar and navi arrows in landscape mode
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        TAknLayoutRect listareaLayoutRect =
                NMRLayoutManager::GetLayoutRect( containerRect,
                        NMRLayoutManager::EMRLayoutListArea );
        TRect listareaRect( listareaLayoutRect.Rect() );

        // List panes default rect needs to be modified due to
        // scrollbar and calendar indication stripe
        
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // Remove stripe width from list pane width when align is R to L
            listareaRect.iBr.iX -= iStripeRect.Width();
            }
        else
            {
            // Remove stripe width from list pane width when align is L to R
            listareaRect.iTl.iX += iStripeRect.Width();
            }
        
        if( iScrollBar )
           {
           TAknLayoutRect scrollareaLayoutRect =
               NMRLayoutManager::GetLayoutRect( containerRect,
                       NMRLayoutManager::EMRLayoutScrollBar );
           TRect scrollareaRect( scrollareaLayoutRect.Rect() );
           // Scroll bar's height is always the same as listpane's height
           scrollareaRect.SetHeight( listareaRect.Height() );
           
           iScrollBar->SetRect( scrollareaRect );
           }

        iListPane->SetRect( listareaRect );
        
        // Make sure, that stripe height is the list pane height
        iStripeRect.SetHeight( iListPane->Rect().Height() );

        if(iViewMode == EESMRViewMR)
            {
            if( iNaviArrowLeft )
                {
                TAknLayoutRect naviArrowLeftLayoutRect =
                    NMRLayoutManager::GetLayoutRect( containerRect,
                            NMRLayoutManager::EMRLayoutMRNaviArrowLeft );
                TRect naviArrowLeftRect( naviArrowLeftLayoutRect.Rect() );
    
                // Left arrow needs to be moved right the amount of
                // stripe width in landscape
                naviArrowLeftRect.Move( iStripeRect.Width(), 0 );
                
                iNaviArrowLeft->SetRect( naviArrowLeftRect );
                }
    
            if( iNaviArrowRight )
                {
                TAknLayoutRect naviArrowRightLayoutRect =
                        NMRLayoutManager::GetLayoutRect( containerRect,
                                NMRLayoutManager::EMRLayoutMRNaviArrowRight );
                TRect naviArrowRightRect( naviArrowRightLayoutRect.Rect() );
    
                // Right arrow needs to be moved left the amount of
                // possible toolbar width in landscape
                TInt scrollWidth( 0 );
                if( iScrollBar )
                    {
                    scrollWidth = iScrollBar->Rect().Width();
                    }
                
                naviArrowRightRect.Move( scrollWidth, 0 );
    
                iNaviArrowRight->SetRect( naviArrowRightRect );
                }
            }
        else
            {
            SetNaviArrowStatus( EFalse, EFalse );
            }
        }

    // Layouting listpane, scroll bar and navi arrows in portrait mode
    else
        {
        TAknLayoutRect listareaLayoutRect =
               NMRLayoutManager::GetLayoutRect( containerRect,
                       NMRLayoutManager::EMRLayoutListArea );
        TRect listareaRect( listareaLayoutRect.Rect() );
        
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // Remove stripe width from list pane width when align is R to L
            listareaRect.iBr.iX -= iStripeRect.Width();
            }
        else
            {
            // Remove stripe width from list pane width when align is L to R
            listareaRect.iTl.iX += iStripeRect.Width();
            }
                
        iListPane->SetRect( listareaRect );
        
        if( iScrollBar )
           {
           TAknLayoutRect scrollareaLayoutRect =
               NMRLayoutManager::GetLayoutRect( containerRect,
                       NMRLayoutManager::EMRLayoutScrollBar );
           TRect scrollareaRect( scrollareaLayoutRect.Rect() );
           // Scroll bar's height is always the same as listpane's height
           scrollareaRect.SetHeight( iListPane->Rect().Height() );
           iScrollBar->SetRect( scrollareaRect );
           }
        
        // Make sure, that stripe height is the list pane height
        iStripeRect.SetHeight( iListPane->Rect().Height() );

        if(iViewMode == EESMRViewMR)
            {
            if( iNaviArrowLeft )
                {
                TAknLayoutRect naviArrowLeftLayoutRect =
                    NMRLayoutManager::GetLayoutRect( containerRect,
                            NMRLayoutManager::EMRLayoutMRNaviArrowLeft );
                TRect naviArrowLeftRect( naviArrowLeftLayoutRect.Rect() );
            
                // Left arrow needs to be moved right the amount of
                // stripe width in portrait
                naviArrowLeftRect.Move( iStripeRect.Width(), 0 );
                
                iNaviArrowLeft->SetRect( naviArrowLeftRect );
                }
            
            if( iNaviArrowRight )
                {
                TAknLayoutRect naviArrowRightLayoutRect =
                        NMRLayoutManager::GetLayoutRect( containerRect,
                                NMRLayoutManager::EMRLayoutMRNaviArrowRight );
                TRect naviArrowRightRect( naviArrowRightLayoutRect.Rect() );
            
                // Right arrow needs to be moved right the amount of
                // stripe width in portrait
                naviArrowRightRect.Move( iStripeRect.Width(), 0 );
                
                iNaviArrowRight->SetRect( naviArrowRightRect );
                }
            }
        else
            {
            SetNaviArrowStatus( EFalse, EFalse );
            }
        }
    
    if ( iAttachmentIndicator )
        {
        iAttachmentIndicator->SetRect( 
                CalculateAttachmentIndicatorLayout() );
        }
    
    if( iBgContext )
        {
        iBgContext->SetRect( containerRect );
        if ( &Window() )
            {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::MinimumSize()
// Gets the main pane size needed by CEikDialog to layout itself correctly
// ---------------------------------------------------------------------------
//
TSize CESMRView::MinimumSize()
    {
    FUNC_LOG;
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    return rect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRView::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRView::OfferKeyEventL(
		const TKeyEvent &aKeyEvent,
		TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( iListPane )
        {
        // Use focus strategy
        iFocusStrategy.EventOccured( MMRFocusStrategy::EFocusKeyEvent );

        TBool forwardEvent( ETrue );
        // Remove Delete hw key command if the focus is not visible
        if ( !iFocusStrategy.IsFocusVisible() )
            {
            if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
                {
                forwardEvent = EFalse;
                }
            }

        if ( forwardEvent )
            {
            response = iListPane->OfferKeyEventL( aKeyEvent, aType );
            iListPane->DrawDeferred();
            }
        }
    return response;
    }


// ---------------------------------------------------------------------------
// CESMRView::Draw
// This Draw is called when background for title pane (iTitle) is drawn
// ---------------------------------------------------------------------------
//
void CESMRView::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();

    // Draw the background for iTitle using the current skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );

    // Draw color stripe
    MESMRCalDbMgr& DbMgr = iEntry->GetDBMgr();

    TRgb stripeColor = DbMgr.GetCurCalendarColor();
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( stripeColor );
    gc.DrawRect( iStripeRect );

    gc.Reset();
    }

// ---------------------------------------------------------------------------
// CESMRView::Draw
// This draw method draws the dialog area
// ---------------------------------------------------------------------------
//
void CESMRView::Draw(
		CWindowGc& /* aGc */,
		const CCoeControl& aControl,
		const TRect& /*aRect*/) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();

    // Draw the background for aControl using the current skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, &aControl, gc, aControl.Rect() );
    }

// ---------------------------------------------------------------------------
// CESMRView::GetTextDrawer
// ---------------------------------------------------------------------------
//
void CESMRView::GetTextDrawer(
		CCoeTextDrawerBase*& /*aText*/,
		const CCoeControl* /*aControl*/) const
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRView::SetTitleL()
// ---------------------------------------------------------------------------
//
void CESMRView::SetTitleL( const TDesC& /*aTitle*/, TBool /*aDefault*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRView::SetControlFocusedL()
// ---------------------------------------------------------------------------
//
void CESMRView::SetControlFocusedL( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    iListPane->SetControlFocusedL( aField );
    }

// ---------------------------------------------------------------------------
// CESMRView::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CESMRView::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    TInt error = KErrNone;

    switch ( aType )
        {
        case  KAknsMessageSkinChange:
            {
            TRAP( error, iListPane->InitializeL() );
            break;
            }
        case KEikMessageColorSchemeChange:
            {
            TRAP( error, iListPane->InitializeL() );
            break;
            }
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
// CESMRView::MopSupplyObject()
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CESMRView::MopSupplyObject( TTypeUid aId )
    {
    FUNC_LOG;
    if ( iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
 // CESMRView::HandlePointerEventL()
 // -----------------------------------------------------------------------------
 //
 void CESMRView::HandlePointerEventL(
     const TPointerEvent &aPointerEvent )
     {
     FUNC_LOG;
     // Call focus strategy
     iFocusStrategy.EventOccured( MMRFocusStrategy::EFocusPointerEvent );

     CCoeControl::HandlePointerEventL( aPointerEvent );
     }

// ---------------------------------------------------------------------------
// CESMRView::InitialScroll()
// ---------------------------------------------------------------------------
//
void CESMRView::InitialScrollL()
    {
    FUNC_LOG;
    iListPane->InitialScrollL();
    }

// ---------------------------------------------------------------------------
// CESMRView::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CESMRView::DynInitMenuPaneL(
        TMRMenuStyle aMenuStyle,
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    // Initialize field specific options in context menu and options menu.
    // Options menu has field specific options only when focus is visible.
    if ( aMenuStyle != EMROptionsMenu
         || ( aMenuStyle == EMROptionsMenu
              && iFocusStrategy.IsFocusVisible() ) )
        {
        CESMRField* field = iStorage->FieldById( ClickedOrFocusedField() );
        field->DynInitMenuPaneL( aResourceId, aMenuPane );
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::SetNaviArrowStatus
// ---------------------------------------------------------------------------
//
void CESMRView::SetNaviArrowStatus( 
        TBool aLeftArrowVisible, 
        TBool aRightArrowVisible )
    {
    FUNC_LOG;
    if ( iNaviArrowLeft )
        {
        iNaviArrowLeft->SetNaviArrowStatus( aLeftArrowVisible );
        }
    if ( iNaviArrowRight )
        {
        iNaviArrowRight->SetNaviArrowStatus( aRightArrowVisible );
        }
    }
// ---------------------------------------------------------------------------
// CESMRView::SetViewMode
// ---------------------------------------------------------------------------
//
void CESMRView::SetViewMode(TESMRViewMode aMode)
	{
    FUNC_LOG;
	iViewMode = aMode;
	}
// ---------------------------------------------------------------------------
// CESMRView::GetViewMode
// ---------------------------------------------------------------------------
//
TESMRViewMode CESMRView::GetViewMode()
	{
    FUNC_LOG;
	return iViewMode;
	}

// ---------------------------------------------------------------------------
// CESMRView::ProcessEventL
// ---------------------------------------------------------------------------
//
void CESMRView::ProcessEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;

    if ( MESMRFieldEvent::EESMRFieldChangeEvent ==  aEvent.Type() )
        {
        TInt* fieldId = static_cast< TInt* >( aEvent.Param( 0 ) );
        
        if ( EESMRFieldViewerAttachments == *fieldId )
            {
            if ( !iAttachmentIndicator )
                {
                TRect attachmentDownloadIndicatorRect( 
                        CalculateAttachmentIndicatorLayout() );
                
                iAttachmentIndicator = CMRAttachmentIndicator::NewL( 
                        attachmentDownloadIndicatorRect, 
                        this );
    
                CFbsBitmap* bitMap( NULL );
                CFbsBitmap* bitMapMask( NULL );
                TSize size( 0,0 );
                NMRBitmapManager::GetSkinBasedBitmapLC(
                                NMRBitmapManager::EMRBitmapDownloadAttachment,
                                bitMap, bitMapMask, size );
                
                CleanupStack::Pop( 2 ); // bitMap, bitMapMask
                
                
                
                iAttachmentIndicator->SetImage( bitMap, bitMapMask );
                iControls.AppendL( iAttachmentIndicator );
                }
            
            MESMRFieldEventValue* value =
                    static_cast< MESMRFieldEventValue* >( aEvent.Param( 1 ) );
            
            iAttachmentIndicator->SetTextL( value->StringValue() );            
            iAttachmentIndicator->MakeVisible( ETrue );
            
            SizeChanged();        
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::CalculateAttachmentIndicatorLayout
// ---------------------------------------------------------------------------
//
TRect CESMRView::CalculateAttachmentIndicatorLayout()
    {
    TRect containerRect( ContainerRect() );
    

	TAknLayoutRect naviArrowLeftLayoutRect =
			NMRLayoutManager::GetLayoutRect( containerRect,
					NMRLayoutManager::EMRLayoutMRNaviArrowLeft );    
	TRect naviArrowLeftRect = naviArrowLeftLayoutRect.Rect();
	
	TAknLayoutRect naviArrowRightLayoutRect =
			NMRLayoutManager::GetLayoutRect( containerRect,
					NMRLayoutManager::EMRLayoutMRNaviArrowRight );
	TRect naviArrowRightRect = naviArrowRightLayoutRect.Rect();
	//Exchange the narrows' rect when LayoutMirrored
	if ( AknLayoutUtils::LayoutMirrored() )
		{
		naviArrowLeftRect = naviArrowRightLayoutRect.Rect();
		naviArrowRightRect= naviArrowLeftLayoutRect.Rect();
		}
	
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {                
        naviArrowLeftRect.Move( iStripeRect.Width(), 0 );

        TInt scrollWidth( 0 );
        if( iScrollBar )
            {
            scrollWidth = iScrollBar->Rect().Width();
            }
        
        naviArrowRightRect.Move( scrollWidth, 0 );    
        }
    else
        {
        // Left arrow needs to be moved right the amount of
        // stripe width in portrait
        naviArrowLeftRect.Move( iStripeRect.Width(), 0 );
        
        TRect naviArrowRightRect = naviArrowRightLayoutRect.Rect();
        
        naviArrowRightRect.Move( iStripeRect.Width(), 0 );
        }   

    // Get height for one row 
    TAknLayoutRect rowLayout = 
        NMRLayoutManager::GetLayoutRect( 
                containerRect, NMRLayoutManager::EMRLayoutTextEditorIcon );
    TRect rowRect( rowLayout.Rect() );
    
    TRect attachmentIndicatorRect;
    
    attachmentIndicatorRect.iTl = 
                TPoint( naviArrowLeftRect.iBr.iX, 
                        naviArrowLeftRect.iBr.iY - naviArrowLeftRect.Height() );    
    
    attachmentIndicatorRect.iBr = 
            TPoint(  naviArrowRightRect.iTl.iX, 
                    naviArrowLeftRect.iBr.iY );
    
    return attachmentIndicatorRect;
    }

// ---------------------------------------------------------------------------
// CESMRView::ContainerRect
// ---------------------------------------------------------------------------
//
TRect CESMRView::ContainerRect() const
    {
    // Remove possible intersection with toolbar from container rect
    TRect containerRect( PositionRelativeToScreen(), Rect().Size() );
    TRect toolbar( iToolbar.Rect() );
    if ( containerRect.Intersects( toolbar ) )
        {
        toolbar.Intersection( containerRect );
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            containerRect.SetWidth( containerRect.Width() - toolbar.Width() );
            }
        else
            {
            containerRect.SetHeight( containerRect.Height() - toolbar.Height() );
            }
        }
    // Now toolbar does not intersect with view, so it must not be taken
    // into account when calculating other component layouts.
    containerRect.SetRect( TPoint( 0, 0 ), containerRect.Size() );
    
    return containerRect;
    }

// EOF
