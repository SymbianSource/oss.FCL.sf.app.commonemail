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
* Description:  Implementation of class CFsTextViewerCoordinator
*
*/


#include "emailtrace.h"
#include "fstextviewercoordinator.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFsTextViewerCoordinator* CFsTextViewerCoordinator::NewL()
    {
    FUNC_LOG;
    CFsTextViewerCoordinator* self = 
        new ( ELeave ) CFsTextViewerCoordinator();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerCoordinator
// ---------------------------------------------------------------------------
//
CFsTextViewerCoordinator::~CFsTextViewerCoordinator()
    {
    FUNC_LOG;
    if ( iObstacles )
        {
        iObstacles->ResetAndDestroy();
        delete iObstacles;
        iObstacles = NULL;
        }
    }

// ---------------------------------------------------------------------------
// RegisterObstacle
// ---------------------------------------------------------------------------
//
void CFsTextViewerCoordinator::RegisterObstacleL( const TRect& aRect )
    {
    FUNC_LOG;
    TRect* append = new (ELeave) TRect();
    append->iTl = aRect.iTl;
    append->iBr = aRect.iBr;
    iObstacles->AppendL( append );
    }

// ---------------------------------------------------------------------------
// Contains
// ---------------------------------------------------------------------------
//
TBool CFsTextViewerCoordinator::Contains( const TRect aRect ) 
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    if ( aRect.iBr.iX > iSize.iWidth + iLeftMargin )
        {
        retVal = ETrue;
        iLastObstacle = -1;
        }
    else 
        {
        for ( TInt i = 0; i < iObstacles->Count(); ++i )
            {
            if ( iObstacles->At(i)->Intersects( aRect ) )
                {
                iLastObstacle = i;
                retVal = ETrue;
                }
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// SetSize
// ---------------------------------------------------------------------------
//
void CFsTextViewerCoordinator::SetSize( const TSize& aSize )
    {
    FUNC_LOG;
    iSize = aSize;
    }

// ---------------------------------------------------------------------------
// NextFreeCol
// ---------------------------------------------------------------------------
//
TInt CFsTextViewerCoordinator::NextFreeCol() const
    {
    FUNC_LOG;
    TInt retVal = 0;
    if ( iLastObstacle >=0 && iLastObstacle < iObstacles->Count() )
        {
        retVal = iLastObstacle == -1 ? 0 : iObstacles->At(iLastObstacle)->iBr.iX;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// NextFreeCol
// ---------------------------------------------------------------------------
TBool CFsTextViewerCoordinator::NextFreeCol( TRect& aRect, TBidiText::TDirectionality aDirection ) const
    {
    FUNC_LOG;
    TRect target = aRect;
    TBool retVal = EFalse;
    TInt dX;
    
    for ( TInt i = 0; i < iObstacles->Count(); ++i )
        {
        if ( target.Intersects( *iObstacles->At(i) ) )
            {
            if ( aDirection == TBidiText::ELeftToRight )
                {
                dX = iObstacles->At(i)->iBr.iX - target.iTl.iX;
                target.iTl.iX += dX;
                target.iBr.iX += dX;
                }
            else
                {
                dX = target.iBr.iX - iObstacles->At(i)->iTl.iX;
                target.iTl.iX -= dX;
                target.iBr.iX -= dX;
                }
            }
        }
    
    if ( target.iBr.iX > iSize.iWidth - iRightMargin || target.iTl.iX < iLeftMargin )
            {
            if ( aDirection == TBidiText::ELeftToRight )
                {
                dX = target.iTl.iX - iLeftMargin ;
                target.iTl.iX -= dX;
                target.iBr.iX -= dX;
                target.iTl.iY += target.Height();
                target.iBr.iY += target.Height();
                retVal = ETrue;
                }
            else 
                {
                dX = iSize.iWidth - iRightMargin - target.iBr.iX;
                target.iTl.iX += dX;
                target.iBr.iX += dX;
                target.iTl.iY += target.Height();
                target.iBr.iY += target.Height();
                retVal = ETrue;
                }
            }
    
    aRect = target;
    return retVal;
    }

// ---------------------------------------------------------------------------
// IsNewLine
// ---------------------------------------------------------------------------
//
TBool CFsTextViewerCoordinator::IsNewLine() const
    {
    FUNC_LOG;
    TBool retVal;
    retVal = iLastObstacle == -1 ? ETrue : EFalse;
    return retVal;
    }

// ---------------------------------------------------------------------------
// SetNewLine
// ---------------------------------------------------------------------------
//
void CFsTextViewerCoordinator::SetNewLine()
    {
    FUNC_LOG;
    iLastObstacle = -1;
    }

// ---------------------------------------------------------------------------
// RemoveRect
// ---------------------------------------------------------------------------
void CFsTextViewerCoordinator::RemoveRect( TRect aRect )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iObstacles->Count(); ++i )
        {
        if ( iObstacles->At(i)->iTl.iX ==  aRect.iTl.iX && iObstacles->At(i)->iTl.iY ==  aRect.iTl.iY )
            {
            delete iObstacles->At(i);
            iObstacles->At(i) = NULL;
            iObstacles->Delete(i);
            }
        }

    iLastObstacle = -1;

    if ( iLastObstacle >= iObstacles->Count() )
        {
        iLastObstacle = iObstacles->Count() - 1;
        }
    }

// ---------------------------------------------------------------------------
// RemoveFromOffset
// ---------------------------------------------------------------------------
void CFsTextViewerCoordinator::RemoveFromOffset( TInt aOffset )
    {
    FUNC_LOG;
    for ( TInt i = aOffset; i < iObstacles->Count(); ++i )
        {
        delete iObstacles->At(i);
        iObstacles->At(i) = NULL;
        iObstacles->Delete(i);
        }
    iLastObstacle = iObstacles->Count() - 1;
    }

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
void CFsTextViewerCoordinator::Reset()
    {
    FUNC_LOG;
    iObstacles->ResetAndDestroy();
    iLastObstacle = -1;
    }

// ---------------------------------------------------------------------------
// SetLeftMargin
// ---------------------------------------------------------------------------
void CFsTextViewerCoordinator::SetLeftMargin( TInt aMargin )
    {
    FUNC_LOG;
    iLeftMargin = aMargin;
    }

// ---------------------------------------------------------------------------
// SetRightMargin
// ---------------------------------------------------------------------------
void CFsTextViewerCoordinator::SetRightMargin( TInt aMargin )
	{
	iRightMargin = aMargin;
	}

// ---------------------------------------------------------------------------
// CFsTextViewerCoordinator
// ---------------------------------------------------------------------------
CFsTextViewerCoordinator::CFsTextViewerCoordinator()
    {
    FUNC_LOG;
    iLastObstacle = -1;
    iLeftMargin = 0;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerCoordinator::ConstructL()
    {
    FUNC_LOG;
    iObstacles = new (ELeave) CArrayPtrSeg<TRect>(10);
    }

