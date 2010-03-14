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
* Description:  Implementation of CFsFastAddressingVisualizer class
*
*/


#include "emailtrace.h"
#include <alf/alfAnchorLayout.h>
#include <alf/alfFlowLayout.h>
#include <alf/alfimagevisual.h>
#include <coecntrl.h>

#include "fsfastaddressingvisualizer.h"
#include "fstextinputvisual.h"
#include "fsgenericpanic.h"

//Height of text input field in pixels
const TInt KFixedTexFieldHeight = 20;

const TInt KFixedSpacing = 5; //Spacing in pixels
const TInt KOrdinalTextField = 0;
const TInt KOrdinalListField = 2;
const TReal32 KOpacityVisible = 1;
const TReal32 KOpacityInvisible = 0;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFsFastAddressingVisualizer::CFsFastAddressingVisualizer( CAlfControl& aOwner )
    : iControlOwner( aOwner ),
    iListPosition( CFsFastAddressingList::EListBelowTextField )
    {
    FUNC_LOG;
    //No implementation needed
    }

// ---------------------------------------------------------------------------
// Two phased constructor
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::ConstructL( CCoeControl& aTextControl )
    {
    FUNC_LOG;
    CAlfLayout* parent = CAlfLayout::AddNewL( iControlOwner );
    iRootLayout = CAlfAnchorLayout::AddNewL( iControlOwner, parent );
    iTextField = CAlfFlowLayout::AddNewL( iControlOwner,
                                          iRootLayout );
    iTextField->SetFlowDirection( CAlfFlowLayout::EFlowHorizontal );

    iItemsList = CAlfLayout::AddNewL( iControlOwner, iRootLayout );
    iIconA = CAlfImageVisual::AddNewL( iControlOwner, iTextField );
    iTextInputVisual = CFsTextInputVisual::AddNewL( iControlOwner,
                                                    aTextControl,
                                                    iTextField );
    iIconB = CAlfImageVisual::AddNewL( iControlOwner, iTextField );
    iTextFieldVisible = ETrue;
//----TO BE REMOVED
    //Set size of the component
    // this part of code exists only for tests purpose
    //<cmail> removed from cmail
    //iRootLayout->SetRect( TAlfRealRect( TRect( 0, 0, 240, 150  ) ), 0 );
    //</cmail>
//----END OF TO BE REMOVED
    SetAnchors();
    }

// ---------------------------------------------------------------------------
// Constructs and returns an CFsFastAddressingVisualizer object.
// ---------------------------------------------------------------------------
//
CFsFastAddressingVisualizer* CFsFastAddressingVisualizer::NewL(
    CAlfControl& aOwner,
    CCoeControl& aTextControl
    /*CCoeControl& aDispControl */)
    {
    FUNC_LOG;
    CFsFastAddressingVisualizer* self =
        CFsFastAddressingVisualizer::NewLC( aOwner,
                                            aTextControl );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructs and returns an CFsFastAddressingVisualizer object.
// ---------------------------------------------------------------------------
//
CFsFastAddressingVisualizer* CFsFastAddressingVisualizer::NewLC(
    CAlfControl& aOwner,
    CCoeControl& aTextControl )
    {
    FUNC_LOG;
    CFsFastAddressingVisualizer* self =
        new( ELeave ) CFsFastAddressingVisualizer( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aTextControl );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsFastAddressingVisualizer::~CFsFastAddressingVisualizer()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Sets icon image.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::SetIcon(
    const CFsFastAddressingList::TIconID aIconID,
    const TAlfImage& aImage )
    {
    FUNC_LOG;
    if ( aIconID == CFsFastAddressingList::EIconA )
        {
        iIconA->SetImage( aImage );
        iIconA->SetSize( TAlfRealSize( KFixedTexFieldHeight,
                                     KFixedTexFieldHeight ) );
        }
    else if ( aIconID == CFsFastAddressingList::EIconB )
        {
        iIconB->SetImage( aImage );
        iIconB->SetSize( TAlfRealSize( KFixedTexFieldHeight,
                                      KFixedTexFieldHeight ) );
        }
        SetAnchors();
    }

// ---------------------------------------------------------------------------
// Controls icon visibility.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::SetIconVisible(
    const CFsFastAddressingList::TIconID aIconID,
    const TBool aVisible )
    {
    FUNC_LOG;
    CAlfImageVisual* icon(NULL);
    if ( aIconID == CFsFastAddressingList::EIconA )
        {
        icon = iIconA;
        }
    else if ( aIconID == CFsFastAddressingList::EIconB )
        {
        icon = iIconB;
        }
    else
        {
        FsGenericPanic( EFsFastAddressingListBadIconId );
        }

    const TAlfImage& img = icon->Image();
    if ( !img.HasTexture())
        {
        FsGenericPanic( EFsFastAddressingListTextureNotSet );
        }

    if ( aVisible )
        {
        TAlfTimedValue opacity( icon->Opacity() );
        opacity.SetValueNow( KOpacityVisible );
        icon->SetOpacity( opacity );
        icon->SetSize( TAlfRealSize( KFixedTexFieldHeight,
                                   KFixedTexFieldHeight ) );
        }
    else
        {
        TAlfTimedValue opacity( icon->Opacity() );
        opacity.SetValueNow( KOpacityVisible );
        icon->SetOpacity( opacity );
        icon->SetSize( TAlfRealSize( 0, 0 ) );
        }
    SetAnchors();
    }


// ---------------------------------------------------------------------------
// Resturns status of the icon visibility.
// ---------------------------------------------------------------------------
//
TBool CFsFastAddressingVisualizer::IconVisible(
    const CFsFastAddressingList::TIconID aIconID ) const
    {
    FUNC_LOG;
    TBool result( EFalse );

    switch ( aIconID )
        {
        case CFsFastAddressingList::EIconA:
            {
            result = TBool( iIconA->Opacity().ValueNow() == KOpacityInvisible );
            break;
            }

        case CFsFastAddressingList::EIconB:
            {
            result = TBool( iIconB->Opacity().ValueNow() == KOpacityInvisible );
            break;
            }

        default:
            {
            FsGenericPanic( EFsFastAddressingListBadIconId );
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Redraws whole fast addressing list component.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::Refresh()
    {
    FUNC_LOG;
    //TO DO
    }

// ---------------------------------------------------------------------------
// Controls visibility of the component
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::SetVisible(const TBool /*aVisible*/)
    {
    FUNC_LOG;
    //TO DO
    }

// ---------------------------------------------------------------------------
// Move selection up.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::MoveSelectionUp()
    {
    FUNC_LOG;
    //TO DO
    }

// ---------------------------------------------------------------------------
// Move selection down.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::MoveSelectionDown()
    {
    FUNC_LOG;
    //TO DO
    }

// ---------------------------------------------------------------------------
// Retuns latyout for the text input field
// ---------------------------------------------------------------------------
//
CAlfLayout* CFsFastAddressingVisualizer::GetTextLayout() const
    {
    FUNC_LOG;
    return iTextField;
    }


// ---------------------------------------------------------------------------
// Controls visibility of the text field
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::SetTextFieldVisible( const TBool aVisible )
    {
    FUNC_LOG;
    iTextFieldVisible = aVisible;
    if ( aVisible )
        {
        TAlfTimedValue opacity( iTextInputVisual->Opacity() );
        opacity.SetValueNow( KOpacityVisible );
        iTextInputVisual->SetOpacity( opacity );
        }
    else
        {
        TAlfTimedValue opacity( iTextInputVisual->Opacity() );
        opacity.SetValueNow( KOpacityInvisible );
        iTextInputVisual->SetOpacity( opacity );
        }
    }

// ---------------------------------------------------------------------------
// Returns status of the text field visibility
// ---------------------------------------------------------------------------
//
TBool CFsFastAddressingVisualizer::IsTextFieldVisible() const
    {
    FUNC_LOG;
    return iTextFieldVisible;
    }

// ---------------------------------------------------------------------------
// Sets list position (above/below of the text input field)
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::SetListPosition(
    const CFsFastAddressingList::TFsListPosition aPos )
    {
    FUNC_LOG;
    iListPosition = aPos;
    SetAnchors();
    }

// ---------------------------------------------------------------------------
// Returns the list position (above/below of the text input field)
// ---------------------------------------------------------------------------
//
CFsFastAddressingList::TFsListPosition
    CFsFastAddressingVisualizer::ListPosition() const
    {
    return iListPosition;
    }

// ---------------------------------------------------------------------------
// Returns pointer to the main layout
// ---------------------------------------------------------------------------
//
CAlfLayout* CFsFastAddressingVisualizer::GetMainLayout() const
    {
    FUNC_LOG;
    return iRootLayout;
    }

// ---------------------------------------------------------------------------
// Returns layout for list component
// ---------------------------------------------------------------------------
//
CAlfLayout* CFsFastAddressingVisualizer::GetListLayout() const
    {
    FUNC_LOG;
    return iItemsList;
    }

// ---------------------------------------------------------------------------
// Draws underline of matching part of text in the list items.
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::DrawUnderline()
    {
    FUNC_LOG;
    //TO DO - probably not needed.
    }

// ---------------------------------------------------------------------------
// Sets anchors in root layout (positioning)
// ---------------------------------------------------------------------------
//
void CFsFastAddressingVisualizer::SetAnchors()
    {
    FUNC_LOG;
    iRootLayout->Reset();
    TAlfTimedPoint rootSize = iRootLayout->Size();
    TPoint size = rootSize.ValueNow();

    if ((!size.iX) || (!size.iY))
        {
        return; //if size is zero its not needed to set anchors.
        }

    TAlfTimedPoint listTopLeft( 0, 0 );
    TAlfTimedPoint listBottomRight( rootSize );

    if ( iTextFieldVisible )
        {
        TPoint iconA = iIconA->Size().ValueNow();
        TPoint iconB = iIconB->Size().ValueNow();
        TAlfTimedPoint textFieldTopLeft;
        TAlfTimedPoint textFieldBottomRight;

        if ( iListPosition == CFsFastAddressingList::EListBelowTextField )
            {
            textFieldTopLeft.SetTarget( TAlfRealPoint( 0, 0 ) );
            textFieldBottomRight.SetTarget( TAlfRealPoint( size.iX,
                                                     KFixedTexFieldHeight ) );
            listTopLeft.SetTarget( TAlfRealPoint( 0,
                                       KFixedTexFieldHeight+KFixedSpacing ) );
            }
        else if (iListPosition ==
                 CFsFastAddressingList::EListOnTopOfTextField )
            {
            textFieldTopLeft.SetTarget( TAlfRealPoint( 0,
                                  size.iY - KFixedTexFieldHeight ) );
            textFieldBottomRight.SetTarget( TAlfRealPoint( size.iX, size.iY ) );
            listBottomRight.SetTarget( TAlfRealPoint( size.iX,
                size.iY - ( KFixedTexFieldHeight+KFixedSpacing ) ) );
            }

        iTextInputVisual->SetSize( TAlfRealSize( ( size.iX )-
                                                 ( iconA.iX + iconB.iX ),
                                               KFixedTexFieldHeight ) );

        iRootLayout->SetAnchor( EAlfAnchorTopLeft,
                                KOrdinalTextField,
                                EAlfAnchorOriginLeft,
                                EAlfAnchorOriginTop,
                                EAlfAnchorMetricAbsolute,
                                EAlfAnchorMetricAbsolute,
                                textFieldTopLeft);

        iRootLayout->SetAnchor( EAlfAnchorBottomRight,
                                KOrdinalTextField,
                                EAlfAnchorOriginLeft,
                                EAlfAnchorOriginTop,
                                EAlfAnchorMetricAbsolute,
                                EAlfAnchorMetricAbsolute,
                                textFieldBottomRight);
        }

    iRootLayout->SetAnchor( EAlfAnchorTopLeft,
                            KOrdinalListField,
                            EAlfAnchorOriginLeft,
                            EAlfAnchorOriginTop,
                            EAlfAnchorMetricAbsolute,
                            EAlfAnchorMetricAbsolute,
                            listTopLeft);

    iRootLayout->SetAnchor( EAlfAnchorBottomRight,
                            KOrdinalListField,
                            EAlfAnchorOriginLeft,
                            EAlfAnchorOriginTop,
                            EAlfAnchorMetricAbsolute,
                            EAlfAnchorMetricAbsolute,
                            listBottomRight);

    iRootLayout->UpdateChildrenLayout();
    }

