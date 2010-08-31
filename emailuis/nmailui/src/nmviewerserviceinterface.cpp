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
* Description: NMail application service interface used for viewing a mail
*              according to given id. The service utilizes the Qt highway framework.
*/

//  INCLUDES
#include "nmuiheaders.h" // Includes also the class header.


/*!
    \class NmViewerServiceInterface
    \brief A service interface for displaying the message according to given id.
*/


/*!
    Class constructor.
*/
NmViewerServiceInterface::NmViewerServiceInterface(QObject *parent,
        NmApplication *application,
        NmUiEngine &uiEngine)
    : XQServiceProvider(emailFullServiceNameMessage, parent),
      mApplication(application),
      mUiEngine(uiEngine),
      mAsyncReqId(0)
{
    publishAll();
}


/*!
    Class desctructor.
*/
NmViewerServiceInterface::~NmViewerServiceInterface()
{
    NM_FUNCTION;
}


/*!
    Opens view to the specific message
 */
void NmViewerServiceInterface::viewMessage(QVariant mailboxId, QVariant folderId, QVariant messageId)
{
    NM_FUNCTION;

    mAsyncReqId = setCurrentRequestAsync();
    NmId mailboxNmId(0);
    mailboxNmId = mailboxId.toULongLong();   
    NmId messageNmId(0);
    messageNmId = messageId.toULongLong(); 
    NmId folderNmId(0);
    folderNmId = folderId.toULongLong();

	// Make sure the app stays background if user presses back in viewer view
	bool visible = mApplication->updateVisibilityState();

    NmMessage *message = mUiEngine.message( mailboxNmId, folderNmId, messageNmId );
    if (message) {
        // bring application to foreground
        if (!XQServiceUtil::isEmbedded()) {
            XQServiceUtil::toBackground(false);
        }
        HbMainWindow *mainWindow = mApplication->mainWindow();
        mainWindow->show();

        // Launch the message list view.
        NmUiStartParam *startParam1 = 
            new NmUiStartParam(NmUiViewMessageList,
                               mailboxNmId,
                               folderNmId, // folder id
                               messageNmId, // message id
                               NmUiEditorCreateNew, // editor start mode
                               NULL, // address list
                               NULL, // attachment list
                               true); // start as service
        mApplication->enterNmUiView(startParam1);
        
        // Launch the message view.
        NmUiStartParam *startParam =
            new NmUiStartParam(NmUiViewMessageViewer,
                               mailboxNmId,
                               folderNmId, // folder id
                               messageNmId, // message id
                               NmUiEditorCreateNew, // editor start mode
                               NULL, // address list
                               NULL, // attachment list
                               false); // not started as service
        mApplication->enterNmUiView(startParam);

        completeRequest(mAsyncReqId,0);
    }
    else {
        // Message was not found

        // if started as embedded, do not hide the app
		if (!XQServiceUtil::isEmbedded() && !visible) {
			XQServiceUtil::toBackground(true);
		}

        completeRequest(mAsyncReqId,1);

        // Close the application if started as service
        if (XQServiceUtil::isService()) {
            // Exit the application when the return value is delivered
            connect(this, SIGNAL(returnValueDelivered()),
                    mApplication, SLOT(delayedExitApplication()));
        }
    }
}

// End of file.
