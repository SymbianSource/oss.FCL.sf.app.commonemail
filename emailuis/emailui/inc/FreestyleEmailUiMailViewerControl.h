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
* Description:  Declares top control class for email viewer
*
*/



#ifndef FREESTYLEEMAILUIMAILVIEWERCONTROL_H
#define FREESTYLEEMAILUIMAILVIEWERCONTROL_H

// SYSTEM INCLUDES
//<cmail> SF
#include <alf/alfcontrol.h>
//</cmail>

// FORWARD DECLARATIONS
class CFSEmailUiMailViewerVisualiser;

/**
* CFreestyleEmailUiMailViewerControl is top control class for capturing option and
*				 key events for the email viewer visualiser to handle
*/
class CFreestyleEmailUiMailViewerControl : public CAlfControl
    {
    
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiMailViewerControl* NewL( CAlfEnv& aEnv, CFSEmailUiMailViewerVisualiser& aVisualiser );
    
    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiMailViewerControl* NewLC( CAlfEnv& aEnv, CFSEmailUiMailViewerVisualiser& aVisualiser);
    
    /**
     * Virtual destructor.
     */
    virtual ~CFreestyleEmailUiMailViewerControl();
 
public: // From CAlfControl

	TBool OfferEventL( const TAlfEvent& aEvent );  

    void HandleCommandL( TInt aCommand );
    
private:

    /**
     * Constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aVisualiser Reference to visualiser for command handling.
     */
    CFreestyleEmailUiMailViewerControl( CFSEmailUiMailViewerVisualiser& aVisualiser);
    
    /**
     * Two-phased constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:

	// Handle to viewer visualiser
	CFSEmailUiMailViewerVisualiser& iVisualiser;
    
    };

#endif /*FREESTYLEEMAILUIMAILVIEWERCONTROL_H*/
