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
* Description:  ESMR single time field implementation
 *
*/


#ifndef CESMRSINGLETIMEFIELD_H
#define CESMRSINGLETIMEFIELD_H

#include "cesmriconfield.h"

class MESMRFieldValidator;
class CEikTimeEditor;
class CAknsFrameBackgroundControlContext;

NONSHARABLE_CLASS( CESMRSingleTimeField ): public CESMRIconField
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
    void InitializeL();
    TBool OkToLoseFocusL( TESMREntryFieldId aNextItem );
    void SetOutlineFocusL( TBool aFocus );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void ActivateL();
    void PositionChanged();

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

private:
    /**
     * Not owned. Validator object.
     */
    MESMRFieldValidator* iValidator;

    /**
     * Not owned. Time editor.
     */
    CEikTimeEditor* iTime;

    /**
     * Background control context. Not own
     */
    MAknsControlContext* iBackground;

    /**
     * Actual background for the editor. Own
     */
    CAknsFrameBackgroundControlContext* iFrameBgContext;
    };

#endif  // CESMRSINGLETIMEFIELD_H

