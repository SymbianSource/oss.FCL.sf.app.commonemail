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
* Description: This file implements class CFsTextViewerSelectHotspot.
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <aknnotewrappers.h>
// </cmail>
#include "fstextviewerselecthotspot.h"
#include "fstextviewervisualizer.h"
#include "fstextviewervisualizerdata.h"
#include "fstextviewerselectsmanager.h"
#include "fstextviewerskindata.h"
#include "fstextstylemanager.h"

CFsTextViewerSelectHotspot* CFsTextViewerSelectHotspot::NewL( 
        CFsTextViewerSelectsManager* aManager )
     {
    FUNC_LOG;
     CFsTextViewerSelectHotspot* self = 
         new (ELeave) CFsTextViewerSelectHotspot( aManager );
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop( self );
     return self;
     }

// ---------------------------------------------------------------------------
// SelectL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectHotspot::SelectL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( !iVisuals[i]->iVisual )
            {
            return;
            }
        CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( iVisuals[i]->iVisual ) );
        if( textVisual )
            {
            textVisual->SetColor( 
                iManager->Visualizer()->iSkinData->iHotspotSelectedColor );
            textVisual->SetTextStyle(
                    iManager->Visualizer()->iTextStyleManager
                    ->GetStyleIDWithOpositeUnderlineL(  
                        iManager->Visualizer()->Navigator()
                        ->GetTextStyle( textVisual ) ) 
                    );
            }
        }
    }

// ---------------------------------------------------------------------------
// Deselect
// ---------------------------------------------------------------------------
void CFsTextViewerSelectHotspot::DeselectL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iVisuals.Count(); ++i )
        {
        if ( iVisuals[i]->iVisual )
            {
            CAlfTextVisual* textVisual( dynamic_cast<CAlfTextVisual*>( iVisuals[i]->iVisual ) );
            if ( textVisual )
                {
                textVisual->SetColor( 
                        iManager->Visualizer()->iSkinData->iHotspotDimmedColor );
                
                textVisual->SetTextStyle(
                        iManager->Visualizer()->iTextStyleManager
                        ->GetStyleIDWithOpositeUnderlineL(  
                            iVisuals[i]->iTextStyleId ) 
                        );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// SelectedL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectHotspot::SelectedL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsTextViewerSelectHotspot
// ---------------------------------------------------------------------------
CFsTextViewerSelectHotspot::CFsTextViewerSelectHotspot( 
        CFsTextViewerSelectsManager* aManager ) 
    {
    FUNC_LOG;
    iManager = aManager;
    iType = EFsHotspotType;
    }

