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
 * Description: Used for observing property changes and notifying listeners.
 *
 */

#ifndef __FREESTYLEEMAILUIPROPERTYSUBSCRIBER_H__
#define __FREESTYLEEMAILUIPROPERTYSUBSCRIBER_H__

#include <e32base.h> // For CActive, link against: euser.lib
#include <e32property.h> // Observer for property change events.

/**
 * Observer interface for property changes.
 */
class MFreestyleEmailUiPropertyChangedObserver
    {
public:

	virtual void PropertyChangedL( TInt aValue ) = 0;
    };


/**
 *  Subcribes property from P&S and listens to property changes.
 */
NONSHARABLE_CLASS( CFreestyleEmailUiPropertySubscriber ) : public CActive
    {
public: // Construction and destruction.

    /**
     * Two-phased constructor.
     * @param aUid P&S property category.
     * @param aKey P&S property key.
     * @param aObserver The given property changed observer.
     */
    static CFreestyleEmailUiPropertySubscriber* NewL(
    	const TUid aUid, 
		const TUint32 aKey,
		MFreestyleEmailUiPropertyChangedObserver& aObserver );

    /**
     * Two-phased constructor.
     * @param aUid P&S property category.
     * @param aKey P&S property key.
     * @param aObserver The given property changed observer.
     */
    static CFreestyleEmailUiPropertySubscriber* NewLC(
    	const TUid aUid, 
        const TUint32 aKey,
        MFreestyleEmailUiPropertyChangedObserver& aObserver );

    /**
     * Cancels and destroys the instance.
     */
    ~CFreestyleEmailUiPropertySubscriber();


private: // Construction.

    /**
     * C++ constructor.
     * @param aObserver The given property changed observer.
     */
	CFreestyleEmailUiPropertySubscriber(
		MFreestyleEmailUiPropertyChangedObserver& aObserver );

    /**
     * Second phase constructor.
     * @param aUid P&S property category.
     * @param aKey P&S property key.
     */
    void ConstructL( const TUid aUid, const TUint32 aKey );


private: // From CActive.

    /**
     * Handles completion.
     */
    void RunL();

    /**
     * How to cancel me.
     */
    void DoCancel();

    
private:

    // The observer for property changes.
	MFreestyleEmailUiPropertyChangedObserver& iPropertyChangeObserver;

    // The property to listen to.
    RProperty iProperty;
    };


#endif /* __FREESTYLEEMAILUIPROPERTYSUBSCRIBER_H__ */

// End of file.

