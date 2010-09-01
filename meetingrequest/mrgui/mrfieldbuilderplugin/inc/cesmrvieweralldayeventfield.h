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
* Description:  Field to inform is the event all day event or not.
*
*/


#ifndef __CESMRVIEWERALLDAYEVENTFIELD_H__
#define __CESMRVIEWERALLDAYEVENTFIELD_H__


#include "cesmrfield.h"

class CMRImage;
class CMRLabel;

/**
 * This field shows whether the event is all day event ot not.
 */
NONSHARABLE_CLASS( CESMRViewerAllDayEventField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerAllDayEventField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerAllDayEventField* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRViewerAllDayEventField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void SetOutlineFocusL( TBool aFocus );
    void LockL();
    TBool ExecuteGenericCommandL( TInt aCommand );
    

public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // Implementation
    CESMRViewerAllDayEventField();
    void ConstructL();

private:
    // Owned: Field icon
    CMRImage* iIcon;
    /// Owned: Lock icon
    CMRImage* iLockIcon;
    // Not own: Field text label
    CMRLabel* iLabel;
    };

#endif //__CESMRVIEWERALLDAYEVENTFIELD_H__
