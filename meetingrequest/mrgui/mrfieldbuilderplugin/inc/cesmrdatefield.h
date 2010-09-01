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
* Description:  ESMR date field impl.
 *
*/


#ifndef CESMRDATEFIELD_H
#define CESMRDATEFIELD_H

#include "cesmrfield.h"

class MESMRFieldValidator;
class CEikDateEditor;
class CMRImage;
class CAknsBasicBackgroundControlContext;

NONSHARABLE_CLASS( CESMRDateField ): public CESMRField
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

protected: // From CESMRField
    TBool OkToLoseFocusL( TESMREntryFieldId aId );
    void SetOutlineFocusL( TBool aFocus );
    void SetValidatorL( MESMRFieldValidator* aValidator );
    TBool ExecuteGenericCommandL( TInt aCommand );

protected: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

private: // Implementation
    CESMRDateField( MESMRFieldValidator* aValidator );
    void ConstructL( TESMREntryFieldId aId );
    void CheckIfValidatingNeededL(
            TInt aStartFieldIndex );
    TBool TriggerValidatorL();
    void InitializeValidatorL();
    void DoEnvChangeL();
    
private:
    
    // Not owned. Date editor.
    CEikDateEditor* iDate;

    // Rect of this field
    TRect iFieldRect;
    
    // Own: Field icon
    CMRImage* iFieldIcon;
    
    // Own: Background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    
    /// Ref: Pointer to container window
    const CCoeControl* iContainerWindow;
    };

#endif  // CESMRDATEFIELD_H

