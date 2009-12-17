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
* Description:  A visualizer for data items with separator.
*
*/


//#include <fsconfig.h>

#include "emailtrace.h"
#include "fsseparatorvisualizer.h"
#include "fsseparatordata.h"
#include "fstreelist.h"
#include "fslayoutmanager.h"
// <cmail> SF
#include <alf/alfanchorlayout.h>
#include <alf/alfcurvepath.h>
#include <alf/alflinevisual.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
// </cmail>

// ======== MEMBER FUNCTIONS ========
 
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsSeparatorVisualizer* 
    CFsSeparatorVisualizer::NewL( CAlfControl& aOwnerControl )
    {
    FUNC_LOG;
    CFsSeparatorVisualizer* self = new( ELeave ) CFsSeparatorVisualizer( aOwnerControl );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsSeparatorVisualizer::~CFsSeparatorVisualizer() 
    {
    FUNC_LOG;

    }


// ---------------------------------------------------------------------------
// The function sets separator's thickness in pixels.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::SetSeparatorThickness( const TInt aThickness )
    {
    FUNC_LOG;
    iThickness = aThickness;
    }


// ---------------------------------------------------------------------------
// The function returns thikness of the separator line.
// ---------------------------------------------------------------------------
//
TInt CFsSeparatorVisualizer::SeparatorThickness( ) const
    {
    FUNC_LOG;
    return iThickness;
    }
    

// ---------------------------------------------------------------------------
// Sets the width of separator line. The separator line is always centered 
// horizontaly. The line width is given as the percentage of the whole item's width.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::SetSeparatorWidth ( TReal32 aWidth )
    {
    FUNC_LOG;
    iWidth = aWidth;
    
    if ( iWidth > 1.0 )
        {
        iWidth = 1.0;
        }
    else if ( iWidth < 0.1 )
        {
        iWidth = 0.1;
        }
    }


// ---------------------------------------------------------------------------
// Returns the width of the separator line.
// ---------------------------------------------------------------------------
//
TReal32 CFsSeparatorVisualizer::SetSeparatorWidth ( ) const
    {
    FUNC_LOG;
    return iWidth;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns visualizer's type.
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsSeparatorVisualizer::Type() const
    {
    FUNC_LOG;
    return EFsSeparatorVisualizer;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets multiple flags for an item at one time.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::SetFlags( TUint32 aFlags )
    {
    FUNC_LOG;
    iFlags = aFlags;
    iFlags &= ~KFsTreeListItemFocusable;
    iFlags &= ~KFsTreeListItemManagedLayout;
    iFlags &= ~KFsTreeListItemExtendable;
    iFlags &= ~KFsTreeListItemExtended;
    iFlags &= ~KFsTreeListItemAlwaysExtended;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns the size of an item in a normal (not extended) state.
// ---------------------------------------------------------------------------
//
TSize CFsSeparatorVisualizer::Size() const
    {
    FUNC_LOG;
    return iSize;
    }
    

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
//  Sets the item's visualization state to extended. 
//  Separator cannot be extended - the function has no effect.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::SetExtended( TBool /*aFlag*/ )
    {
    FUNC_LOG;
    iFlags &= ~KFsTreeListItemExtended;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns extended/not extended state of the item visualization.
// Separator cannot be extended - the function always returns EFalse.
// ---------------------------------------------------------------------------
//
TBool CFsSeparatorVisualizer::IsExtended() const
    {
    FUNC_LOG;
    return EFalse;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets if an item can be in extended state or not.
// Separator is not extendable - the function has no effect.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::SetExtendable( TBool /*aFlag*/ )
    {
    FUNC_LOG;
    iFlags &= ~KFsTreeListItemExtendable;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns an information if item can be in extended state.
// Separator is not extendable - the function always returns EFalse.
// ---------------------------------------------------------------------------
//
TBool CFsSeparatorVisualizer::IsExtendable( )
    {
    FUNC_LOG;
    return EFalse;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// This functions sets wether an item should always be in extended state.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::SetAlwaysExtended( TBool /*aAlwaysExtended*/ )
    {
    FUNC_LOG;
    iFlags &= ~KFsTreeListItemAlwaysExtended;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// This function returns wether an item is always in extended state.
// ---------------------------------------------------------------------------
//
TBool CFsSeparatorVisualizer::IsAlwaysExtended( ) const
    {
    FUNC_LOG;
    return EFalse;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item is requested to draw itself
// within specified parent layout.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::ShowL( CAlfLayout& aParentLayout,
                                        const TUint aTimeout )
    {
    FUNC_LOG;
    if (!iLayout)
        {
        iParentLayout = &aParentLayout;
        iLayout = CAlfAnchorLayout::AddNewL(iOwnerControl,iParentLayout);
        iLayout->EnableBrushesL();
        iLayout->SetClipping(ETrue);
        iLayout->SetFlag( EAlfVisualFlagIgnorePointer );
        
        CAlfCurvePath* path = CAlfCurvePath::NewLC( iOwnerControl.Env() );
        path->AppendLineL(TPoint(0, 1), TPoint(100, 1), 1 );
        iLineVisual = CAlfLineVisual::AddNewL( iOwnerControl, iLayout );
        iLineVisual->SetPath(path, EAlfHasOwnership);
        iLineVisual->SetThickness( TAlfTimedValue(static_cast<TReal32>(iThickness), 0) );
        iLineVisual->SetColor(KRgbBlack);
        CleanupStack::Pop(path);
        iLineVisual->SetFlag( EAlfVisualFlagIgnorePointer );
        }
    else
        {
        //visuals are already created but not attached to the list layout
        if (!iLayout->Layout()) 
            {
            aParentLayout.Append(iLayout, aTimeout);
            iParentLayout = &aParentLayout;
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item is requested to update its
// visual content.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::UpdateL( const MFsTreeItemData& aData,
                                          TBool /*aFocused*/,
                                          const TUint aLevel,
                                          CAlfTexture*& /*aMarkIcon*/,
                                          CAlfTexture*& /*aMenuIcon*/,
                                          const TUint /*aTimeout*/,
                                          TBool /*aUpdateData*/)
    {
    FUNC_LOG;
    if ( iLayout && iLineVisual )
        {
        if ( aData.Type() == KFsSeparatorDataType )
            {
            const CFsSeparatorData* data = static_cast<const CFsSeparatorData*>(&aData);
            
            TSize size;
            
            size = Size();
            size.iWidth = iParentLayout->Size().Target().iX - iParentLayout->HorizontalPadding()*2;
            //shrink width if we are at some level in the hierarchy
            TInt indentation(0);
            indentation = iTextIndentation * ( aLevel - 1 );
            TRect currentSize = size;
            if (CFsLayoutManager::IsMirrored())
                {
                currentSize.iBr = currentSize.iBr - TPoint(indentation, 0);
                }
            else
                {
                currentSize.iTl = currentSize.iTl + TPoint(indentation, 0);
                }
            
            CAlfCurvePath* path(NULL);
            path = iLineVisual->Path();
            
            if ( path )
                {
                path->Reset();
                
                //draw the line in the middle of item's height
                TInt y(0);
                y = ( currentSize.Height() - iThickness ) / 2;
                
                //width
                TRect separatorRect = currentSize;
                
                separatorRect.iTl.iY = y;
                separatorRect.iBr.iY = y;
                
                TInt lineWidth(0), space(0);
                lineWidth = currentSize.Width() * iWidth;
                space = ( currentSize.Width() - lineWidth ) / 2;
                
                separatorRect.iTl += TPoint( space, 0 );
                separatorRect.iBr -= TPoint( space, 0 );
                
                path->AppendLineL( separatorRect.iTl, separatorRect.iBr, 1 );
                
                iLineVisual->SetThickness( TAlfTimedValue(static_cast<TReal32>(iThickness), 0) );
                iLineVisual->SetColor( data->SeparatorColor() );                
                }
            }        
        
        iLayout->UpdateChildrenLayout( 0 );
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item goes out of the visible items
// scope.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::Hide( const TInt aTimeout )
    {
    FUNC_LOG;
    if ( iLayout )
        {
        iParentLayout->Remove(iLayout, aTimeout);
        iLayout->RemoveAndDestroyAllD();
        iLayout = NULL;
        iParentLayout = NULL;
        iLineVisual = NULL;
        }
    }

// ---------------------------------------------------------------------------
//  From class MFsTreeItemVisualizer.
//  Method to marquee the text when it's too long.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::MarqueeL(const TFsTextMarqueeType /*aMarqueeType*/,
                                                 const TUint /*aMarqueeSpeed*/,
                                                 const TInt /*aMarqueeStartDelay*/,
                                                 const TInt /*aMarqueeCycleStartDelay*/,
                                                 const TInt /*aMarqueeRepetitions*/)
    {
    FUNC_LOG;

    }


// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsSeparatorVisualizer::CFsSeparatorVisualizer( CAlfControl& aOwnerControl )
    : CFsTreeItemVisualizerBase( aOwnerControl ),
    iThickness( KDefaultSeparatorThickness ),
    iWidth( 1.0 )
    {
    FUNC_LOG;
    TUint flags(0);
    SetFlags( flags );
    SetSize( TSize( KDefaultWidth, KDefaultHeight ));
    }

                                                
// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsSeparatorVisualizer::ConstructL( )
    {
    FUNC_LOG;
    CFsTreeItemVisualizerBase::ConstructL();
    }

