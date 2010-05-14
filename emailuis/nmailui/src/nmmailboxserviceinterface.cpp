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
* Description: NMail application service interface used for displaying the
*              messages contained by the given mailbox. The service utilizes
*              thr Qt highway framework.
*/

//  INCLUDES
#include "nmuiheaders.h" // Includes also the class header.


/*!
    \class NmMailboxServiceInterface
    \brief A service interface for displaying the messages contained by the
           given mailbox.
*/


/*!
    Class constructor.
*/
NmMailboxServiceInterface::NmMailboxServiceInterface(QObject *parent,
        NmUiEngine &uiEngine,
        NmApplication *application)
#ifndef NM_WINS_ENV
    : XQServiceProvider(emailFullServiceNameMailbox, parent),
#else
    : QObject(parent),
#endif
      mUiEngine(uiEngine),
      mApplication(application),
      mAsyncReqId(0)
{
#ifndef NM_WINS_ENV
    publishAll();
#endif
}


/*!
    Class desctructor.
*/
NmMailboxServiceInterface::~NmMailboxServiceInterface()
{
}


/*!
    Called by some external application when the content of a mailbox needs to
    be viewed.
    \param data The ID of the mailbox to open.
*/
void NmMailboxServiceInterface::displayInboxByMailboxId(QVariant data)
{
    NMLOG("NmMailboxServiceInterface::displayInboxByMailboxId()");

#ifndef NM_WINS_ENV

    // Get the given ID and convert it into NmId type.
    NmId mailboxNmId(data.toULongLong());

    mAsyncReqId = setCurrentRequestAsync();

    // Verify that the ID matches one of the existing mailboxes.
    if (mailboxExistsById(mailboxNmId)) {

        // Get standard folder inbox id.
        const NmId inboxId = mUiEngine.standardFolderId(mailboxNmId, NmFolderInbox);

        // Bring the application to the foreground.
        XQServiceUtil::toBackground(false);

        // Launch the message list view.
        NmUiStartParam *startParam =
            new NmUiStartParam(NmUiViewMessageList,
                               mailboxNmId,
                               inboxId, // folder id
                               0, // message id
                               NmUiEditorCreateNew, // editor start mode
                               NULL, // address list
                               NULL, // attachment list
                               true); // start as service
        mApplication->enterNmUiView(startParam);

        completeRequest(mAsyncReqId, 0);
    }
    else {
        // No mailbox found with the given ID.

        // if started as embedded, do not hide the app
		if (!XQServiceUtil::isEmbedded()) {
			XQServiceUtil::toBackground(true);
		}

        completeRequest(mAsyncReqId, 1);

        // Close the application if started as a service.
        if (XQServiceUtil::isService()) {
            connect(this, SIGNAL(returnValueDelivered()),
                    mApplication, SLOT(delayedExitApplication()));
        }
    }
#endif
}


/*!
    Resolves whether a mailbox with the given ID exists or not.
    \param mailboxId The mailbox ID to look for.
    \return True if a mailbox with the given ID exists, false otherwise.
*/
bool NmMailboxServiceInterface::mailboxExistsById(const NmId &mailboxId) const
{
    NMLOG("NmMailboxServiceInterface::mailboxExistsById()");

    const NmMailboxListModel& mailboxListModel = mUiEngine.mailboxListModel();
    int mailboxCount = mailboxListModel.rowCount();

    QModelIndex modelIndex;
    QVariant mailbox;
    NmMailboxMetaData *mailboxMetaData = NULL;
    NmId currentId;

    // Try to find the mailbox with the given ID.
    for (int i = 0; i < mailboxCount; ++i) {
        modelIndex = mailboxListModel.index(i, 0);
        mailbox = mailboxListModel.data(modelIndex);
        mailboxMetaData = mailbox.value<NmMailboxMetaData*>();
        currentId = mailboxMetaData->id();

        if (currentId.id() == mailboxId.id()) {
            // Found a mailbox with the matching ID.
            return true;
        }
    }

    // No mailbox exist with the given ID.
    return false;
}


// End of file.
