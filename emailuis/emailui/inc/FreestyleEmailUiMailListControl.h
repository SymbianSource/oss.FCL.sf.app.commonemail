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
* Description:  Declares control class for the application.
*
*/



#ifndef FREESTYLEMAILUI_MAILLISTCONTROL_H_
#define FREESTYLEMAILUI_MAILLISTCONTROL_H_

// SYSTEM INCLUDES
//<cmail>
#include <alf/alfcontrol.h>
#include <alf/alftexturemanager.h>
#include <aknenv.h>
//</cmail>

// FORWARD DECLARATIONS
class CFSEmailUiMailListVisualiser;

/**
* CFsGenericListAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CFreestyleEmailUiMailListControl : public CAlfControl,
                                  public MAlfTextureLoadingCompletedObserver
    {
    
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiMailListControl* NewL(CAlfEnv& aEnv, CFSEmailUiMailListVisualiser* aVisualiser);
    
    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiMailListControl* NewLC(CAlfEnv& aEnv, CFSEmailUiMailListVisualiser* aVisualiser);
    
    /**
     * Texture loading observer method.
     * 
     * @param aTexture Reference to the loaded texture object.
     * @param aTextureId Texture id.
     * @param aErrorCode Error code.
     */
    void TextureLoadingCompleted( CAlfTexture& aTexture,
                                  TInt aTextureId,
                                  TInt aErrorCode );
 
	TBool OfferEventL( const TAlfEvent& aEvent );  
    void HandleCommandL(TInt aCommand);
     
    /**
     * Virtual destructor.
     */
    ~CFreestyleEmailUiMailListControl();
    
    // <cmail> Touch
    /**
     * From CAlfControl
     */
    void NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& aDisplay );
    // </cmail>
    
private:

    /**
     * Constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aVisualiser Reference to visualiser for command handling.
     */
    CFreestyleEmailUiMailListControl(CFSEmailUiMailListVisualiser* aVisualiser);
    
    /**
     * Two-phased constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:
	CFSEmailUiMailListVisualiser* iVisualiser;
    
    };

#endif /*FREESTYLEMAILUI_MAILLISTCONTROL_H_*/
