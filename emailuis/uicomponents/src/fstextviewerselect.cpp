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
* Description:  Base class for selectables (hotspots and expand areas)
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfvisual.h>
#include <alf/alftextvisual.h>
// </cmail>
#include "fstextviewerselect.h"

#include "fstextviewervisualizer.h"
#include "fstextviewervisualizerdata.h"
#include "fstextviewerselectsmanager.h"

// ---------------------------------------------------------------------------
// ~CFsTextViewerSelect
// ---------------------------------------------------------------------------
CFsTextViewerSelect::~CFsTextViewerSelect()
    {
    FUNC_LOG;
    iVisuals.Close();
    }

// ---------------------------------------------------------------------------
// SetType
// ---------------------------------------------------------------------------
void CFsTextViewerSelect::SetType( TFsTextViewerSelectType aType )
    {
    FUNC_LOG;
    iType = aType;
    }

// ---------------------------------------------------------------------------
// GetType
// ---------------------------------------------------------------------------
CFsTextViewerSelect::TFsTextViewerSelectType CFsTextViewerSelect::GetType()
    {
    FUNC_LOG;
    return iType;
    }

// ---------------------------------------------------------------------------
// SetId
// ---------------------------------------------------------------------------
void CFsTextViewerSelect::SetId( TInt aId )
    {
    FUNC_LOG;
    iId = aId;
    }

// ---------------------------------------------------------------------------
// GetId
// ---------------------------------------------------------------------------
TInt CFsTextViewerSelect::GetId() const
    {
    FUNC_LOG;
    return iId;
    }

// ---------------------------------------------------------------------------
// AppendVisualL
// ---------------------------------------------------------------------------
void CFsTextViewerSelect::AppendVisualL( TFsRangedVisual* aVisual )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( aVisual == iVisuals[i] )
            {
            return;
            }
        }
    if ( iVisuals.Count() > 0 )
        {
        iRect.iBr = aVisual->iRect.iBr;
        }
    else
        {
        iRect = aVisual->iRect;
        }
    iEndChar = aVisual->iEnd;
    iVisuals.AppendL( aVisual );
    }

// ---------------------------------------------------------------------------
// RemoveVisual
// ---------------------------------------------------------------------------
void CFsTextViewerSelect::RemoveVisual( TFsRangedVisual* aVisual )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i] == aVisual )
            {
            iVisuals.Remove( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// GetStartHeight
// ---------------------------------------------------------------------------
TInt CFsTextViewerSelect::GetStartHeight()
    {
    FUNC_LOG;
    TInt retVal = -1;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i]->iVisual->DisplayRectTarget().iTl.iY > retVal )
            {
            retVal = iVisuals[i]->iRect.iTl.iY;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetStartLine
// ---------------------------------------------------------------------------
TInt CFsTextViewerSelect::GetStartLine()
    {
    FUNC_LOG;
    TInt retVal = -1;
    CFsTextViewerVisualizerData* navigator = 
        iManager->Visualizer()->Navigator();
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i]->iLineNumber < retVal 
              || retVal < 0 ) 
            {
            retVal = iVisuals[i]->iLineNumber;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// IsVisible
// ---------------------------------------------------------------------------
TBool CFsTextViewerSelect::IsVisible()
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    TInt minY = -1;
    TInt maxY = -1;
    
    
    if ( iVisuals.Count() > 0 )
        {
        if ( !iVisuals[0]->iVisual )
            {
            retVal = EFalse;
            }
        else 
            {
            minY = iVisuals[0]->iVisual->DisplayRectTarget().iTl.iY;
            }
        }

    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( !iVisuals[i]->iVisual )
            {
            retVal = EFalse;
            }
        else
            {
            if ( minY > iVisuals[i]->iVisual->DisplayRectTarget().iTl.iY )
                {
                minY = iVisuals[i]->iVisual->DisplayRectTarget().iTl.iY;
                }
            if ( maxY < iVisuals[i]->iVisual->DisplayRectTarget().iBr.iY )
                {
                maxY = iVisuals[i]->iVisual->DisplayRectTarget().iBr.iY;
                }
            }
        }
    TInt screenHeight = iManager->Visualizer()->iDisplaySize.iHeight;
    if ( minY >= 0 && maxY <= screenHeight )
        {
        retVal = ETrue;
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// ContructL
// ---------------------------------------------------------------------------
void CFsTextViewerSelect::ConstructL()
    {
    FUNC_LOG;
    }

