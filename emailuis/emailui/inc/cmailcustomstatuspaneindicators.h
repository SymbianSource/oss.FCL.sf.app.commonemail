/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines class CCustomStatuspaneIndicators.
*
*/

#ifndef CCUSTOMSTATUSPANEINDICATORS_H_
#define CCUSTOMSTATUSPANEINDICATORS_H_

// INCLUDES
#include <coecntrl.h>
#include <eikspane.h>  		// CEikStatusPane
#include "ncsconstants.h" 	// TMsgPriority

// FORWARD DECLARATIONS
class CFbsBitmap;
class CEikImage;

/**
 * Navi pane handler for the email.
 */
class CCustomStatuspaneIndicators : public CCoeControl
	{
public:
	CCustomStatuspaneIndicators( );
	virtual ~CCustomStatuspaneIndicators();
	
    /**
     * Creates new CCustomStatuspaneIndicators object. Ownership
     * is transferred to caller.
     * @param aStatusPane pointer to aplications status pane
     * @return Pointer to created object,
     */
    static CCustomStatuspaneIndicators* NewL( CEikStatusPane* aStatusPane );
    
    /**
     * Set priority flag given as parameter into status pane
     */
    void SetPriorityFlag( TMsgPriority aFlagType );
    
    /**
     * Set followup flag ON or OFF in status pane
     */
    enum TFollowUpFlagType { EFollowUp, EFollowUpComplete, EFollowUpNone };
    void SetFollowUpFlag( TFollowUpFlagType aFlagType );
    
    /**
     * Hide all status pane flags
     */
    void HideStatusPaneIndicators();
    
    /**
     * Show status pane flags, whan called flags are sett OFF
     */
    void ShowStatusPaneIndicators();
            
private:
    /**
     * Construction
     * @param aStatusPane pointer to aplications status pane
     */
    void ConstructL( CEikStatusPane* aStatusPane );

    /**
     * Read icon graphic controls from the texture manager
     */     
	void ConstructControlsL();
	
    /**
     * Get topmost navi pane decorator and check if scroll buttons are visible
     */     
	TBool CheckScrollButtonState();

	/**
	 * Sets this control's window rect
	 */
	void DoSetRectL();
	
public:  // From CCoeControl
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
private:  // From CCoeControl
    void SizeChanged();
    void HandleResourceChange( TInt aType );

private: // Data
	CEikStatusPane* iStatusPane;
	// Icons which are shown in navi pane
    CEikImage* iPriorityIconLow;
    CEikImage* iPriorityIconHigh;
  	CEikImage* iFollowUpIcon;
    /// Own: Pointers to the bitmaps. Icons are created using these bitmaps
    CFbsBitmap* iBitmapPriorityLow;
    CFbsBitmap* iBitmapMaskPriorityLow;
    CFbsBitmap* iBitmapPriorityHigh;
    CFbsBitmap* iBitmapMaskPriorityHigh;
    CFbsBitmap* iBitmapFollowUp;
    CFbsBitmap* iBitmapFollowUpMask;
    CFbsBitmap* iBitmapFollowUpComplete;
    CFbsBitmap* iBitmapFollowUpCompleteMask;
	};

#endif /* CCUSTOMSTATUSPANEINDICATORS_H_ */
