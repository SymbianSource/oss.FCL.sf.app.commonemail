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
* Description:  Responsible for marking capabilities
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfanchorlayout.h>
#include <alf/alfcontrol.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfborderbrush.h>
#include <alf/alfbrusharray.h>
// </cmail>

#include "fstextviewermarkfacility.h"
#include "fstextviewerskindata.h"


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerMarkFacility* CFsTextViewerMarkFacility::NewL( 
        CAlfLayout* aLayout, 
        CAlfControl* aControl, 
        CFsTextViewerVisualizerData* aNavigator,
        CFsTextViewerSkinData* aSkinData )
    {
    FUNC_LOG;
    CFsTextViewerMarkFacility* self = new (ELeave) CFsTextViewerMarkFacility( 
            aLayout, 
            aControl, 
            aNavigator, 
            aSkinData );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerMarkFacility
// ---------------------------------------------------------------------------
CFsTextViewerMarkFacility::~CFsTextViewerMarkFacility()
    {
    FUNC_LOG;
    if ( iLayout )
        {
        iLayout->RemoveAndDestroyAllD();
        }

    if ( iBorder )
        {
        delete iBorder;
        }
    
    iVisuals.Close();
    }

// ---------------------------------------------------------------------------
// SetHeadOffset
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::SetHeadOffsetL( TInt aHead )
    {
    FUNC_LOG;
    iHead = aHead;
    TInt visualIndex = iNavigator->Find( iHead );
    
    TInt begin = iNavigator->GetVisStartOffset( visualIndex );
    TInt end = iNavigator->GetVisEndOffset( visualIndex );

    CAlfVisual* visual = CreateMarkVisualL( 
            iNavigator->GetVisual( visualIndex ), 
            begin,
            end );
    }

// ---------------------------------------------------------------------------
// SetTailOffset
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::SetTailOffsetL( TInt /* aTail */ )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// FollowCursor
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::FollowCursorL()
    {
    FUNC_LOG;
    if ( iHead < iNavigator->GetCursorCharOffset() )
        {
        iTail = iNavigator->FindPrevChar();
        }
    else if ( iHead > iNavigator->GetCursorCharOffset() )
        {
        iTail = iNavigator->GetCursorCharOffset();
        }
    else
        {
        iTail = iHead;
        }

    TInt visualIndex = iNavigator->Find( iTail );
    
    TInt begin = iNavigator->GetVisStartOffset( visualIndex );
    TInt end = iNavigator->GetVisEndOffset( visualIndex );
    CAlfVisual* visual = NULL;
    
    visual = Find( iTail );

    if ( !visual )
        {
        visual = CreateMarkVisualL( 
            iNavigator->GetVisual( visualIndex ), 
            begin,
            end );
        }

    CAlfImageVisual* imageVisual( dynamic_cast<CAlfImageVisual*>( visual ) );
    CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( visual ) );
    if ( imageVisual )
        {
        UpdateImageVisualL( visual, iNavigator->GetVisual( visualIndex ) );
        }
    else if ( textVisual )
        {
        if ( iHead == iNavigator->GetCursorCharOffset() )
            {
            begin = LocalVisOffset( iHead );
            end = LocalVisOffset( iHead );
            }
        else if ( iTail >= iHead )
            {
            begin = IsHeadVisual( iTail ) ? LocalVisOffset( iHead ) : 0;
            end = LocalVisOffset( iTail ) + 1;
            }
        else if ( iTail < iHead )
            {
            begin = LocalVisOffset( iTail );
            end = IsHeadVisual( iTail ) 
                ? LocalVisOffset( iHead ) 
                : VisualLength( visual );
            }

        UpdateTextVisualL( 
                visual, 
                iNavigator->GetVisual( visualIndex ), 
                begin, 
                end );
        }

    DeleteUnused();
    }

// ---------------------------------------------------------------------------
// SetLayout
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::SetLayout( CAlfLayout* aLayout )
    {
    FUNC_LOG;
    iLayout = aLayout;
    iVisuals.Reset();
    }

// ---------------------------------------------------------------------------
// GetRange
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::GetRange( TInt& aBegin, TInt& aEnd ) const
    {
    FUNC_LOG;
    aBegin = iHead;
    aEnd = iTail;
    }

// ---------------------------------------------------------------------------
// CreateMarkVisualL
// ---------------------------------------------------------------------------
CAlfVisual* CFsTextViewerMarkFacility::CreateMarkVisualL( 
        CAlfVisual* aVisual, 
        TInt aBegin, 
        TInt aEnd )
    {
    FUNC_LOG;
    CAlfImageVisual* imageVisual( dynamic_cast<CAlfImageVisual*>( aVisual ) );
    CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( aVisual ) );
    TFsRangedVisual visual;

    visual.iStart = aBegin;
    visual.iEnd = aEnd;

    if ( imageVisual )
        {
        visual.iVisual = CAlfImageVisual::AddNewL( *iControl, iLayout );
        }
    else if ( textVisual )
        {
        visual.iVisual = CAlfTextVisual::AddNewL( *iControl, iLayout );
        static_cast<CAlfTextVisual*>(visual.iVisual)->SetStyle( 
                EAlfTextStyleNormal, 
                EAlfBackgroundTypeLight );
        static_cast<CAlfTextVisual*>(visual.iVisual)->EnableShadow( EFalse );
        static_cast<CAlfTextVisual*>(visual.iVisual)->SetAlign( 
                EAlfAlignHLeft, 
                EAlfAlignVBottom );
        static_cast<CAlfTextVisual*>(visual.iVisual)->SetColor( 
                iSkinData->iMarkedTextColor );
        static_cast<CAlfTextVisual*>(visual.iVisual)->SetTextStyle(
                static_cast<CAlfTextVisual*>(aVisual)->Style() );
        visual.iVisual->SetClipping();
        }

    visual.iVisual->EnableBrushesL();
    visual.iVisual->Brushes()->AppendL( iBorder, EAlfDoesNotHaveOwnership );
    
    iVisuals.AppendL( visual );

    return visual.iVisual;
    }

// ---------------------------------------------------------------------------
// UpdateImageVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::UpdateImageVisualL( 
        CAlfVisual* aVisual, 
        CAlfVisual* aBaseVisual )
    {
    FUNC_LOG;
    TRect markRect = aBaseVisual->DisplayRectTarget();
    aVisual->SetRect( markRect );
    }

// ---------------------------------------------------------------------------
// UpdateTextVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::UpdateTextVisualL( CAlfVisual* aVisual, 
                                    CAlfVisual* aBaseVisual, 
                                    TInt aBegin, TInt aEnd )
    {
    FUNC_LOG;
    CAlfTextVisual* visual = static_cast<CAlfTextVisual*>(aBaseVisual);
    CAlfTextVisual* targetVisual = static_cast<CAlfTextVisual*>(aVisual);
    TRect markRect = visual->SubstringExtents( aBegin, aEnd );
    markRect.Move( visual->DisplayRectTarget().iTl );
    
    HBufC* text = visual->Text().AllocL();
    targetVisual->SetTextL( text->Mid( aBegin, aEnd - aBegin ) );
    targetVisual->SetRect( markRect );
    delete text;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::ConstructL()
    {
    FUNC_LOG;
    iBorder = CAlfBorderBrush::NewL( iControl->Env(), 1, 1, 0, 0  );
    iBorder->SetColor( KRgbBlack );
    iBorder->SetOpacity( .55f );
    }

// ---------------------------------------------------------------------------
// CFsTextViewerMarkFacility
// ---------------------------------------------------------------------------
CFsTextViewerMarkFacility::CFsTextViewerMarkFacility( 
        CAlfLayout* aLayout, 
        CAlfControl* aControl,
        CFsTextViewerVisualizerData* aNavigator,
        CFsTextViewerSkinData* aSkinData )
    {
    FUNC_LOG;
    iLayout = aLayout;
    iControl = aControl;
    iNavigator = aNavigator;
    iSkinData = aSkinData;
    }

// ---------------------------------------------------------------------------
// VisualLength
// ---------------------------------------------------------------------------
TInt CFsTextViewerMarkFacility::VisualLength( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    TInt retVal = KErrNone;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i].iVisual == aVisual )
            {
            retVal = iVisuals[i].iEnd - iVisuals[i].iStart + 1;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------
TInt CFsTextViewerMarkFacility::Find( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    TInt retVal = KErrNone;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i].iVisual == aVisual )
            {
            retVal = i;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------
CAlfVisual* CFsTextViewerMarkFacility::Find( TInt aOffset )
    {
    FUNC_LOG;
    CAlfVisual* retVal = NULL;

    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( aOffset >= iVisuals[i].iStart && aOffset <= iVisuals[i].iEnd )
            {
            retVal = iVisuals[i].iVisual;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------
TInt CFsTextViewerMarkFacility::FindIndex( TInt aOffset )
    {
    FUNC_LOG;
    TInt retVal = -1;

    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( aOffset >= iVisuals[i].iStart && aOffset <= iVisuals[i].iEnd )
            {
            retVal = i;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// LocalVisOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerMarkFacility::LocalVisOffset( TInt aGlobalOffset )
    {
    FUNC_LOG;
    TInt retVal = -1;
    TInt start = 
        iNavigator->GetVisStartOffset( iNavigator->Find( aGlobalOffset ) );
    retVal = aGlobalOffset - start;

    return retVal;
    }

// ---------------------------------------------------------------------------
// DeleteUnused
// ---------------------------------------------------------------------------
void CFsTextViewerMarkFacility::DeleteUnused()
    {
    FUNC_LOG;
    TInt begin = Min( iHead, iTail );
    TInt end = Max( iHead, iTail );
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( ( iVisuals[i].iStart >= begin 
                   && iVisuals[i].iStart <= end ) 
             || ( iVisuals[i].iEnd >= begin 
                   && iVisuals[i].iEnd <= end ) 
             || ( iVisuals[i].iStart <= begin 
                   && iVisuals[i].iEnd >= begin ) )
            {
            
            }
        else
            {
            iVisuals[i].iVisual->RemoveAndDestroyAllD();
                iVisuals.Remove( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// IsHeadVisual
// ---------------------------------------------------------------------------
TBool CFsTextViewerMarkFacility::IsHeadVisual( TInt aGlobalOffset )
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    TInt headIndex = FindIndex( iHead );
    
    if ( headIndex >= 0 
            && headIndex < iVisuals.Count()
            && aGlobalOffset <= iVisuals[headIndex].iEnd 
            && aGlobalOffset >= iVisuals[headIndex].iStart )
        {
        retVal = ETrue;
        }

    return retVal;
    }

