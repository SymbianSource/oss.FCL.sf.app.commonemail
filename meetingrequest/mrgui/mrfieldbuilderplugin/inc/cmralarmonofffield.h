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
* Description:  ESMR alarm on off field header
 *
*/


#ifndef CMRALARMONOFFFIELD_H
#define CMRALARMONOFFFIELD_H

#include "CESMRfield.h"
#include "mmrabsolutealarmcontroller.h"

class CMRLabel;
class CMRImage;
class MESMRFieldValidator;
//class MMRAbsoluteAlarmController;

/**
 * Creates an alarm on off field for setting absolute alarm on/off
 */
NONSHARABLE_CLASS( CMRAlarmOnOffField ): public CESMRField,
                                         public MMRAbsoluteAlarmController
    {
public:
    /**
     * Creates new CMRAlarmOnOffField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CMRAlarmOnOffField* NewL( MESMRFieldValidator* aValidator );

    /**
     * C++ Destructor.
     */
    ~CMRAlarmOnOffField();
    
public: // From MMRAbsoluteAlarmController
    void SetAbsoluteAlarmOn();
    void SetAbsoluteAlarmOff();

private: // From CMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );

private: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void MakeVisible(TBool aVisible);
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SetContainerWindowL( const CCoeControl& aContainer );

private:
    /**
     * Constructor.
     *
     * @param aValidator validator object
     */
    CMRAlarmOnOffField();

    /**
     * Second phase constructor.
     */
    void ConstructL( MESMRFieldValidator* aValidator );

    /**
     * Reset field value.
     */
    void ResetFieldL();

    /**
     * Switch middle softkey: on/off
     */
    void SwitchMiddleSoftKeyL();

private:
  
    // Not Own. Field label.
    CMRLabel* iLabel;

    // Own. Field icon
    CMRImage* iIcon;
    
    // Current value.
    TBool iStatus;
    };

#endif  // CMRALARMONOFFFIELD_H

