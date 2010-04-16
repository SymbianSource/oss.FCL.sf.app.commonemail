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

//  CONSTANTS
static const QString emailSendSubjectKey = "subject";
static const QString emailSendToKey = "to";
static const QString emailSendCcKey = "cc";
static const QString emailSendBccKey = "bcc";


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
    : mSubject(0),
      mToAddresses(0),
      mCcAddresses(0),
      mBccAddresses(0),
      mAttachmentList(0),
      mEditorStartMode(NmUiEditorCreateNew)
    {
    }

    /*!
        Class destructor.
    */
    inline ~NmStartParamDataHelper()
    {
    }

    /*!
        Extracts the data from the given QVariant into the class members.
        \param data QVariant containing the data.
        \return True if success, false otherwise.
    */
    inline bool extractData(const QVariant &data)
    {
        QVariant::Type dataType = data.type();
        bool success = false;

        switch (dataType) {
            case QVariant::String: {
                // The given data contains a single attachment.
                QString attachment = data.toString();
                mAttachmentList = new QStringList(attachment);
                success = true;
                break;
            }
            case QVariant::StringList: {
                // The given data contains a list of attachments.
                QStringList attachmentList = data.toStringList();
                mAttachmentList = new QStringList(attachmentList);
                success = true;
                break;
            }
            case QVariant::Map: {
                // The given data may contain a mail subject and recipient lists.
                QMap<QString, QVariant> map = data.toMap();
                success = processMap(map);
                break;
            }
            default: {
                // Data type not supported!
                NMLOG("NmStartParamDataHelper::extractData(): Data type not supported!");
                break;
            }
        } // switch ()

        // Determine the editor start mode.
        if (mToAddresses || mCcAddresses || mBccAddresses) {
            mEditorStartMode = NmUiEditorMailto;
        }
        else if (mAttachmentList) {
            mEditorStartMode = NmUiEditorCreateNew;
        }

        return success;
    }

    /*!
        Deletes the class members. Must be used if NmUiStartParam does not
        take ownership of the members.
    */
    inline void deleteData()
    {
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

        delete mAttachmentList;
        mAttachmentList = 0;
    }


private:

    /*!
        Extracts the data from the given map into the class members.
        \param map The map to extract the data from.
        \return True if success, false otherwise.
    */
    inline bool processMap(const QMap<QString, QVariant> &map)
    {
        QMap<QString, QVariant>::const_iterator i = map.constBegin();
        QString key;
        QVariant value;
        
        while (i != map.constEnd()) {
            key = i.key();
            value = i.value();
            
            if (!key.compare(emailSendSubjectKey, Qt::CaseInsensitive) &&
                value.type() == QVariant::String) {
                // Extract the mail subject.
                if (mSubject) {
                    // A subject string has already been extracted! Discard the
                    // old subject.
                    delete mSubject;
                }

                mSubject = new QString(value.toString());
            }
            else if (!key.compare(emailSendToKey, Qt::CaseInsensitive)) {
                // Extract the "to" recipients.
                addAddressesToList(value, &mToAddresses);
            }
            else if (!key.compare(emailSendCcKey, Qt::CaseInsensitive)) {
                // Extract the "cc" recipients.
                addAddressesToList(value, &mCcAddresses);
            }
            else if (!key.compare(emailSendBccKey, Qt::CaseInsensitive)) {
                // Extract the "bcc" recipients.
                addAddressesToList(value, &mBccAddresses);
            }

            ++i;
        }

        // In principle it is ok even if the map does not contain any data e.g.
        // in a case where the editor view needs to be opened without any
        // predefined data. Therefore, we always return true.
        return true;
    }

    /*!
        Appends the given addresses into the given list.
        \param address The addresses to append.
        \param list The list where the addresses are appended to.
    */
    inline void addAddressesToList(const QVariant &addresses,
                                   QList<NmAddress*> **list)
    {
      
        if (!list) {
            // Invalid argument!
            return;
        }

        QVariant::Type dataType = addresses.type();
        QList<NmAddress*> foundAddresses;

        switch (dataType) {
            case QVariant::String: {
                // A single address.
                NmAddress *address = new NmAddress(addresses.toString());
                foundAddresses.append(address);
                break;
            }
            case QVariant::StringList: {
                // A list of addresses.
                QStringList addressList = addresses.toStringList();

                foreach (QString addressAsString, addressList) {
                    NmAddress *address = new NmAddress(addressAsString);
                    foundAddresses.append(address);
                }

                break;
            }
            default: {
                break;
            }
        }

        if (foundAddresses.count()) {
            // Append the found addresses into the given list.
            if (!(*list)) {
                *list = new QList<NmAddress*>();
            }

            (*list)->append(foundAddresses);
        }
    }


public: // Data

    QString *mSubject; // Not owned.
    QList<NmAddress*> *mToAddresses; // Not owned.
    QList<NmAddress*> *mCcAddresses; // Not owned.
    QList<NmAddress*> *mBccAddresses; // Not owned.
    QStringList *mAttachmentList; // Not owned.
    NmUiEditorStartMode mEditorStartMode;
};



/*!
    \class NmSendServiceInterface
    \brief NMail application service interface which provides an email sending
           interface for other application using the Qt Highway. 
*/

/*!
    Class constructor.
*/
NmSendServiceInterface::NmSendServiceInterface(QObject *parent,
                                               NmUiEngine &uiEngine,
                                               NmApplication *application)
#ifndef NM_WINS_ENV
    : XQServiceProvider(QLatin1String("com.nokia.symbian.IMessage.Send"), parent),
#else
    : QObject(parent),
#endif
      mApplication(application),
      mUiEngine(uiEngine),
      mAsyncReqId(0)
{
#ifndef NM_WINS_ENV
    publishAll();
#endif
}


/*!
    Class desctructor.
*/
NmSendServiceInterface::~NmSendServiceInterface()
{
}


/*!
    Queries the user for a mailbox to use.
    \param mailboxId Where the ID of the selected mailbox is set.
    \return True if a mailbox was selected, false otherwise.
*/
bool NmSendServiceInterface::selectMailbox(NmId& mailboxId)
{
	NmMailboxSelectionDialog *dialog =
	    new NmMailboxSelectionDialog(mUiEngine.mailboxListModel());
	bool ok = dialog->exec(mailboxId);
	delete dialog;
	return ok;
}


/*!
    Used for sending email messages from external applications. Used by the
    Share UI. In addition, the interface can be used to send email to a contact
    from another application. The method sets the given data (e.g. recipients,
    attachments) to a mail composer (editor) view and displays it.

    \param data If used by Share UI, will contain the list of filenames to
                attach. Can also contain a map with key value pairs containing
                subject and recipient data. 
*/
void NmSendServiceInterface::send(QVariant data)
{
    NMLOG("NmSendServiceInterface::send()");

#ifndef NM_WINS_ENV
    // Make sure the NMail application is on the foreground.
    XQServiceUtil::toBackground(false);
    HbMainWindow *mainWindow = mApplication->mainWindow();
    HbView *currentView = mainWindow->currentView();

    // Hide the current view.
    if (currentView) {
        currentView->hide();
    }

    // Check the given data.
    NmStartParamDataHelper dataHelper;
    bool validData = dataHelper.extractData(data);

    NmMailboxListModel &mailboxListModel = mUiEngine.mailboxListModel();
    const int count = mailboxListModel.rowCount();
    NmId mailboxId(0);
    bool launchEditor(true);

    if (!validData) {
        // Failed to extract the data!
        NMLOG("NmSendServiceInterface::send(): Failed to process the given data!");
        launchEditor = false;
    }
    else if (count == 0) {
        // No mailboxes.
        HbMessageBox note(hbTrId("txt_mail_dialog_no_mailboxes_defined"),
                          HbMessageBox::MessageTypeInformation);
        note.setTimeout(HbMessageBox::NoTimeout);
        note.exec();
        launchEditor = false;
    }
    else if (count == 1) {
        // A single mailbox exists.
        QModelIndex modelIndex = mailboxListModel.index(0, 0);
        QVariant mailbox(mailboxListModel.data(modelIndex));
        NmMailboxMetaData *mailboxMetaData = mailbox.value<NmMailboxMetaData*>();
        mailboxId = mailboxMetaData->id();
    }
    else if (count > 1 && !selectMailbox(mailboxId)) {
        // More than one mailboxes exist but the user cancelled the mailbox
        // selection dialog.
        launchEditor = false;
    }

    mAsyncReqId = setCurrentRequestAsync();
    
    if (launchEditor) {
        // Make the previous view visible again.
        if (currentView) {
            currentView->show();
        }

        NmUiStartParam *startParam = new NmUiStartParam(
            NmUiViewMessageEditor,
            mailboxId,
            0, // folder id
            0, // message id
            dataHelper.mEditorStartMode, // editor start mode
            dataHelper.mToAddresses, // address list
            dataHelper.mAttachmentList, // attachment list
            true, // start as service
            dataHelper.mSubject, // message subject
            dataHelper.mCcAddresses, // list containing cc recipient addresses
            dataHelper.mBccAddresses // list containing bcc recipient addresses
        );

        mApplication->enterNmUiView(startParam);
        completeRequest(mAsyncReqId, 1);
    } 
    else {
        // Delete the extracted data since the editor was not launched and thus
        // NmUiStartParam did not take the ownership of the data.
        dataHelper.deleteData();

        // If the service was started as embedded, do not hide the app.
		if (!XQServiceUtil::isEmbedded()) {
			XQServiceUtil::toBackground(true);
		}

        completeRequest(mAsyncReqId, 0);

        // If started as service, the application must be closed now.
        if (XQServiceUtil::isService()) {
            connect(this, SIGNAL(returnValueDelivered()),
                    mApplication, SLOT(delayedExitApplication()));
        }
        else {
            // Make the previous view visible again.
            if (currentView) {
                currentView->show();
            }
        }
    }
#endif /* NM_WINS_ENV */
}


// End of file.
