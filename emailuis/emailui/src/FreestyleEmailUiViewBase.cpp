/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition for the common base class for all FsEmailUi views.
*                The class is not intended for instantation.
*
*/
 

// <cmail> SF
#include "emailtrace.h"
#include <alf/alfcontrolgroup.h>
#include <alf/alfvisual.h>
// </cmail>
#include <eikbtgpc.h>
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiStatusIndicator.h"
#include "FSEmail.pan"

// <cmail> Toolbar
#include <AknUtils.h>
#include <akntoolbar.h>
#include <FreestyleEmailUi.rsg>

#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>

// </cmail> Toolbar

// ---------------------------------------------------------------------------
//
CFsEmailUiViewBase::CFsEmailUiViewBase( CAlfControlGroup& aControlGroup, 
        CFreestyleEmailUiAppUi& aAppUi )
    : iControlGroup( aControlGroup ), iAppUi( aAppUi )
    {
    FUNC_LOG;
    // Set control groups to ignore input by default
    iControlGroup.SetAcceptInput( EFalse );
    iKeyboardFlipOpen = aAppUi.IsFlipOpen();
    }

// ---------------------------------------------------------------------------
//
TUid CFsEmailUiViewBase::Id() const
    {
    FUNC_LOG;
    // This must be overriden by the inherited class
    __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiBaseViewMethodNotOverriden) );
    return TUid::Null();
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::DoActivateL( const TVwsViewId& aPrevViewId,
	                                  TUid aCustomMessageId,
	                                  const TDesC8& aCustomMessage )
    {
    FUNC_LOG;    
    SetViewActive( ETrue );
    //<cmail>
    iAppUi.StatusPane()->DrawNow();
    //</cmail>
    iFocusVisible = iAppUi.IsTimerFocusShown();
    
    // fix view stack in case of external activation
    if ( aPrevViewId.iAppUid != KFSEmailUiUid )
        {
        iActiveMailboxBeforeExternalActivation = iAppUi.GetActiveMailboxId();
        iAppUi.ViewActivatedExternallyL( Id() );
        }

    // hide previous view
    CFsEmailUiViewBase* previousView = iAppUi.PreviousActiveView();
    TBool pluginSettingsActive = iAppUi.IsPluginSettingsViewActive();
    if ( pluginSettingsActive )
        {
        iAppUi.ReturnFromPluginSettingsView();
        }
    else if ( previousView )
        {
        // In case of external activation, hide previous control group
        // before view activation to avoid screen flickering
        if ( aPrevViewId.iAppUid != KFSEmailUiUid )
            {
            previousView->DeactivateControlGroup();
            }
        previousView->DoTransition( ETrue ); // fade out prev view
        }
    
    // Clear the flag of long transition effect
    SetNextTransitionOutLong( EFalse );
    UpdateToolbarL();

    SetStatusBarLayout();

    // Make sure Alfred display is of correct size (there is some problems with toolbar)
    TRect metricsRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, metricsRect);
    CAlfEnv::Static()->PrimaryDisplay().ForceSetVisibleArea(metricsRect);
    
    // Finally call child classes activation method
    TRAPD( error, ChildDoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage) );    
    if ( !error )
        {
        // View activated succesfully. Change visible control group, but not
        // when application is being sent to background.
        if ( !iAppUi.SwitchingToBackground() )
            {
            DoTransition( EFalse );
            ActivateControlGroup();
            }
        }
    else
        {
        // In case of error, navigate to the previous view
        DeactivateControlGroup();
        NavigateBackL();
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::UpdateToolbarL()
    {
    FUNC_LOG;
    if (AknLayoutUtils::PenEnabled())
        {
        if (HasToolbar())
            {
            const TInt resourceId(ToolbarResourceId());
            RArray<TInt> dimmedItems;
            CleanupClosePushL(dimmedItems);
            GetInitiallyDimmedItemsL(resourceId, dimmedItems);
            CAknToolbar* toolbar(CAknToolbar::NewL(resourceId));
            for (TInt i = 0; i < dimmedItems.Count(); i++)
                {
                toolbar->SetItemDimmed(dimmedItems[i], ETrue, EFalse);
                }
            CleanupStack::PopAndDestroy(); // dimmedItems.Close()
            toolbar->SetToolbarObserver(this);
            SetToolbar(toolbar);
            ShowToolbar();
            }
        else
            {
            HideToolbar();
            }
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::GetInitiallyDimmedItemsL( 
        const TInt /*aResourceId*/, RArray<TInt>& aDimmedItems ) const
    {
    FUNC_LOG;
    aDimmedItems.Reset();
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::HideToolbar()
    {
    FUNC_LOG;
    if (AknLayoutUtils::PenEnabled())
        {
        CAknToolbar* toolbar(Toolbar());
        if (toolbar)
            {
            //toolbar->SetToolbarVisibility(EFalse);
            // Call MakeVisible directly because SetToolbarVisibility never
            // actually hides fixed toolbar.
            toolbar->MakeVisible(EFalse);
            }
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::ShowToolbar()
    {
    FUNC_LOG;
    if (AknLayoutUtils::PenEnabled())
        {
        CAknToolbar* toolbar(Toolbar());
        if (toolbar)
            {
            toolbar->SetToolbarVisibility(ETrue);
            }
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::SetToolbarItemDimmed( const TInt aCommandId, const TBool aDimmed )
    {
    FUNC_LOG;
    if (AknLayoutUtils::PenEnabled())
        {
        CAknToolbar* toolbar(Toolbar());
        if (toolbar)
            {
            toolbar->SetItemDimmed(aCommandId, aDimmed, ETrue);
            }
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::DoDeactivate()
    {
    FUNC_LOG;
    SetViewActive( EFalse );    

    HideToolbar();
   
    // Make control group ignore input and hide it if it's not already fading out.
    DeactivateControlGroup();

    // do view specific deactivation
    ChildDoDeactivate();
    
    // Return to another app in case this view was activated externally
    if ( iSendToBackgroundOnDeactivation )
        {
        iSendToBackgroundOnDeactivation = EFalse;
        
        // Try to bring calling external app into foreground if found
        TApaTaskList taskList( iEikonEnv->WsSession() );
        TApaTask prevAppTask = taskList.FindApp( iPreviousAppUid );
        TApaTask fsEmailTask = taskList.FindApp( KFSEmailUiUid );
        if ( prevAppTask.Exists() )
            {
            prevAppTask.BringToForeground();
            }
        if ( fsEmailTask.Exists() && iPreviousAppUid != KMessageReaderUid )
            {
            fsEmailTask.SendToBackground();
            }

        iAppUi.SetSwitchingToBackground( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// Check if transition effects are enabled and run the effect if needed.
//
void CFsEmailUiViewBase::DoTransition( TBool aDirectionOut )
    {
    FUNC_LOG;
    if ( iAppUi.LayoutHandler() && iAppUi.LayoutHandler()->ViewFadeEffectActive() )
        {
        DoTransitionEffect( aDirectionOut );
        }
    }

// ---------------------------------------------------------------------------
// Inheriting Alfred views can override these functions, if they need to do
// some special tricks for transition effects
//
void CFsEmailUiViewBase::DoTransitionEffect( TBool aDirectionOut )
    {  
    FUNC_LOG;
    RPointerArray<CAlfVisual> layoutArray;
    TRAP_IGNORE( GetParentLayoutsL( layoutArray ) );
    
    // For views that return valid parent layout(s), use the fade effect,
    // for all other views, use the showing/hiding of control groups
    for ( TInt i = 0 ; i < layoutArray.Count() ; i++ )
        {
        // Returned parent layout might be NULL, if view construction is not
        // fully completed
        if ( layoutArray[i] != NULL )
            {
            // Ensure the view is invisible before doing fade in (because views
            // are automatically visible rigt after construction)
            if ( !aDirectionOut )
                {
                FadeVisual( layoutArray[i], ETrue, ETrue );
                }
            
            FadeVisual( layoutArray[i], aDirectionOut );
            }
        }

    layoutArray.Close();
    }

// ---------------------------------------------------------------------------
// Inheriting Alfred views should override this and return their parent
// layout(s) in array given as parameter. Default implementation
// does nothing, so then the array is empty.
//
void CFsEmailUiViewBase::GetParentLayoutsL( RPointerArray<CAlfVisual>& /*aLayoutArray*/ ) const
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Fade given Alfred visual
//
void CFsEmailUiViewBase::FadeVisual(
        CAlfVisual* aVisual,
        TBool aFadeDirectionOut,
        TBool aImmediate /*= EFalse*/)
    {   
    FUNC_LOG;
    // Check that we are not exiting and is fade effect activated
    if ( !iAppUi.AppUiExitOngoing() )
        {
        TInt effectTime = 0;
        
        if ( !aImmediate && iAppUi.LayoutHandler() )
            {
            if( aFadeDirectionOut )
                {
                effectTime = iAppUi.LayoutHandler()->ViewFadeOutEffectTime();
                }
            else
                {
                effectTime = iAppUi.LayoutHandler()->ViewFadeInEffectTime();
                }
            }
        
        if ( aFadeDirectionOut && IsNextTransitionOutLong() )
            {
            effectTime = effectTime * 2;
            }
    
        // Initialize for out fading
        TAlfTimedValue timedValue( 0, effectTime );
        // Check value and initialze for in fading if set
        if ( !aFadeDirectionOut )
            {
            timedValue.SetTarget( 1, effectTime ); 
            }
        aVisual->SetOpacity( timedValue );
        }
    }

// ---------------------------------------------------------------------------
// Used to indicate that the next out transition effect should be long
// (some long opening view is opened next)
//
void CFsEmailUiViewBase::SetNextTransitionOutLong( TBool aLongTransitionOut )
    {
    FUNC_LOG;
    iIsNextTransitionEffectOutLong = aLongTransitionOut;
    }

// ---------------------------------------------------------------------------
//
TBool CFsEmailUiViewBase::IsNextTransitionOutLong()
    {
    FUNC_LOG;
    return iIsNextTransitionEffectOutLong;
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::ActivateControlGroup( TInt aDelay /*= 0*/ )
    {
    FUNC_LOG;
    if ( !iAppUi.AppUiExitOngoing() )
        {
        TAlfGroupCommand showCmd( ControlGroup(), EAlfOpShow, &iAppUi.Display() );
        iAppUi.AlfEnv().Send( showCmd, aDelay );            
        ControlGroup().SetAcceptInput( ETrue );
    
        // Status indicator is "always on top" while visible
        /*if ( iAppUi.StatusIndicator() && iAppUi.StatusIndicator()->IsVisible() )
            {
            CAlfControlGroup& indicatorGroup = iAppUi.AlfEnv().ControlGroup( KStatusIndicatorDisplayGroup );
            TAlfGroupCommand indicShowCmd( indicatorGroup, EAlfOpShow, &iAppUi.Display() );
            iAppUi.AlfEnv().Send( indicShowCmd, 0 );        
            indicatorGroup.SetAcceptInput( EFalse ); 
            }*/
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::DeactivateControlGroup( TInt aDelay /*= 0*/ )
    {
    FUNC_LOG;
    const TReal32 KDelta = 0.01; // floating point comparison tolerance
    
    if ( !iAppUi.AppUiExitOngoing() )
        {
        ControlGroup().SetAcceptInput( EFalse );
    
        // Hide the control group immediately only if it's not already faded or 
        // fading out. This is to not interfere with the cross fading effect.
        RPointerArray<CAlfVisual> layoutArray;
        TRAP_IGNORE( GetParentLayoutsL( layoutArray ) );
        TInt layoutCount = layoutArray.Count();
        TInt fadingCount = 0;
        for ( TInt i = 0 ; i < layoutCount ; ++i )
            {
            CAlfVisual* layout = layoutArray[i]; 
            if ( layout && layout->Opacity().Target() < KDelta )
                {
                fadingCount++;
                }
            }
        layoutArray.Close();
        
        if ( !layoutCount || fadingCount != layoutCount )
            {
            iAppUi.AlfEnv().CancelCommands( &ControlGroup(), EAlfOpShow );
            TAlfGroupCommand hideCmd( ControlGroup(), EAlfOpHide, &iAppUi.Display() );
            iAppUi.AlfEnv().Send( hideCmd, aDelay );
            }
        }
    }

// ---------------------------------------------------------------------------
//
CAlfControlGroup& CFsEmailUiViewBase::ControlGroup()
    {
    FUNC_LOG;
    return iControlGroup;
    }

// ---------------------------------------------------------------------------
// Handles application foreground events.
// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::HandleAppForegroundEventL( TBool aForeground )
    {
    FUNC_LOG;
    if ( aForeground )
        {
        HandleForegroundEventL();

        // Activate control group in case the view was activated when
        // application was being sent to background, and the control group
        // was left inactive.
        const TInt KActivationDelay = 200; // ms
        ActivateControlGroup( KActivationDelay );
        }
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::HandleDynamicVariantSwitchL( TDynamicSwitchType aType )
    {
    FUNC_LOG;
// <cmail> Toolbar    
    switch (aType)
        {
        case EScreenLayoutChanged:
            UpdateToolbarL();           
            break;
        }
// </cmail> Toolbar    
    // Inherited classes should override this to react to skin and layout changes
    
#ifdef __WINSCW__
    // For emulator testing: landscape mode is "flip open"
    FlipStateChangedL(  Layout_Meta_Data::IsLandscapeOrientation() );
#endif
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::HandleDynamicVariantSwitchOnBackgroundL( TDynamicSwitchType /*aType*/ )
    {
    FUNC_LOG;
    // Inherited classes should override this to react to skin and layout changes
    // which happen while the view is on background
    
#ifdef __WINSCW__
    // For emulator testing: landscape mode is "flip open"
    FlipStateChangedL(  Layout_Meta_Data::IsLandscapeOrientation() );
#endif
    }

// ---------------------------------------------------------------------------
//
TBool CFsEmailUiViewBase::IsNextMsgAvailable( TFSMailMsgId /*aCurrentMsgId*/, 
                                              TFSMailMsgId& /*aFoundNextMsgId*/,
                                              TFSMailMsgId& /*aFoundNextMsgFolderId*/ ) const
    {
    FUNC_LOG;
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
TBool CFsEmailUiViewBase::IsPreviousMsgAvailable( TFSMailMsgId /*aCurrentMsgId*/, 
                                                  TFSMailMsgId& /*aFoundPreviousMsgId*/,
                                                  TFSMailMsgId& /*aFoundPrevMsgFolderId*/ ) const
    {
    FUNC_LOG;
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
TInt CFsEmailUiViewBase::MoveToNextMsgL( TFSMailMsgId /*aCurrentMsgId*/,
                                         TFSMailMsgId& /*aFoundNextMsgId*/ )
    {
    FUNC_LOG;
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
//
TInt CFsEmailUiViewBase::MoveToPreviousMsgL( TFSMailMsgId /*aCurrentMsgId*/, 
                                             TFSMailMsgId& /*aFoundPreviousMsgId*/ )
    {
    FUNC_LOG;
    return KErrNotFound;
    }

//---------------------------------------------------------------------------
// Concrete view class may override
//---------------------------------------------------------------------------
void CFsEmailUiViewBase::PrepareExitL()
    {
    FUNC_LOG;
    }

//---------------------------------------------------------------------------
// CFsEmailUiViewBase::FlipStateChangedL
//---------------------------------------------------------------------------
void CFsEmailUiViewBase::FlipStateChangedL( TBool aKeyboardFlipOpen )
	{
	FUNC_LOG;
	iKeyboardFlipOpen = aKeyboardFlipOpen;
	}

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::HandleForegroundEventL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
//
void CFsEmailUiViewBase::ViewEntered( const TVwsViewId& aPrevViewId )
    {
    FUNC_LOG;
    iPreviousAppUid = aPrevViewId.iAppUid;
    }

// ---------------------------------------------------------------------------
// CFsEmailUiViewBase::NavigateBackL
// ---------------------------------------------------------------------------
void CFsEmailUiViewBase::NavigateBackL()
    {
    FUNC_LOG;
    if ( !iAppUi.ViewSwitchingOngoing() )
        {
        TUid prevView = iAppUi.ReturnToPreviousViewL();

        if ( iPreviousAppUid != KFSEmailUiUid && iPreviousAppUid != KNullUid )
            {
            // Email app should be hidden once the view gets deactivated. Note that hiding
            // should not happen before control group switching is over because that
            // may cause views of other Alfred apps to get distorted.
            iSendToBackgroundOnDeactivation = ETrue;
            iAppUi.SetSwitchingToBackground( ETrue );

            // Re-activate previously active mailbox if it got changed in the external activation.
            // As an exception, if previous view is the laucher grid, the previously active
            // mailbox does not need to be reactivated. This prevents unnecessary mailbox switching
            // when mailbox is entered from home screen, user returns with Back, and enters the same
            // mailbox again.
            if ( prevView != AppGridId &&
                 iActiveMailboxBeforeExternalActivation != iAppUi.GetActiveMailboxId() &&
                 !iActiveMailboxBeforeExternalActivation.IsNullId() )
                {
                iAppUi.SetActiveMailboxL( iActiveMailboxBeforeExternalActivation, EFalse );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::ChangeMskCommandL
// Utility function to change the command ID and label on the middle soft key
// -----------------------------------------------------------------------------
void CFsEmailUiViewBase::ChangeMskCommandL( TInt aLabelResourceId )
    {
    FUNC_LOG;
    CEikButtonGroupContainer* cba = Cba();
    if ( cba ) 
        {
        cba->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
                            aLabelResourceId );
        cba->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::HasToolbar
// Method to check whether the view has toolbar defined or not.
// -----------------------------------------------------------------------------
TBool CFsEmailUiViewBase::HasToolbar() const
    {
    FUNC_LOG;
    return ToolbarResourceId() != R_FREESTYLE_EMAIL_UI_TOOLBAR_BLANK;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::ToolbarResourceId
// 
// -----------------------------------------------------------------------------
TInt CFsEmailUiViewBase::ToolbarResourceId() const
    {
    FUNC_LOG;    
    return R_FREESTYLE_EMAIL_UI_TOOLBAR_BLANK;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::OfferToolbarEventL
// -----------------------------------------------------------------------------
void CFsEmailUiViewBase::OfferToolbarEventL( TInt /*aCommand*/ )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::SetViewActive
// -----------------------------------------------------------------------------
void CFsEmailUiViewBase::SetViewActive( const TBool aActive )
    {
    FUNC_LOG;
    iViewActive = aActive;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::IsViewActive
// -----------------------------------------------------------------------------
TBool CFsEmailUiViewBase::IsViewActive() const
    {
    FUNC_LOG;
    return iViewActive;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::IsFocusShown
// -----------------------------------------------------------------------------
TBool CFsEmailUiViewBase::IsFocusShown()
    {
    return iFocusVisible;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::SetStatusBarLayout
// -----------------------------------------------------------------------------
void CFsEmailUiViewBase::SetStatusBarLayout()
	{
	if ( StatusPane()->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL )
		{
		TRAP_IGNORE( StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL));
		}
	}

// -----------------------------------------------------------------------------
// CFsEmailUiViewBase::HandleTimerFocusStateChange
// -----------------------------------------------------------------------------
void CFsEmailUiViewBase::HandleTimerFocusStateChange(TBool aShow)
	{
    iFocusVisible = aShow;
	}


// end of file

