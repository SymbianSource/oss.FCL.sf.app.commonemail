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
* Description:  ESMR checkbox component for es mr gui (editor)
 *
*/


#ifndef CESMRCHECKBOX_H
#define CESMRCHECKBOX_H

#include "cesmrfield.h"

class CMRLabel;
class CMRImage;
class MESMRFieldValidator;

/**
 * CESMRCheckBox implements the All Day Event checkbox field.
 * When the checkbox is checked, all day event time and date fields
 * are added to list. When the checkbox is unchecked, date and time
 * fields are removed from the list.
 */
NONSHARABLE_CLASS( CESMRCheckBox ): public CESMRField
    {
public:
    /**
     * Two phase constructor.
     *
     * @param aValidator validator object
     */
    static CESMRCheckBox* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRCheckBox();

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );

private: // Implemenation
    /**
     * Set checked or unchecked icon.
     *
     * @param aChecked ETrue for checked icon
     */
    void SetIconL( TBool aChecked );
    
    /**
     * Handles triggering checkbox, checks/unchecks it.
     */
    void HandleCheckEventL();

private:
    /**
     * Constructor.
     *
     * @param aValidator validator object
     */
    CESMRCheckBox( MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Changes msk label from mark to unmark
     * and other way around.
     *
     */
    void SwitchMSKLabelL();
    void SendFieldChangeEventL( TESMREntryFieldId aFieldId );
private:
    
    //Not owned. Checkbox text label
    CMRLabel* iLabel;
    
    // Owned. checkbox_mark/checkbox_unmark
    CMRImage* iFieldIcon;

    // If checkbox is marked or unmarked.
    TBool iChecked;
    };

#endif  // CESMRCHECKBOX_H

