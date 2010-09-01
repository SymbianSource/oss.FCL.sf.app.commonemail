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
* Description:  Expand area class
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alftextvisual.h>
#include <alf/alflayout.h>
// </cmail>
#include "fstextviewerselectembed.h"
#include "fstextviewervisualizer.h"
#include "fstextviewervisualizerdata.h"
#include "fstextviewerselectsmanager.h"

CFsTextViewerSelectEmbed* CFsTextViewerSelectEmbed::NewL( 
        CFsTextViewerSelectsManager* aManager )
     {
    FUNC_LOG;
     CFsTextViewerSelectEmbed* self = 
         new (ELeave) CFsTextViewerSelectEmbed( aManager );
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop( self );
     return self;
     }

// ---------------------------------------------------------------------------
// SelectL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectEmbed::SelectL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i]->iVisual )
            {
            CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( iVisuals[i]->iVisual ) );
            if ( textVisual )
                {
                textVisual->SetColor( KRgbRed );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Deselect
// ---------------------------------------------------------------------------
void CFsTextViewerSelectEmbed::DeselectL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i]->iVisual )
            {
            CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( iVisuals[i]->iVisual ) );
            if ( textVisual )
                {
                textVisual->SetColor( KRgbBlack );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// SelectedL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectEmbed::SelectedL()
    {
    FUNC_LOG;
    if ( iIsExpanded )
        {
        iManager->Visualizer()->CollapseL( iLayout, iId );
        iIsExpanded = EFalse;
        }
    else if ( EFalse == iIsExpanded && iLayout )
        {
        iManager->Visualizer()->ExpandExistingL( iLayout, iLayoutSize, iId );
        iIsExpanded = ETrue;
        }
    else 
        {
        TInt visIndex;
        visIndex = 
            iManager->Visualizer()->Navigator()->Find( 
                    iVisuals[iVisuals.Count() - 1]->iVisual );
        TInt rtStart = 
            iManager->Visualizer()->Navigator()->GetVisEndOffset( 
                    visIndex ) + 1;
        iLayout = iManager->Visualizer()->ExpandL( 
                iId, 
                rtStart, 
                iVisuals[iVisuals.Count() - 1]->iVisual );
        iLayoutSize = TSize( 
                iLayout->Size().Target().iX, 
                iLayout->Size().Target().iY );
        iIsExpanded = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CFsTextViewerSelectEmbed
// ---------------------------------------------------------------------------
CFsTextViewerSelectEmbed::CFsTextViewerSelectEmbed( 
        CFsTextViewerSelectsManager* aManager )
    {
    FUNC_LOG;
    iManager = aManager;
    iType = EFsEmbedType;
    iLayout = NULL;
    iIsExpanded = EFalse;
    }

