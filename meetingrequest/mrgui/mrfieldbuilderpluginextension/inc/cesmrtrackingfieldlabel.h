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
* Description:  This class is for showing the attendees in tracking mode.
 *
*/


#ifndef CESMRTRACKINGFIELDLABEL_H_
#define CESMRTRACKINGFIELDLABEL_H_

#include <caluser.h>
#include <eikedwob.h>
#include "cesmrfield.h"
#include "resmrstatic.h"

class CEikLabel;

/**
 * This class shows the label of attendee type in tracking view (viewer-type) mode.
 */
NONSHARABLE_CLASS( CESMRTrackingFieldLabel ) : public CESMRField,
public MEikEdwinSizeObserver
    {
public:
    /**
     * Constructor for the tracking field.
     * @param CCalAttendee::TCalRole - Information that tells is this
     *                                 field for optional or for
     *                                 required attendees.
     * @return New field object.
     */
    static CESMRTrackingFieldLabel* NewL( CCalAttendee::TCalRole );

    /**
     * C++ Destructor.
     */
    ~CESMRTrackingFieldLabel( );

public: // From CCoeControl
    TInt CountComponentControls( ) const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged( );

public: // From CESMRField
    void InitializeL();

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL(CEikEdwin* aEdwin, TEdwinSizeEvent aType,
            TSize aDesirableEdwinSize );

private:
    CESMRTrackingFieldLabel( CCalAttendee::TCalRole aType );
    void ConstructL( );

private:
    // Title of this field
    CEikLabel* iTitle;
    // Size of the title
    TSize iTitleSize;
    // Inofrmation about attendee type
    CCalAttendee::TCalRole iRole;
    };

#endif /*CESMRTRACKINGFIELDLABEL_H_*/
