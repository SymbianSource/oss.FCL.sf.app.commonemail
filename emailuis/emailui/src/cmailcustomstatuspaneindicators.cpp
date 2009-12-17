/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Window which draws priority and followup flags on navi
* pane area. Real navi pane is not used because there was problems to
* show editor indicator in custom navi pane. 
*
*/

// SYSTEM INCLUDES
#include <AknUtils.h>
#include <aknnavide.h> // CAknNavigationDecorator
#include <eikspane.h>  // CEikStatusPane

#include <aknlayoutscalable_apps.cdl.h>

// INTERNAL INCLUDES
#include "cmailcustomstatuspaneindicators.h"
#include "ncsconstants.h"	// TMsgPriority
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiAppui.h"
#include "freestyleemailui.mbg"

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::CCustomStatuspaneIndicators
// ---------------------------------------------------------------------------
//
CCustomStatuspaneIndicators* CCustomStatuspaneIndicators::NewL( CEikStatusPane* aStatusPane )
    {
    CCustomStatuspaneIndicators* self = new (ELeave) CCustomStatuspaneIndicators;
    CleanupStack::PushL( self );
    self->ConstructL( aStatusPane );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::CCustomStatuspaneIndicators
// ---------------------------------------------------------------------------
//
CCustomStatuspaneIndicators::CCustomStatuspaneIndicators()
{
}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::~CCustomStatuspaneIndicators
// ---------------------------------------------------------------------------
//
CCustomStatuspaneIndicators::~CCustomStatuspaneIndicators()
{
    delete iPriorityIconLow;
    delete iPriorityIconHigh;
    delete iBitmapFollowUp;
    delete iBitmapFollowUpMask;
    delete iBitmapFollowUpComplete;
    delete iBitmapFollowUpCompleteMask;
    delete iFollowUpIcon;
}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::ConstructL
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::ConstructL( CEikStatusPane* aStatusPane )
{
	iStatusPane = aStatusPane;

	// We use normal window instead of navigation decorator because
	// Editor indicator cannot be shown with custom navi pane.
	// This transparent window is drawn on top of navi pane.
	CreateWindowL();
	SetNonFocusing();
	
	// Read icon graphics
	ConstructControlsL();
	DoSetRectL();
	
    Window().SetTransparencyAlphaChannel();
	Window().SetOrdinalPosition( 0 );
	ActivateL();
	
	HideStatusPaneIndicators();
    }

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::MinimumSize
// ---------------------------------------------------------------------------
//

TSize CCustomStatuspaneIndicators::MinimumSize()
    {
    return TSize( Rect().Width(), Rect().Height() );
    }

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CCustomStatuspaneIndicators::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iPriorityIconLow )
        {
        ++count;
        }
    if ( iPriorityIconHigh )
        {
        ++count;
        }
    if ( iFollowUpIcon )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CCustomStatuspaneIndicators::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
   			return iPriorityIconLow;
        case 1:
        	return iPriorityIconHigh;
        case 2:
            return iFollowUpIcon;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::SizeChanged
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::SizeChanged()
	{
	TRect rect = Rect();
	
    // icon area
    TAknLayoutRect naviPane;
    naviPane.LayoutRect( rect,
            AknLayoutScalable_Apps::navi_pane_mv_g2( 3 ) );
    TRect naviPaneRect( naviPane.Rect() );
    
    // icon size
	CFreestyleEmailUiAppUi* fsAppUi =
	            static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    TSize iconSize = fsAppUi->LayoutHandler()->statusPaneIconSize();
	
    // calculate margins
    TInt marginX( Abs( naviPaneRect.Width() - iconSize.iWidth ) / 2 );
    TInt marginY( Abs( naviPaneRect.Height() - iconSize.iHeight ) / 2 );

    // adjust for scroll arrows
    if ( CheckScrollButtonState() )
        {
        TAknLayoutRect leftScrollPane;
        leftScrollPane.LayoutRect( rect,
                AknLayoutScalable_Apps::navi_pane_g2( 5 ) );
        
        marginX -= leftScrollPane.Rect().Width();
        }
        
	if( iPriorityIconLow )
		{
		AknLayoutUtils::SAknLayoutRect layout;
		layout.iC=ELayoutEmpty;
		layout.iL=naviPaneRect.iTl.iX + marginX;   // X
		layout.iT=naviPaneRect.iTl.iY + marginY;   // Y
		layout.iR=ELayoutEmpty; 			       // r-border
		layout.iB=ELayoutEmpty; 			       // y-border
		layout.iW=iconSize.iWidth;   			   // Width
		layout.iH=iconSize.iHeight; 			   // Height
    
		AknLayoutUtils::LayoutImage( iPriorityIconLow, rect, layout );
		}
	if( iPriorityIconHigh )
		{
		AknLayoutUtils::SAknLayoutRect layout;
		layout.iC=ELayoutEmpty;
		layout.iL=naviPaneRect.iTl.iX + marginX;
		layout.iT=naviPaneRect.iTl.iY + marginY;
		layout.iR=ELayoutEmpty;
		layout.iB=ELayoutEmpty;
		layout.iW=iconSize.iWidth;
		layout.iH=iconSize.iHeight;
    
		AknLayoutUtils::LayoutImage( iPriorityIconHigh, rect, layout );
		}
	if( iFollowUpIcon )
		{
		AknLayoutUtils::SAknLayoutRect layout;
		layout.iC=ELayoutEmpty;
		layout.iL=iconSize.iWidth + naviPaneRect.iTl.iX + marginX;
		layout.iT=naviPaneRect.iTl.iY + marginY;
		layout.iR=ELayoutEmpty;
		layout.iB=ELayoutEmpty;
		layout.iW=iconSize.iWidth;
		layout.iH=iconSize.iHeight;
    
		AknLayoutUtils::LayoutImage( iFollowUpIcon, rect, layout );
		}
	}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    
    switch ( aType )
        {
        case KAknsMessageSkinChange:
        case KEikMessageColorSchemeChange:
            {
            TRAP_IGNORE(ConstructControlsL());
            }
        case KEikDynamicLayoutVariantSwitch:
        case KAknLocalZoomLayoutSwitch:
            {
            // Get new navipane rect
            TRAP_IGNORE( DoSetRectL() );
            SizeChanged();
            break;
            }
        default:
            {
            break;
            }
        }
    }
	
// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::SetPriorityFlag
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::SetPriorityFlag( TMsgPriority aFlagType )
	{
    if ( iPriorityIconHigh && iPriorityIconLow )
    	{
    	switch ( aFlagType )
    		{
    		case EMsgPriorityLow:
    			{
    			iPriorityIconLow->MakeVisible( ETrue );
    			iPriorityIconHigh->MakeVisible( EFalse );
    			break;
    			}
    		case EMsgPriorityHigh:
    			{
    			iPriorityIconLow->MakeVisible( EFalse );
    			iPriorityIconHigh->MakeVisible( ETrue );
    			break;
    			}
    		default: // EMsgPriorityNormal
    			{
    			// Normal priority -> icons not visible
    			iPriorityIconLow->MakeVisible( EFalse );
    			iPriorityIconHigh->MakeVisible( EFalse );
    			}
    		}
    	DrawNow();
    	}
	}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::SetFollowUpFlag
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::SetFollowUpFlag( CCustomStatuspaneIndicators::TFollowUpFlagType aFlag )
	{
	CFreestyleEmailUiAppUi* fsAppUi =
	            static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    TSize iconSize = fsAppUi->LayoutHandler()->statusPaneIconSize();

    if ( iFollowUpIcon )
    	{
    	switch (aFlag)
    	    {
    	    case EFollowUp:
                {
                AknIconUtils::SetSize( iBitmapFollowUp, iconSize, EAspectRatioPreserved  );
                iFollowUpIcon->SetPicture(iBitmapFollowUp, iBitmapFollowUpMask);
                iFollowUpIcon->SetBrushStyle( CWindowGc::ENullBrush );
                iFollowUpIcon->SetSize( iconSize );
                }
                break;
            case EFollowUpComplete:
                {
                AknIconUtils::SetSize( iBitmapFollowUpComplete, iconSize, EAspectRatioPreserved  );
                iFollowUpIcon->SetPicture(iBitmapFollowUpComplete, iBitmapFollowUpCompleteMask);
                iFollowUpIcon->SetBrushStyle( CWindowGc::ENullBrush );
                iFollowUpIcon->SetSize( iconSize );
                }
                break;
            case EFollowUpNone:
                {
                
                }
                break;
    	    }
    	iFollowUpIcon->MakeVisible( aFlag != EFollowUpNone );
    	DrawDeferred();
    	}
	}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::HideStatusPaneIndicatorsL
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::HideStatusPaneIndicators()
	{
	MakeVisible( EFalse );
    SetPriorityFlag( EMsgPriorityNormal );
    SetFollowUpFlag( EFollowUpNone );
    DrawNow();
	}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::ShowStatusPaneIndicatorsL
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::ShowStatusPaneIndicators()
	{
	MakeVisible( ETrue );
    TRAP_IGNORE( DoSetRectL() );
    SetPriorityFlag( EMsgPriorityNormal );
    SetFollowUpFlag( EFollowUpNone );
    DrawNow();
	}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::CheckScrollButtonState
// ---------------------------------------------------------------------------
//
TBool CCustomStatuspaneIndicators::CheckScrollButtonState()
	{
	CAknNavigationControlContainer* NaviPane = NULL;
	TRAP_IGNORE( NaviPane =	(CAknNavigationControlContainer*)
			iStatusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
	CAknNavigationDecorator* NaviDecorator = NaviPane->Top();
	if ( NaviDecorator )
		{
		return NaviDecorator->ScrollButtonVisible();
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::DoSetRectL
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::DoSetRectL()
    {
    TRect rect = iStatusPane->PaneRectL( TUid::Uid( EEikStatusPaneUidNavi ) );
    
    // adjust for scroll arrows (to avoid overlapping the scrolling arrows)
    if ( CheckScrollButtonState() )
        {
        TAknLayoutRect leftScrollPane;
        leftScrollPane.LayoutRect( rect,
                AknLayoutScalable_Apps::navi_pane_g2( 5 ) );
        
        rect.iTl.iX += leftScrollPane.Rect().Width();

        TAknLayoutRect rightScrollPane;
        rightScrollPane.LayoutRect( rect,
                AknLayoutScalable_Apps::navi_pane_g3( 5 ) );
        
        rect.iBr.iX -= rightScrollPane.Rect().Width();
        }
    SetRect( rect );
    }

// ---------------------------------------------------------------------------
// CCustomStatuspaneIndicators::ConstructControlsL
// ---------------------------------------------------------------------------
//
void CCustomStatuspaneIndicators::ConstructControlsL()
	{
	CCustomStatuspaneIndicators::TFollowUpFlagType followUpFlagType = CCustomStatuspaneIndicators::EFollowUpNone;
	TMsgPriority priorityIconType = EMsgPriorityNormal;
	if (iPriorityIconLow && iPriorityIconLow->IsVisible())
	    {
	    priorityIconType = EMsgPriorityLow;
	    }
	else if (iPriorityIconHigh && iPriorityIconHigh->IsVisible())
	    {
	    priorityIconType = EMsgPriorityHigh;
	    }
	
	if (iFollowUpIcon)
	    {
	    if (iFollowUpIcon->Bitmap() == iBitmapFollowUp)
	        {
	        followUpFlagType = CCustomStatuspaneIndicators::EFollowUp;
	        }
	    else if (iFollowUpIcon->Bitmap() == iBitmapFollowUpComplete)
	        {
	        followUpFlagType = CCustomStatuspaneIndicators::EFollowUpComplete;
	        }
	    }
	
	delete iPriorityIconLow;
	iPriorityIconLow = NULL;
	delete iPriorityIconHigh;
	iPriorityIconHigh = NULL;
    delete iFollowUpIcon;
    iFollowUpIcon = NULL;
    iBitmapPriorityLow = NULL;
    iBitmapMaskPriorityLow = NULL;
    iBitmapPriorityHigh = NULL;
    iBitmapMaskPriorityHigh = NULL;
    delete iBitmapFollowUp;
    iBitmapFollowUp = NULL;
    delete iBitmapFollowUpMask;
    iBitmapFollowUpMask = NULL;
    delete iBitmapFollowUpComplete;
    iBitmapFollowUpComplete = NULL;
    delete iBitmapFollowUpCompleteMask;
    iBitmapFollowUpCompleteMask = NULL;

    CFreestyleEmailUiAppUi* fsAppUi =
			static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    fsAppUi->FsTextureManager()->ProvideBitmapL(
    		EComposerTexturePriorityLow,
    		iBitmapPriorityLow,
    		iBitmapMaskPriorityLow );
    fsAppUi->FsTextureManager()->ProvideBitmapL(
    		EComposerTexturePriorityHigh,
    		iBitmapPriorityHigh,
    		iBitmapMaskPriorityHigh );
    fsAppUi->FsTextureManager()->ProvideBitmapL(
    		EFollowUpFlag,
    		iBitmapFollowUp,
    		iBitmapFollowUpMask );
    fsAppUi->FsTextureManager()->ProvideBitmapL(
            EFollowUpFlagComplete,
            iBitmapFollowUpComplete,
            iBitmapFollowUpCompleteMask );

   	TSize iconSize = fsAppUi->LayoutHandler()->statusPaneIconSize();

  	if ( iBitmapPriorityLow && iBitmapMaskPriorityLow )
        {
        iPriorityIconLow = new (ELeave) CEikImage;
        iPriorityIconLow->SetPictureOwnedExternally( EFalse );
        AknIconUtils::SetSize( iBitmapPriorityLow, iconSize, EAspectRatioPreserved  );
        iPriorityIconLow->SetPicture( iBitmapPriorityLow,
									  iBitmapMaskPriorityLow );
        iPriorityIconLow->SetBrushStyle( CWindowGc::ENullBrush );
        iPriorityIconLow->SetSize( iconSize );
        iPriorityIconLow->SetContainerWindowL( *this );
        iPriorityIconLow->MakeVisible(priorityIconType == EMsgPriorityLow);
        }
    if ( iBitmapPriorityHigh && iBitmapMaskPriorityHigh )
        {
        iPriorityIconHigh = new (ELeave) CEikImage;
        iPriorityIconHigh->SetPictureOwnedExternally( EFalse );
        AknIconUtils::SetSize( iBitmapPriorityHigh, iconSize, EAspectRatioPreserved  );
        iPriorityIconHigh->SetPicture( iBitmapPriorityHigh,
        		                       iBitmapMaskPriorityHigh );
        iPriorityIconHigh->SetBrushStyle( CWindowGc::ENullBrush );
        iPriorityIconHigh->SetSize( iconSize );
        iPriorityIconHigh->SetContainerWindowL( *this );
        iPriorityIconHigh->MakeVisible(priorityIconType == EMsgPriorityHigh);
        }
    if ( iBitmapFollowUp && iBitmapFollowUpMask && iBitmapFollowUpComplete && iBitmapFollowUpCompleteMask )
    	{
        iFollowUpIcon = new (ELeave) CEikImage;
        iFollowUpIcon->SetPictureOwnedExternally( ETrue );
        if (followUpFlagType == CCustomStatuspaneIndicators::EFollowUp)
            {
            AknIconUtils::SetSize( iBitmapFollowUp, iconSize, EAspectRatioNotPreserved );
            iFollowUpIcon->SetPicture( iBitmapFollowUp,
                                       iBitmapFollowUpMask );
            }
        else if (followUpFlagType == CCustomStatuspaneIndicators::EFollowUpComplete)
            {
            AknIconUtils::SetSize( iBitmapFollowUpComplete, iconSize, EAspectRatioNotPreserved );
            iFollowUpIcon->SetPicture( iBitmapFollowUpComplete,
                                       iBitmapFollowUpCompleteMask );
            }
        iFollowUpIcon->SetBrushStyle( CWindowGc::ENullBrush );
        iFollowUpIcon->SetSize( iconSize );
        iFollowUpIcon->SetContainerWindowL( *this );
        iFollowUpIcon->MakeVisible(followUpFlagType != CCustomStatuspaneIndicators::EFollowUpNone);
    	}
    DrawDeferred();
	}
