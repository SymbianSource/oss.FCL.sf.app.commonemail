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
* Description: Subscriber class for Publish & Subsribe events
*
*/

#ifndef PSSUBSCRIBER_H
#define PSSUBSCRIBER_H

#include <e32base.h>
#include <e32property.h>

class MPSSubscribeHandler;

/**
*  CPSSubscriber is an acitve object that subscribes to a single
*  Publish and Subscribe Property and notifies about the Property changes.
*/
class CPSSubscriber : public CActive
{
public:
    /**
    * Two-phased constructor.
	*
	* @param aPropertyResponder The object to be notified about Property changes
	* @param aCategory The category of the Property to be observed
	* @param aKey The subkey of the Property to be observed
    */
    static CPSSubscriber* NewL( MPSSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey );

    /**
    * Destructor.
    */
     ~CPSSubscriber();

	 /**
	  * Subscribes to the Property given in constructor
	  */
     void Subscribe();

private:
	/**
     * C++ default constructor.
	 *
  	 * @param aPropertyResponder The object to be notified about Property changes
	 * @param aCategory The category of the Property to be observed
	 * @param aKey The subkey of the Property to be observed
     */
    CPSSubscriber( MPSSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
	 void ConstructL();

public: // from CActive
    /**
	*   RunL
    */
    void RunL();

    /**
	*   RunError
    *	@param aError the error returned
	*
    *	@return error
    */
    TInt RunError( TInt aError );

    /**
    *	DoCancel
    */
    void DoCancel();

private: // Methods not implemented
    CPSSubscriber( const CPSSubscriber& );           // Copy constructor
    CPSSubscriber& operator=( const CPSSubscriber& );// Assigment operator

private: // data

	// The object to be notified about Property changes
    MPSSubscribeHandler&      iPropertyResponder;

	// The property object that subscribes to the Property to be observed
    RProperty                 iProperty;

	// The category of the Property to be observed
    TUid                      iCategory;

	// The subkey of the Property to be observed
	TUint                     iKey;
};

#endif // PSSUBSCRIBER_H
