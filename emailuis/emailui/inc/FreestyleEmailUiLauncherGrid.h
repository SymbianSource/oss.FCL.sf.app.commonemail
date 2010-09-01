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
* Description:  FreestyleEmailUi main UI grid control definition
*
*/



#ifndef __FREESTYLEEMAILUI_LAUNCHERGRID_H__
#define __FREESTYLEEMAILUI_LAUNCHERGRID_H__

// SYSTEM INCLUDES

// INTERNAL INCLUDES
#include "FreestyleEmailUiLauncherGridVisualiser.h"

// FORWARD DECLARATIONS
class CAlfAnchorLayout;
class CAlfEnv;
class CFreestyleEmailUiAppUi;


class CFSEmailUiLauncherGrid : public CAlfControl
    {
public:
    static CFSEmailUiLauncherGrid* NewL( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi );
    static CFSEmailUiLauncherGrid* NewLC( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi );
    virtual ~CFSEmailUiLauncherGrid();

public:
	void SetVisualiserL( CFSEmailUiLauncherGridVisualiser* aVisualiser );
  	virtual TBool OfferEventL( const TAlfEvent& aEvent );  

protected:  	
  	/**
  	 * @see CAlfControl::NotifyControlVisibility 
  	 */
  	void NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& aDisplay );
  	
private: 
    CFSEmailUiLauncherGrid( CFreestyleEmailUiAppUi* aAppUi );
    void ConstructL(CAlfEnv& aEnv);
    
private:
	CFreestyleEmailUiAppUi* iAppUi; 
    CFSEmailUiLauncherGridVisualiser* iVisualiser;
    TInt iSelectedIndex; 
    TBool iUpdatingVisualizer;       
    // <cmail>            
    TBool iDraggingSet; 
    // </cmail> 
    };

#endif // __FREESTYLEEMAILUI_LAUNCHERGRID_H__

