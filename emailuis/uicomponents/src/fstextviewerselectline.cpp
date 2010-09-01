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
* Description:  Whole line highlight class
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfimagevisual.h>
#include <alf/alftextvisual.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfimage.h>
// </cmail>
#include <aknnotewrappers.h>

#include "fstextviewerselectline.h"
#include "fstextviewervisualizer.h"
#include "fstextviewervisualizerdata.h"
#include "fstextviewerselectsmanager.h"
#include "fstextviewervisualizersettings.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerSelectLine* CFsTextViewerSelectLine::NewL( 
        CFsTextViewerSelectsManager* aManager )
     {
    FUNC_LOG;
     CFsTextViewerSelectLine* self = 
         new (ELeave) CFsTextViewerSelectLine( aManager );
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop( self );
     return self;
     }

// ---------------------------------------------------------------------------
// SelectL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectLine::SelectL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( !iVisuals[i]->iVisual )
            {
            return;
            }
        }
    
    if(!iIsShown)
        {
        iIsShown = ETrue;
        
        TRect target = TRect( 0, 0, 0, 0 );
        TInt targetLine = 0;
        if ( iVisuals.Count() > 0 )
            {
            if ( iVisuals[0]->iIsText )
                {
//                CAlfTextVisual* tmp = static_cast<CAlfTextVisual*>( iVisuals[0]->iVisual );
                target = iVisuals[0]->iRect;
//                target.SetSize( tmp->TextExtents() );
                }
            else
                {
                target = iVisuals[0]->iRect;
                }
            targetLine = 
                iManager->Visualizer()->Navigator()
                ->GetRangedVisual(iVisuals[0]->iVisual)->iLineNumber;
            }
        TRect visual;
        TInt visualLine;
        for ( TInt i = 0; i < iVisuals.Count(); ++i )
            {
            if ( iVisuals[i]->iIsText )
                {
//                CAlfTextVisual* tmp = static_cast<CAlfTextVisual*>( iVisuals[i]->iVisual );
                visual = iVisuals[i]->iRect;
//                visual.SetSize( tmp->TextExtents() );
                }
            else
                {
                visual = iVisuals[i]->iRect;
                }
            
            visualLine = iVisuals[i]->iLineNumber;
            
            if( targetLine != visualLine )
                {
                CAlfImageVisual* highlightLine;
                highlightLine = CAlfImageVisual::AddNewL( 
                        *iManager->iControl, 
                        iManager->Visualizer()->BackgroundLayout() );
                highlightLine->SetFlag( EAlfVisualFlagManualLayout );
                highlightLine->SetScaleMode( CAlfImageVisual::EScaleFit );
                highlightLine->SetImage( 
                        TAlfImage( *iManager->iHighlightLine ) );
                highlightLine->SetRect( target );
                highlightLine->SetOpacity( 
                        iManager->iHighlightLineOpacity );
                    
                TFsPartBgVisual toAdd;
                toAdd.iVisual = highlightLine;
                toAdd.iRect = target;
                iManager->Visualizer()->Navigator()->AppendBgL( toAdd );
                    
                iHighlightLines.Append(highlightLine); 
                
                target = visual;
                targetLine = visualLine; 
                }
            else
                {
                
                if ( target.iTl.iX > visual.iTl.iX )
                    {
                    target.iTl.iX = visual.iTl.iX;
                    }
                if ( target.iBr.iX < visual.iBr.iX )
                    {
                    target.iBr.iX = visual.iBr.iX;
                    }
                if(target.iTl.iY > visual.iTl.iY)
                    {
                    target.iTl.iY = visual.iTl.iY;
                    }
                    
                if(target.iBr.iY < visual.iBr.iY)
                    {
                    target.iBr.iY = visual.iBr.iY;
                    }
                }
            
            if(i == (iVisuals.Count() - 1))
                {
                CAlfImageVisual* highlightLine;
                highlightLine = CAlfImageVisual::AddNewL( 
                        *iManager->iControl, 
                        iManager->Visualizer()->BackgroundLayout() );
                highlightLine->SetFlag( EAlfVisualFlagManualLayout );
                highlightLine->SetScaleMode( CAlfImageVisual::EScaleFit );
                highlightLine->SetImage( 
                        TAlfImage( *iManager->iHighlightLine ) );
                highlightLine->SetRect( target );
                highlightLine->SetOpacity( 
                        iManager->iHighlightLineOpacity );
                    
                TFsPartBgVisual toAdd;
                toAdd.iVisual = highlightLine;
                toAdd.iRect = target;
                iManager->Visualizer()->Navigator()->AppendBgL( toAdd );
                    
                iHighlightLines.Append( highlightLine ); 
                }
            }
            
        if( iIsActionButton && !iActionButton && iVisuals.Count() > 0)
            {     
            TAlfTimedPoint highlightedLineSize = iHighlightLines[0]->Size();
            TAlfTimedPoint highlightedLinePos = iHighlightLines[0]->Pos();
                    
            TInt actionButtonWidth = iManager->Visualizer()->iSettings->iActionButtonWidth;
            TInt actionButtonHeight = iManager->Visualizer()->iSettings->iActionButtonHeight;
            TInt actionButtonMargin = iManager->Visualizer()->iSettings->iActionButtonMargin;

            // Width of the actual text content area (without scroll bar)
            TInt contentWidth = iManager->Visualizer()->iContentSize.iWidth + 
                                iManager->Visualizer()->iSettings->iLeftMargin + 
                                iManager->Visualizer()->iSettings->iRightMargin;

            // If the width of the whole main pane is more than the width of
            // the actual content area, it means that scroll bar is visible.
            // In that case we don't need to leave any extra margin for the
            // action button, because the scroll bar has some margin already.
            // This way the button overlaps the text bit less in long lines.
            if( iManager->Visualizer()->iDisplaySize.iWidth > contentWidth )
                {
                actionButtonMargin = 0;
                }
                   
            TInt yTranslate = (highlightedLineSize.Target().iY - actionButtonHeight) / 2;
                   
            if(iManager->Visualizer()->iSettings->iMirroring)
                {   
                target.iTl.iX = actionButtonMargin;                
                target.iTl.iY = highlightedLinePos.Target().iY + yTranslate;
                target.iBr.iX = target.iTl.iX + actionButtonWidth;
                target.iBr.iY = target.iTl.iY + actionButtonHeight;
                }
            else
                {
                target.iTl.iX = contentWidth - actionButtonMargin - actionButtonWidth;
                target.iTl.iY = highlightedLinePos.Target().iY + yTranslate;
                target.iBr.iX = target.iTl.iX + actionButtonWidth;
                target.iBr.iY =target.iTl.iY + actionButtonHeight;
                }
                
            TFsPartBgVisual toAdd;
            iActionButton = CAlfImageVisual::AddNewL( 
                    *iManager->iControl, 
                    iManager->Visualizer()->BackgroundLayout() );
            iActionButton->SetScaleMode( CAlfImageVisual::EScaleFit );
            iActionButton->SetImage( TAlfImage( *iManager->iActionButton ) );
            iActionButton->SetRect( target );
            iActionButton->SetOpacity( iManager->iActionButtonOpacity );

/*
            if(iManager->Visualizer()->iSettings->iMirroring)
                {
                iActionButton->SetTurnAngle(180);
                }
*/
                
            toAdd.iVisual = iActionButton;
            toAdd.iRect = target;
            iManager->Visualizer()->Navigator()->AppendBgL( toAdd );
            }
        }
    }

// ---------------------------------------------------------------------------
// Deselect
// ---------------------------------------------------------------------------
void CFsTextViewerSelectLine::DeselectL()
    {
    FUNC_LOG;
    TInt ala = iHighlightLines.Count();
    
    for( TInt i = iHighlightLines.Count() - 1 ; i >= 0 ; --i )
        {
        if ( iManager->Visualizer()->Navigator()
                ->RemoveBg( iHighlightLines[i] ) )
            {
            iHighlightLines[i]->RemoveAndDestroyAllD();
            }
        
        iHighlightLines[i] = NULL;
        iHighlightLines.Remove(i);
        }
        
    iIsShown = EFalse;

    if ( iActionButton )
        {
        if(iManager->Visualizer()->Navigator()->RemoveBg( iActionButton ))
            {
            iActionButton->RemoveAndDestroyAllD();
            }
        
        iActionButton = NULL;
        }
    }

// ---------------------------------------------------------------------------
// SelectedL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectLine::SelectedL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// HideActionButton
// ---------------------------------------------------------------------------
void CFsTextViewerSelectLine::HideActionButton()
    {
    FUNC_LOG;
    if ( iActionButton )
        {
        if(iManager->Visualizer()->Navigator()->RemoveBg( iActionButton ))
            {
            iActionButton->RemoveAndDestroyAllD();
            }
        
        iActionButton = NULL;
        }
        
    iIsActionButton = EFalse;
    }

// ---------------------------------------------------------------------------
// ShowActionButtonL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectLine::ShowActionButtonL()
    {
    FUNC_LOG;
    if(!iIsActionButton && iVisuals.Count() > 0 && !iActionButton )
        {
        iIsActionButton = ETrue;
        
        TInt pxTranslation = 10;    
        TRect target = TRect( 0, 0, 0, 0 );
           
        if(iManager->Visualizer()->iSettings->iMirroring)
            {
            target = iVisuals[0]->iVisual->DisplayRectTarget();
                   
            target.iBr.iX = target.iTl.iX + pxTranslation;
            target.iTl.iX -= pxTranslation; 
            }
        else 
            {
            target = iVisuals[iVisuals.Count() - 1]->iVisual->DisplayRectTarget();
            
            target.iTl.iX = target.iBr.iX - pxTranslation;
            target.iBr.iX += pxTranslation;   
            }
                
        TFsPartBgVisual toAdd;
        iActionButton = CAlfImageVisual::AddNewL( 
                    *iManager->iControl, 
                    iManager->Visualizer()->BackgroundLayout() );
        iActionButton->SetScaleMode( CAlfImageVisual::EScaleFit );
        iActionButton->SetImage( TAlfImage( *iManager->iActionButton ) );
        iActionButton->SetRect( target );
        iActionButton->SetOpacity( iManager->iActionButtonOpacity );

/*
        if(iManager->Visualizer()->iSettings->iMirroring)
            {
            iActionButton->SetTurnAngle(180);
            }
*/
               
        toAdd.iVisual = iActionButton;
        toAdd.iRect = target;
        iManager->Visualizer()->Navigator()->AppendBgL( toAdd );
        }
    }
// ---------------------------------------------------------------------------
// ~CFsTextViewerSelectLine
// ---------------------------------------------------------------------------
CFsTextViewerSelectLine::~CFsTextViewerSelectLine()
    {
    FUNC_LOG;
    for(TInt i = iHighlightLines.Count() - 1 ; i >= 0 ; --i)
        {
        iHighlightLines[i]->RemoveAndDestroyAllD();
        iHighlightLines[i] = NULL;
        iHighlightLines.Remove(i);
        }
    
    iHighlightLines.Close();
    }

// ---------------------------------------------------------------------------
// CFsTextViewerSelectLine
// ---------------------------------------------------------------------------
CFsTextViewerSelectLine::CFsTextViewerSelectLine( 
        CFsTextViewerSelectsManager* aManager ) 
    {
    FUNC_LOG;
    iManager = aManager;
    iType = EFsLineType;
    iActionButton = NULL;
    
    iIsShown = EFalse;
    iIsActionButton = ETrue;
    }


