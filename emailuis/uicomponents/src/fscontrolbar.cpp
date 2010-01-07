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
* Description:  Main class for Control Bar component.
*
*/


//<cmail> SF
#include "emailtrace.h"
#include <alf/alfevent.h>
#include <alf/alfdecklayout.h>
//</cmail>
#include <AknsConstants.h>
#include <AknUtils.h>

#include "fscontrolbar.h"
#include "fscontrolbarmodel.h"
#include "fscontrolbarvisualiser.h"
#include "fscontrolbutton.h"
#include "fsgenericpanic.h"
#include "fscontrolbuttonconst.h"
#include "fstextstylemanager.h"
#include "fscontrolbuttonmodel.h"
#include "fslayoutmanager.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
// <cmail> Touch
CFsControlBar::CFsControlBar() :
    iSelectedButton( KNoButtonFocused ), iTouchEnabled(ETrue)
// </cmail>
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsControlBar::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );

    iTextStyleManager =
        CFsTextStyleManager::NewL( this->Env().TextStyleManager() );

    // create model for controlBar
    iModel = CFsControlBarModel::NewL();

    // create visualiser for controlBar
    iVisualiser = CFsControlBarVisualiser::NewL( *this, *iModel );

    // Start listening TAlfActionCommand messages.
    Env().AddActionObserverL( this );
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsControlBar* CFsControlBar::NewL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CFsControlBar* self = CFsControlBar::NewLC( aEnv );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsControlBar* CFsControlBar::NewLC( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CFsControlBar* self = new (ELeave) CFsControlBar();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsControlBar::~CFsControlBar()
    {
    FUNC_LOG;
    Env().RemoveActionObserver( this );
    delete iModel;
    delete iVisualiser;
    iObservers.Reset();
    delete iTextStyleManager;
    }


// ---------------------------------------------------------------------------
// Adds new button to model. Panics if button with specified id already
// exists in model.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::AddButtonL(
    TInt aId,
    TFsControlButtonType aType )
    {
    FUNC_LOG;
    if ( iModel->ExistsButtonWithId( aId ) )
        {
        FsGenericPanic( EFsControlBarButtonWithSpecifiedIdAlreadyExists );
        User::Leave( KErrAlreadyExists );
        }

    if ( aId == ECBFirstFocusableButton )
        {
        FsGenericPanic( EFsControlButtonIdRestricted );
        User::Leave( KErrArgument );
        }

    TRect targetRect( GetLayoutRect( iModel->Count() + 1 ) );
    CFsControlButton* button(
        CFsControlButton::NewL( Env(), *this, aId, aType, targetRect,
            *(iVisualiser->Layout() ), iTextStyleManager ) );
    button->SetParentIndex( iModel->Count() + 1 );

    iModel->AddButtonL( *button );
    }


// ---------------------------------------------------------------------------
// Adds new button to model.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsControlBar::AddButtonL( TFsControlButtonType aType )
    {
    FUNC_LOG;
    TInt newId( iModel->GenerateButtonId() );

    AddButtonL( newId, aType );

    return newId;
    }


// ---------------------------------------------------------------------------
// Removes button with specified id from model. Function RemoveButtonById
// panics if button with specified id doesn't exist in model.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsControlBar::RemoveButtonL( TInt aId )
    {
    FUNC_LOG;
    // if button is focused -> change focus to next
    if ( iModel->ButtonById( aId )->IsFocused() )
        {
        CFsControlButton* button( iModel->NextButton( aId ) );

        if ( !button )
            {
            button = iModel->PrevButton( aId );
            }

        ChangeFocusL( button );
        }

    TInt retVal( iModel->RemoveButtonById( aId ) );
    iVisualiser->RefreshL();

    return retVal;
    }


// ---------------------------------------------------------------------------
// Retrieves button with specified id from control bar. Panics if button
// with specified id doesn't exist in model.
// ---------------------------------------------------------------------------
//
EXPORT_C MFsControlButtonInterface* CFsControlBar::ButtonById( TInt aId )
    {
    FUNC_LOG;
    return iModel->ButtonById( aId );
    }


// ---------------------------------------------------------------------------
// Sets focus to first button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetFocusL( TBool aFocused )
    {
    FUNC_LOG;
    if ( aFocused )
        {
        // Gain focus.
        CFsControlButton* button(
            iModel->ButtonById( ECBFirstFocusableButton ) );
        if ( button )
            {
            SetFocusByIdL( button->Id() );
            }
        }
    else
        {
        // Lose focus.
        if ( iModel->IsFocused() )
            {
            LooseFocus( MFsControlBarObserver::EEventFocusLost );
            }
        }
    }


// ---------------------------------------------------------------------------
// Sets focus to specified button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetFocusByIdL( TInt aButtonId )
    {
    FUNC_LOG;
    TBool focusedAlready( iModel->IsFocused() );

    if ( focusedAlready && iSelectedButton == aButtonId  )
        {
        // Button already has the focus.
        return;
        }

    CFsControlButton* button( iModel->ButtonById( aButtonId ) );

    if ( !button )
        {
        User::Leave( KErrNotFound );
        }

    // cannot set focus to button that is dimmed or not visible
    if ( button->IsDimmed() || !button->IsVisible() )
        {
        User::Leave( KErrNotSupported );
        }

    // bar already focused -> clear focus on button.
    if ( focusedAlready )
        {
        // clear focus on previous selected button
        iModel->ButtonById( iSelectedButton )->SetFocus( EFalse );
        }

    button->SetFocus();
    iSelectedButton = aButtonId;

    if ( !focusedAlready )
        {
        // set state for bar
        iModel->SetFocus();
        // send event that bar is focused
        NotifyObservers(
            MFsControlBarObserver::EEventFocusGained, aButtonId );
        }

    iVisualiser->RefreshL();

    // send event that selection changed
    NotifyObservers( MFsControlBarObserver::EEventSelectionChanged );
    }


// ---------------------------------------------------------------------------
// Gets focused button or NULL if not focused.
// ---------------------------------------------------------------------------
//
EXPORT_C MFsControlButtonInterface* CFsControlBar::GetFocusedButton() const
    {
    FUNC_LOG;
    MFsControlButtonInterface* focusedButton( NULL );

    if( iModel->IsFocused() )
        {
        focusedButton = iModel->ButtonById( iSelectedButton );
        }

    return focusedButton;
    }


// ---------------------------------------------------------------------------
// Sets height of the bar in pixels.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetHeightL( TInt aHeight )
    {
    FUNC_LOG;
    iModel->SetHeight( aHeight );
    iVisualiser->UpdateSizeL();
    }


// ---------------------------------------------------------------------------
// Retrieves height of the bar in pixels.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsControlBar::Height() const
    {
    FUNC_LOG;
    return iModel->Size().iHeight;
    }


// ---------------------------------------------------------------------------
// Sets width of the bar in pixels.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetWidthL( TInt aWidth )
    {
    FUNC_LOG;
    iModel->SetWidth( aWidth );
    iVisualiser->UpdateSizeL();
    }

// ---------------------------------------------------------------------------
// Set rect for control bar
// ---------------------------------------------------------------------------
//
void CFsControlBar::SetRectL( const TRect& aRect )
    {
    iModel->SetRect( aRect );
    iVisualiser->UpdateSizeL();
    }

// ---------------------------------------------------------------------------
// Retrieves width of the bar in pixels.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsControlBar::Width() const
    {
    FUNC_LOG;
    return iModel->Size().iWidth;
    }


// ---------------------------------------------------------------------------
// Sets controlbar's background color.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetBackgroundColor( const TRgb& aColor )
    {
    FUNC_LOG;
    iVisualiser->SetBackgroundColor( aColor );
    }


// ---------------------------------------------------------------------------
// Clears bar's background color. Bar is transparent.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::ClearBackgroundColor()
    {
    FUNC_LOG;
    iVisualiser->ClearBackgroundColor();
    }


// ---------------------------------------------------------------------------
// Sets bar's background image.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetBackgroundImageL( CAlfTexture& aImage )
    {
    FUNC_LOG;
    iVisualiser->SetBackgroundImageL( aImage );
    }


// ---------------------------------------------------------------------------
// Clears bar's background image.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::ClearBackgroundImage()
    {
    FUNC_LOG;
    iVisualiser->ClearBackgroundImage();
    }


// ---------------------------------------------------------------------------
// Adds additional observer for the bar events.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::AddObserverL( MFsControlBarObserver& aObserver )
    {
    FUNC_LOG;
    // add only if not already added.
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }


// ---------------------------------------------------------------------------
// Removes specified observer from the bar events.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::RemoveObserver(
    MFsControlBarObserver& aObserver )
    {
    FUNC_LOG;
    TInt index( iObservers.Find( &aObserver ) );

    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }


// ---------------------------------------------------------------------------
// Sets transition time for selector.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetSelectorTransitionTimeL(
    TInt aTransitionTime )
    {
    FUNC_LOG;
    iVisualiser->SetSelectorTransitionTimeL( aTransitionTime );
    }


// ---------------------------------------------------------------------------
// Sets selector's image and opacity.
// Ownership of the brush is gained.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::SetSelectorImageL(
    CAlfBrush* aSelectorBrush,
    TReal32 aOpacity )
    {
    FUNC_LOG;
    iVisualiser->SetSelectorImageL( aSelectorBrush, aOpacity );
    }


// ---------------------------------------------------------------------------
// Retrieves amount of space in pixels from specified point to the end of the
// screen.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual* CFsControlBar::Visual()
    {
    FUNC_LOG;
    return iVisualiser->Layout();
    }


// ---------------------------------------------------------------------------
// Resolve the area reserved for requested layout slot.
// ---------------------------------------------------------------------------
//
TRect CFsControlBar::GetLayoutRect( TInt aIndex )
    {
    FUNC_LOG;
    CFsLayoutManager::TFsLayoutMetrics layoutItem;
    switch( aIndex )
        {
        case 1:
            layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarPaneG1;
            break;
        case 2:
            layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPane;
            break;
        case 3:
            layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarButtonPaneCp01;
            break;
        case 4:
            layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarPaneG2;
            break;
        default:
            layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarPane;
            break;
        }

    TRect targetRect;
    TRect parentRect;

    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EScreen, parentRect );
    CFsLayoutManager::LayoutMetricsRect( parentRect,
        CFsLayoutManager::EFsLmApplicationWindow, targetRect );
    parentRect = targetRect;
    CFsLayoutManager::LayoutMetricsRect( parentRect,
        CFsLayoutManager::EFsLmMainPane, targetRect );
    parentRect = targetRect;
    CFsLayoutManager::LayoutMetricsRect( parentRect,
        CFsLayoutManager::EFsLmMainSpFsCtrlbarPane, targetRect );
    parentRect = TRect( TPoint( 0, 0 ), targetRect.Size() );
    if ( CFsLayoutManager::EFsLmMainSpFsCtrlbarPane != layoutItem )
        {
        CFsLayoutManager::LayoutMetricsRect(
            parentRect, layoutItem, targetRect );
        }

    return targetRect;
    }

// <cmail> Touch
// ---------------------------------------------------------------------------
// Called to disable/enable touch events
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::EnableTouch(TBool aEnabled)
    {
    iTouchWasEnabled = EFalse;
    iTouchEnabled = aEnabled;
    }
// </cmail>

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBar::MakeSelectorVisible(
        TBool aShow, TBool aFromTouch )
    {
    FUNC_LOG;
    iVisualiser->MakeSelectorVisible( aShow, aFromTouch );
    }


// ---------------------------------------------------------------------------
// From MAlfActionObserver.
// Called by the scheduler when an action command is executed.
// ---------------------------------------------------------------------------
//
void CFsControlBar::HandleActionL( const TAlfActionCommand& aActionCommand )
    {
    FUNC_LOG;
    if ( KFsMessageLayoutChange == aActionCommand.Id() )
        {
        iModel->UpdateButtonsPositions();
        iVisualiser->RefreshL( ETrue );
        }
    }


// ---------------------------------------------------------------------------
// From class CHuiControl/CAlfControl.
// Notifies the control that one of the visuals it ows has been laid out.
// ---------------------------------------------------------------------------
//
void CFsControlBar::VisualLayoutUpdated( CAlfVisual& aVisual )
    {
    FUNC_LOG;
    TBool screenSizeChanged( EFalse );
    TInt displayAreaWidth( DisplayArea().Width() );

    // We don't want more layout updates. Flag is set if the layout needs to
    // be updated.
    aVisual.ClearFlags( EAlfVisualFlagLayoutUpdateNotification );

    if ( displayAreaWidth != iLastScreenWidth )
        {
        iLastScreenWidth = displayAreaWidth;
        screenSizeChanged = ETrue;
        }

    if ( iVisualiser )
        {
        TRAP_IGNORE( iVisualiser->RefreshL( screenSizeChanged ) );
        }
    }


// ---------------------------------------------------------------------------
// From class CHuiControl/CAlfControl.
// Called when an input event is being offered to the control.
// ---------------------------------------------------------------------------
//
TBool CFsControlBar::OfferEventL( const TAlfEvent& aEvent )
    {
    FUNC_LOG;
    TBool eventHandled( EFalse );

    // Bar not focused -> no key handling
    if ( !iModel->IsFocused() )
        {
        return eventHandled;
        }

    if ( aEvent.IsKeyEvent() )
        {
        CFsControlButton* button( NULL );

        button = iModel->ButtonById( iSelectedButton );

        if ( button && button->OfferEventL( aEvent ) )
            {
            eventHandled = ETrue;
            }
        else
            {
            if ( EEventKey == aEvent.Code() )
                {
                switch ( aEvent.KeyEvent().iScanCode )
                    {
                    case EStdKeyLeftArrow:
                        {
                        button = iModel->PrevButton( iSelectedButton );
                        ChangeFocusL( button );
                        // Always after keypress focus needs to be shown.
                        MakeSelectorVisible( ETrue );
                        eventHandled = ETrue;
                        break;
                        }

                    case EStdKeyRightArrow:
                        {
                        button = iModel->NextButton( iSelectedButton );
                        ChangeFocusL( button );
                        // Always after keypress focus needs to be shown.
                        MakeSelectorVisible( ETrue );
                        eventHandled = ETrue;
                        break;
                        }

                    case EStdKeyDownArrow:
                        {
                        LooseFocus(
                                MFsControlBarObserver::EEventFocusLostAtBottom );
                        eventHandled = ETrue;
                        break;
                        }

                    default:
                        {
                        eventHandled = EFalse;
                        break;
                        }
                    }
                }
            }
        }
    else if( aEvent.IsPointerEvent() )
        {
        const TInt buttonCount( iModel->Count() );
        TInt buttonIndex( 0 );
        for( ; buttonIndex < buttonCount && !eventHandled; ++buttonIndex )
            {
            eventHandled = iModel->ButtonByIndex(
                buttonIndex )->OfferEventL( aEvent );
            }
        }


    return eventHandled;
    }


// ---------------------------------------------------------------------------
// From class MFsControlButtonObserver.
// Handles events from control buttons.
// ---------------------------------------------------------------------------
//
TBool CFsControlBar::HandleButtonEvent(
    TFsControlButtonEvent aEvent,
    TInt aButtonId )
    {
    FUNC_LOG;
    TBool eventHandled( ETrue );
    switch ( aEvent )
        {
        // <cmail> Touch
        case EEventButtonTouchPressed:
            {
            MakeSelectorVisible( ETrue, ETrue );
            if (iTouchEnabled)
                {
                iTouchWasEnabled = ETrue;
                CFsControlButton* button( iModel->ButtonById( aButtonId ) );
                if( button && !button->IsFocused())
                    {
                    TRAP_IGNORE( ChangeFocusL( button ) );
                    MakeSelectorVisible( ETrue, ETrue );
                    NotifyObservers( MFsControlBarObserver::EEventTouchFocused, aButtonId );
                    }
                }
            else
                {
                eventHandled = iTouchWasEnabled = EFalse;
                }
            break;
            }
        // </cmail>

        case EEventButtonPressed:
            {
            NotifyObservers(
                MFsControlBarObserver::EEventButtonPressed, aButtonId );
            break;
            }

        case EEventButton:
            {
            NotifyObservers( MFsControlBarObserver::EEventButton, aButtonId );
            break;
            }

        // <cmail> Touch
        case EEventButtonTouchReleased:
            {
            // Notify that button is released, focus can be hidden.
            NotifyObservers( MFsControlBarObserver::EEventFocusVisibilityChanged );

            if (iTouchEnabled && iTouchWasEnabled)
                {
                if( aButtonId > KErrNotFound )
                    {
                    NotifyObservers(
                        MFsControlBarObserver::EEventButtonTouched, aButtonId );
                    }
                else
                    {
                    MakeSelectorVisible( EFalse );
                    }
                }
            else
                {
                // Mark event not handled, because list might be waiting for this up event
                eventHandled = EFalse;
                }
            break;
            }
        // </cmail>

        case EEventButtonReleased:
            {
            NotifyObservers(
                MFsControlBarObserver::EEventButtonReleased, aButtonId );
            break;
            }

        case EEventButtonVisibilityChanged:
            {
            CFsControlButton* button( iModel->ButtonById( aButtonId ) );

            if ( button->IsVisible() )
                {
                // Need to update visual order when a button changes it's
                // state from hidden to visible.
                iVisualiser->ReorderVisuals();
                }

            TRAP_IGNORE( iVisualiser->RefreshL() );
            }
            // Intended flow through.
        case EEventButtonDimmStateChanged:
            {
            // The focus is not needed to be changed if the selected button's
            // dimm state isn't changed.
            if ( aButtonId != iSelectedButton )
                {
                break;
                }

            CFsControlButton* button( iModel->ButtonById( aButtonId ) );

            if ( iModel->IsFocused()
                && ( button->IsDimmed() || !button->IsVisible() ) )
                {
                const TBool mirrored( CFsLayoutManager::IsMirrored() );
                button = mirrored
                    ? iModel->PrevButton( aButtonId )
                    : iModel->NextButton( aButtonId );

                if ( !button )
                    {
                    button = mirrored
                        ? iModel->NextButton( aButtonId )
                        : iModel->PrevButton( aButtonId );
                    }

                TRAP_IGNORE( ChangeFocusL( button ) );
                }
            break;
            }

        default:
            {
            FsGenericPanic( EFsControlBarUnknownEvent );
            break;
            }
        }
    return eventHandled;
    }


// ---------------------------------------------------------------------------
// Sends event to observers.
// ---------------------------------------------------------------------------
//
void CFsControlBar::NotifyObservers(
    MFsControlBarObserver::TFsControlBarEvent aEvent,
    TInt aButtonId )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleControlBarEvent( aEvent, aButtonId );
        }
    }


// ---------------------------------------------------------------------------
// Changes focus to another button or looses focus from control bar.
// ---------------------------------------------------------------------------
//
void CFsControlBar::ChangeFocusL( CFsControlButton* aButton )
    {
    FUNC_LOG;
    if ( aButton )
        {
        // set focus to specified button
        SetFocusByIdL( aButton->Id() );
        }
    else
        {
        // loose focus from bar
        LooseFocus( MFsControlBarObserver::EEventFocusLostAtSide );
        }
    }


// ---------------------------------------------------------------------------
// Sets needed variables when bar loses focus.
// ---------------------------------------------------------------------------
//
void CFsControlBar::LooseFocus(
    MFsControlBarObserver::TFsControlBarEvent aLooseFocusEvent )
    {
    FUNC_LOG;
    if ( aLooseFocusEvent == MFsControlBarObserver::EEventFocusLostAtBottom
         || aLooseFocusEvent == MFsControlBarObserver::EEventFocusLostAtSide
         || aLooseFocusEvent == MFsControlBarObserver::EEventFocusLost )
        {
        CFsControlButton* button( iModel->ButtonById( iSelectedButton ) );

        if ( button )
            {
            button->SetFocus( EFalse );
            iSelectedButton = KNoButtonFocused;
            iModel->SetFocus( EFalse );
            iVisualiser->HideSelector();

            NotifyObservers( aLooseFocusEvent );
            }
        }
    }

