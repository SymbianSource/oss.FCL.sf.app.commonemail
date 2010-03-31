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
* Description:  ESMR single time field implementation
 *
*/


#ifndef CESMRSINGLETIMEFIELD_H
#define CESMRSINGLETIMEFIELD_H

#include "cesmrfield.h"

class MESMRFieldValidator;
class CEikTimeEditor;
class CMRImage;
class CAknsBasicBackgroundControlContext;

NONSHARABLE_CLASS( CESMRSingleTimeField ): public CESMRField
    {

public:
    /**
     * Creates new CESMRSingleTimeField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRSingleTimeField* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRSingleTimeField();

public: // From CESMRField
    TBool OkToLoseFocusL( TESMREntryFieldId aNextItem );
    void SetOutlineFocusL( TBool aFocus );
    void SetValidatorL( MESMRFieldValidator* aValidator );
    TBool ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SetContainerWindowL( const CCoeControl& aContainer );

private:
    /**
     * Constructor.
     * @param aValidator validator object
     */
    CESMRSingleTimeField( MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    void CheckIfValidatingNeededL( TInt aStartFieldIndex );

    void DoEnvChangeL();
    
private:

    // Not owned. Time editor.
    CEikTimeEditor* iTime;
    
    // Own. Field icon
    CMRImage* iFieldIcon;
        
    // Own. Background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    /// Ref: Pointer to container window
    const CCoeControl* iContainerWindow;
    };

#endif  // CESMRSINGLETIMEFIELD_H

