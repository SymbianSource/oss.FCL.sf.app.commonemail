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
* Description:  Storage class for visuals belonging to visualizer
*
*/


//<cmail>
//#include <fsconfig.h>
#include "emailtrace.h"
#include <alf/alfvisual.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfborderbrush.h>
// </cmail>

// <cmail>
#include <alf/alfcontrol.h>
#include <alf/alfbrusharray.h>
// </cmail>
#include "fstextviewervisualizerdata.h"
#include "fstextparser.h"
#include "fstexture.h"
#include "fstextviewerselectsmanager.h"
#include "fstextviewercoordinator.h"

TAny* TKeyArrayPtr::At(TInt aIndex) const
    {
    FUNC_LOG;
    if (aIndex==KIndexPtr)
        {
        return *(TUint8**)iPtr+iKeyOffset;
        }
    return *(TUint8**)iBase->Ptr(aIndex*sizeof(TUint8**)).Ptr()+iKeyOffset;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerVisualizerData* CFsTextViewerVisualizerData::NewL( 
        CAlfControl* aControl, 
        CFsTextParser* aParser, 
        CFsTextViewerSelectsManager* aSelector )
    {
    FUNC_LOG;
    CFsTextViewerVisualizerData* self = 
        new (ELeave) CFsTextViewerVisualizerData( 
                aControl, 
                aParser, 
                aSelector );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerVisualizerData
// ---------------------------------------------------------------------------
CFsTextViewerVisualizerData::~CFsTextViewerVisualizerData()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i] && iVisArray[i]->iVisual && iSelector )
            {
            iSelector->RemoveVisual( iVisArray[i] );
            }
        }
    
    iVisArray.ResetAndDestroy();
//    delete iDebugBrush;
    iVisArray.Close();
    iLayoutArray.Close();
    iBgArray.Close();
    }

// ---------------------------------------------------------------------------
// AppendVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::AppendVisualL( TFsRangedVisual aVisual )
    {
    FUNC_LOG;
    TFsRangedVisual* rVisual = new (ELeave) TFsRangedVisual();
    rVisual->iStart = aVisual.iStart;
    rVisual->iEnd = aVisual.iEnd;
    rVisual->iVisual = aVisual.iVisual;
    rVisual->iIsActive = aVisual.iIsActive;
    rVisual->iRect = aVisual.iRect;
    rVisual->iLayout = aVisual.iLayout;
    rVisual->iIsText = aVisual.iIsText;
    rVisual->iEmbed = aVisual.iEmbed;
    rVisual->iHotspot = aVisual.iHotspot;
    rVisual->iHotspotType = aVisual.iHotspotType;
    rVisual->iId = aVisual.iId;
    rVisual->iTextStyleId = aVisual.iTextStyleId;
    rVisual->iTextColor = aVisual.iTextColor;
    rVisual->iLineNumber = aVisual.iLineNumber;

    iVisArray.Append( rVisual );
    }

// ---------------------------------------------------------------------------
// AppendVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::AppendVisualL( TFsRangedVisual* aVisual )
    {
    FUNC_LOG;
    iVisArray.Append( aVisual );
    }

// ---------------------------------------------------------------------------
// AppendVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::AppendVisualL( 
        CAlfVisual* aVisual, 
        TFsVisRange aRange )
    {
    FUNC_LOG;
    TFsRangedVisual* visual = new (ELeave) TFsRangedVisual();
    visual->iVisual = aVisual;
    visual->iStart = aRange.iStart;
    visual->iEnd = aRange.iEnd;
    visual->iIsActive = ETrue;

    iVisArray.Append( visual );
    }
    
// ---------------------------------------------------------------------------
// AppendLayoutL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::AppendLayoutL( 
        CAlfVisual* aVisual, 
        TFsVisRange aRange )
    {
    FUNC_LOG;
    TFsRangedVisual visual;
    visual.iVisual = aVisual;
    visual.iStart = aRange.iStart;
    visual.iEnd = aRange.iEnd;
    visual.iIsActive = ETrue;

    iLayoutArray.AppendL( visual );
    }

// ---------------------------------------------------------------------------
// AppendBgL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::AppendBgL( TFsPartBgVisual aBg )
    {
    FUNC_LOG;
    iBgArray.AppendL( aBg );
    }

// ---------------------------------------------------------------------------
// RemoveBg
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizerData::RemoveBg( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    for ( TInt i = 0; i < iBgArray.Count(); ++i )
        {
        if ( iBgArray[i].iVisual == aVisual )
            {
            iBgArray.Remove( i );
            i = iBgArray.Count();
            retVal = ETrue;
            }
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// ResetBgArray
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::ResetBgArray()
    {
    FUNC_LOG;
    for ( TInt i =  0; i < iBgArray.Count(); ++i )
        {
        iBgArray[i].iVisual->RemoveAndDestroyAllD();
        }
    iBgArray.Reset();
    }

// ---------------------------------------------------------------------------
// InsertVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::InsertVisualL( TInt aIndex, 
                        CAlfVisual* aVisual, TFsVisRange aRange )
    {
    FUNC_LOG;
    TFsRangedVisual* visual = new (ELeave) TFsRangedVisual();
    visual->iVisual = aVisual;
    visual->iStart = aRange.iStart;
    visual->iEnd = aRange.iEnd;
    visual->iIsActive = ETrue;

    iVisArray.Insert( visual, aIndex );
    }

// ---------------------------------------------------------------------------
// PopVisual
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::PopVisual()
    {
    FUNC_LOG;
    iVisArray.Remove( iVisArray.Count() - 1 );
    }

// ---------------------------------------------------------------------------
// RemoveVisual
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::RemoveVisual( TInt aIndex )
    {
    FUNC_LOG;
    iVisArray.Remove( aIndex );
    }

// ---------------------------------------------------------------------------
// DisableVisual
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::DisableVisual( TInt aIndex, TBool aStatus )
    {
    FUNC_LOG;
    if ( aIndex >=0 && aIndex < iVisArray.Count() )
        {
        iVisArray[aIndex]->iIsActive = !aStatus;
        }
    }

// ---------------------------------------------------------------------------
// DisableVisual
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::DisableVisual( 
        CAlfVisual* aVisual, 
        TBool aStatus )
    {
    FUNC_LOG;
    TInt index = Find( aVisual );
    if ( index >= 0 && index < iVisArray.Count() )
        {
        iVisArray[index]->iIsActive = !aStatus;
        }
    }

// ---------------------------------------------------------------------------
// EnableVisual
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::EnableVisual( TInt aIndex, TBool aStatus )
    {
    FUNC_LOG;
    if ( aIndex >=0 && aIndex < iVisArray.Count() )
        {
        iVisArray[aIndex]->iIsActive = aStatus;
        }
    }

// ---------------------------------------------------------------------------
// EnableVisual
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::EnableVisual( 
        CAlfVisual* aVisual, 
        TBool aStatus )
    {
    FUNC_LOG;
    TInt index = Find( aVisual );
    if ( index >= 0 && index < iVisArray.Count() )
        {
        iVisArray[index]->iIsActive = aStatus;
        }
    }

// ---------------------------------------------------------------------------
// GetVisual
// ---------------------------------------------------------------------------
CAlfVisual* CFsTextViewerVisualizerData::GetVisual( TInt aIndex )
    {
    FUNC_LOG;
    CAlfVisual* retVal = NULL;
    if ( aIndex >= 0 && aIndex < iVisArray.Count() )
        {
        retVal = iVisArray[aIndex]->iVisual;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetVisLength
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetVisLength( TInt aIndex )
    {
    FUNC_LOG;
    TInt retVal = -1;
    
    if ( aIndex >= 0 && aIndex < iVisArray.Count() )
        {
        retVal = 
            iVisArray[aIndex]->iEnd - iVisArray[aIndex]->iStart + 1;
        }
    else
        {
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetVisStartOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetVisStartOffset( TInt aIndex )
    {
    FUNC_LOG;
    TInt retVal = -1;
    if ( aIndex >= 0 && aIndex < iVisArray.Count() )
        {
        retVal = iVisArray[aIndex]->iStart;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetVisEndOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetVisEndOffset( TInt aIndex )
    {
    FUNC_LOG;
    TInt retVal = -1;
    if ( aIndex >= 0 && aIndex < iVisArray.Count() )
        {
        retVal = iVisArray[aIndex]->iEnd;
        }
        
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// GetLayEndOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetLayEndOffset( TInt aIndex )
    {
    FUNC_LOG;
    TInt retVal = -1;
    if ( aIndex >= 0 && aIndex < iLayoutArray.Count() )
        {
        retVal = iLayoutArray[aIndex].iEnd;
        }
        
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetVisOffsets
// ---------------------------------------------------------------------------
TFsVisRange CFsTextViewerVisualizerData::GetVisOffsets( TInt aIndex )
    {
    FUNC_LOG;
    TFsVisRange retVal;
    retVal.iStart = 0;
    retVal.iEnd = 0;
    if ( aIndex >= 0 && aIndex < iVisArray.Count() )
        {
        retVal.iStart = iVisArray[aIndex]->iStart;
        retVal.iEnd = iVisArray[aIndex]->iEnd;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// Count
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::Count()
    {
    FUNC_LOG;
    return iVisArray.Count();
    }

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::Find( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    TInt retVal = -1;

    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iVisual == aVisual )
            {
            retVal = i;
//            i = iVisArray.Count();
            }
        }

    return retVal;
    }
    
// ---------------------------------------------------------------------------
// FindLayout
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::FindLayout( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    TInt retVal = -1;

    for ( TInt i = 0; i < iLayoutArray.Count(); ++i )
        {
        if ( iLayoutArray[i].iVisual == aVisual )
            {
            retVal = i;
            i = iLayoutArray.Count();
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::Find( TInt aOffset )
    {
    FUNC_LOG;
    TInt retVal = -1;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( aOffset >= iVisArray[i]->iStart && aOffset <= 
                iVisArray[i]->iEnd && iVisArray[i]->iIsActive )
            {
            retVal = i;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// MoveToNextCharL
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizerData::MoveToNextCharL()
    {
    FUNC_LOG;
    TInt i;
    TInt maxChar = 0;
    for ( i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iIsActive 
                && iVisArray[i]->iEnd > maxChar )
            {
            maxChar = iVisArray[i]->iEnd;
            }
        }
        
    if ( iCurrentChar >= maxChar - 1 )
        {
        return EFalse;
        }
        
    iPrevChar = iCurrentChar;
    TBool notFound = ETrue;
    ++iCurrentChar;

    if ( !iVisArray[iPosition.iVisual]->iVisual )
        {
        RestoreL( iVisArray[iPosition.iVisual] );
        }

    CAlfImageVisual* visual( dynamic_cast<CAlfImageVisual*>( iVisArray[iPosition.iVisual]->iVisual ) );
    if ( visual )
        {
        iCurrentChar = iVisArray[iPosition.iVisual]->iEnd + 1;
        }
    while ( notFound )
        {
        if ( iCurrentChar >= iLength )
            {
            notFound = EFalse;
            }
        for ( i = 0; i < iVisArray.Count(); ++i )
            {
            if ( iCurrentChar >= iVisArray[i]->iStart 
                    && iCurrentChar <= iVisArray[i]->iEnd )
                {
                if ( iVisArray[i]->iIsActive )
                    {
                    iPosition.iVisual = i;
                    iPosition.iOffset = 
                        iCurrentChar - iVisArray[i]->iStart;
                    notFound = EFalse;
                    }
                else 
                    {
                    iCurrentChar = iVisArray[i]->iEnd + 1;
                    }
                }
            }
        if ( notFound )
            {
            ++iCurrentChar;
            }
        }
    if ( iCurrentChar >= iLength )
            {
            iPosition.iVisual = iVisArray.Count() - 1;
            iPosition.iOffset = iVisArray[iVisArray.Count() - 1]->iEnd;
            }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// MoveToPrevCharL
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizerData::MoveToPrevCharL()
    {
    FUNC_LOG;
    if ( iCurrentChar == 0 )
        {
        return EFalse;
        }
    iPrevChar = iCurrentChar;
    TBool notFound = ETrue;
    TInt i;
    
    --iCurrentChar;

    if ( iPosition.iVisual >= 0 
            && iPosition.iVisual < iVisArray.Count() 
            && !iVisArray[iPosition.iVisual]->iVisual )
        {
        RestoreL( iVisArray[iPosition.iVisual] );
        }

    CAlfImageVisual* visual( NULL );
    if ( iPosition.iVisual >= 0 && iPosition.iVisual < iVisArray.Count() )
        {
        visual = dynamic_cast<CAlfImageVisual*>( iVisArray[iPosition.iVisual]->iVisual );
        }
    else
        {
        return EFalse;
        }
    if ( visual  )
        {
        iCurrentChar = iVisArray[iPosition.iVisual]->iStart - 1;
        }
    
    while ( notFound )
        {
        if ( iCurrentChar <= 0 )
            {
            notFound = EFalse;
            }
        for ( i = 0; i < iVisArray.Count(); ++i )
            {
            if ( iCurrentChar >= iVisArray[i]->iStart 
                    && iCurrentChar <= iVisArray[i]->iEnd )
                {
                if ( iVisArray[i]->iIsActive )
                    {
                    iPosition.iVisual = i;
                    iPosition.iOffset = 
                        iCurrentChar - iVisArray[i]->iStart;
                    notFound = EFalse;
                    }
                else 
                    {
                    iCurrentChar = iVisArray[i]->iStart - 1;
                    }
                }
            }
            if ( notFound )
                {
                --iCurrentChar;
                }
        }
    if ( iCurrentChar <= 0 )
            {
            iPosition.iVisual = 0;
            iPosition.iOffset = 0;
            }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// FindPrevChar
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::FindPrevChar()
    {
    FUNC_LOG;
    TInt retVal = -1;

    TInt current = iCurrentChar - 1;
    TBool notFound = ETrue;

    while ( notFound && current >= 0 )
        {
        if ( 0 > Find( current ) )
            {
            --current;
            }
        else
            {
            retVal = current;
            notFound = EFalse;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetCursorRectL
// ---------------------------------------------------------------------------
TRect CFsTextViewerVisualizerData::GetCursorRectL()
    {
    FUNC_LOG;
    TRect retVal;
    if ( iPosition.iVisual < 0 || iPosition.iVisual >= iVisArray.Count() )
        {
        return retVal;
        }
    if ( !iVisArray[iPosition.iVisual]->iVisual )
        {
        RestoreL( iVisArray[iPosition.iVisual] );
        }

    CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( iVisArray[iPosition.iVisual]->iVisual ) );
    if ( textVisual )
        {
        retVal = static_cast<CAlfTextVisual*>
            ( iVisArray[iPosition.iVisual]->iVisual )->
                SubstringExtents( iPosition.iOffset, iPosition.iOffset + 1 );
        }
    else
        {
        retVal = TRect( 
            TPoint( 0, 0 ),
            iVisArray[iPosition.iVisual]->iVisual->Size().Target() );
        }

    TPoint translation; 
    translation = 
        iVisArray[iPosition.iVisual]->iVisual->DisplayRectTarget().iTl;
    retVal.Move( translation );

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetCursorCharOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetCursorCharOffset()
    {
    FUNC_LOG;
    return iCurrentChar;
    }

// ---------------------------------------------------------------------------
// SetCursorCharOffset
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::SetCursorCharOffset( TInt aOffset )
    {
    FUNC_LOG;
    iCurrentChar = aOffset;
    }

// ---------------------------------------------------------------------------
// Sort
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::Sort()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iEnd > iLength )
            {
            iLength = iVisArray[i]->iEnd;
            }
        }   
    }

// ---------------------------------------------------------------------------
// RestoreL
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::RestoreL( TFsRangedVisual* aVisual )
    {
    FUNC_LOG;
    if ( !aVisual || aVisual->iVisual || iVisualAmount > 300 )
        {
        return KErrNone;
        }

    CAlfVisual* visual = NULL;
    CFsTexture *texture = NULL;
    if ( aVisual->iIsText )
        {
        visual = CAlfTextVisual::AddNewL( *iControl, aVisual->iLayout );
        HBufC* text;
        text = iParser->GetTextL( 
                aVisual->iStart, 
                aVisual->iEnd );
        
        static_cast<CAlfTextVisual*>(visual)->SetTextL( text->Des() );
        delete text;

        static_cast<CAlfTextVisual*>(visual)->SetTextStyle( aVisual->iTextStyleId );
        static_cast<CAlfTextVisual*>(visual)->EnableShadow( EFalse );
        static_cast<CAlfTextVisual*>(visual)->SetColor(
                aVisual->iTextColor );
        if ( aVisual->iDirection == TBidiText::ELeftToRight )
            {
        static_cast<CAlfTextVisual*>(visual)->SetAlign( EAlfAlignHLeft, EAlfAlignVBottom );
            }
        else 
            {
            static_cast<CAlfTextVisual*>(visual)->SetAlign( EAlfAlignHRight, EAlfAlignVBottom );
            }
//        visual->EnableBrushesL();
//        visual->Brushes()->AppendL( iDebugBrush, EAlfDoesNotHaveOwnership );
        visual->SetRect( aVisual->iRect );
        aVisual->iVisual = visual;
        }
    else
        {
        visual = CAlfImageVisual::AddNewL( *iControl, aVisual->iLayout );

        static_cast<CAlfImageVisual*>(visual)->SetScaleMode( 
                CAlfImageVisual::EScaleFit );
        static_cast<CAlfImageVisual*>(visual)->SetImage( TAlfImage( 
            ( ( iParser->GetTextureL( aVisual->iStart ) ).Texture() ) ) );
        visual->SetRect( aVisual->iRect );
        aVisual->iVisual = visual;
        }

    visual->SetFlag( EAlfVisualFlagManualLayout );
    ++iVisualAmount;

    if ( aVisual->iEmbed )
        {
        iSelector->AppendSelectVisL( 
                 aVisual,
                 aVisual->iId,
                 CFsTextViewerSelect::EFsEmbedType );
        }
    else if ( aVisual->iHotspot )
        {
        if ( 0 == aVisual->iHotspotType )
            {
            iSelector->AppendSelectVisL(
                     aVisual,
                     aVisual->iId,
                     CFsTextViewerSelect::EFsHotspotType );
            }
        else 
            {
            iSelector->AppendSelectVisL( 
                     aVisual, 
                     aVisual->iId,
                     CFsTextViewerSelect::EFsLineType );
            }
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ScrollAllL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::ScrollAllL( 
        TInt aTrans, 
        TInt aTime, 
        CAlfLayout* aRootLayout )
    {
    FUNC_LOG;
    aRootLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    TRect visibleRect = TRect( 0, 0, 500, iScreenHeight );
    
    TInt heightBuffer = 0;
    if ( aTime != 0 )
        {
        visibleRect.iTl.iY -= Abs( aRootLayout->ScrollOffset().iY.ValueNow() - iScreenOffset );
        visibleRect.iBr.iY += Abs( aRootLayout->ScrollOffset().iY.ValueNow() - iScreenOffset );
        heightBuffer = Abs( aTrans );
        }
    else 
        {
        HideAll();
        visibleRect.Move( 0, -aTrans );
        }
    visibleRect.Move( 0, iScreenOffset );
    iScreenOffset -= aTrans;
    
    
    iScrollDirection = aTrans < 0 ? EFsScrollDown : EFsScrollUp;
    
    TFsRangedVisual* current = NULL;
    
    if ( iScrollDirection == EFsScrollDown )
        {
        visibleRect.iBr.iY += heightBuffer;
        for ( TInt i = 0; i < iVisArray.Count(); ++i )
            {
            current = iVisArray[i];
            if ( visibleRect.Intersects( current->iRect ) || current->iLayout != aRootLayout )
                {
                if ( !current->iVisual )
                    {
                    RestoreL( current );
                    }
                }
            else
                {
                if ( current->iVisual )
                    {
                    current->iVisual->RemoveAndDestroyAllD();
                    current->iVisual = NULL;
                    --iVisualAmount;
                    }   
                }
            }        
        }
    else
        {
        visibleRect.iTl.iY -= heightBuffer;
        for ( TInt i = iVisArray.Count() - 1; i >= 0; --i )
            {
            current = iVisArray[i];
            if ( visibleRect.Intersects( current->iRect ) || current->iLayout != aRootLayout )
                {
                if ( !current->iVisual )
                    {
                    RestoreL( current );
                    }
                }
            else
                {
                if ( current->iVisual )
                    {
                    current->iVisual->RemoveAndDestroyAllD();
                    current->iVisual = NULL;
                    --iVisualAmount;
                    }                        
                }
            }
        }
    TAlfTimedPoint offset;
    offset.iY.SetTarget( iScreenOffset, aTime );
    aRootLayout->SetScrollOffset( offset );
    aRootLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
    iStatusLineLayout->SetPos( TPoint( 0, GetTotalHeight() - iStatusLineLayout->Size().Target().iY ) );
    RefreshSelectsL();
    }

// ---------------------------------------------------------------------------
// GetTextStyle
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetTextStyle( CAlfTextVisual* aVisual )
    {
    FUNC_LOG;
    TInt retVal = 0;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( aVisual == iVisArray[i]->iVisual )
            {
            retVal = iVisArray[i]->iTextStyleId;
            i = iVisArray.Count();
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// DimAllHotspots
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::DimAllHotspotsL()
    {
    FUNC_LOG;
    iSelector->DimAllL();
    }

// ---------------------------------------------------------------------------
// RefreshSelects
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::RefreshSelectsL()
    {
    FUNC_LOG;
    iSelector->RefreshSelectionL();
    }

// ---------------------------------------------------------------------------
// GetVisualPos
// ---------------------------------------------------------------------------
TRect CFsTextViewerVisualizerData::GetVisualPos( TInt aCharIndex )
    {
    FUNC_LOG;
    TRect retVal;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( aCharIndex >= iVisArray[i]->iStart 
                && aCharIndex <= iVisArray[i]->iEnd )
            {
            retVal = iVisArray[i]->iRect;
            i = iVisArray.Count();
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetVisualAtIndex
// ---------------------------------------------------------------------------
TFsRangedVisual* CFsTextViewerVisualizerData::GetVisualAtIndex( TInt aIndex )
    {
    FUNC_LOG;
    TFsRangedVisual* retVal = NULL;
    if ( aIndex >= 0 && aIndex < iVisArray.Count() )
        {
        retVal = iVisArray[aIndex];
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// ScrollFromCharL //<cmail> leaving
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::ScrollFromCharL( //<cmail> leaving
        TInt aCharIndex, 
        TInt aTrans, 
        TInt aTime,
        CAlfLayout* aRootLayout )
    {
    FUNC_LOG;
    aRootLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    TRect visibleRect = TRect( 0, 0, 500, iScreenHeight + Abs( aTrans ) );
    visibleRect.iTl.iY -= Abs( aRootLayout->ScrollOffset().iY.ValueNow() );
    visibleRect.iBr.iY += Abs( aRootLayout->ScrollOffset().iY.ValueNow() );
    visibleRect.Move( 0, iScreenOffset );
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iStart > aCharIndex )
            {
            if ( visibleRect.Intersects( iVisArray[i]->iRect ) )
                {
                RestoreL( iVisArray[i] );
                }
            iVisArray[i]->iRect.iTl.iY += aTrans;
            iVisArray[i]->iRect.iBr.iY += aTrans;
            if ( iVisArray[i]->iVisual )
                {
                iVisArray[i]->iVisual->Move( TPoint( 0, aTrans ), aTime );
                }
            }
        }
    aRootLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
    }

// ---------------------------------------------------------------------------
// SetScreenHeight
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::SetScreenHeight( TInt aScreenHeight )
    {
    FUNC_LOG;
    iScreenHeight = aScreenHeight;
    iTopBorder = -1 * iScreenHeight;
    iBottomBorder = 2 * iScreenHeight;
    }

// ---------------------------------------------------------------------------
// GetVisualWithChar
// ---------------------------------------------------------------------------
TFsRangedVisual* CFsTextViewerVisualizerData::GetVisualWithChar( 
        TInt aCharIndex )
    {
    FUNC_LOG;
    TFsRangedVisual* retVal = NULL;

    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iStart <= aCharIndex 
                && iVisArray[i]->iEnd >= aCharIndex )
            {
            retVal = iVisArray[i];
            i = iVisArray.Count();
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetFirstVisible
// ---------------------------------------------------------------------------
TFsRangedVisual* CFsTextViewerVisualizerData::GetFirstVisible()
    {
    FUNC_LOG;
    TFsRangedVisual* retVal = NULL;

    if ( iVisArray.Count() > 0 )
        {
        retVal = iVisArray[0];
        }
    
    if(retVal) // Coverity error fix, retVal could be NULL and used
        {
        for ( TInt i = 0; i < iVisArray.Count(); ++i )
            {
            if ( iVisArray[i]->iRect.iTl.iY >= iScreenOffset )
                {
                if ( iVisArray[i]->iRect.iTl.iY < retVal->iRect.iTl.iY )
                    {
                    retVal = iVisArray[i];
                    }
                else if ( iVisArray[i]->iRect.iTl.iY 
                        == retVal->iRect.iTl.iY )
                    {
                    if ( iVisArray[i]->iRect.iTl.iX < retVal->iRect.iTl.iX )
                        {
                        retVal = iVisArray[i];
                        }
                    }
                else if ( retVal->iRect.iTl.iY < iScreenOffset )
                    {
                    retVal = iVisArray[i];
                    }
                }
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetLastVisible
// ---------------------------------------------------------------------------
TFsRangedVisual* CFsTextViewerVisualizerData::GetLastVisible()
    {
    FUNC_LOG;
    TFsRangedVisual* retVal = NULL;

    if ( iVisArray.Count() > 0 )
        {
        retVal = iVisArray[0];
        }
    if(retVal) // Coverity error fix, retval could be null and used
        {
        for ( TInt i = 0; i < iVisArray.Count(); ++i )
            {
            if ( iVisArray[i]->iRect.iBr.iY <= iScreenOffset + iScreenHeight )
                {
                if ( iVisArray[i]->iRect.iBr.iY > retVal->iRect.iBr.iY )
                    {
                    retVal = iVisArray[i];
                    }
                else if ( iVisArray[i]->iRect.iBr.iY 
                        == retVal->iRect.iBr.iY )
                    {
                    if ( iVisArray[i]->iRect.iBr.iX > retVal->iRect.iBr.iX )
                        {
                        retVal = iVisArray[i];
                        }
                    }
                else 
                    {
                    }
                }
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetRangedVisual
// ---------------------------------------------------------------------------
TFsRangedVisual* CFsTextViewerVisualizerData::GetRangedVisual( 
        CAlfVisual* aVisual )
    {
    FUNC_LOG;
    TFsRangedVisual* retVal = NULL;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iVisual == aVisual )
            {
            retVal = iVisArray[i];
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// UpdateLinesNumbers
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::UpdateLinesNumbers()
    {
    FUNC_LOG;
    TInt lastLine = 1;
    TInt i = 0;
    if ( iVisArray.Count() <= 0 )
        {
        return;
        }
    do 
        {
        if ( iVisArray[i]->iIsActive )
            {
            lastLine = iVisArray[i]->iLineNumber;
            }
        ++i;
        } while (
                i < iVisArray.Count()
                && ( iVisArray[i]->iLineNumber == lastLine
                    || iVisArray[i]->iLineNumber == lastLine + 1 )
                );

    TInt lineNumber = lastLine;
    for ( ; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iIsActive )
            {
            if ( iVisArray[i]->iLineNumber != lastLine )
                {
                ++lineNumber;
                }
            lastLine = iVisArray[i]->iLineNumber;
            iVisArray[i]->iLineNumber = lineNumber;
            }
        }
    }

// ---------------------------------------------------------------------------
// GetTotalLines
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetTotalLines()
    {
    FUNC_LOG;
    TInt retVal = 0;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iIsActive 
                && iVisArray[i]->iLineNumber > retVal )
            {
            retVal = iVisArray[i]->iLineNumber;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// RemoveFromLine
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::RemoveFromLineL( 
        TInt aLineNumber, 
        CFsTextViewerCoordinator* aHelper )
    {
    FUNC_LOG;
    TInt retVal = GetFirstCharInLine( aLineNumber );
    if ( GetCursorCharOffset() >= retVal )
        {
        while ( GetCursorCharOffset() >= retVal || GetCursorCharOffset() == 0 )
            {
            MoveToPrevCharL();
            }
        SetCursorCharOffset( retVal - 1 );
        }
    
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iLineNumber >= aLineNumber )
            {
            if ( iVisArray[i]->iVisual )
                {
                iSelector->RemoveVisual( iVisArray[i] );
                iVisArray[i]->iVisual->RemoveAndDestroyAllD();
                iVisArray[i]->iVisual = NULL;
                }
            aHelper->RemoveRect( iVisArray[i]->iRect );
            delete iVisArray[i];
            iVisArray.Remove(i);
            --i;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetFirstCharInLine - gets the first character in line position
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetFirstCharInLine( TInt aLineNumber )
    {
    FUNC_LOG;
    TInt retVal = iLength;
    for ( TInt i = 0; i < iVisArray.Count(); ++i ) 
        {
        if ( iVisArray[i]->iLineNumber == aLineNumber  
                && iVisArray[i]->iStart < retVal )
            {
            retVal = iVisArray[i]->iStart;
            }
        }
    return retVal;
    }

// <cmail>
// ---------------------------------------------------------------------------
// GetCharLineNumber - returns line number for character position (offset)
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetCharLineNumber( TInt aCharNumber )
    {
    FUNC_LOG;
    TInt retVal = -1;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iStart <= aCharNumber  && aCharNumber <= iVisArray[i]->iEnd )
            {
            retVal = iVisArray[i]->iLineNumber;
            i = iVisArray.Count();
            }
        }
    return retVal;
    }
// </cmail>
// ---------------------------------------------------------------------------
// SetParser
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::SetParser( CFsTextParser* aParser )
    {
    FUNC_LOG;
    iParser = aParser;
    }

// ---------------------------------------------------------------------------
// FindAndHighlightHotspotL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::FindAndHighlightHotspotL( TInt aLine )
    {
    FUNC_LOG;
    TFsRangedVisual* visualFound = NULL;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iLineNumber == aLine )
            {
            if ( ( !visualFound 
                        || visualFound->iStart > iVisArray[i]->iStart ) 
                    && ( iVisArray[i]->iHotspot 
                        ||  iVisArray[i]->iEmbed ) )
                {
                visualFound = iVisArray[i];
                }
            }
        }
    
    if ( visualFound )
        {
        iSelector->SelectL( visualFound->iId );
        }
    }

// ---------------------------------------------------------------------------
// GetCursorLine
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetCursorLine()
    {
    FUNC_LOG;
    TInt retVal = -1;
    if ( iPosition.iVisual >= 0 && iPosition.iVisual < iVisArray.Count() )
        {
        retVal = iVisArray[iPosition.iVisual]->iLineNumber;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// ReloadPicturesL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::ReloadPicturesL( TInt aLine )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( ( iVisArray[i]->iVisual 
                    && !iVisArray[i]->iIsText ) 
                && ( aLine == -1 
                    || iVisArray[i]->iLineNumber == aLine ) )
            {
            iVisArray[i]->iVisual->RemoveAndDestroyAllD();
            iVisArray[i]->iVisual = NULL;
            RestoreL( iVisArray[i] );
            }
        }
    }

// ---------------------------------------------------------------------------
// GetTotalHeight
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetTotalHeight()
    {
    FUNC_LOG;
    TInt retVal = 0;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iRect.iBr.iY > retVal )
            {
            retVal = iVisArray[i]->iRect.iBr.iY; 
            }
        }

    retVal += iStatusLineLayout->Size().ValueNow().iY;

    return retVal;
    }

// ---------------------------------------------------------------------------
// HideActionButtonFromCurrentHighlightedHotspot
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData
::HideActionButtonFromCurrentHighlightedHotspot()
    {
    if(iSelector && iSelector->IsHotspotHighlighted())
        {
        iSelector->HideActionButtonFromCurrentHighlightedHotspot();
        }
    }

// ---------------------------------------------------------------------------
// ShowActionButtonForCurrentHighlightedHotspotL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData
::ShowActionButtonForCurrentHighlightedHotspotL()
    {
    if(iSelector && iSelector->IsHotspotHighlighted())
        {
        iSelector->ShowActionButtonForCurrentHighlightedHotspotL();
        }
    }

// ---------------------------------------------------------------------------
// HideAll
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::HideAll()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iVisual )
            {
            iVisArray[i]->iVisual->RemoveAndDestroyAllD();
            iVisArray[i]->iVisual = NULL;
            --iVisualAmount;
            }
        }
    }

// ---------------------------------------------------------------------------
// RestoreVisibleL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::RestoreVisibleL()
    {
    FUNC_LOG;
    TRect visibleArea = TRect( 0, 0, 500, iScreenHeight );
    visibleArea.Move( 0, iScreenOffset );
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( visibleArea.Intersects( iVisArray[i]->iRect ) )
            {
            RestoreL( iVisArray[i] );
            }
        }
    }

// ---------------------------------------------------------------------------
// RegisterHotspotsL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::RegisterHotspotsL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisArray.Count(); ++i )
        {
        if ( iVisArray[i]->iEmbed )
            {
            iSelector->AppendSelectVisL( 
                    iVisArray[i],
                    iVisArray[i]->iId,
                     CFsTextViewerSelect::EFsEmbedType );
            }
        else if ( iVisArray[i]->iHotspot )
            {
            if ( 0 == iVisArray[i]->iHotspotType )
                {
                iSelector->AppendSelectVisL(
                        iVisArray[i],
                        iVisArray[i]->iId,
                         CFsTextViewerSelect::EFsHotspotType );
                }
            else 
                {
                iSelector->AppendSelectVisL( 
                        iVisArray[i], 
                        iVisArray[i]->iId,
                         CFsTextViewerSelect::EFsLineType );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// SetStatusLineLayout
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::SetStatusLineLayout( CAlfAnchorLayout* aLayout )
    {
    FUNC_LOG;
    iStatusLineLayout = aLayout;
    }

// ---------------------------------------------------------------------------
// GetLineOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizerData::GetLineOffset( TInt aLine )
	{
	TInt retVal = -1;
	
	TInt i = 0;
	TInt lowestLine = 0;
	TInt highestLine = GetTotalLines();
	TInt positionInArrayOfLowest = 0;
	TInt positionInArrayOfHighest = 0;
	TBool exactMatch = EFalse;
	
	for ( i = 0; i < iVisArray.Count(); ++i )
		{
		if ( iVisArray[i]->iLineNumber == aLine )
			{
			retVal = iVisArray[i]->iRect.iTl.iY;
			exactMatch = ETrue;
			i = iVisArray.Count();
			}
		else if ( iVisArray[i]->iLineNumber < aLine && lowestLine < aLine )
			{
			lowestLine = iVisArray[i]->iLineNumber;
			positionInArrayOfLowest = i;
			}
		else if ( iVisArray[i]->iLineNumber > aLine && highestLine - aLine > iVisArray[i]->iLineNumber - aLine )
			{
			highestLine = iVisArray[i]->iLineNumber;
			positionInArrayOfHighest = i;
			}
		}
	
	if ( !exactMatch )
		{
		retVal = ( iVisArray[positionInArrayOfLowest]->iRect.iTl.iY + iVisArray[positionInArrayOfHighest]->iRect.iTl.iY ) / 2; 
		}
	
	return retVal;
	}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizerData::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CFsTextViewerVisualizerData
// ---------------------------------------------------------------------------
CFsTextViewerVisualizerData::CFsTextViewerVisualizerData(
        CAlfControl* aControl,
        CFsTextParser* aParser,
        CFsTextViewerSelectsManager* aSelector )
    : iControl( aControl ), iParser( aParser ), iSelector( aSelector )
    {
    FUNC_LOG;
    }

