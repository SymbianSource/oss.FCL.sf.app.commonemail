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
* Description:  FreestyleEmailUi attachments list control class
*
*/



#ifndef FREESTYLEMAILUI_ATTACHMENTSLISTCONTROL_H_
#define FREESTYLEMAILUI_ATTACHMENTSLISTCONTROL_H_

// SYSTEM INCLUDES
//<cmail>
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>
//</cmail>

// FORWARD DECLARATIONS
class CFSEmailUiAttachmentsListVisualiser;

/**
* CFsGenericListAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CFreestyleEmailUiAttachmentsListControl : public CAlfControl,
                                  public MAlfTextureLoadingCompletedObserver
    {
    
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiAttachmentsListControl* NewL(CAlfEnv& aEnv, CFSEmailUiAttachmentsListVisualiser* aVisualiser);
    
    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiAttachmentsListControl* NewLC(CAlfEnv& aEnv, CFSEmailUiAttachmentsListVisualiser* aVisualiser);
    
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
    ~CFreestyleEmailUiAttachmentsListControl();
    
private:

    /**
     * Constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aVisualiser Reference to visualiser for command handling.
     */
    CFreestyleEmailUiAttachmentsListControl(CFSEmailUiAttachmentsListVisualiser* aVisualiser);
    
    /**
     * Two-phased constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:
	CFSEmailUiAttachmentsListVisualiser* iVisualiser;
    
    };

#endif /*FREESTYLEMAILUI_MAILLISTCONTROL_H_*/
