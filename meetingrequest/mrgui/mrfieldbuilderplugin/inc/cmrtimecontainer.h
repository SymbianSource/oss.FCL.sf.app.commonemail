/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ESMR time (start-end) field implementation
 *
*/

#ifndef CMRTIMECONTAINER_H
#define CMRTIMECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "esmrdef.h"

// FORWARD DECLARATIONS
class MAknsControlContext;
class CAknsFrameBackgroundControlContext;
class MESMRFieldValidator;
class CESMRLayoutManager;
class CEikTimeEditor;
class CMRLabel;
class CAknsBasicBackgroundControlContext;

/**
 * Container control for the time fields and the separator.
 */
NONSHARABLE_CLASS( CMRTimeContainer ): public CCoeControl
    {

public:
    /**
     * Creates new CMRTimeContainer object. Ownership
     * is transferred to caller.
     * @param aValidator validator object
     * @param aParent parent control
     * @param aFieldId CESMRField id of the control/container
     * @return Pointer to created object,
     */
    static CMRTimeContainer* NewL(
            MESMRFieldValidator* aValidator,
            CCoeControl* aParent,
            TESMREntryFieldId aFieldId );

    /**
     * Destructor.
     */
    ~CMRTimeContainer();

public: // From CCoeControl
    void SetContainerWindowL( const CCoeControl& aControl );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void FocusChanged(TDrawNow aDrawNow);
    void HandlePointerEventL( const TPointerEvent &aPointerEvent );
    void ActivateL();
    
public: // interface

    /**
     * Returns currently focused editor.
     *
     * @return currently focused editor
     */
    CEikTimeEditor* FocusedEditor();

    /**
     * Returns ETrue if its ok to change focus.
     *
     * @param current field id
     * @param next field id
     * @return ETrue if its ok to change focus
     */
    TBool OkToLoseFocusL( TESMREntryFieldId aFrom, TESMREntryFieldId aTo );
    
    /**
     * Sets Background rect of Time editor field.
     * 
     * @param current background rect of time field
     */
    void SetBgRect( TRect bgRect );
    
    /**
     * Sets Field valiator
     * 
     * @param aValidator
     */
    void SetValidatorL( MESMRFieldValidator* aValidator );
    
    /**
     * Called when locale environemnt has changed
     */
    void EnvironmentChangedL();

private: // Implementation
    /**
     * Constructor.
     *
     * @param aValidator validator object
     */
    CMRTimeContainer( MESMRFieldValidator* aValidator, TESMREntryFieldId aFieldId );

    /**
     * Second phase constructor.
     *
     * @param aParent parent control
     */
    void ConstructL( CCoeControl* aParent );

    /**
     * Changes focus to left if its possible.
     *
     * @param aEvent key event
     * @param aType event code
     */
    TKeyResponse ChangeFocusLeftL(const TKeyEvent& aEvent, TEventCode aType);

    /**
     * Changes focus to right if its possible.
     *
     * @param aEvent key event
     * @param aType event code
     */
    TKeyResponse ChangeFocusRightL(const TKeyEvent& aEvent, TEventCode aType);

    void CheckIfValidatingNeededL(
            TBool aStartFocusedBefore,
            TInt aStartFieldIndex,
            TBool aEndFocusedBefore,
            TInt aEndFieldIndex );

    void TriggerStartTimeChangedL();
    void TriggerEndTimeChangedL();

    void InitializeL();
    void Reset();
    
private: // data 
    /**
     * Own. Start time editor.
     */
    CEikTimeEditor* iStartTime;

    /**
     * Own. End time editor.
     */
    CEikTimeEditor* iEndTime;

    /**
     * Own. Separator label between start and end fields.
     */
    CMRLabel* iSeparator;

    /**
     * Not owned. Validator class for time and sanity checks.
     */
    
    MESMRFieldValidator* iValidator;
    // Own: Field Id
    TESMREntryFieldId iFieldId;
    // Background control context
    MAknsControlContext* iBackground;
    // Actual background for the start time editor
    CAknsFrameBackgroundControlContext* iStartTimeBgContext;
    // Actual background for the end time editor
    CAknsFrameBackgroundControlContext* iEndTimeBgContext;
    // Not owned. Layout manager.
    CESMRLayoutManager* iLayout;
    // Own. Field background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    /// Own: Focus rect
    TRect iBgRect;
    /// Ref: Pointer to container window
    const CCoeControl* iContainerWindow;
    };

#endif // CMRTIMECONTAINER_H
// End of file

