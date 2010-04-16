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

#include <QVariant>
#include <HbAction>
#include <HbGlobal>
#include <HbMessageBox>
#include <HbProgressDialog>
#include <cpsettingformitemdata.h>

#include "nmipssettingshelper.h"
#include "nmipssettingsmanagerbase.h"

/*!
    \class NmIpsSettingsHelper
    \brief Helper class to save data into database when user has made changes.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsHelper.
    \param settingsManager Reference used by settingshelper to read and store values
*/
NmIpsSettingsHelper::NmIpsSettingsHelper(NmIpsSettingsManagerBase &settingsManager)
: mCurrentLineEditChanged(false),
  mSettingsManager(settingsManager)
{
}

/*!
    Destructor of NmIpsSettingsHelper.
*/
NmIpsSettingsHelper::~NmIpsSettingsHelper()
{
    mContentItems.clear();
}

/*!
    Inserts content item pointers into map, which then can be used to access the widgets data.
    \param IpsServices::SettingItem. Key that can be used to access the value from map.
    \param  CpSettingFormItemData *. Pointer to the content item.
*/
void NmIpsSettingsHelper::insertContentItem(IpsServices::SettingItem key,
                                            CpSettingFormItemData *value)
{
    mContentItems.insert(key, value);
}

/*!
    Saves the My Name value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveMyName()
{
    if (mCurrentLineEditChanged) {
        CpSettingFormItemData *item = mContentItems.value(IpsServices::EmailAlias);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::EmailAlias, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::myNameTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Saves the Mailbox name value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveMailboxName()
{
    if (mCurrentLineEditChanged) {
        CpSettingFormItemData *item = mContentItems.value(IpsServices::MailboxName);
        QVariant data = item->contentWidgetData(QString("text"));
        // Only save mailbox name if it's length is greater than zero. CEmailAccounts does not
        // allow zero-length mailbox names.
        if (data.toString().length() > 0) {
            mSettingsManager.writeSetting(IpsServices::MailboxName, data);
            QVariant property(NmSettings::MailboxName);
            emit mailboxPropertyChanged(mSettingsManager.mailboxId(), property, data);
        }
        else {
            if (mSettingsManager.readSetting(IpsServices::MailboxName, data)) {
                item->setContentWidgetData(QString("text"), data);
            }
        }

    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::mailboxNameTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Saves the Mail address value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveMailAddress()
{
    if (mCurrentLineEditChanged) {
        CpSettingFormItemData *item = mContentItems.value(IpsServices::EmailAddress);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::EmailAddress, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::mailAddressTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Saves the Username value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveUserName()
{
    if (mCurrentLineEditChanged) {
        CpSettingFormItemData *item = mContentItems.value(IpsServices::LoginName);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::LoginName, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::userNameTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Saves the Password value into database
*/
void NmIpsSettingsHelper::savePassword()
{
    CpSettingFormItemData *item = mContentItems.value(IpsServices::Password);
    QVariant data = item->contentWidgetData(QString("text"));
    mSettingsManager.writeSetting(IpsServices::Password, data);
}

/*!
    Saves the Reply to value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveReplyTo()
{
    if (mCurrentLineEditChanged) {
        CpSettingFormItemData *item = mContentItems.value(IpsServices::ReplyAddress);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::ReplyAddress, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::replyToTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Deletes mailbox and displays the proper notes.
*/
void NmIpsSettingsHelper::deleteButtonPress()
{
    HbMessageBox confirmationDialog(HbMessageBox::MessageTypeQuestion);
    confirmationDialog.setText(hbTrId("txt_mail_dialog_do_you_want_to_delete_the_mailbox"));
    confirmationDialog.setModal(true);
    confirmationDialog.setTimeout(HbMessageBox::NoTimeout);

    HbAction *action = confirmationDialog.exec();

    if (action != confirmationDialog.primaryAction()) {
        // The user did not confirm the deletion.
        return;
    }

    QVariant mailboxName;
    mSettingsManager.readSetting(IpsServices::MailboxName, mailboxName);

    // Display the progress note. Before display the note, remove the cancel
    // button.
    HbProgressDialog progressNote(HbProgressDialog::WaitDialog);
    progressNote.setText(hbTrId("txt_mail_dpophead _1_deleted"));
    action = progressNote.primaryAction();
    progressNote.removeAction(action);
    progressNote.delayedShow();

    if (!mSettingsManager.deleteMailbox()) {
        // The mailbox was deleted successfully. Hide the progress note and
        // display the "mailbox deleted" dialog.
        progressNote.close();

        HbMessageBox infoDialog(HbMessageBox::MessageTypeInformation);
        infoDialog.setText(
            hbTrId("txt_mail_dialog_1_deleted").arg(mailboxName.toString()));
        infoDialog.setModal(true);
        HbAction *action = infoDialog.exec();

        // Emit the signal to update the UI.
        emit mailboxListChanged(mSettingsManager.mailboxId(),
                                NmSettings::MailboxDeleted);
    } else {
        // Failed to delete the mailbox!
        // TODO: Should an error note be displayed?
        progressNote.close();
    }
}

/*!
    Saves the selected receiving schedule setting.
    \index Selected receiving schedule.
*/
void NmIpsSettingsHelper::receivingScheduleChange(int index)
{
    mSettingsManager.writeSetting(IpsServices::ReceptionActiveProfile, QVariant(index));
}
