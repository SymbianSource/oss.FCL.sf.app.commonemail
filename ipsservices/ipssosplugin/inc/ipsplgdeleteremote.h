/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Deletes messages first locally and then from server
*
*/

#ifndef IPSPLGDELETEREMOTE_H
#define IPSPLGDELETEREMOTE_H

//  INCLUDES
#include <e32def.h>
#include <e32des8.h>    // TDesC8

class CMsvOperation;
class CMsvEntry;
class CMsvEntrySelection;
class CMsvSession;
class TRequestStatus;

/**
* class CIpsPlgDeleteRemote
* Encapsulates delete locally operation and delete from server operation.
* First deletes message locally and after it has been completed, deletes from the server
*/
class CIpsPlgDeleteRemote :
    public CMsvOperation
    {
    public:
        /**
        * Constructor.
        */
        static CIpsPlgDeleteRemote* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection& aDeletedEntries );
            
        /**
        * Destructor.
        */
        virtual ~CIpsPlgDeleteRemote();
        
        /**
        *
        */
        virtual const TDesC8& ProgressL();


    private:
    
        enum IpsPlgDeleteMessagesState 
            {
            EDeletingMessagesStateLocally = 0,
            EDeletingMessagesStateFromServer
            };

        /**
        * Constructor.
        */
        CIpsPlgDeleteRemote(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );

        /**
        * Constructor.
        */
        void ConstructL( CMsvEntrySelection& aDeletedEntries );
        
        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        */
        void RunL();

        /**
        * Starts new local delete progress
        */
        void StartNextDeleteLocally();

        /**
        * Creates object of CImumDeleteMessagesLocally
        */
        void MakeNextDeleteLocallyL();

        /**
        * Starts entry delete after local delete is completed.
        */
        void StartDeleteFromServer();

        /**
        * Creates delete operation
        */
        void MakeDeleteFromServerL();

    private:
        CMsvOperation*              iOperation;
        CMsvEntry*                  iEntry;
        CMsvEntrySelection*         iEntrySelection; 
        TInt                        iEntryCount;
        TBuf8<1>                    iBlank;
        IpsPlgDeleteMessagesState   iState;
        
    };

#endif      // IPSPLGDELETEREMOTE_H

// End of File
