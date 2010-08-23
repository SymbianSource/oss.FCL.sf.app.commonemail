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
* Description: NMail Application Launcher interface used for interfacing between
*              QT highway and other applications
*
*/

//  INCLUDES
#include "nmuiheaders.h"

/*!
    \class NmStartParamDataHelper
    \brief A helper class for processing the data given to the actual service.
*/
class NmStartParamDataHelper
{
public:

    /*!
        Class constructor.
    */
    inline NmStartParamDataHelper()
    : mSubject(NULL),
      mToAddresses(NULL),
      mCcAddresses(NULL),
      mBccAddresses(NULL)
    {
        NM_FUNCTION;
    }

    /*!
        Class destructor.
    */
    inline ~NmStartParamDataHelper()
    {
        NM_FUNCTION;
    }

    /*!
        Extracts the data from the given QString into the class members.
        \param data QString containing the data.
        \return True if success, false otherwise.
    */
    inline bool extractData(const QString &data)
    {
        NM_FUNCTION;
        
        bool success(false);
        
        QUrl uri(data);
        
        if (uri.isValid()) {
            
            mSubject = new QString(uri.queryItemValue(emailSendSubjectKey));
            QString to = uri.path();
            QString cc = uri.queryItemValue(emailSendCcKey);
            QString bcc = uri.queryItemValue(emailSendBccKey);
            
            addAddressesToList(to, &mToAddresses);
            addAddressesToList(cc, &mCcAddresses);
            addAddressesToList(bcc, &mBccAddresses);
            
            success = true;
        }
        
        return success;
    }

    /*!
        Appends the given addresses into the given list.
        \param address The addresses to append.
        \param list The list where the addresses are appended to.
    */
    inline void addAddressesToList(QString &addresses,
                                   QList<NmAddress*> **list)
    {
        NM_FUNCTION;
        
        if (!addresses.isEmpty()) {

            QList<NmAddress*> foundAddresses;
                    
            // Process multiple addresses.
            if (addresses.contains(",")) {
                QString str;               
                while (addresses.contains(",")) {
                    str = addresses.section(",", 0, 0); // Get the occurance.
                    addresses.remove(0, (addresses.indexOf(",")+1)); // Remove the occurance.
                    if (!str.isEmpty()) { // In case str would be empty on some error data. 
                        NmAddress *address = new NmAddress(str);
                        foundAddresses.append(address);    
                    }
                }
            }
            if (!addresses.isEmpty()) { // In case addresses would be empty on some error data. 
                // Last one or single address.
                NmAddress *address = new NmAddress(addresses);
                foundAddresses.append(address);
            }
            // Append the found addresses into the given list.
            *list = new QList<NmAddress*>();
            (*list)->append(foundAddresses);
        }
    }
        
    /*!
        Deletes the class members. Must be used if NmUiStartParam does not
        take ownership of the members.
    */
    inline void deleteData()
    {
        NM_FUNCTION;
        
        delete mSubject;
        mSubject = 0;

        if (mToAddresses) {
            qDeleteAll(*mToAddresses);
            delete mToAddresses;
            mToAddresses = 0;
        }

        if (mCcAddresses) {
            qDeleteAll(*mCcAddresses);
            delete mCcAddresses;
            mCcAddresses = 0;
        }

        if (mBccAddresses) {
            qDeleteAll(*mBccAddresses);
            delete mBccAddresses;
            mBccAddresses = 0;
        }
    }

public: // Data

    QString *mSubject; // Not owned.
    QList<NmAddress*> *mToAddresses; // Not owned.
    QList<NmAddress*> *mCcAddresses; // Not owned.
    QList<NmAddress*> *mBccAddresses; // Not owned.
};

/*!
    \class NmUriServiceInterface
    \brief NMail application service interface which provides an email sending
           interface for other application using the Qt Highway.
*/

/*!
    Class constructor.
*/
NmUriServiceInterface::NmUriServiceInterface(QObject *parent,
                                               NmUiEngine &uiEngine,
                                               NmApplication *application)
    : XQServiceProvider(emailServiceName+"."+XQI_URI_VIEW, parent),
      mApplication(application),
      mUiEngine(uiEngine),
      mAsyncReqId(0),
      mStartParam(NULL),
      mSelectionDialog(NULL),
      mCurrentView(NULL)
{
    publishAll();
}


/*!
    Class desctructor.
*/
NmUriServiceInterface::~NmUriServiceInterface()
{
    NM_FUNCTION;
    
    delete mStartParam;
    delete mSelectionDialog;
}


/*!
    Queries the user for a mailbox to use.
    \param mailboxId Where the ID of the selected mailbox is set.
    \return True if a mailbox was selected, false otherwise.
*/
void NmUriServiceInterface::selectionDialogClosed(NmId &mailboxId)
{
    NM_FUNCTION;
    
    if (mailboxId.id()) { // mailbox selected
        launchEditorView(mailboxId);
    }
    else {
        cancelService();
    }
}


/*!
    Used from external applications to handle mailto: uri's.

    \param data <mailto:> uri
*/
bool NmUriServiceInterface::view(const QString& uri)
{
    NM_FUNCTION;
      
    HbMainWindow *mainWindow(NULL);
    if (mApplication) {
        // Make sure that nmail stays background if user presses back in editorview
        mApplication->updateVisibilityState();
        
        mainWindow = mApplication->mainWindow();
        if (mainWindow) {
            mCurrentView = mainWindow->currentView();  
        } 
    }

    // Hide the current view.
    if (mCurrentView) {
        mCurrentView->hide();
    }
    
    // Check the given data.
    NmStartParamDataHelper dataHelper;
    bool validData = dataHelper.extractData(uri);
     
    NmMailboxListModel &mailboxListModel = mUiEngine.mailboxListModel();
    const int count = mailboxListModel.rowCount();
    NmId mailboxId(0);

    mAsyncReqId = setCurrentRequestAsync();

    if (!validData) {
        // Failed to extract the data!
        NM_ERROR(1,"NmUriServiceInterface::view(): failed to process the given data");
        cancelService();
    }
    else if (count == 0) {
        HbDeviceMessageBox note(hbTrId("txt_mail_dialog_no_mailboxes_defined"),
                          HbMessageBox::MessageTypeInformation);
        note.setTimeout(HbMessageBox::NoTimeout);
        note.show();
        cancelService();
    }
    else { // count > 0
        if (mainWindow) {
            mainWindow->show();        
        }

    	mStartParam = new NmUiStartParam(
        	NmUiViewMessageEditor,
	        0, // account id
	        0, // folder id
    	    0, // message id
    	    NmUiEditorMailto, // editor start mode
	        dataHelper.mToAddresses, // address list
    	    0, // attachment list
        	true, // start as service
	        dataHelper.mSubject, // message subject
	        dataHelper.mCcAddresses, // list containing cc recipient addresses
    	    dataHelper.mBccAddresses // list containing bcc recipient addresses
	    );

        if (count == 1) {
            // A single mailbox exists.
            QModelIndex modelIndex = mailboxListModel.index(0, 0);
            QVariant mailbox(mailboxListModel.data(modelIndex));
            NmMailboxMetaData *mailboxMetaData = mailbox.value<NmMailboxMetaData*>();
            mailboxId = mailboxMetaData->id();
        	launchEditorView(mailboxId);
        }
        else { // count > 1
            if (!mSelectionDialog) {
                mSelectionDialog =
                    new NmMailboxSelectionDialog(mUiEngine.mailboxListModel());
            }
            
            if (!XQServiceUtil::isEmbedded()) {
                XQServiceUtil::toBackground(false);
            }
            
            connect(mSelectionDialog,SIGNAL(selectionDialogClosed(NmId&)),
                this,SLOT(selectionDialogClosed(NmId&)));
            mSelectionDialog->open();

            // launch the editor when the dialog is closed
        }
    }
    
    return true;
}

/*!
    Called when mailbox id is known and editor can be opened
    \param mailboxId mailbox using in editor
 */
void NmUriServiceInterface::launchEditorView(NmId mailboxId)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmUriServiceInterface::launchEditorView(): mailboxId=%1").arg(mailboxId.id()));
    
    // Make the previous view visible again.
    if (mCurrentView) {
        mCurrentView->show();
        mCurrentView = NULL;
    }

    if (mStartParam) {
        // Make sure the NMail application is in the foreground
        if (!XQServiceUtil::isEmbedded()) {
            XQServiceUtil::toBackground(false);    
        }
        
        mStartParam->setMailboxId(mailboxId);
        mApplication->enterNmUiView(mStartParam);
        mStartParam = NULL; // ownership passed
    }
    completeRequest(mAsyncReqId, 1);
    mAsyncReqId = 0;
}

void NmUriServiceInterface::cancelService()
{
    NM_FUNCTION;
    
    delete mStartParam;
    mStartParam = NULL;

    // If the service was started as embedded, do not hide the app.
    if (!XQServiceUtil::isEmbedded()) {
        XQServiceUtil::toBackground(true);
    }

    completeRequest(mAsyncReqId, 0);
    mAsyncReqId = 0;

    // If started as service, the application must be closed now.
    if (XQServiceUtil::isService()) {
        connect(this, SIGNAL(returnValueDelivered()),
            mApplication, SLOT(delayedExitApplication()));
    }
    else {
        // Make the previous view visible again
        if (mCurrentView) {
            mCurrentView->show();
            mCurrentView = NULL;
        }
    }
}

// End of file.
