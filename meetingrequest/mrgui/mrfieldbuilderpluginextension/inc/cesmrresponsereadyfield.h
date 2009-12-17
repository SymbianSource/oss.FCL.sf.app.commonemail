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
* Description:  This field is shown when MR has a response value,
*
*/

#ifndef __CESMRRESPONSEREADYFIELD_H__
#define __CESMRRESPONSEREADYFIELD_H__

#include <eiklabel.h>
#include "cesmrfield.h"

class MESMRResponseObserver;

/**
 * This field is shown instead of response field when MR has response value.
 * When pressing "OK" button on the top of this field, this field is hided and
 * response field is shown.
 */
NONSHARABLE_CLASS( CESMRResponseReadyField ): public CESMRField
    {
public:
    /**
     * Creates new instance from response ready field and returns it.
     * @param aResponseObserver - Observer is called when user wants to change response value
     * @return New CESMRResponseReadyField object.
     */
    static CESMRResponseReadyField* NewL( MESMRResponseObserver* aResponseObserver );

    /**
     * C++ Destructor.
     */
    ~CESMRResponseReadyField();
    
public: // From CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    void ExecuteGenericCommandL( TInt aCommand );

protected:
    CEikLabel* iLabel;

private: // Implementation
    void ConstructL();
    CESMRResponseReadyField( MESMRResponseObserver* aResponseObserver );
    
private: //data
    // Pointer to response observer. not own.
    MESMRResponseObserver* iResponseObserver;
    };

#endif // __CESMRRESPONSEREADYFIELD_H__
