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
* Description:  Main class for Control Button component.
*
*/



// <cmail> SF
#include "emailtrace.h"
#include <alf/alfevent.h>
#include <alf/alfdecklayout.h>
// </cmail>
// Needed for pointer events.
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>

#include <AknUtils.h>

#include "fscontrolbutton.h"
#include "fscontrolbuttonmodel.h"
#include "fscontrolbuttonvisualiser.h"
#include "fscontrolbuttonobserver.h"
#include "fsgenericpanic.h"
#include "fstriggeredcomponent.h"
#include "fstextstylemanager.h"
#include "fslayoutmanager.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFsControlButton::CFsControlButton(
    MFsControlButtonObserver& aParent,
    CAlfDeckLayout& aParentLayout ) :
    iParent( aParent ),
    iParentLayout( aParentLayout )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsControlButton::ConstructL(
    CAlfEnv& aEnv,
    TInt aId,
    TFsControlButtonType aType,
    TRect& aStartPoint,
    CFsTextStyleManager* aTextStyleManager )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );

    iModel = CFsControlButtonModel::NewL(
        aId, aType, aStartPoint, iParentLayout );

    iVisualiser = CFsControlButtonVisualiser::NewL(
        *this, iParentLayout, aTextStyleManager );
    iVisualiser->InitializeL( *iModel );
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsControlButton* CFsControlButton::NewL(
    CAlfEnv& aEnv,
    MFsControlButtonObserver& aParent,
    TInt aId,
    TFsControlButtonType aType,
    TRect& aStartPoint,
    CAlfDeckLayout& aParentLayout,
    CFsTextStyleManager* aTextStyleManager )
    {
    FUNC_LOG;
    CFsControlButton* self =
        new( ELeave ) CFsControlButton( aParent, aParentLayout );
    CleanupStack::PushL( self );
    self->ConstructL(
        aEnv, aId, aType, aStartPoint, aTextStyleManager );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsControlButton::~CFsControlButton()
    {
    FUNC_LOG;
    delete iModel;
    delete iVisualiser;
    }


// ---------------------------------------------------------------------------
// Sets first line of text in button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetTextL(
    const TDesC& aLabel,
    TFsButtonContent aContent )
    {
    FUNC_LOG;
    iModel->SetTextL( aLabel, aContent );
    switch ( aContent )
        {
        case EFsButtonFirstLine:
            iVisualiser->UpdateElementL( ECBElemLabelFirstLine );
            break;
        case EFsButtonSecondLine:
            iVisualiser->UpdateElementL( ECBElemLabelSndLine );
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }


// ---------------------------------------------------------------------------
// Sets visualiser for the button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetVisualiserL(
    CFsControlButtonVisualiser* aVisualiser )
    {
    FUNC_LOG;
    __ASSERT_ALWAYS( aVisualiser, User::Leave( KErrArgument ) );

    CFsControlButtonVisualiser* newVisualiser( aVisualiser );
    CleanupStack::PushL( newVisualiser );

    newVisualiser->SetParentControlL( this );
    newVisualiser->SetParentLayoutL( &iParentLayout );
    newVisualiser->InitializeL( *iModel );

    CleanupStack::Pop( newVisualiser );

    delete iVisualiser;
    iVisualiser = newVisualiser;
    }


// ---------------------------------------------------------------------------
// Retrieves button's visualiser.
// ---------------------------------------------------------------------------
//
CFsControlButtonVisualiser* CFsControlButton::Visualiser()
    {
    FUNC_LOG;
    return iVisualiser;
    }


// ---------------------------------------------------------------------------
// Set the layout position index on controlbar.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetParentIndex( TInt aLayoutPos )
    {
    FUNC_LOG;
    iModel->SetParentIndex( aLayoutPos );
    }


// ---------------------------------------------------------------------------
// Retrieves id of the button.
// ---------------------------------------------------------------------------
//
TInt CFsControlButton::Id() const
    {
    FUNC_LOG;
    return iModel->Id();
    }


// ---------------------------------------------------------------------------
// Sets icon A or B. Panics if button's type is incorrect.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetIconL(
    CAlfTexture& aIcon,
    TFsControlButtonElem aWhich )
    {
    FUNC_LOG;
    iModel->SetIconL( aIcon, aWhich );
    iVisualiser->UpdateElementL( aWhich );
    }


// ---------------------------------------------------------------------------
// Sets width of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetWidth( TInt aWidth )
    {
    FUNC_LOG;
    iModel->SetAutoSizeMode( MFsControlButtonInterface::EFsManual );
    iModel->SetWidth( aWidth );
    iVisualiser->Refresh();
    }

// <cmail> Platform layout changes
// ---------------------------------------------------------------------------
// Sets size of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetSize( const TSize& aSize )
    {
    FUNC_LOG;
    iModel->SetAutoSizeMode( MFsControlButtonInterface::EFsManual );
    iModel->SetSize( aSize );
    iVisualiser->Refresh();
    }
// </cmail> Platform layout changes

// ---------------------------------------------------------------------------
// Set auto size mode for button.
// Defines how the buttons size is changed.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetAutoSizeMode( TFsAutoSizeMode aAutoSizeMode )
    {
    FUNC_LOG;
    TBool refresh( aAutoSizeMode != iModel->AutoSizeMode() );
    iModel->SetAutoSizeMode( aAutoSizeMode );
    if ( refresh )
        {
        iVisualiser->Refresh();
        }
    }


// ---------------------------------------------------------------------------
// Sets focus to control button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetFocus( TBool aState )
    {
    FUNC_LOG;
    iModel->SetFocus( aState );
    iVisualiser->Refresh();
    }


// ---------------------------------------------------------------------------
// Refresh the button's position.
// ---------------------------------------------------------------------------
//
void CFsControlButton::RefreshButtonPosition( TSize& aParentSize )
    {
    FUNC_LOG;
    iModel->RefreshButtonPosition( aParentSize );
    }


// ---------------------------------------------------------------------------
// Checks if button has focus.
// ---------------------------------------------------------------------------
//
TBool CFsControlButton::IsFocused() const
    {
    FUNC_LOG;
    return iModel->IsFocused();
    }

// <cmail> Platform layout changes
// ---------------------------------------------------------------------------
// Sets position of the button (top left point).
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetPos( const TPoint& aTlPoint )
    {
    FUNC_LOG;
    iModel->SetPos( aTlPoint );
    iModel->SetAutoSizeMode( MFsControlButtonInterface::EFsManual );

    // Refresh buttons position to apply mirroring to it.
    TSize parent( iParentLayout.Size().Target().AsSize() );
    iModel->RefreshButtonPosition( parent );

    iVisualiser->Refresh();
    }
// </cmail> Platform layout changes


// ---------------------------------------------------------------------------
// Retrieves position of button.
// ---------------------------------------------------------------------------
//
const TAlfTimedPoint CFsControlButton::Pos() const
    {
    FUNC_LOG;
    return iVisualiser->Layout()->Pos();
    }


// ---------------------------------------------------------------------------
// Retrieves size of button.
// ---------------------------------------------------------------------------
//
const TAlfTimedPoint CFsControlButton::Size() const
    {
    FUNC_LOG;
    return iVisualiser->Layout()->Size();
    }


// ---------------------------------------------------------------------------
// Sets component to button which will be triggered when button
// is pressed.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetTriggeredComponent(
    MFsTriggeredComponent& aComponent )
    {
    FUNC_LOG;
    iTriggeredComponent = &aComponent;
    }


// ---------------------------------------------------------------------------
// Clears triggered component. No more events to triggered
// component are sent.
// ---------------------------------------------------------------------------
//
void CFsControlButton::ClearTriggeredComponent()
    {
    FUNC_LOG;
    iTriggeredComponent = NULL;
    }


// ---------------------------------------------------------------------------
// Sets alignment of element.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetElemAlignL(
    TFsControlButtonElem aButtonElem,
    TAlfAlignHorizontal aHAlign,
    TAlfAlignVertical aVAlign )
    {
    FUNC_LOG;
    iVisualiser->SetElemAlignL( aButtonElem, aHAlign, aVAlign );
    iVisualiser->Refresh();
    }


// ---------------------------------------------------------------------------
// Sets dimm state of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetDimmed( TBool aDimmed )
    {
    FUNC_LOG;
    if ( aDimmed != IsDimmed() )
        {
        iModel->SetDimmed( aDimmed );
        iVisualiser->Refresh();

        iParent.HandleButtonEvent(
            MFsControlButtonObserver::EEventButtonDimmStateChanged,
            iModel->Id() );
        }
    }


// ---------------------------------------------------------------------------
// Checks dimm state of the button.
// ---------------------------------------------------------------------------
//
TBool CFsControlButton::IsDimmed() const
    {
    FUNC_LOG;
    return iModel->IsDimmed();
    }


// ---------------------------------------------------------------------------
// Shows (enables) button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::ShowButtonL()
    {
    FUNC_LOG;
    if ( !iVisualiser->IsVisible() )
        {
        iVisualiser->ShowL();

        iParent.HandleButtonEvent(
            MFsControlButtonObserver::EEventButtonVisibilityChanged,
            iModel->Id() );
        }
    }


// ---------------------------------------------------------------------------
// Hides (disables) button.
// ---------------------------------------------------------------------------
//
void CFsControlButton::HideButton()
    {
    FUNC_LOG;
    if ( iVisualiser->IsVisible() )
        {
        iVisualiser->Hide();

        iParent.HandleButtonEvent(
            MFsControlButtonObserver::EEventButtonVisibilityChanged,
            iModel->Id() );
        }
    }


// ---------------------------------------------------------------------------
// Checks if button is visible or hidden.
// ---------------------------------------------------------------------------
//
TBool CFsControlButton::IsVisible() const
    {
    FUNC_LOG;
    return iVisualiser->IsVisible();
    }


// ---------------------------------------------------------------------------
// Sets button's background image.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetBackgroundImageL( CAlfImageBrush* aImage )
    {
    FUNC_LOG;
    iVisualiser->SetBackgroundImageL( aImage );
    }


// ---------------------------------------------------------------------------
// Sets button's background color.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetBackgroundColor( const TRgb& aColor )
    {
    FUNC_LOG;
    iModel->SetTextColor( aColor, CFsControlButtonModel::EButtonBackground );
    iVisualiser->SetBackgroundColor( aColor );
    }


// ---------------------------------------------------------------------------
// Clears button's background color. Button is transparent.
// ---------------------------------------------------------------------------
//
void CFsControlButton::ClearBackgroundColor()
    {
    FUNC_LOG;
    iVisualiser->ClearBackgroundColor();
    }


// ---------------------------------------------------------------------------
// Clears button's background image.
// ---------------------------------------------------------------------------
//
void CFsControlButton::ClearBackgroundImage()
    {
    FUNC_LOG;
    iVisualiser->ClearBackgroundImage();
    }


// ---------------------------------------------------------------------------
// Retrieves text from control button.
// ---------------------------------------------------------------------------
//
TPtrC CFsControlButton::Text( TFsButtonContent aContent ) const
    {
    FUNC_LOG;
    return iModel->Text( aContent );
    }


// ---------------------------------------------------------------------------
// Set new height for the button text.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetTextHeight( const TInt aTextHeight )
    {
    FUNC_LOG;
    iVisualiser->SetTextHeight( aTextHeight );
    }


// ---------------------------------------------------------------------------
// Change the current font.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetTextFontL( const TFontSpec& aFontSpec )
    {
    FUNC_LOG;
    iVisualiser->SetTextFontL( aFontSpec );
    }


// ---------------------------------------------------------------------------
// Set button's text color when it's not focused or dimmed.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetNormalTextColor( const TRgb& aColor )
    {
    FUNC_LOG;
    iModel->SetTextColor( aColor, CFsControlButtonModel::EButtonNormal );
    iVisualiser->Refresh();
    }


// ---------------------------------------------------------------------------
// Set button's text color when it's focused.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetFocusedTextColor( const TRgb& aColor )
    {
    FUNC_LOG;
    iModel->SetTextColor( aColor, CFsControlButtonModel::EButtonFocused );
    iVisualiser->Refresh();
    }


// ---------------------------------------------------------------------------
// Set button's text color when it's dimmed.
// ---------------------------------------------------------------------------
//
void CFsControlButton::SetDimmedTextColor( const TRgb& aColor )
    {
    FUNC_LOG;
    iModel->SetTextColor( aColor, CFsControlButtonModel::EButtonDimmed );
    iVisualiser->Refresh();
    }


// ---------------------------------------------------------------------------
// Retrieves control button type.
// ---------------------------------------------------------------------------
//
TFsControlButtonType CFsControlButton::ControlButtonType() const
    {
    FUNC_LOG;
    return iModel->Type();
    }

    // <cmail> Touch
// ---------------------------------------------------------------------------
// Returns this as CAlfControl
// ---------------------------------------------------------------------------
//
CAlfControl* CFsControlButton::AsAlfControl()
    {
    return this;
    }
    // </cmail>

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsControlButton::MakeFocusVisible( TBool aShow )
    {
    FUNC_LOG;
    iVisualiser->MakeFocusVisible( aShow );
    }

// ---------------------------------------------------------------------------
// From class CHuiControl/CAlfControl.
// Called when an input event is being offered to the control.
// ---------------------------------------------------------------------------
//
TBool CFsControlButton::OfferEventL( const TAlfEvent& aEvent )
    {
    FUNC_LOG;
    TBool result( EFalse );

    // no key handling when not shown or dimmed.
    if ( !IsVisible() || IsDimmed() )
        {
        return result;
        }

    if ( aEvent.IsKeyEvent() )
        {
        switch ( aEvent.Code() )
            {
            case EEventKeyDown:
                {
                if ( aEvent.KeyEvent().iScanCode == EStdKeyDevice3 )
                    {
                    iParent.HandleButtonEvent(
                        MFsControlButtonObserver::EEventButtonPressed,
                        -1 );
                    result = ETrue;
                    }
                break;
                }

            case EEventKey:
                {
                if ( aEvent.KeyEvent().iScanCode == EStdKeyDevice3 )
                    {
                    iParent.HandleButtonEvent(
                        MFsControlButtonObserver::EEventButton,
                        iModel->Id() );
                    result = ETrue;

                    if ( iTriggeredComponent )
                        {
                        iTriggeredComponent->OpenComponent();
                        }
                    }
                break;
                }

            case EEventKeyUp:
                {
                if ( aEvent.KeyEvent().iScanCode == EStdKeyDevice3 )
                    {
                    iParent.HandleButtonEvent(
                        MFsControlButtonObserver::EEventButtonReleased,
                        -1 );
                    result = ETrue;
                    }
                break;
                }

            default:
                {
                // event not consumed - result still got value EFalse
                break;
                }
            }
        }
    else if ( aEvent.IsPointerEvent() )
        {
        const TPointerEvent& pointerEvent( aEvent.PointerEvent() );
        switch (pointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
                {
                if( HitTest( aEvent.PointerEvent().iParentPosition ) )
                    {
                    iTouchPressed = ETrue;
                    GrabPointerEvents( ETrue );
                    result = iParent.HandleButtonEvent(
                        MFsControlButtonObserver::EEventButtonTouchPressed,
                        iModel->Id() );
                    }
                break;
                }
            case TPointerEvent::EButton1Up:
                {
                if( iTouchPressed )
                    {
                    GrabPointerEvents( EFalse );

                    TInt buttonId( KErrNotFound );
                    if( HitTest( aEvent.PointerEvent().iParentPosition ) )
                        {
                        buttonId = iModel->Id();
                        }
                    result = iParent.HandleButtonEvent(
                        MFsControlButtonObserver::EEventButtonTouchReleased,
                        buttonId );
                    iTouchPressed = EFalse;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsControlButton::GrabPointerEvents( TBool aGrab )
    {
    CAlfDisplay* display = Display();
    if( display )
        {
        if ( aGrab )
            {
            display->Roster().AddPointerEventObserver(
                EAlfPointerEventReportUnhandled, *this );
            }
        else
            {
            display->Roster().RemovePointerEventObserver(
                EAlfPointerEventReportUnhandled, *this );
            }
        }
    }
