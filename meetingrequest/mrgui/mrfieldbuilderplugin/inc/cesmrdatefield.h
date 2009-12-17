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
* Description:  ESMR date field impl.
 *
*/


#ifndef CESMRDATEFIELD_H
#define CESMRDATEFIELD_H

#include "cesmriconfield.h"

class MESMRFieldValidator;
class CEikDateEditor;
class CAknsFrameBackgroundControlContext;
class MAknsControlContext;

NONSHARABLE_CLASS( CESMRDateField ): public CESMRIconField
    {
public:
    /**
     * Two phase constructor.
     *
     * @param aValidator validator object
     * @param aId field id
     */
    static CESMRDateField* NewL( MESMRFieldValidator* aValidator, TESMREntryFieldId aId );

    /**
     * Destructor.
     */
    ~CESMRDateField();

public: // From CESMRField
    void InitializeL();
    TBool OkToLoseFocusL( TESMREntryFieldId aId );
    void SetOutlineFocusL( TBool aFocus );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void ActivateL();
    void PositionChanged();

private: // Implementation
    CESMRDateField( MESMRFieldValidator* aValidator );
    void ConstructL( TESMREntryFieldId aId );
    void CheckIfValidatingNeededL(
            TInt aStartFieldIndex );
    TBool TriggerValidatorL();
    
private:
    /**
     * Not owned. Date editor.
     */
    CEikDateEditor* iDate;

    /**
     * Not owned. Validator object.
     */
    MESMRFieldValidator* iValidator;

    // Background control context
    MAknsControlContext* iBackground;

    // Actual background for the editor
    CAknsFrameBackgroundControlContext* iFrameBgContext;//own

    // Rect of this field
    TRect iFieldRect;
    };

#endif  // CESMRDATEFIELD_H

