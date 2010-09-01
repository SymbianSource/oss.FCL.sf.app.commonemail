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
* Description:  Model class for Control Button component.
*
*/



#include "emailtrace.h"
#include <AknsUtils.h>
//<cmail> SF
#include <alf/alflayout.h>
//</cmail>
#include "fscontrolbuttonmodel.h"
#include "fsgenericpanic.h"
#include "fslayoutmanager.h"
#include "fscontrolbar.h"

const TInt KDefaultLabelHPadding1 = 4;
const TInt KDefaultLabelHPadding2 = 4;
const TInt KDefaultLabelVPadding1 = 2;
const TInt KDefaultLabelVPadding2 = 0;

// Number of layout defined button postions on controlbar.
const TInt KMaxButtonLayoutItems = 4;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFsControlButtonModel::CFsControlButtonModel(
    TInt aId,
    TFsControlButtonType aType,
    const TRect& aStartPoint,
    CAlfLayout& aParentLayout ) :
    iType( aType ),
    iId( aId ),
    iFocused( EFalse ),
    iLabelHPadding1( KDefaultLabelHPadding1 ),
    iLabelHPadding2( KDefaultLabelHPadding2 ),
    iLabelVPadding1( KDefaultLabelVPadding1 ),
    iLabelVPadding2( KDefaultLabelVPadding2 ),
    iStartPoint( aStartPoint.iTl ),
    iDimmed( EFalse ),
    iSizeChanged( EFalse ),
    iAutoPosition( ETrue ),
    iPosChanged( EFalse ),
    iSize( aStartPoint.Size() ),
    iAutoSizeMode( MFsControlButtonInterface::EFsLayout ),
    iParentLayout( aParentLayout ),
    iLayoutPos( 0 ),
    iUseLayoutData( ETrue )
    {
    FUNC_LOG;
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), iColorFocused,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG2 );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), iColorNormal,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG7 );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), iColorDimmed,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG61 );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), iColorBackground,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG21 );
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::ConstructL()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsControlButtonModel* CFsControlButtonModel::NewL(
    TInt aId,
    TFsControlButtonType aType,
    const TRect& aRect,
    CAlfLayout& aParentLayout )
    {
    FUNC_LOG;
    CFsControlButtonModel* self(
        new( ELeave )CFsControlButtonModel(
            aId, aType, aRect, aParentLayout ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsControlButtonModel::~CFsControlButtonModel()
    {
    FUNC_LOG;
    delete iFirstTextLine;
    delete iSecondTextLine;
    }


// ---------------------------------------------------------------------------
// Sets id of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetId( TInt aId )
    {
    FUNC_LOG;
    iId = aId;
    }


// ---------------------------------------------------------------------------
// Retrieves button's id.
// ---------------------------------------------------------------------------
//
TInt CFsControlButtonModel::Id()
    {
    FUNC_LOG;
    return iId;
    }


// ---------------------------------------------------------------------------
// Sets type of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetType( TFsControlButtonType aType )
    {
    FUNC_LOG;
    iType = aType;
    }


// ---------------------------------------------------------------------------
// Retrieves button's type.
// ---------------------------------------------------------------------------
//
TFsControlButtonType CFsControlButtonModel::Type()
    {
    FUNC_LOG;
    return iType;
    }


// ---------------------------------------------------------------------------
// Stores text for first line. Allocates or realocates memory
// for specified text.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetTextL(
    const TDesC& aLabel,
    MFsControlButtonInterface::TFsButtonContent aContent )
    {
    FUNC_LOG;
    switch ( aContent )
        {
        case MFsControlButtonInterface::EFsButtonFirstLine:
            delete iFirstTextLine;
            iFirstTextLine = NULL;
            iFirstTextLine = aLabel.AllocL();
            break;

        case MFsControlButtonInterface::EFsButtonSecondLine:
            delete iSecondTextLine;
            iSecondTextLine = NULL;
            iSecondTextLine = aLabel.AllocL();
            break;

        default:
            FsGenericPanic( EFsControlButtonIncorrectButtonType );
            User::Leave( KErrNotSupported );
            break;
        }
    }


// ---------------------------------------------------------------------------
// Change the color of defined target.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetTextColor(
    const TRgb& aColor,
    TFsButtonStateType aType )
    {
    FUNC_LOG;
    switch ( aType )
        {
        case EButtonNormal:
            iColorNormal = aColor;
            break;
        case EButtonFocused:
            iColorFocused = aColor;
            break;
        case EButtonDimmed:
            iColorDimmed = aColor;
            break;
        case EButtonBackground:
            iColorBackground = aColor;
            break;
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// Retrieves the requested target color.
// ---------------------------------------------------------------------------
//
const TRgb& CFsControlButtonModel::TextColor( TFsButtonStateType aType ) const
    {
    FUNC_LOG;
    const TRgb* retVal( NULL );
    switch ( aType )
        {
        case EButtonNormal:
            retVal = &iColorNormal;
            break;
        case EButtonBackground:
            retVal = &iColorBackground;
            break;
        case EButtonFocused:
            retVal = &iColorFocused;
            break;
        case EButtonDimmed:
            retVal = &iColorDimmed;
            break;
        default:
            break;
        }
    return *retVal;
    }


// ---------------------------------------------------------------------------
// Retrieves first line of button's text.
// ---------------------------------------------------------------------------
//
TPtrC CFsControlButtonModel::Text(
    MFsControlButtonInterface::TFsButtonContent aContent )
    {
    FUNC_LOG;
    TPtrC retVal;

    switch ( aContent )
        {
        case MFsControlButtonInterface::EFsButtonFirstLine:
            if( !ContainsElement( ECBElemLabelFirstLine ) )
                {
                FsGenericPanic( EFsControlButtonIncorrectButtonType );
                }
            else
                {
                retVal.Set( iFirstTextLine->Des() );
                }
            break;

        case MFsControlButtonInterface::EFsButtonSecondLine:
            if( !ContainsElement( ECBElemLabelSndLine ) )
                {
                FsGenericPanic( EFsControlButtonIncorrectButtonType );
                }
            else
                {
                retVal.Set( iSecondTextLine->Des() );
                }
            break;

        default:
            FsGenericPanic( EFsControlButtonIncorrectButtonType );
            break;
        }

    return retVal;
    }


// ---------------------------------------------------------------------------
// Retireves horizontal padding for text in button.
// ---------------------------------------------------------------------------
//
TInt CFsControlButtonModel::LabelHPadding(
    TFsControlButtonType aType ) const
    {
    FUNC_LOG;
    TInt retVal( 0 );
    switch ( aType )
        {
        case ECBTypeOneLineLabelOnly:
        case ECBTypeOneLineLabelIconA:
        case ECBTypeOneLineLabelIconB:
        case ECBTypeOneLineLabelTwoIcons:
            retVal = iLabelHPadding1;
            break;

        case ECBTypeTwoLinesLabelOnly:
        case ECBTypeTwoLinesLabelIconA:
        case ECBTypeTwoLinesLabelIconB:
        case ECBTypeTwoLinesLabelTwoIcons:
            retVal = iLabelHPadding2;
            break;

        default:
            break;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// Retireves vertical padding for text in button.
// ---------------------------------------------------------------------------
//
TInt CFsControlButtonModel::LabelVPadding(
    TFsControlButtonType aType ) const
    {
    FUNC_LOG;
    TInt retVal( 0 );
    switch ( aType )
        {
        case ECBTypeOneLineLabelOnly:
        case ECBTypeOneLineLabelIconA:
        case ECBTypeOneLineLabelIconB:
        case ECBTypeOneLineLabelTwoIcons:
            retVal = iLabelVPadding1;
            break;

        case ECBTypeTwoLinesLabelOnly:
        case ECBTypeTwoLinesLabelIconA:
        case ECBTypeTwoLinesLabelIconB:
        case ECBTypeTwoLinesLabelTwoIcons:
            retVal = iLabelVPadding2;
            break;

        default:
            break;
        }
    return retVal;
    }


// ---------------------------------------------------------------------------
// Sets width of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetWidth( TInt aWidth )
    {
    FUNC_LOG;
    TInt width( iSize.iWidth );

    if ( iSize.iWidth != aWidth )
        {
        iSizeChanged = ETrue;
        }
    iSize.iWidth = aWidth;

    if ( MFsControlButtonInterface::EFsLayout != AutoSizeMode()
        && CFsLayoutManager::IsMirrored() )
        {
        TPoint pos( TopLeftPoint() );
        pos.iX = pos.iX - Size().iWidth + width;
        SetPos( pos, EFalse );
        }
    }


// ---------------------------------------------------------------------------
// Set size for the button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetSize( TSize aSize )
    {
    FUNC_LOG;
    if ( iSize != aSize )
        {
        iSizeChanged = ETrue;
        }
    iSize = aSize;
    }


// ---------------------------------------------------------------------------
// Retrieves the size of the button.
// ---------------------------------------------------------------------------
//
const TSize& CFsControlButtonModel::Size() const
    {
    FUNC_LOG;
    return iSize;
    }


// ---------------------------------------------------------------------------
// Set auto size mode for button.
// Defines how the buttons size is changed.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetAutoSizeMode(
    MFsControlButtonInterface::TFsAutoSizeMode aAutoSizeMode )
    {
    FUNC_LOG;
    if ( iAutoSizeMode != aAutoSizeMode )
        {
        iSizeChanged = ETrue;
        iAutoSizeMode = aAutoSizeMode;

        // Update manual position
        iManualSetPos = TopLeftPoint();
        if ( CFsLayoutManager::IsMirrored() )
            {
            iManualSetPos.iX = iParentLayout.Size().Target().iX
                - iManualSetPos.iX - Size().iWidth;
            }
        }
    }


// ---------------------------------------------------------------------------
// Resolve the current mode.
// ---------------------------------------------------------------------------
//
MFsControlButtonInterface::TFsAutoSizeMode
    CFsControlButtonModel::AutoSizeMode()
    {
    return iAutoSizeMode;
    }


// ---------------------------------------------------------------------------
// Sets focus to control button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetFocus( TBool aState )
    {
    FUNC_LOG;
    iFocused = aState;
    }


// ---------------------------------------------------------------------------
// Checks if button has focus.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::IsFocused()
    {
    FUNC_LOG;
    return iFocused;
    }


// ---------------------------------------------------------------------------
// Sets dimmed state of control button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetDimmed( TBool aDimmed )
    {
    FUNC_LOG;
    iDimmed = aDimmed;
    }


// ---------------------------------------------------------------------------
// Checks if button is dimmed (read only).
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::IsDimmed()
    {
    FUNC_LOG;
    return iDimmed;
    }


// ---------------------------------------------------------------------------
// Sets icon in the button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetIconL(
    CAlfTexture& aIcon,
    TFsControlButtonElem aElemType )
    {
    FUNC_LOG;
    if( !ContainsElement( aElemType ) )
        {
        FsGenericPanic( EFsControlButtonIncorrectButtonType );
        User::Leave( KErrNotSupported );
        }

    if ( aElemType == ECBElemIconA )
        {
        iIconA = &aIcon;
        }
    else
        {
        iIconB = &aIcon;
        }
    }


// ---------------------------------------------------------------------------
// Retrieves specified icon image.
// ---------------------------------------------------------------------------
//
CAlfTexture* CFsControlButtonModel::Icon( TFsControlButtonElem aElemType )
    {
    FUNC_LOG;
    CAlfTexture* result( NULL );

    if( !ContainsElement( aElemType ) )
        {
        FsGenericPanic( EFsControlButtonIncorrectButtonType );
        }
    else if ( aElemType == ECBElemIconA )
        {
        result = iIconA;
        }
    else
        {
        result = iIconB;
        }

    return result;
    }

// <cmail> Platform layout changes
// ---------------------------------------------------------------------------
// Sets position of the button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetPos( const TPoint& aTlPoint, TBool aClearFlag )
    {
    FUNC_LOG;
    if ( iStartPoint != aTlPoint )
        {
        iPosChanged = ETrue;
        }
    iStartPoint = aTlPoint;
    if ( aClearFlag )
        {
        iAutoPosition = EFalse;
        iManualSetPos = aTlPoint;
        }
    }
// <cmail> Platform layout changes


// ---------------------------------------------------------------------------
// Retrieves top left point of the button.
// ---------------------------------------------------------------------------
//
TPoint CFsControlButtonModel::TopLeftPoint()
    {
    FUNC_LOG;
    return iStartPoint;
    }


// ---------------------------------------------------------------------------
// Checks if current set button type contains specified icon type.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::ContainsElement( TFsControlButtonElem aWhich )
    {
    FUNC_LOG;
    TBool result( EFalse );

    switch ( aWhich )
        {
        case ECBElemIconA:
            {
            if ( iType == ECBTypeIconOnly
                || iType == ECBTypeOneLineLabelIconA
                || iType == ECBTypeOneLineLabelTwoIcons
                || iType == ECBTypeTwoLinesLabelIconA
                || iType == ECBTypeOneLineLabelTwoIcons
                || iType == ECBTypeTwoLinesLabelTwoIcons )
                {
                result = ETrue;
                }
            break;
            }

        case ECBElemIconB:
            {
            if ( iType == ECBTypeOneLineLabelIconB
                || iType == ECBTypeOneLineLabelTwoIcons
                || iType == ECBTypeTwoLinesLabelIconB
                || iType == ECBTypeTwoLinesLabelTwoIcons )
                {
                result = ETrue;
                }
            break;
            }

        case ECBElemLabelFirstLine:
            {
            if ( iType != ECBTypeIconOnly )
                {
                result = ETrue;
                }
            break;
            }

        case ECBElemLabelSndLine:
            {
            if ( iType == ECBTypeTwoLinesLabelOnly
                || iType == ECBTypeTwoLinesLabelIconA
                || iType == ECBTypeTwoLinesLabelIconB
                || iType == ECBTypeTwoLinesLabelTwoIcons )
                {
                result = ETrue;
                }
            break;
            }

        default:
            {
            FsGenericPanic( EFsControlButtonIncorrectButtonElement );
            break;
            }
        }

    return result;
    }


// ---------------------------------------------------------------------------
// Checks if size of button was changed.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::ButtonSizeChanged()
    {
    FUNC_LOG;
    TBool result( iSizeChanged );
    iSizeChanged = EFalse;
    return result;
    }


// ---------------------------------------------------------------------------
// Checks if position of button was changed.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::ButtonPosChanged()
    {
    FUNC_LOG;
    TBool result( iPosChanged );
    iPosChanged = EFalse;
    return result;
    }


// ---------------------------------------------------------------------------
// Checks if button should be automatically positioned.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::AutoPosition()
    {
    FUNC_LOG;
    return iAutoPosition;
    }


// ---------------------------------------------------------------------------
// Resolve the button's size from layout.
// ---------------------------------------------------------------------------
//
TSize CFsControlButtonModel::GetLayoutSize()
    {
    FUNC_LOG;
    TRect parentRect( TPoint( 0, 0 ), iParentLayout.Size().Target() );
    return GetLayoutSize( parentRect );
    }


// ---------------------------------------------------------------------------
// Resolve the button's size from layout.
// ---------------------------------------------------------------------------
//
TSize CFsControlButtonModel::GetLayoutSize( TRect& aParentRect )
    {
    FUNC_LOG;
    CFsLayoutManager::TFsLayoutMetrics layoutItem(
        CFsLayoutManager::EFsLmMainSpFsCtrlbarPaneG1 );
    switch ( Type() )
        {
        case ECBTypeIconOnly:
            if ( 4 == iLayoutPos && iUseLayoutData )
                {
                layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarPaneG2;
                }
            else
                {
                layoutItem = CFsLayoutManager::EFsLmMainSpFsCtrlbarPaneG1;
                }
            break;
        case ECBTypeOneLineLabelOnly:
        case ECBTypeOneLineLabelIconA:
        case ECBTypeOneLineLabelIconB:
        case ECBTypeOneLineLabelTwoIcons:
        case ECBTypeTwoLinesLabelOnly:
        case ECBTypeTwoLinesLabelIconA:
        case ECBTypeTwoLinesLabelIconB:
        case ECBTypeTwoLinesLabelTwoIcons:
            if ( 3 == iLayoutPos && iUseLayoutData )
                {
                layoutItem =
                    CFsLayoutManager::EFsLmMainSpFsCtrlbarButtonPaneCp01;
                }
            else
                {
                layoutItem =
                    CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPane;
                }
            break;
        default:
            break;
        }

    TSize layoutSize( 0, 0 );

    CFsLayoutManager::LayoutMetricsSize( aParentRect,
        layoutItem, layoutSize,
        0 );

    return layoutSize;
    }


// ---------------------------------------------------------------------------
// Get the layout position on controlbar.
// ---------------------------------------------------------------------------
//
TInt CFsControlButtonModel::GetLayoutPos()
    {
    FUNC_LOG;
    return iLayoutPos;
    }


// ---------------------------------------------------------------------------
// Set the layout position index on controlbar.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::SetParentIndex( TInt aLayoutPos )
    {
    FUNC_LOG;
    if ( 0 >= aLayoutPos || KMaxButtonLayoutItems < aLayoutPos )
        {
        iUseLayoutData = EFalse;
        }
    else
        {
        SetSize( GetLayoutSize() );
        TPoint buttonPos( CFsControlBar::GetLayoutRect( aLayoutPos ).iTl );
        SetPos( buttonPos, EFalse );
        // Auto position flag needs to be reseted every time.
        iAutoPosition = ETrue;
        iUseLayoutData = ETrue;
        }
    iLayoutPos = aLayoutPos;
    }


// ---------------------------------------------------------------------------
// Resolve if layout data is used to define the size of the button.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonModel::IsLayoutSize()
    {
    FUNC_LOG;
    return iUseLayoutData;
    }


// ---------------------------------------------------------------------------
// Refresh button's position.
// ---------------------------------------------------------------------------
//
void CFsControlButtonModel::RefreshButtonPosition( const TSize& aParentSize )
    {
    FUNC_LOG;
    TPoint point;

    if ( iUseLayoutData && AutoPosition()
        && MFsControlButtonInterface::EFsLayout == AutoSizeMode() )
        {
        TRect layoutRect( CFsControlBar::GetLayoutRect( GetLayoutPos() ) );

        SetSize( layoutRect.Size() );
        point = layoutRect.iTl;
        }
    else
        {
        TRect parentRect( TPoint( 0, 0 ), aParentSize );
        if ( CFsLayoutManager::IsMirrored() )
            {
            point.iX = parentRect.Width() - iManualSetPos.iX - iSize.iWidth;
            point.iY = iStartPoint.iY;
            }
        else
            {
            point = iManualSetPos;
            }
        }

    SetPos( point, EFalse );
    }

