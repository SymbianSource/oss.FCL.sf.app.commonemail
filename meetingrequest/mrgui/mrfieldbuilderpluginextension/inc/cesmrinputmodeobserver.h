/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Input mode observer class definition
*
*/


#ifndef CESMRINPUTMODEOBSERVER_H
#define CESMRINPUTMODEOBSERVER_H

// INCLUDES
#include <e32base.h>                // CTimer
#include <cenrepnotifyhandler.h>    // MCenRepNotifyHandlerCallback

// FORWARD DECLARATION
class CESMRClsListsHandler;
class CRepository;

// CLASS DECLARATION

/**
 * This class handles observing FEP events.
 */
NONSHARABLE_CLASS( CESMRInputModeObserver ) : public CBase, public MCenRepNotifyHandlerCallback
    {
public:
    /**
    * Destructor
    */
    virtual ~CESMRInputModeObserver();

    /**
    * CESMRInputModeObserver two phase construction
    * @param aCr reference to aknfepcenrep session
    * @param aListHandler reference to cls list handler
    * @return A pointer to the created object
    */
    static CESMRInputModeObserver* NewL( CRepository& aCr,
                                                CESMRClsListsHandler& aListHandler );

private://Implementation
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );
    void HandleNotifyReal( TUint32 aId, TReal aNewValue );
    void HandleNotifyString( TUint32 aId, const TDesC16& aNewValue );

    void HandleNotifyBinary( TUint32 aId, const TDesC8& aNewValue );
    void HandleNotifyGeneric( TUint32 aId );
    void HandleNotifyError( TUint32 aId, TInt aError, CCenRepNotifyHandler* aHandler );

    /**
    * Constructor
    * @param aCr reference to aknfepcenrep session
    * @param aListHandler reference to cls list handler
    */
    CESMRInputModeObserver( CRepository& aCr, CESMRClsListsHandler& aListHandler );

    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL();

private://data
    // Handle to parrot server client side
    CRepository&                iAkvkFepCenRep;
    CCenRepNotifyHandler*       iCenRepNotifyWrapper;

    CESMRClsListsHandler&  iClsListHandler;
    };

#endif // CESMRINPUTMODEOBSERVER_H

// End of file

