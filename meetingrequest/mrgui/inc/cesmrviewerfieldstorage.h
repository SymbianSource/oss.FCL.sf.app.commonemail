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
* Description:  ESMR field storage for viewer fields owned by fieldcontainer
*
*/

#ifndef CESMRVIEWERFIELDSTORAGE_H
#define CESMRVIEWERFIELDSTORAGE_H

#include "cesmrfieldstorage.h"
#include "tesmrentryfield.h"

class CESMRPolicy;
class CESMRTimeWrapper;
class MESMRCalEntry;
class MESMRResponseObserver;

/**
 * This class storages the fields needed in viewer mode.
 */
NONSHARABLE_CLASS( CESMRViewerFieldStorage ) : public CESMRFieldStorage
    {

public:
    /**
     * Constructor for the viewer field storage.
     *
     * @param aPolicy - pointer to policy information
     * @param aResponseObserver - Observer to be called when response 
     *                            action is needed
     * @param aResponseReady - Currently not in use
     * @return New viewer field storage object
     */
    static CESMRViewerFieldStorage* NewL( 
    		const CESMRPolicy& aPolicy,
    		MESMRResponseObserver* aResponseObserver,
    		TBool aResponseReady,
    		MESMRFieldEventObserver& aEventObserver,
    		TInt aReqAttendee = 0,
    		TInt aOptAttendee = 0);

    // Destructor
    ~CESMRViewerFieldStorage();

public:
    // overwritten because the time wrapper needs to be
    // externalized also.
    void ExternalizeL( MESMRCalEntry& aEntry );

private: // Implementation
    CESMRViewerFieldStorage(
            MESMRFieldEventObserver& aEventObserver );

    void ConstructL( const CESMRPolicy& aPolicy,
                     MESMRResponseObserver* aResponseObserver,
                     TBool aResponseReady,
                     const TInt &aReqAttendee ,
                     const TInt &aOptAttendee );

    void CreatePolicyFieldsL( const CESMRPolicy& aPolicy,
                MESMRResponseObserver* aResponseObserver,
                TBool aResponseReady);

    void CreateTrackingFieldsL( const CESMRPolicy& aPolicy,
                MESMRResponseObserver* aResponseObserver,
                TBool aResponseReady,
                const TInt &aReqAttendeee, const TInt &aOptAttendee);
    };

#endif //CESMRVIEWERFIELDSTORAGE_H
