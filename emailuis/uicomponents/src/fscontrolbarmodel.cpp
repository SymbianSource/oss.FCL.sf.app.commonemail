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
* Description:  Model class for Control Bar component.
*
*/



#include "emailtrace.h"
#include <AknUtils.h>

#include "fscontrolbarmodel.h"
#include "fscontrolbutton.h"
#include "fsgenericpanic.h"
#include "fscontrolbuttonvisualiser.h"
#include "fslayoutmanager.h"

const TUint KDefaultSelectorSpeed = 1000;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFsControlBarModel::CFsControlBarModel() :
    iSelectorTransitionTime( KDefaultSelectorSpeed ),
    iWidthUseDefault( ETrue ),
    iHeightUseDefault( ETrue )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::ConstructL()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsControlBarModel* CFsControlBarModel::NewL()
    {
    FUNC_LOG;
    CFsControlBarModel* self = CFsControlBarModel::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsControlBarModel* CFsControlBarModel::NewLC()
    {
    FUNC_LOG;
    CFsControlBarModel* self = new (ELeave) CFsControlBarModel;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsControlBarModel::~CFsControlBarModel()
    {
    FUNC_LOG;
    // <cmail> Touch
    // Buttons are now alf controls and owned by alf environment, not deleted here
    /*for( TInt i = 0; i < Count(); i++ )
        {
        delete iButtons[i];
        }*/
    // </cmail>

    iButtons.Reset();
    }


// ---------------------------------------------------------------------------
// Adds button object to collection.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::AddButtonL( CFsControlButton& aButton )
    {
    FUNC_LOG;
    iButtons.AppendL( &aButton );
    }


// ---------------------------------------------------------------------------
// Retrieves button from collection with specified index.
// ---------------------------------------------------------------------------
//
CFsControlButton* CFsControlBarModel::ButtonByIndex( TInt aIndex )
    {
    FUNC_LOG;
    CFsControlButton* result( NULL );

    if ( aIndex >= 0 && aIndex < Count() )
        {
        result = iButtons[ aIndex ];
        }

    return result;
    }


// ---------------------------------------------------------------------------
// Retrieves button from collection with specified id.
// Panics if id is incorrect.
// ---------------------------------------------------------------------------
//
CFsControlButton* CFsControlBarModel::ButtonById( TInt aId )
    {
    FUNC_LOG;
    CFsControlButton* button( NULL );
    CFsControlButton* result( NULL );
    TInt i( 0 );

    if ( ECBFirstFocusableButton == aId )
        {
        while ( Count() > i && !result )
            {
            button = ButtonByIndex( i );
            if ( button->IsVisible() && !button->IsDimmed() )
                {
                result = button;
                }
            ++i;
            }
        }
    else
        {
        while ( Count() > i && !result )
            {
            button = ButtonByIndex( i );
            if( button->Id() == aId )
                {
                result = button;
                }
            ++i;
            }

        // panic only when specified id passed by component user is searched
        if ( !result )
            {
            FsGenericPanic( EFsControlButtonIncorrectButtonId );
            }
        }

    return result;
    }


// ---------------------------------------------------------------------------
// Retrieves number of buttons in collection.
// ---------------------------------------------------------------------------
//
TInt CFsControlBarModel::Count() const
    {
    FUNC_LOG;
    return iButtons.Count();
    }


// ---------------------------------------------------------------------------
// Removes button from collection using index.
// Used function ButtonByIndex panics - see ButtonByIndex.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::RemoveButtonByIndex( TInt aIndex )
    {
    FUNC_LOG;
    delete ButtonByIndex( aIndex );
    iButtons.Remove( aIndex );
    }


// ---------------------------------------------------------------------------
// Removes button from collection using id.
// ---------------------------------------------------------------------------
//
TInt CFsControlBarModel::RemoveButtonById( TInt aId )
    {
    FUNC_LOG;
    TInt retVal( KErrNotFound );
    for ( TInt i( 0 ); Count() > i; i++ )
        {
        if( ButtonByIndex( i )->Id() == aId )
            {
            RemoveButtonByIndex( i );
            retVal = KErrNone;
            break;
            }
        }

    if ( retVal )
        {
        FsGenericPanic( EFsControlButtonIncorrectButtonId );
        }

    return retVal;
    }


// ---------------------------------------------------------------------------
// Checks if button with specified id exists in collection.
// ---------------------------------------------------------------------------
//
TBool CFsControlBarModel::ExistsButtonWithId( TInt aId )
    {
    FUNC_LOG;
    TBool result( EFalse );

    for( TInt i( 0 ); iButtons.Count() > i; i++ )
        {
        if( iButtons[i]->Id() == aId )
            {
            result = ETrue;
            break;
            }
        }

    return result;
    }


// ---------------------------------------------------------------------------
// Retrieves index of button with specified id.
// ---------------------------------------------------------------------------
//
TInt CFsControlBarModel::IndexById( TInt aButtonId )
    {
    FUNC_LOG;
    TInt result( -1 );
    TInt i( 0 );

    while ( -1 == result && Count() > i )
        {
        if( ButtonByIndex( i )->Id() == aButtonId )
            {
            result = i;
            }
        i++;
        }

    return result;
    }


// ---------------------------------------------------------------------------
// Retrieves next focusable button.
// ---------------------------------------------------------------------------
//
CFsControlButton* CFsControlBarModel::NextButton(
        TInt aButtonId, TBool aLandscape )
    {
    FUNC_LOG;
    TInt index( KErrNotFound );
    CFsControlButton* currentBtn( ButtonById( aButtonId ) );

    // Find current button's id.
    TInt buttonId( 0 );
    while ( iButtons.Count() > buttonId )
        {
        if ( iButtons[buttonId] == currentBtn )
            {
            break;
            }
        buttonId++;
        }

    // Find the next button on right.
    TAlfRealPoint curPosTarget( currentBtn->Visualiser()->Pos().Target() );
    TInt curPos( curPosTarget.iX );
    if( aLandscape )
        {
        curPos = curPosTarget.iY;
        }

    TBool candidate( EFalse );
    TInt next( 0 );
    for ( TInt i( 0 ); iButtons.Count() > i; i++ )
        {
        if ( i == buttonId
            || !iButtons[i]->IsVisible()
            || iButtons[i]->IsDimmed() )
            {
            continue;
            }
        TAlfRealPoint btnPosTarget( iButtons[i]->Visualiser()->Pos().Target() );
        TInt btnPos( btnPosTarget.iX );
        if( aLandscape )
            {
            btnPos = btnPosTarget.iY;
            }
        if ( btnPos >= curPos )
            {
            if ( btnPos == curPos )
                {
                if ( i < buttonId )
                    {
                    continue;
                    }
                }
            if ( !candidate )
                {
                // This is the first proper candidate.
                index = i;
                next = btnPos;
                candidate = ETrue;
                }
            else if ( btnPos <= next )
                {
                // This button is closer than previously found button.
                index = i;
                next = btnPos;
                if ( btnPos == curPos )
                    {
                    break;
                    }
                }
            }
        }
    return ButtonByIndex( index );
    }


// ---------------------------------------------------------------------------
// Retrieves previous focusable button.
// ---------------------------------------------------------------------------
//
CFsControlButton* CFsControlBarModel::PrevButton(
        TInt aButtonId, TBool aLandscape )
    {
    FUNC_LOG;
    TInt index( KErrNotFound );
    CFsControlButton* currentBtn( ButtonById( aButtonId ) );

    // Find current button's id.
    TInt buttonId( 0 );
    while ( iButtons.Count() > buttonId )
        {
        if ( iButtons[buttonId] == currentBtn )
            {
            break;
            }
        buttonId++;
        }

    // Find the next button on left or up.
    TAlfRealPoint curPosTaget( currentBtn->Visualiser()->Pos().Target() );

    TInt curPos( curPosTaget.iX );
    if( aLandscape )
        {
        curPos = curPosTaget.iY;
        }
    TBool candidate( EFalse );
    TInt next( 0 );
    for ( TInt i( iButtons.Count() - 1 ); 0 <= i; i-- )
        {
        if ( ( i == buttonId ) ||
             ( !iButtons[i]->IsVisible() ) ||
             ( iButtons[i]->IsDimmed() ) )
            {
            continue;
            }
        TAlfRealPoint btnPosTarget( iButtons[i]->Visualiser()->Pos().Target() );
        TInt btnPos( btnPosTarget.iX );
        if( aLandscape )
            {
            btnPos = btnPosTarget.iY;
            }
        if ( btnPos <= curPos )
            {
            if ( btnPos == curPos )
                {
                if ( i > buttonId )
                    {
                    continue;
                    }
                }
            if ( !candidate )
                {
                // This is the first proper candidate.
                index = i;
                next = btnPos;
                candidate = ETrue;
                }
            else if ( btnPos >= next )
                {
                // This button is closer than previously found button.
                index = i;
                next = btnPos;
                if ( btnPos == curPos )
                    {
                    break;
                    }
                }
            }
        }

    return ButtonByIndex( index );
    }


// ---------------------------------------------------------------------------
// Sets focus for control bar.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::SetFocus( TBool aState )
    {
    FUNC_LOG;
    iFocused = aState;
    }


// ---------------------------------------------------------------------------
// Retrieves focus state of control bar.
// ---------------------------------------------------------------------------
//
TBool CFsControlBarModel::IsFocused() const
    {
    FUNC_LOG;
    return iFocused;
    }


// ---------------------------------------------------------------------------
// Generates free id value for new button.
// ---------------------------------------------------------------------------
//
TInt CFsControlBarModel::GenerateButtonId()
    {
    FUNC_LOG;
    TInt result( 0 );

    for ( TInt i( 0 ); Count() > i; i++ )
        {
        if ( result == ButtonByIndex( i )->Id() )
            {
            ++result;
            }
        }

    return result;
    }


// ---------------------------------------------------------------------------
// Retrieves default bar background color.
// ---------------------------------------------------------------------------
//
const TRgb& CFsControlBarModel::BarBgColor() const
    {
    FUNC_LOG;
    return iBgColor;
    }


// ---------------------------------------------------------------------------
// Gets transition time for moving selector.
// ---------------------------------------------------------------------------
//
TInt CFsControlBarModel::SelectorTransitionTime() const
    {
    FUNC_LOG;
    return iSelectorTransitionTime;
    }


// ---------------------------------------------------------------------------
// Sets transition time for moving selector.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::SetSelectorTransitionTime( TInt aTransitionTime )
    {
    FUNC_LOG;
    iSelectorTransitionTime = aTransitionTime;
    }


// ---------------------------------------------------------------------------
// Refresh the buttons' positions.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::UpdateButtonsPositions()
    {
    FUNC_LOG;
    TSize parentSize( Size() );

    for ( TInt i( 0 ); iButtons.Count() > i; i++ )
        {
        iButtons[i]->RefreshButtonPosition( parentSize );
        }
    }

// ---------------------------------------------------------------------------
// Set new position and size
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::SetRect( const TRect& aRect )
    {
    iSize = aRect.Size();
    iWidthUseDefault = EFalse;
    iHeightUseDefault = EFalse;
    iTl = aRect.iTl;
    }

// ---------------------------------------------------------------------------
// Get top-left position of the bar
// ---------------------------------------------------------------------------
//
TPoint CFsControlBarModel::Pos() const
    {
    return iTl;
    }

// ---------------------------------------------------------------------------
// Sets width in pixels of controlbar and refreshes control.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::SetWidth( TInt aWidth )
    {
    FUNC_LOG;
    iSize.iWidth = aWidth;
    iWidthUseDefault = EFalse;
    }


// ---------------------------------------------------------------------------
// Sets height in pixels of controlbar and refreshes control.
// ---------------------------------------------------------------------------
//
void CFsControlBarModel::SetHeight( TInt aHeight )
    {
    FUNC_LOG;
    iSize.iHeight = aHeight;
    iHeightUseDefault = EFalse;
    }


// ---------------------------------------------------------------------------
// Retrieves controlbar's size in pixels.
// ---------------------------------------------------------------------------
//
TSize CFsControlBarModel::Size() const
    {
    FUNC_LOG;
    TRect layoutRect;

    if ( iWidthUseDefault || iHeightUseDefault )
        {
        TRect parentRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EScreen, parentRect );

        CFsLayoutManager::LayoutMetricsRect( parentRect,
            CFsLayoutManager::EFsLmMainSpFsCtrlbarPane, layoutRect );
        }

    return TSize(
        iWidthUseDefault ? layoutRect.Width() : iSize.iWidth,
        iHeightUseDefault ? layoutRect.Height() : iSize.iHeight );
    }

