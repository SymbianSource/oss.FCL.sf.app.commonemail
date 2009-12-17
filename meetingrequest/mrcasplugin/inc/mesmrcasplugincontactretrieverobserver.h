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
* Description:  ESMR CAS plug-in contact retriever observer
*
*/
#ifndef MESMRCASPLUGINCONTACTRETRIEVEROBSERVER_H
#define MESMRCASPLUGINCONTACTRETRIEVEROBSERVER_H

class CCalUser;

/**
 * MESMRCasPluginContactRetrieverObserver defines observer
 * interface for contact retriever operations.
 */
class MESMRCasPluginContactRetrieverObserver
    {
public: // Destruction
    virtual ~MESMRCasPluginContactRetrieverObserver() {}

public: // Interface

    /**
     * Called to notify that MR contact count fetching is complete.
     * @param aCanExecute ETrue if at least 1 contact with email was found.
     */
    virtual void MRCanExecuteComplete( TBool aCanExecute ) = 0;
    
    /**
     * Notify observer about error.
     * @param aError Error code.
     */
    virtual void MRCanExecuteError(
            TInt aError ) = 0;

    /**
     * Called to notify that MR contact fetching is complete.
     * @param aContactCount Total number of contacts.
     * @param aContacts Array of fecthed MR attendees.
     */
    virtual void MRGetAttendeesComplete(
            TInt aContactCount,
            const RPointerArray<CCalUser>& aAttendees ) = 0;
    
    /**
     * Notify observer about error.
     * @param aError Error code.
     */
    virtual void MRGetAttendeesError(
            TInt aError ) = 0;

    };


#endif // MESMRCASPLUGINCONTACTRETRIEVEROBSERVER_H

// EOF
