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
* Description:  A visualizer for empty tree root's data.
*
*/




#include "emailtrace.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreeplainrootvisualizer.h"
#include "fstextstylemanager.h"
// <cmail> SF
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alftexture.h>
// </cmail>

// ======== MEMBER FUNCTIONS ========
 
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreePlainRootVisualizer* CFsTreePlainRootVisualizer::NewL( 
                                                  CAlfControl& aOwnerControl )
    {
    FUNC_LOG;
    CFsTreePlainRootVisualizer* self = 
        new( ELeave ) CFsTreePlainRootVisualizer( aOwnerControl );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainRootVisualizer::~CFsTreePlainRootVisualizer() 
    {
    FUNC_LOG;
    }

// from base class MFsTreeNodeVisualizer


// ---------------------------------------------------------------------------
// Returns visualizer's type.
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsTreePlainRootVisualizer::Type() const
    {
    FUNC_LOG;
    return EFsTreePlainRootVisualizer;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the size of the item visualization when item is not in expanded 
// state. The value is taken from layout manager. 
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::UpdateLayout( )
    {
    FUNC_LOG;
    
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
//  A call to this function means that the visualization of empty root 
// becomes visible and should draw itself within specified parent layout. 
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::ShowL( CAlfLayout& aParentLayout,
                                        const TUint /*aTimeout*/ )
    {
    FUNC_LOG;

    if (!iLayout)
        {
        iParentLayout = &aParentLayout;        
        iLayout = CAlfAnchorLayout::AddNewL(iOwnerControl, &aParentLayout);
        iLayout->SetSize(aParentLayout.Size().ValueNow().AsSize());
        
        if (!iTextVisual)
            {
            iTextVisual = CAlfTextVisual::AddNewL( 
                    iOwnerControl, iLayout );
            iTextVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }
        }
    }

// ---------------------------------------------------------------------------
// A call to this function means that the item is requested to update its
// visual content.
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::UpdateL( const MFsTreeItemData& aData,
                                          TBool /*aFocused*/,
                                          const TUint /*aLevel*/,
                                          CAlfTexture*& /*aMarkIcon*/,
                                          CAlfTexture*& /*aMenuIcon*/,
                                          const TUint /*aTimeout*/,
                                          TBool aUpdateData)
    {
    FUNC_LOG;
    if (aData.Type() == KFsTreePlainOneLineItemDataType)
        {
        const CFsTreePlainOneLineItemData* data = 
                    static_cast<const CFsTreePlainOneLineItemData*>(&aData);
                    
        if ( iTextVisual && aUpdateData )
            {            
            iTextVisual->SetTextL(data->Data( ));
            iTextVisual->SetTextStyle(iTextStyleId);
            
            TAlfAlignHorizontal currTextAlign( EAlfAlignHCenter );            
            currTextAlign = CurrentTextAlignmentL( iTextAlign, &data->Data(), iTextVisual );
            iTextVisual->SetAlign( currTextAlign, EAlfAlignVCenter );
            
            iTextVisual->EnableShadow(iFlags & KFsTreeListItemTextShadow);
            if (!iIsCustomTextNormalColor)
                {//custom color not set, use color from skin - load it in case theme has changed
                iNormalTextColor = NormalStateTextSkinColor( );
                }
            iTextVisual->SetColor( iNormalTextColor );
            iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapBreak);

            TInt styleId = ModifiedStyleIdL( );
            iTextVisual->SetTextStyle( styleId );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the the empty root visualization 
// should become hidden.
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::Hide( const TInt aTimeout )
    {
    FUNC_LOG;
    if ( iLayout )
        {
        iParentLayout->Remove(iLayout, aTimeout);
        iLayout->RemoveAndDestroyAllD();
        iLayout = NULL;
        iParentLayout = NULL;
        iTextVisual = NULL;
        }
    }


// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  This method marquees the text in tree item if it is too long.
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::MarqueeL(const TFsTextMarqueeType /*aMarqueeType*/,
                                          const TUint /*aMarqueeSpeed*/,
                                          const TInt /*aMarqueeStartDelay*/,
                                          const TInt /*aMarqueeCycleStartDelay*/,
                                          const TInt /*aMarqueeRepetitions*/)
    {
    FUNC_LOG;
   
    }


// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainRootVisualizer::CFsTreePlainRootVisualizer( 
                                                  CAlfControl& aOwnerControl )
    : CFsTreeNodeVisualizerBase( aOwnerControl ),
    iTextVisual(NULL)
    {
    FUNC_LOG;
    iTextAlign = EAlfAlignHCenter;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::ConstructL( )
    {
    FUNC_LOG;
    CFsTreeNodeVisualizerBase::ConstructL();
    }

// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  This method sets flag disable wrapping the text item.
// ---------------------------------------------------------------------------
//
void CFsTreePlainRootVisualizer::OffWrapping()
	{
	FUNC_LOG;
	}
