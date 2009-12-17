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
* Description:  FreestyleEmailUi status indicator implementation
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>
#include <StringLoader.h>
// <cmail> SF
#include <alf/alfdecklayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfbrusharray.h>
// </cmail>
#include <FreestyleEmailUi.rsg>

// INTERNAL INCLUDES
#include "FreestyleEmailUiStatusIndicator.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiLayoutHandler.h"

const TReal KPopupListBackgroundOpacity(1);//0.94);
const TInt KIndicatorDefaultDuration( 3000 );
const TInt KIconArraySize(50);

CFSEmailUiStatusIndicator::CFSEmailUiStatusIndicator( CAlfEnv& aEnv,
    CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup  )
    : CAlfControl(), iEnv(aEnv), iAppUi(aAppUi), iControlGroup(aControlGroup)
    {
    FUNC_LOG;
    }

CFSEmailUiStatusIndicator::~CFSEmailUiStatusIndicator()
    {
    FUNC_LOG;
    delete iTimer;
    iIconArray.Close();
    }

CFSEmailUiStatusIndicator* CFSEmailUiStatusIndicator::NewL( CAlfEnv& aEnv,
    CAlfControlGroup& aControlGroup, CFreestyleEmailUiAppUi* aAppUi )
    {
    FUNC_LOG;
    CFSEmailUiStatusIndicator* self = new (ELeave) CFSEmailUiStatusIndicator( aEnv, aAppUi, aControlGroup );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CFSEmailUiStatusIndicator::ConstructL()
	{
    FUNC_LOG;
    
    // These are done already here, otherwise the ownership of this
    // control is not transferred to AlfEnv (AlfEnv deletes only controls that
    // are added to control groups) -> memory leak if DoFirstStartL not called
    CAlfControl::ConstructL( iEnv );
    iControlGroup.AppendL( this );

    iFirstStartCompleted = EFalse;
    
	}

// CFSEmailUiStatusIndicator::DoFirstStartL()
// Purpose of this function is to do first start things only when indicator is
// really needed to be shown for the first time. Implemented to make app startup faster.
void CFSEmailUiStatusIndicator::DoFirstStartL()
    {
    FUNC_LOG;
    
    CalculateSizeAttributes();
    LoadIconsL();

    iParentLayout = CAlfDeckLayout::AddNewL(*this);
    
    iParentLayout->SetFlags( EAlfVisualFlagManualLayout );
    iParentLayout->SetRect( iParentRect );

    // Set list padding
    //TInt padding = iAppUi->LayoutHandler()->ControlBarListPadding();
    iParentLayout->SetPadding( TPoint(iBorderPadding, iBorderPadding) );

    // Set list background
    iParentLayout->EnableBrushesL();
    CAlfImageBrush* bgBrush = iAppUi->FsTextureManager()->NewControlBarListBgBrushLC();
    iParentLayout->Brushes()->AppendL( bgBrush, EAlfHasOwnership );
    CleanupStack::Pop( bgBrush );

    iContentLayout = CAlfAnchorLayout::AddNewL( *this, iParentLayout );
    
    // Create new image visual for our control
    iImageVisual = CAlfImageVisual::AddNewL(*this, iContentLayout);
    iImageVisual->SetScaleMode( CAlfImageVisual::EScaleFitWidth );
    
    // Working text:
    iFirstTextVisual = CAlfTextVisual::AddNewL( *this, iContentLayout );
    iFirstTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );

    iSecondTextVisual = CAlfTextVisual::AddNewL( *this, iContentLayout );
    iSecondTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iFirstTextVisual->SetAlign( EAlfAlignHRight, EAlfAlignVCenter );
        iSecondTextVisual->SetAlign( EAlfAlignHRight, EAlfAlignVCenter );
        }
    else
        {
        iFirstTextVisual->SetAlign( EAlfAlignHLeft, EAlfAlignVCenter );
        iSecondTextVisual->SetAlign( EAlfAlignHLeft, EAlfAlignVCenter );
        }

    iTimer = CFSEmailUiGenericTimer::NewL( this );  
    iFirstStartCompleted = ETrue;    

    }


void CFSEmailUiStatusIndicator::PositionVisuals()
	{
    FUNC_LOG;
	// Note: the method's bit rewritten for Alfred as the anchors are handled more obnoxiously

	TReal firstLineTopPos = 0;
	TReal firstLineBottomPos = 0;
	TReal secondLineTopPos = 0;
	TReal secondLineBottomPos = 0;
	if( iHasSecondTextLine )
		{
		firstLineBottomPos = -0.5;
		secondLineTopPos = 0.5;
		}

	// If mirrored layout in use, flip left and right anchors and their types,
	// and invert metrics by setting mirrorMultiplier to -1.
	if ( AknLayoutUtils::LayoutMirrored() )
		{
		// Set anchors for icon image visual
		iContentLayout->SetAnchor(EAlfAnchorTopLeft,
						0,
						EAlfAnchorOriginRight,
						EAlfAnchorOriginTop,
						EAlfAnchorMetricAbsolute,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(-1 * iIconWidth, 0));
		iContentLayout->SetAnchor(EAlfAnchorBottomRight,
						0,
						EAlfAnchorOriginRight,
						EAlfAnchorOriginBottom,
						EAlfAnchorMetricRelativeToSize,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(0, 0));

		// Set anchors for first line text visual
		iContentLayout->SetAnchor(EAlfAnchorTopLeft,
						1,
						EAlfAnchorOriginLeft,
						EAlfAnchorOriginTop,
						EAlfAnchorMetricRelativeToSize,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(0, firstLineTopPos));
		iContentLayout->SetAnchor(EAlfAnchorBottomRight,
						1,
						EAlfAnchorOriginRight,
						EAlfAnchorOriginBottom,
						EAlfAnchorMetricAbsolute,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(-1 * (iIconWidth+iTextPadding), firstLineBottomPos));

		// Set anchors for second line text visual
		iContentLayout->SetAnchor(EAlfAnchorTopLeft,
						2,
						EAlfAnchorOriginLeft,
						EAlfAnchorOriginTop,
						EAlfAnchorMetricRelativeToSize,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(0, secondLineTopPos));
		iContentLayout->SetAnchor(EAlfAnchorBottomRight,
						2,
						EAlfAnchorOriginRight,
						EAlfAnchorOriginBottom,
						EAlfAnchorMetricAbsolute,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(-1 * (iIconWidth+iTextPadding), secondLineBottomPos));
		}
	else
		{
		// Set anchors for icon image visual
		iContentLayout->SetAnchor(EAlfAnchorTopLeft,
						0,
						EAlfAnchorOriginLeft,
						EAlfAnchorOriginTop,
						EAlfAnchorMetricRelativeToSize,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(0, 0));
		iContentLayout->SetAnchor(EAlfAnchorBottomRight,
						0,
						EAlfAnchorOriginLeft,
						EAlfAnchorOriginBottom,
						EAlfAnchorMetricAbsolute,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(iIconWidth, 0));

		// Set anchors for first line text visual
		iContentLayout->SetAnchor(EAlfAnchorTopLeft,
						1,
						EAlfAnchorOriginLeft,
						EAlfAnchorOriginTop,
						EAlfAnchorMetricAbsolute,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint((iIconWidth+iTextPadding), firstLineTopPos));		
		iContentLayout->SetAnchor(EAlfAnchorBottomRight,
						1,
						EAlfAnchorOriginRight,
						EAlfAnchorOriginBottom,
						EAlfAnchorMetricRelativeToSize,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(0, firstLineBottomPos));

		// Set anchors for second line text visual
		iContentLayout->SetAnchor(EAlfAnchorTopLeft,
						2,
						EAlfAnchorOriginLeft,
						EAlfAnchorOriginTop,
						EAlfAnchorMetricAbsolute,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint((iIconWidth+iTextPadding), secondLineTopPos));
		iContentLayout->SetAnchor(EAlfAnchorBottomRight,
						2,
						EAlfAnchorOriginRight,
						EAlfAnchorOriginBottom,
						EAlfAnchorMetricRelativeToSize,
						EAlfAnchorMetricRelativeToSize,
						TAlfTimedPoint(0, secondLineBottomPos));
		}
	}


void CFSEmailUiStatusIndicator::CalculateSizeAttributes()
	{
    FUNC_LOG;
    iBorderPadding = iAppUi->LayoutHandler()->ControlBarListBorderRoundingSize() / 2;
 	iContentHeight = iAppUi->LayoutHandler()->OneLineListItemHeight();
	iIconWidth = iContentHeight;

    if( iHasSecondTextLine )
    	{
		iIconWidth += iIconWidth/2;
	 	iContentHeight += iContentHeight;
    	}

 	if( iHasIcon )
 		{
		iTextPadding = iBorderPadding;
 		}
 	else
 		{
 		iIconWidth = 0;
 		iTextPadding = 0;
 		}

 	// Calculate size and location for the parent rect
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, iParentRect );
    TInt popupWidth = iParentRect.Width();
    if ( iFirstTextVisual && iSecondTextVisual )
        {
        if ( iUseCompactLayout )
            {
            iFirstTextVisual->SetStyle( EAlfTextStyleSmall );
            iSecondTextVisual->SetStyle( EAlfTextStyleSmall );
            /*
            iFirstTextVisual->ReportChanged();
            iSecondTextVisual->ReportChanged();
            
            TInt contentWidth = Max( iFirstTextVisual->TextExtents().iWidth, 
                                     iSecondTextVisual->TextExtents().iWidth );
            contentWidth += iIconWidth;
            contentWidth += 2*iBorderPadding;
            popupWidth = Min( iParentRect.Width(), contentWidth );
            */
            popupWidth = popupWidth * 3 / 4;
            }
        else
            {
            iFirstTextVisual->SetStyle( EAlfTextStyleNormal );
            iSecondTextVisual->SetStyle( EAlfTextStyleNormal );
            }
        }
    TInt leftMargin = ( iParentRect.Width() - popupWidth ) / 2;   

 	iParentRect.SetRect( leftMargin, iParentRect.Height() - iContentHeight - 2*iBorderPadding, 
 	                     leftMargin + popupWidth, iParentRect.Height() );
 	//iParentRect.Shrink(100, 100);
 	//iParentRect.SetHeight(10);
 	//iParentRect.Move(0, 150);
	}

void CFSEmailUiStatusIndicator::SetTextByResourceIdL( TInt aResourceId, CAlfTextVisual* aTextVisual )
	{
    FUNC_LOG;
	if ( iFirstStartCompleted )
	    {
	    HBufC* headingText = StringLoader::LoadLC( aResourceId );
	    aTextVisual->SetTextL( *headingText );
	    CleanupStack::PopAndDestroy(headingText);	    
	    }
	}

void CFSEmailUiStatusIndicator::ShowIndicatorL(
	TDesC* aFirstLineText,
	TDesC* aSecondLineText,
	CAlfTexture* aIconTexture,
	TInt aDuration )
	{
    FUNC_LOG;
	// Do first construction if not completed
	if ( !iFirstStartCompleted )
	    {
	    DoFirstStartL();
	    }	
	
	// Set first line of text
	if( aFirstLineText )
		{
		iFirstTextVisual->SetTextL( *aFirstLineText );
		}
	else
		{
		iFirstTextVisual->SetTextL( KNullDesC );
		}

	// Set second line of text
	if( aSecondLineText )
		{
		iSecondTextVisual->SetTextL( *aSecondLineText );
		iHasSecondTextLine = ETrue;
		}
	else
		{
		iSecondTextVisual->SetTextL( KNullDesC );
		iHasSecondTextLine = EFalse;
		}

	// Set icon texture
	if( aIconTexture )
		{
		iHasIcon = ETrue;
		iImageVisual->SetImage( TAlfImage( *aIconTexture ) );
		}
	else
		{
		iHasIcon = EFalse;
	    iImageVisual->SetImage( TAlfImage() );
		}

    DoShowIndicator( aDuration );
	}
	                     
void CFSEmailUiStatusIndicator::ShowIndicatorL(
	TStatusIndicationTypes aIndicationType,
	const TDesC* aFirstLineText,
	TInt aDuration )
	{
    FUNC_LOG;
    // Do first construction if not completed
    if ( !iFirstStartCompleted )
        {
        DoFirstStartL();
        }
	
	iHasIcon = ETrue;
    iImageVisual->SetImage( TAlfImage( *iIconArray[aIndicationType] ) );

    iHasSecondTextLine = EFalse;
	iUseCompactLayout = EFalse;
	iSecondTextVisual->SetTextL( KNullDesC );
	
    CAlfTextVisual* textVisual = iFirstTextVisual;
    if( aFirstLineText )
        {
        iFirstTextVisual->SetTextL( *aFirstLineText );
        textVisual = iSecondTextVisual;
        iHasSecondTextLine = ETrue;
        }

	// <cmail> S60 Skin support
    TRgb textColor = KRgbBlack;
	textColor = iAppUi->LayoutHandler()->DropdownMenuTextColor();    

	iFirstTextVisual->SetColor (textColor);
	iSecondTextVisual->SetColor (textColor);
	//</cmail>

	switch( aIndicationType )
		{
		case EIndicationSynchronising:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_UPDATING, textVisual );
			}
			break;

		case EIndicationConnected:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_CONNECTED, textVisual );
			}
			break;
		
		case EIndicationConnecting:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_CONNECTING, textVisual );
			}
			break;
				
		case EIndicationDisconnectedGeneral:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_DISCONNECTED, textVisual );
			}
			break;
				
		case EIndicationDisconnectedError:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_DISCONNECTED, textVisual );
			}
			break;
				
		case EIndicationDisconnectedLowBattery:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_DISCONNECTED, textVisual );
			}
			break;
				
		case EIndicationMailSent:
			{
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_EMAIL_SENT, textVisual );
			}
			break;
		case EIndicationCancelSynchronising:
			{
		    iImageVisual->SetImage( TAlfImage() );
		    iHasIcon = EFalse;
			SetTextByResourceIdL( R_FREESTYLE_EMAIL_UI_STATUS_CANCEL_SYNC, textVisual );
			}
			break;
		case EIndicationDownloadStarted:
		    {
            SetTextByResourceIdL( R_FSE_VIEWER_NOTE_ATTACHMENT_DOWNLOADING, textVisual );
		    }
		    break;
		case EIndicationDownloadProgress:
		    {
		    iHasSecondTextLine = EFalse;
		    iUseCompactLayout = ETrue;
		    }
		    break;
		default:
			return;	
		}

	DoShowIndicator( aDuration );
	}

void CFSEmailUiStatusIndicator::DoShowIndicator( TInt aDuration )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        CalculateSizeAttributes();
        PositionVisuals();

        // Default values used when just updating the view after layout change
        TInt transitionTime( 0 );
        TRect startRect = iParentRect;
        
        // Set indicator transparent and out of the screen as a preparation
        // for transformation effect (fade and slide in). Except if we are
        // just updating the visuals as a result of layout change.
        if ( aDuration != EIndicationLayoutUpdated )
            {
            SetOpacity( 0, 0 );

            startRect.Move( 0, iContentHeight - 2*iBorderPadding );
            
            transitionTime = iAppUi->LayoutHandler()->StatusIndicatorFadeEffectTime();
            }

        // Show the indicator
        CAlfControlGroup* group = ControlGroup();
        iEnv.Send(TAlfGroupCommand(*group, EAlfOpShow, &iAppUi->Display()), 0);
        //Display().Roster().ShowL(*group);
        SetOpacity( KPopupListBackgroundOpacity, transitionTime );

        // First set the transformation starting rect immediately,
        // and then the actual rect with transtition time
        iParentLayout->SetRect( startRect );
        iParentLayout->SetRect( iParentRect, transitionTime );

        if( aDuration == EIndicationDefaultDuration )
            {
            aDuration = KIndicatorDefaultDuration;
            }
        
        // Start hiding timer only if some reasonable duration is given.
        // In other cases the indicator is going to be hided manually, or
        // the original duration is preserved.
        if( aDuration >=0 )
            {
            iTimer->Start( aDuration );
            }        
        iVisible = ETrue;     
        }
	}

void CFSEmailUiStatusIndicator::SetContentsL( const TDesC* aFirstLineText,
                                              const TDesC* aSecondLineText /*= NULL*/,
                                              const CAlfTexture* aIconTexture /*= NULL*/ )
    {
    FUNC_LOG;
    // Do first construction if not completed
    if ( !iFirstStartCompleted )
        {
        DoFirstStartL();
        }    
    if ( aFirstLineText )
        {
        iFirstTextVisual->SetTextL( *aFirstLineText );
        }

    if ( aSecondLineText )
        {
        iSecondTextVisual->SetTextL( *aSecondLineText );
        iHasSecondTextLine = ETrue;
        }

    if ( aIconTexture )
        {
        iHasIcon = ETrue;
        iImageVisual->SetImage( TAlfImage( *aIconTexture ) );
        }
    }

void CFSEmailUiStatusIndicator::HideIndicator( TInt aDelayBeforeHidingInMs /*= 0*/ )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        // Cancel any running hiding timer to prevent indicator being slided off twice.
        iTimer->Stop();
        
        if ( aDelayBeforeHidingInMs > 0 )
            {
            iTimer->Start( aDelayBeforeHidingInMs );
            }
        else // hide now
            {
            CAlfControlGroup* group = ControlGroup();
            CAlfDisplay* temp = Display();
            Env().Send(TAlfGroupCommand(*group, EAlfOpHide, Display()), iAppUi->LayoutHandler()->StatusIndicatorFadeEffectTime());
            //Display().Roster().ShowL(*group);
            SetOpacity(0, iAppUi->LayoutHandler()->StatusIndicatorFadeEffectTime());
        
            TRect tempRect = iParentRect;
            tempRect.Move( 0, iContentHeight - 2*iBorderPadding );
            iParentLayout->SetRect( tempRect, iAppUi->LayoutHandler()->StatusIndicatorFadeEffectTime() );
            
            iVisible = EFalse;
            }        
        }
	}

TBool CFSEmailUiStatusIndicator::IsVisible()
    {
    FUNC_LOG;
    return iVisible;
    }

// ---------------------------------------------------------------------------
// HandleForegroundEventL
// The function is called by Appui or Mail viewer visualiser, to resize the
// window correctly after the screensaver, if the status indicator is visible
// ---------------------------------------------------------------------------
void CFSEmailUiStatusIndicator::HandleForegroundEventL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iVisible )
        {
        DoShowIndicator( EIndicationLayoutUpdated );
        }
    }

void CFSEmailUiStatusIndicator::NotifyLayoutChange()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iVisible )
        {
        DoShowIndicator( EIndicationLayoutUpdated );
        }
    }

void CFSEmailUiStatusIndicator::HandleCommandL(TInt /*aCommand*/)
	{
    FUNC_LOG;
	// Status indicator does not handle any commands.
	}

TBool CFSEmailUiStatusIndicator::OfferEventL(const TAlfEvent& /*aEvent*/)
	{
    FUNC_LOG;
	return EFalse; // Not consumed. Status indicator does not handle any events.
	}

void CFSEmailUiStatusIndicator::SetOpacity( TReal aOpacity, TInt aTransitionTime )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        TAlfTimedValue opacity = iParentLayout->Opacity();
        opacity.SetTarget( aOpacity, aTransitionTime );
        iParentLayout->SetOpacity( opacity );        
        }
	}

// ---------------------------------------------------------------------------
// Load the needed icons
// ---------------------------------------------------------------------------
//
void CFSEmailUiStatusIndicator::LoadIconsL()
	{
    FUNC_LOG;
	iIconArray.Reset();
	// NOTE: Must be appended same order as are in TStatusIndicationTypes!
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureSynchronising ) );   // EIndicationSynchronising
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureConnected ) );  // EIndicationConnected
 	// Change to EStatusTextureConnecting when icon available!!!
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureConnecting ) );  // EIndicationConnecting
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureDisconnectedGeneral ) );    // EIndicationDisconnectedGeneral
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureDisconnectedError ) );    // EIndicationDisconnectedError
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureDisconnectedLowBattery ) );    // EIndicationDisconnectedLowBattery
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureMailSent ) ); // EIndicationMailSent
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EStatusTextureDisconnectedGeneral ) ); // EIndicationCancelSynchronising // there's no icon for this but add entry anyway to maintain the array order
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EAttachmentsDownload ) ); //EIndicationDownloadStarted
 	iIconArray.AppendL( &iAppUi->FsTextureManager()->TextureByIndex( EAttachmentsDownload ) ); // EIndicationDownloadProgress 
 	
 	for( TInt i = 0; i < iIconArray.Count(); ++i )
 	    {
 	    if ( i != EIndicationDownloadProgress &&
             i != EIndicationDownloadStarted ) // attachment icon should use its original (smaller) size
 	        {
 	        iIconArray[i]->Size().SetSize( KIconArraySize, KIconArraySize );
 	        }
 	    }	
	}


void CFSEmailUiStatusIndicator::TimerEventL( CFSEmailUiGenericTimer* /*aTriggeredTimer*/ )
	{
    FUNC_LOG;
	HideIndicator();
	}

