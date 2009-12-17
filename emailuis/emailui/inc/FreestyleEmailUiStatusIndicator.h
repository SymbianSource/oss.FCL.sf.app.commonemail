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
* Description:  FreestyleEmailUi status indicator definition
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_STATUSINDICATOR_H__
#define __FREESTYLEEMAILUI_STATUSINDICATOR_H__

//<cmail> SF
#include <alf/alftextvisual.h>
#include <alf/alfanchorlayout.h>
//</cmail>
#include "FreestyleEmailUiUtilities.h"	// FSEmailUiGenericTimer

class CFreestyleEmailUiAppUi;

typedef RPointerArray<CAlfTexture> TFsEmailUiStatusIconArray;

/**
 *  Generic indicator class.
 *
 *  Shows an indication in bottom of the screen. Indication can have one icon
 *  and/or one line of text.
 *
 */
class CFSEmailUiStatusIndicator : public CAlfControl,
								  public MFSEmailUiGenericTimerCallback
	{
public:
    // These special values are supposed to be less than zero
	enum TStatusIndicationDuration
		{
		/* Use indicator default duration (currently 3 seconds) */
		EIndicationDefaultDuration = -1000,
		
		/* Indicator is not hided automatically, caller needs to hide it
		   manually by calling HideIndicatorL() */
		EIndicationNoAutomaticHiding,
		
		/* Indicator is first shown fully opague and then it fades out 
		   partially so that it becomes partially transparent.
		   Indicator is not hided automatically, caller needs to hide it
		   manually by calling HideIndicatorL(). */
		EIndicationNoAutomaticHidingFading,
		
        /* For internal use. Shows the indicator with changed layout values,
           without any additional move/fade effects. Original duration is
           not affected. */
        EIndicationLayoutUpdated
		};

	/* Built-in indication types */
	enum TStatusIndicationTypes 
		{
		EIndicationSynchronising,
		EIndicationConnected,
		EIndicationConnecting,
		EIndicationDisconnectedGeneral,
		EIndicationDisconnectedError,
		EIndicationDisconnectedLowBattery,
		EIndicationMailSent,
		EIndicationCancelSynchronising,
		EIndicationDownloadStarted,
		EIndicationDownloadProgress
		};
	
public:

    /**
     * Symbian two-phased constructor. Constructs an instance of
     * CFSEmailUiStatusIndicator and adds it to given control group. The
     * ownership of constructed instance is not returned to caller.
     */
    static CFSEmailUiStatusIndicator* NewL( CAlfEnv& aEnv,
        CAlfControlGroup& aControlGroup, CFreestyleEmailUiAppUi* aAppUi );

    /**
     * Destructor.
     */
	~CFSEmailUiStatusIndicator();

    /**
     * Show one of the built in indications defined by TStatusIndicationTypes.
     *
     * @param aIndicationType Indicator type
     * @param aFirstLineText Text shown in first line (e.g. mail box name).
     *                       If NULL (default parameter), then only the default text is shown.
     * @param aDuration How long the indication is shown. If not specified, uses
     *                  the default duration. If set to EIndicationNoAutomaticHiding,
     *                  need to be hided manually by calling HideIndicatorL().
     */
	void ShowIndicatorL( TStatusIndicationTypes aIndicationType,
						 const TDesC* aFirstLineText = NULL,
	                     TInt aDuration = EIndicationDefaultDuration );

    /**
     * Show custom indication defined by parameters.
     *
     * @param aFirstLineText Indication first line text. Set to NULL if no text in indication.
     * @param aSecondLineText Indication second line text.If NULL (default parameter),
     *                        only one line of text is shown.
     * @param aIconTexture Icon texture. Set to NULL (default parameter) if no
     *                     icon in indication.
     * @param aDuration How long the indication is shown. If not specified, uses
     *                  the default duration (3 seconds). If set to
     *                  EIndicationNoAutomaticHiding, need to be hided manually
     *                  by calling HideIndicatorL().
     */
	void ShowIndicatorL( TDesC* aFirstLineText,
						 TDesC* aSecondLineText = NULL,
						 CAlfTexture* aIconTexture = NULL,
	                     TInt aDuration = EIndicationDefaultDuration );
	                     
    /**
     * Resets the contents of the indicator.
     *
     * @param aFirstLineText Indication first line text. Set to NULL if the text should not be changed.
     * @param aSecondLineText Indication second line text. Set to NULL (default parameter) 
     *                        if the text should not be changed.
     * @param aIconTexture Icon texture. Set to NULL (default parameter) if icon should not be changed.
     */
    void SetContentsL( const TDesC* aFirstLineText,
                       const TDesC* aSecondLineText = NULL,
                       const CAlfTexture* aIconTexture = NULL );

    /**
     * Hide currently shown indicator.
     * @param   aDelayBeforeHidingInMs  Hiding happens after this amount on milliseconds. Defaults to 0.
     */
    void HideIndicator( TInt aDelayBeforeHidingInMs = 0 );

	TBool IsVisible();
	
	/**
	 * This function should be used to notify the notifier about changes in screen layout.
	 */
	void NotifyLayoutChange();
	
	// Handle foreground event (called by mail viewer visualiser for download screen and appui for connect popup)
	void HandleForegroundEventL();
	
public: // From base class CAlfControl

	void HandleCommandL(TInt aCommand);

	TBool OfferEventL(const TAlfEvent& aEvent);

public: // From base class MFSEmailUiGenericTimerCallback

    /**
     * Timer callback function.
     */
	void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );
	
private:
	CFSEmailUiStatusIndicator(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );
	void ConstructL();

	void DoShowIndicator( TInt aDuration );
	void PositionVisuals();
	void CalculateSizeAttributes();
	void LoadIconsL();
	void SetTextByResourceIdL( TInt aResourceId, CAlfTextVisual* aTextVisual );
	void SetOpacity( TReal aOpacity, TInt aTransitionTime );
	void RotateImageL();
	
	void DoFirstStartL();
	
private: // data

    CAlfEnv& iEnv;
	CFreestyleEmailUiAppUi* iAppUi;
	CAlfControlGroup& iControlGroup;
	CAlfLayout* iParentLayout;
	CAlfAnchorLayout* iContentLayout;
	
   	TRect iParentRect;
	TInt iContentHeight;
	TInt iIconWidth;

	TInt iTextPadding;
	TInt iBorderPadding;
	
	TBool iHasIcon;
	TBool iHasSecondTextLine;
	TBool iVisible;
	TBool iUseCompactLayout;

	TFsEmailUiStatusIconArray iIconArray;
	
	CAlfTextVisual* iFirstTextVisual;
	CAlfTextVisual* iSecondTextVisual;
	CAlfImageVisual* iImageVisual;
	CAlfTexture* iIconSynchronisation;

	CFSEmailUiGenericTimer* iTimer;
	TBool iFadeOutAfterTimeOut;
	TBool iFirstStartCompleted;
};

#endif //__FREESTYLEEMAILUI_STATUSINDICATOR_H__
