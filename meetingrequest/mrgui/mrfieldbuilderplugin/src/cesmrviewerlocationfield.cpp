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
* Description:  Meeting request location field
 *
*/

#include "cesmrviewerlocationfield.h"
#include "cesmrrichtextviewer.h"
#include "mesmrlistobserver.h"
#include "nmrlayoutmanager.h"
#include "cmrimage.h"
#include "cesmrfeaturesettings.h"
#include "emailtrace.h"
#include "cesmrglobalnote.h"
#include "cmrbutton.h"

#include <calentry.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>
#include <aknlayout2scalabledef.h>
#include <gulicon.h>
#include <stringloader.h>

// LOCAL DEFINITIONS

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::NewL
// ---------------------------------------------------------------------------
//
CESMRViewerLocationField* CESMRViewerLocationField::NewL( )
    {
    CESMRViewerLocationField* self = new (ELeave) CESMRViewerLocationField;
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::CESMRViewerLocationField
// ---------------------------------------------------------------------------
//
CESMRViewerLocationField::~CESMRViewerLocationField()
    {
    delete iFieldButton;
    delete iWaypointIcon;
    delete iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRViewerLocationField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );

    TRect fieldRect =
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect viewerRect( NMRLayoutManager::GetLayoutText(
       fieldRect,
       NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );

    // Adjust field size so that there's room for expandable editor.
    fieldRect.Resize( 0, iSize.iHeight - viewerRect.Height() );

    return fieldRect.Size();
    }


// ---------------------------------------------------------------------------
// CESMRViewerLocationField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::InitializeL()
    {
    // Setting Font for the rich text viewer
    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    iRichTextViewer->SetFontL( text.Font() );
    iRichTextViewer->ApplyLayoutChangesL();

    // This is called so that theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iRichTextViewer->FocusChanged( EDrawNow );
        }
    iRichTextViewer->SetEventQueue( iEventQueue );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::InternalizeL( MESMRCalEntry& aEntry )
    {
    TPtrC text = aEntry.Entry().LocationL( );

    // Hide this field if location is not set
    if( text.Length() == 0 )
        {
        iObserver->HideControl( FieldId() );
        }
    else
        {
        iRichTextViewer->SetTextL( &text, ETrue );
        }

    // If the geo value has set, the waypoint icon has to be shown on right
    // side of the location field
    CCalGeoValue* geoValue = aEntry.Entry().GeoValueL();
    TReal dummy;
    if( !iLocked )
    	{
    	if( geoValue && geoValue->GetLatLong( dummy, dummy ) )
    		{
    		iWaypointIcon =
    		CMRImage::NewL( NMRBitmapManager::EMRBitmapLocationWaypoint, ETrue );
    		iWaypointIcon->SetParent( this );
    		}
    	}    
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//

TKeyResponse CESMRViewerLocationField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    return iRichTextViewer->OfferKeyEventL( aEvent, aType );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::HandleEdwinSizeEventL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerLocationField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    TBool reDraw( EFalse );

    // Let's save the required size for the iRichTextViewer
    iSize = aSize;

    if ( iObserver && aEdwin == iRichTextViewer )
        {
        iObserver->ControlSizeChanged( this );
        reDraw = ETrue;
        }

    return reDraw;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::CESMRViewerLocationField()
// ---------------------------------------------------------------------------
//
CESMRViewerLocationField::CESMRViewerLocationField()
    {
    SetFieldId( EESMRFieldLocation );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::ConstructL( )
    {
	iFieldButton = CMRButton::NewL( NMRBitmapManager::EMRBitmapLocation );
    iFieldButton->SetParent( this );
    iFieldButton->SetObserver(this);

    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    CESMRField::ConstructL( iRichTextViewer ); // ownership transfered
    iRichTextViewer->SetEdwinSizeObserver( this );
    iRichTextViewer->SetParent( this );

    iFeatures = CESMRFeatureSettings::NewL();
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::ListObserverSet()
    {
    iRichTextViewer->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerLocationField::ExecuteGenericCommandL( TInt aCommand )
    {
    TBool isUsed( EFalse );
    switch ( aCommand )
        {
        case EESMRCmdClipboardCopy:
            {
            iRichTextViewer->CopyCurrentLinkToClipBoardL();
            isUsed = ETrue;
            break;
            }
        case EESMRCmdDisableWaypointIcon:
        case EESMRCmdEnableWaypointIcon:
            {
            SetWaypointIconL( aCommand == EESMRCmdEnableWaypointIcon );
            isUsed = ETrue;
            break;
            }
        case EAknSoftkeySelect:
            {
            iRichTextViewer->LinkSelectedL();
            isUsed = ETrue;

            break;
            }
        case EAknCmdOpen:
            {
            // Open command is handled only when field is locked
            if ( IsLocked() )
            	{
				HandleTactileFeedbackL();

            	CESMRGlobalNote::ExecuteL(
            			CESMRGlobalNote::EESMRUnableToEdit );
            	isUsed = ETrue;
            	}
            break;
            }
        default:
            {
            break;
            }
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL( aFocus );

    if ( aFocus )
       {
       ChangeMiddleSoftKeyL( EAknSoftkeyContextOptions, R_QTN_MSK_OPEN );
       }
   else
       {
       //need to tell action menu that focus has changed
       iRichTextViewer->ResetActionMenuL();
       }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::SetWaypointIconL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SetWaypointIconL( TBool aEnabled )
    {
    delete iWaypointIcon;
    iWaypointIcon = NULL;

    if ( aEnabled )
        {
        iWaypointIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapLocationWaypoint, ETrue );
        iWaypointIcon->SetParent( this );
        }

    // Relayout
    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRViewerLocationField::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iFieldButton )
        {
        ++count;
        }

    if ( iRichTextViewer )
        {
        ++count;
        }

    if( iWaypointIcon )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerLocationField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFieldButton;
        case 1:
            return iRichTextViewer;
        case 2:
            return iWaypointIcon;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SizeChanged( )
    {
    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect(
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect rowRect( rowLayoutRect.Rect() );

    // Layouting field icon
    if( iFieldButton )
        {
        TAknWindowComponentLayout iconLayout(
            NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutSingleRowAColumnGraphic ) );
        AknLayoutUtils::LayoutControl( iFieldButton, rowRect, iconLayout );
        }

    // Layouting waypoint icon
    if( iWaypointIcon )
        {
        TAknWindowComponentLayout iconLayout(
                NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
        AknLayoutUtils::LayoutImage( iWaypointIcon, rowRect, iconLayout );
        }

    TAknLayoutText viewerLayoutText;

    if( iWaypointIcon )
        {
        viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect,
                    NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
        }
    else
        {
        viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect,
                    NMRLayoutManager::EMRTextLayoutTextEditor );
        }

    // Layouting viewer field
    TRect viewerRect( viewerLayoutText.TextRect() );

    // Resize height according to actual height required by edwin.
    viewerRect.Resize( 0, iSize.iHeight - viewerRect.Height() );
    iRichTextViewer->SetRect( viewerRect );

    // Layouting focus
    TRect bgRect( viewerRect );

    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    // Failures are ignored.
    TRAP_IGNORE(
    		// Try setting font
    		iRichTextViewer->SetFontL( viewerLayoutText.Font() );
    		// Try applying changes
    		iRichTextViewer->ApplyLayoutChangesL();
				);
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SetContainerWindowL(
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iRichTextViewer->SetContainerWindowL( aContainer );
    iRichTextViewer->SetParent( this );
    iFieldButton->SetContainerWindowL( aContainer );
    iFieldButton->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::HandleLongtapEventL(
        const TPoint& aPosition )
    {
    if ( !iRichTextViewer->LinkSelectedL() )
        {
        HandleTapEventL( aPosition );
        }
    else
        {
        // Reset action menu after link selection
        iRichTextViewer->ResetActionMenuL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_MR_VIEWER_MENU ||
         aResourceId == R_MR_VIEWER_LOCATION_MENU )
        {
        TBool showOnMap( EFalse );

        if ( iWaypointIcon )
            {
            showOnMap = ETrue;
            }

        TBool searchFromMap = !showOnMap;

        if ( !iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            // Maps disabled
            showOnMap = EFalse;
            searchFromMap = EFalse;
            }

        aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
        aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, !searchFromMap );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::LockL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::LockL()
	{
	FUNC_LOG;
	if( IsLocked() )
		{
		return;
		}
	
	CESMRField::LockL();

	delete iWaypointIcon;
	iWaypointIcon = NULL;
	iWaypointIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapLockField, ETrue );

	iWaypointIcon->SetParent( this );
	iWaypointIcon->SetObserver( this );	 
	}

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::GetCursorLineVerticalPos
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::GetCursorLineVerticalPos(
        TInt& aUpper, TInt& aLower)
    {
    aLower = iRichTextViewer->CurrentLineNumber() * iRichTextViewer->RowHeight();
    aUpper = aLower - iRichTextViewer->RowHeight();
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::HandleControlEventL
// ---------------------------------------------------------------------------
//

void CESMRViewerLocationField::HandleControlEventL(
        CCoeControl* aControl,TCoeEvent aEventType )
    {
    if ( aControl == iFieldButton )
        {
        if( iLocked )
        	{
        	//if field is locked, iFieldButton can't be used.
        	return;
        	}
        switch ( aEventType )
            {
            // Button state changed (button was pressed)
            case EEventRequestFocus:
                {
                // Clear viewer selection
                iRichTextViewer->ClearSelectionL();
                iRichTextViewer->DrawDeferred();
        		
                // Show context menu
                HandleTactileFeedbackL();
                NotifyEventL( EAknSoftkeyContextOptions );
                }

            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::HandleSingletapEventL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerLocationField::HandleSingletapEventL( const TPoint& aPosition )
	{
	return HandleTapEventL( aPosition );
	}

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::HandletapEventL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerLocationField::HandleTapEventL( const TPoint& aPosition )
	{
	TBool ret( EFalse );

	if( iLocked )
		{
		HandleTactileFeedbackL();

		CESMRGlobalNote::ExecuteL(
			CESMRGlobalNote::EESMRUnableToEdit );

		// Field locked, let's consume the event
		ret = ETrue;
		}
	else
		{
		// Button events are handled by HandleControlEvent.
		// Tap on link is handled by rich text viewer.
		if ( !iFieldButton->Rect().Contains( aPosition )
		     && !iRichTextViewer->GetSelectedLink() )
			{
			NotifyEventL( EAknSoftkeyContextOptions );

			ret = ETrue;

			HandleTactileFeedbackL();
			}
		}

    return ret;
	}

//EOF
