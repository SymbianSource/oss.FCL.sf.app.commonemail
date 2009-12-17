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



#ifndef __FREESTYLEMAILUI_MSGDETAILSCONTROL_H_
#define __FREESTYLEMAILUI_MSGDETAILSCONTROL_H_

// SYSTEM INCLUDES
//<cmail> SF
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>
//</cmail>

// FORWARD DECLARATIONS
class CFSEmailUiMsgDetailsVisualiser;

class CFSEmailUiMsgDetailsControl : public CAlfControl
    {
    
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFSEmailUiMsgDetailsControl* NewL(CAlfEnv& aEnv, CFSEmailUiMsgDetailsVisualiser& aVisualiser);
    
    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFSEmailUiMsgDetailsControl* NewLC(CAlfEnv& aEnv, CFSEmailUiMsgDetailsVisualiser& aVisualiser);
    
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
    void HandleCommandL(TInt aCommand);
     
    /**
     * Virtual destructor.
     */
    ~CFSEmailUiMsgDetailsControl();
    
private:

    /**
     * Constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aVisualiser Reference to visualiser for command handling.
     */
    CFSEmailUiMsgDetailsControl(CFSEmailUiMsgDetailsVisualiser& aVisualiser);
    
    /**
     * Two-phased constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:
	CFSEmailUiMsgDetailsVisualiser& iVisualiser;
    
    };

#endif /*__FREESTYLEMAILUI_MSGDETAILSCONTROL_H_*/
