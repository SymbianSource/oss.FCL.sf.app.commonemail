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
* Description:  Declares control class for the application.
*
*/



#ifndef __FREESTYLEMAILUI_FOLDERLISTCONTROL_H_
#define __FREESTYLEMAILUI_FOLDERLISTCONTROL_H_

// SYSTEM INCLUDES
//<cmail>
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>
//</cmail>

#include "FreestyleEmailUiUtilities.h"

// FORWARD DECLARATIONS
class CFSEmailUiFolderListVisualiser;

class CFSEmailUiFolderListControl : public CAlfControl,
									public MFSEmailUiGenericTimerCallback
    {
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aEnv Pointer to visualiser, ownership not transferred.
     */
    static CFSEmailUiFolderListControl* NewL( CAlfEnv& aEnv, CFSEmailUiFolderListVisualiser& aVisualiser );
    
    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFSEmailUiFolderListControl* NewLC( CAlfEnv& aEnv, CFSEmailUiFolderListVisualiser& aVisualiser );
    
    /**
     * Event handling
     *
     * @param aEvent Event to be handled
     * @return ETrue if event was consumed, otherwise EFalse
     */
	TBool OfferEventL( const TAlfEvent& aEvent );  

    /**
     * Command handling
     *
     * @param aCommand Command to be handled
     */
    void HandleCommandL( TInt aCommand );
     
    /**
     * Virtual destructor.
     */
    ~CFSEmailUiFolderListControl();

public: // From base class MFSEmailUiGenericTimerCallback
    /**
     * Timer callback function.
     */
	void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );
	
	// Only for testing
	//void StarTestTimer();
private:

    /**
     * Constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aVisualiser Reference to visualiser for command handling.
     */
    CFSEmailUiFolderListControl( CFSEmailUiFolderListVisualiser& aVisualiser );
    
    /**
     * Two-phased constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:
	CFSEmailUiFolderListVisualiser& iVisualiser;
    
    };

#endif /*__FREESTYLEMAILUI_FOLDERLISTCONTROL_H_*/
