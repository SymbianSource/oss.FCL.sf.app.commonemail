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
* Description:  Meeting request location field
 *
*/

#include <calentry.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>
#include <AknLayout2ScalableDef.h>

#include "cesmrviewerlocationfield.h"
#include "cesmrrichtextviewer.h"
#include "mesmrlistobserver.h"
#include "cesmrfeaturesettings.h"
#include "nmrlayoutmanager.h"
#include "cmrimage.h"


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
    iObserver = NULL;
    delete iFieldIcon;
    delete iRichTextViewer;
    delete iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRViewerLocationField::MinimumSize()
    {
    // Let's calculate the required rect of the iRichTextViewer.
    // We will not use directly the iRichTextViewer height, because it might
    // not exist, or the height of the viewer might still be incorrect
    TRect richTextViewerRect = RichTextViewerRect();

    // We will use as minimum size the parents width 
    // but the calculated iRichTextViewers height 
    return TSize( Parent()->Size().iWidth, richTextViewerRect.Height() );
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
    
    iRichTextViewer->SetFontL( text.Font(), iLayout );

    // This is called so that theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iRichTextViewer->FocusChanged( EDrawNow );
        }
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
        iObserver->RemoveControl( FieldId() );
        }
    else
        {
        iRichTextViewer->SetTextL( &text, ETrue );
        // After setting the text, the viewer line count is known
        iLineCount = iRichTextViewer->LineCount();
        }
    iDisableRedraw = ETrue;
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
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::ConstructL( )
    {
    SetFocusType( EESMRHighlightFocus );
   
    iFieldIcon = CMRImage::NewL( KAknsIIDQgnMeetReqIndiLocation );
    iFieldIcon->SetParent( this );
    
    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    iRichTextViewer->SetEdwinSizeObserver( this );
    iRichTextViewer->SetParent( this );
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
void CESMRViewerLocationField::ExecuteGenericCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EESMRCmdClipboardCopy:
        {
        iRichTextViewer->CopyCurrentLinkToClipBoardL();
        break;
            }
        case EESMRCmdDisableWaypointIcon:
        case EESMRCmdEnableWaypointIcon:
            {
            SetWaypointIconL( aCommand == EESMRCmdEnableWaypointIcon );
            break;
            }
        case EAknSoftkeySelect:
            {
            iRichTextViewer->LinkSelectedL();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL( aFocus );
    
    iRichTextViewer->SetFocus( aFocus );

    if ( FeaturesL().FeatureSupported(
            CESMRFeatureSettings::EESMRUILocationFeatures ) )
        {

        if ( aFocus )
            {
            //Store MSK function before changing it
            CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
            if ( cba->PositionById( EESMRCmdEdit ) ==
                 CEikButtonGroupContainer::EMiddleSoftkeyPosition )
                {
                iMskCommandId = EESMRCmdEdit;
        
                }
            else if ( cba->PositionById( EESMRCmdEditLocal ) ==
                      CEikButtonGroupContainer::EMiddleSoftkeyPosition )
                {
                iMskCommandId = EESMRCmdEditLocal;
                }
            else if ( cba->PositionById( EESMRCmdCalEntryUIEdit ) ==
                      CEikButtonGroupContainer::EMiddleSoftkeyPosition )
                {
                iMskCommandId = EESMRCmdCalEntryUIEdit;
                }
            
            ChangeMiddleSoftKeyL( EAknSoftkeyContextOptions, R_QTN_MSK_OPEN );
            }
        else
            {
            switch ( iMskCommandId )
                {
                case EESMRCmdEdit:
                case EESMRCmdEditLocal:
                case EESMRCmdCalEntryUIEdit:
                    {
                    ChangeMiddleSoftKeyL( iMskCommandId, R_QTN_MSK_EDIT );
                    break;
                    }
                default:
                    {
                    CEikButtonGroupContainer* cba =
                        CEikButtonGroupContainer::Current();
                    cba->SetCommandL(
                            CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                            R_MR_SELECT_SOFTKEY );
                    cba->DrawNow();
                    break;
                    }
                }
            
            //need to tell action menu that focus has changed
            iRichTextViewer->ResetActionMenuL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::FeaturesL()
// ---------------------------------------------------------------------------
//
CESMRFeatureSettings& CESMRViewerLocationField::FeaturesL()
    {
    if ( !iFeatures )
        {
        iFeatures = CESMRFeatureSettings::NewL();
        }
    return *iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetWaypointIconL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SetWaypointIconL( TBool /*aEnabled*/ )
    {
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRViewerLocationField::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iFieldIcon )
        {
        ++count;
        }

    if ( iRichTextViewer )
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
            return iFieldIcon;
        case 1:
            return iRichTextViewer;
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
    TRect rect = Rect();

    // LAYOUTING FIELD ICON
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout = 
            NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutTextEditorIcon );
        AknLayoutUtils::LayoutImage( iFieldIcon, rect, iconLayout );
        }

    // LAYOUTING FIELD BACKGROUND
    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect( rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    // LAYOUTING FIELD TEXT VIEWER
    if( iRichTextViewer )
        {
        iRichTextViewer->SetRect( RichTextViewerRect() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRViewerLocationField::SetContainerWindowL( const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iRichTextViewer->SetContainerWindowL( aContainer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLocationField::RichTextViewerRect
// ---------------------------------------------------------------------------
//
TRect CESMRViewerLocationField::RichTextViewerRect()
    {
    TRect rect = Rect();
    
    TAknTextComponentLayout edwinLayout = NMRLayoutManager::GetTextComponentLayout( 
            NMRLayoutManager::EMRTextLayoutTextEditor );

    // Text layout rect for one line viewer
    TAknLayoutText textLayout;
    textLayout.LayoutText( rect, edwinLayout );
    TRect textLayoutRect = textLayout.TextRect();

    TRect viewerRect = textLayoutRect;
    
    // If iRichTextViewer has lines and iSize has been set, 
    // we will use iSize.iHeight as the viewers height
    if( iLineCount > 0 && iSize.iHeight > 0 )
        {
        viewerRect.SetHeight( iSize.iHeight );
        }
    // Otherwise we will use one row height as the height of the 
    // iRichTextViewer
    else
        {
        TAknLayoutRect rowLayoutRect = 
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
        viewerRect.SetHeight( rowLayoutRect.Rect().Height() );
        }
   
    return viewerRect;
    }

//EOF
