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



#ifndef FREESTYLEMAILUI_SEACRHLISTCONTROL_H_
#define FREESTYLEMAILUI_SEACRHLISTCONTROL_H_

// SYSTEM INCLUDES
//<cmail>
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>
//</cmail>

// FORWARD DECLARATIONS
class CFSEmailUiSearchListVisualiser;


class CFreestyleEmailUiSearchListControl : public CAlfControl,
                                  public MAlfTextureLoadingCompletedObserver
    {
    
public:

    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiSearchListControl* NewL(CAlfEnv& aEnv, CFSEmailUiSearchListVisualiser* aVisualiser);
    
    /**
     * Two-phased constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     */
    static CFreestyleEmailUiSearchListControl* NewLC(CAlfEnv& aEnv, CFSEmailUiSearchListVisualiser* aVisualiser);
    
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
    ~CFreestyleEmailUiSearchListControl();
    
private:

    /**
     * Constructor.
     * 
     * @param aEnv Reference to the ALF environment.
     * @param aVisualiser Reference to visualiser for command handling.
     */
    CFreestyleEmailUiSearchListControl(CFSEmailUiSearchListVisualiser* aVisualiser);
    
    /**
     * Two-phased constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:
	CFSEmailUiSearchListVisualiser* iVisualiser;
    
    };

#endif /*FREESTYLEMAILUI_MAILLISTCONTROL_H_*/
