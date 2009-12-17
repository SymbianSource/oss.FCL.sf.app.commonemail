/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CESMRTIMEFIELD_H
#define CESMRTIMEFIELD_H

#include "cesmriconfield.h"

//Forward declarations
class CEikLabel;
class CTimeContainer;
class MESMRFieldValidator;
class CEikTimeEditor;

/**
 * Time field for ESMR editor. Time field has start and end time
 * separated by a '-'. Focus traverses from left to right and right
 * to left.
 */
NONSHARABLE_CLASS( CESMRTimeField ): public CESMRIconField
    {
public:
    /**
     * Creates new CESMRTimeField object. Ownership
     * is transferred to caller.
     * @param aValidator validator object
     * @return Pointer to created object,
     */
    static CESMRTimeField* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRTimeField();

public: // From CCoeControl
    void SetContainerWindowL( const CCoeControl& aControl );
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);

public: // From ESMRField
    void InitializeL();
    TBool OkToLoseFocusL( TESMREntryFieldId aNext );
    void SetOutlineFocusL( TBool aFocus );

private:
    /**
     * Constructor.
     */
    CESMRTimeField();

    /**
     * Second phase constructor.
     *
     * @param aValidator validator object
     */
    void ConstructL( MESMRFieldValidator* aValidator );

private:
    /**
     * Own. Container for the time fields.
     */
    CTimeContainer* iContainer;
    };


//Forward declarations
class MAknsControlContext;
class CAknsFrameBackgroundControlContext;

/**
 * Container control for the time fields and the separator.
 */
NONSHARABLE_CLASS( CTimeContainer ): public CCoeControl
    {

public:
    /**
     * Creates new CTimeContainer object. Ownership
     * is transferred to caller.
     * @param aValidator validator object
     * @param aParent parent control
     * @param aFieldId CESMRField id of the control/container
     * @return Pointer to created object,
     */
    static CTimeContainer* NewL(
            MESMRFieldValidator* aValidator,
            CCoeControl* aParent,
            TESMREntryFieldId aFieldId );

    /**
     * Destructor.
     */
    ~CTimeContainer();

public: // From CCoeControl
    void SetContainerWindowL( const CCoeControl& aControl );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void FocusChanged(TDrawNow aDrawNow);
    void ActivateL();
    void PositionChanged();

public: // interface
    /**
     * Sets font and font color for editors.
     *
     * @param aFont new font
     * @param aLayout pointer to layoutmanager
     */
    void SetFontL( const CFont* aFont, CESMRLayoutManager* aLayout );

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

private: // Implementation
    /**
     * Constructor.
     *
     * @param aValidator validator object
     */
    CTimeContainer( MESMRFieldValidator* aValidator, TESMREntryFieldId aFieldId );

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
    CEikLabel* iSeparator;

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
    };

#endif  // CESMRTIMEFIELD_H

