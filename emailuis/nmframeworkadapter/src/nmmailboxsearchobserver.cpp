/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "nmframeworkadapterheaders.h"


/*!
    \class NmMailboxSearchObserver
    \brief A Qt wrapper class for MFSMailBoxSearchObserver.
*/


/*!
    Class constructor.
*/
NmMailboxSearchObserver::NmMailboxSearchObserver()
{
    
}


/*!
    Class destructor.
*/
NmMailboxSearchObserver::~NmMailboxSearchObserver()
{
    
}


/*!
    From MFSMailBoxSearchObserver.
    Converts the ID of the received message into NmId and emits it with the
    signal notifying that a match was found.

    \param aMatchMessage A message that matches the search criteria. This class
                         takes the ownership of the given instance.
*/
void NmMailboxSearchObserver::MatchFoundL(CFSMailMessage *aMatchMessage)
{
    if (aMatchMessage) {
        NmMessage *message = aMatchMessage->GetNmMessage();

        if (message) {
            NmMessageEnvelope messageEnvelope = message->envelope();
            emit matchFound(messageEnvelope.messageId(), messageEnvelope.folderId());
            delete message;
        }
    }
}


/*!
    From MFSMailBoxSearchObserver.
    Emits a signal to notify that the search is complete.
*/
void NmMailboxSearchObserver::SearchCompletedL()
{
    emit searchComplete();
}


/*!
    From MFSMailBoxSearchObserver.
    TODO Implement if necessary.
*/
void NmMailboxSearchObserver::ClientRequiredSearchPriority(
    TInt *apRequiredSearchPriority)
{
    Q_UNUSED(apRequiredSearchPriority);
}


// End of file.
