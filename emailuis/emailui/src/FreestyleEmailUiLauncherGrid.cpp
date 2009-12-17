/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  FreestyleEmailUi editor control implementation
*
*/


// INTERNAL INCLUDES
#include "emailtrace.h"
#include "FreestyleEmailUiLauncherGrid.h"
#include "FreestyleEmailUiLauncherGridVisualiser.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutHandler.h"

#include <AknUtils.h>

#include <alf/alfevent.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alfmappingfunctions.h>
#include <alf/alfroster.h>


CFSEmailUiLauncherGrid* CFSEmailUiLauncherGrid::NewL( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi )
	{
    FUNC_LOG;
    CFSEmailUiLauncherGrid* self = CFSEmailUiLauncherGrid::NewLC( aEnv, aAppUi );
    CleanupStack::Pop(self);
    return self;
	}


CFSEmailUiLauncherGrid* CFSEmailUiLauncherGrid::NewLC( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi )
	{
    FUNC_LOG;
    CFSEmailUiLauncherGrid* self = new (ELeave) CFSEmailUiLauncherGrid( aAppUi );
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
	}
// <cmail>
CFSEmailUiLauncherGrid::CFSEmailUiLauncherGrid(CFreestyleEmailUiAppUi* aAppUi )
	: CAlfControl(),
	iAppUi(aAppUi),
	iDraggingSet(EFalse)
	{
    FUNC_LOG;
	}

CFSEmailUiLauncherGrid::~CFSEmailUiLauncherGrid()
	{
    FUNC_LOG;
	}

TBool CFSEmailUiLauncherGrid::OfferEventL(const TAlfEvent& aEvent)
	{
    FUNC_LOG;
	TBool result(EFalse);
	if ( iVisualiser ) // Offer events to view
        {
        if( aEvent.IsPointerEvent() )
            {
            result = iVisualiser->HandlePointerEventL( aEvent ); 
            }
        else
            {
            result = iVisualiser->OfferEventL( aEvent );
            }
        }
    else
        {
        result = KErrGeneral; 
        }
    return result;    
	}
// </cmail>
void CFSEmailUiLauncherGrid::ConstructL( CAlfEnv& aEnv )
	{
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);
	}


void CFSEmailUiLauncherGrid::SetVisualiserL( CFSEmailUiLauncherGridVisualiser* aVisualiser )
	{
    FUNC_LOG;
    iVisualiser = aVisualiser;
	}

void CFSEmailUiLauncherGrid::NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& aDisplay )
    {
    FUNC_LOG;
    
    //Add & remove extra touch events. 
    if(aIsVisible && !iDraggingSet)
        {
        iDraggingSet = ETrue; 
        aDisplay.Roster().AddPointerEventObserver(EAlfPointerEventReportDrag, *this);
        aDisplay.Roster().AddPointerEventObserver(EAlfPointerEventReportLongTap, *this);
        aDisplay.Roster().AddPointerEventObserver(EAlfPointerEventReportUnhandled, *this);
        }
    else if(!aIsVisible && iDraggingSet )
        {
        aDisplay.Roster().RemovePointerEventObserver(EAlfPointerEventReportDrag, *this);
        aDisplay.Roster().RemovePointerEventObserver(EAlfPointerEventReportLongTap, *this);
        aDisplay.Roster().RemovePointerEventObserver(EAlfPointerEventReportUnhandled, *this);        
        iDraggingSet = EFalse;
        }
    }
