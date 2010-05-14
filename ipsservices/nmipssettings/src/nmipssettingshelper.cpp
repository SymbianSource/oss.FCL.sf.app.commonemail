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
#include <HbInputDialog>
#include <cpsettingformitemdata.h>
#include <hbdataformmodel.h>
#include <qstringlist.h>
#include <hbextendedlocale.h>
#include <hblineedit.h>
#include <HbValidator>
#include <QIntValidator>
#include <HbStringUtil>
#include <HbDataForm>

#include "nmipssettingshelper.h"
#include "nmipssettingsmanagerbase.h"
#include "nmipssettingscustomitem.h"
#include "nmipsextendedsettingsmanager.h"

// CONSTANTS

// Dynamic receiving schedule group items
const IpsServices::SettingItem NmIpsSettingsReceivingSchedule[] = {
        IpsServices::ReceptionInboxSyncWindow,
        IpsServices::ReceptionWeekDays,
        IpsServices::ReceptionDayStartTime,
        IpsServices::ReceptionDayEndTime,
        IpsServices::ReceptionRefreshPeriodDayTime};

/*!
    \class NmIpsSettingsHelper
    \brief Helper class to save data into database when user has made changes.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsHelper.
    \param settingsManager Reference used by settingshelper to read and store values
*/
NmIpsSettingsHelper::NmIpsSettingsHelper(NmIpsSettingsManagerBase &settingsManager,
    HbDataForm &dataForm, HbDataFormModel &dataFormModel)
: mCurrentLineEditChanged(false),
  mEmitOnline(false),
  mSettingsManager(settingsManager),
  mDataForm(dataForm),
  mDataFormModel(dataFormModel),
  mDeleteConfirmationDialog(0),
  mDeleteInformationDialog(0),
  mIncomingPortInputDialog(0),
  mIncomingPortInputValidator(0),
  mFolderPathInputDialog(0),
  mOutgoingPortInputDialog(0),
  mOutgoingPortInputValidator(0),
  mServerInfoDynamicItemsVisible(false),
  mRadioButtonPreviousIndex(0)
{
}

/*!
    Destructor of NmIpsSettingsHelper.
*/
NmIpsSettingsHelper::~NmIpsSettingsHelper()
{
    mContentItems.clear();
    delete mDeleteConfirmationDialog;
    delete mDeleteInformationDialog;
    delete mIncomingPortInputDialog;
    delete mIncomingPortInputValidator;
    delete mFolderPathInputDialog;
    delete mOutgoingPortInputDialog;
}

/*!
    Inserts content item pointers into map, which then can be used to access the widgets data.
    \param IpsServices::SettingItem. Key that can be used to access the value from map.
    \param  HbDataFormModelItem *. Pointer to the content item.
*/
void NmIpsSettingsHelper::insertContentItem(IpsServices::SettingItem key,
    HbDataFormModelItem *value)
{
    mContentItems.insert(key, value);
}

/*!
    Returns a content item from contentitem map.
*/
HbDataFormModelItem *NmIpsSettingsHelper::contentItem(IpsServices::SettingItem key) const
{
    return mContentItems.value(key);
}

/*!
    Sets the group item for dynamic receiving schedule items.
    \param item Group item.
*/
void NmIpsSettingsHelper::setReceivingScheduleGroupItem(HbDataFormModelItem *item)
{
    mReceivingScheduleGroupItem = item;
}

/*!
    Sets the group item for dynamic receiving schedule items.
    \param item Group item.
*/
void NmIpsSettingsHelper::setServerInfoGroupItem(HbDataFormModelItem *item)
{
    mServerInfoGroupItem = item;
}

/*!
    Creates or updates the setting items under the receiving schedule group item.
*/
void NmIpsSettingsHelper::createOrUpdateReceivingScheduleGroupDynamicItem(
    IpsServices::SettingItem item)
{
    // Create the items if not already created and visible
    if (mReceivingScheduleGroupItem) {
        CpSettingFormItemData *formItemData =
            static_cast<CpSettingFormItemData *>(mContentItems.value(item));

        switch(item) {

            // 3. Receiving weekdays
            case IpsServices::ReceptionWeekDays:
            {
                // If not exist, create one
                if (!formItemData) {
                    formItemData =
                        new CpSettingFormItemData(
                            static_cast<HbDataFormModelItem::DataItemType>(NmIpsSettingsCustomItem::MultiSelectionItem),
                                hbTrId("txt_mailips_setlabel_receiving_weekdays"));

                    insertContentItem(IpsServices::ReceptionWeekDays, formItemData);
                    mReceivingScheduleGroupItem->appendChild(formItemData);
                    QStringList weekdayItems;

                    weekdayItems << hbTrId("txt_mailips_setlabel_download_images_val_mon")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_tue")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_wed")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_thu")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_fri")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_sat")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_sun");

                    QList<QVariant> weekdayItemValues;
                    weekdayItemValues << 0x01
                                      << 0x02
                                      << 0x04
                                      << 0x08
                                      << 0x10
                                      << 0x20
                                      << 0x40;

                    formItemData->setData(HbDataFormModelItem::HbDataFormModelItem::DescriptionRole + 1,
                        weekdayItemValues);

                    formItemData->setContentWidgetData(QString("heading"),
                        hbTrId("txt_mailips_dialog_heading_receiving_weekdays"));

                    formItemData->setContentWidgetData(QString("items"), weekdayItems);

                    mDataForm.addConnection(formItemData, SIGNAL(editingFinished()),
                        this, SLOT(receivingWeekdaysModified()));
                }

                // Update data
                QVariant value;
                mSettingsManager.readSetting(item, value);
                QList<QVariant> selectedDays;
                selectedDays.clear();
                int days(value.toInt());

                // 0-6 Individual weekdays selected
                for (int i=0; i<=6; ++i) {
                   if (days & (1 << i)) {
                       selectedDays.append(i);
                   }
                }

                formItemData->setContentWidgetData(QString("selectedItems"), selectedDays);

                // Every weekday selected
                if (days == 0x7f) {
                    formItemData->setContentWidgetData(QString("text"),
                        hbTrId("txt_mailips_setlabel_download_images_val_every_day"));
                }

                break;
            }

            // 4. Day start time
            case IpsServices::ReceptionDayStartTime:
            {
                // If not exist, create one
                if (!formItemData) {
                    formItemData = new CpSettingFormItemData(
                        static_cast<HbDataFormModelItem::DataItemType>(NmIpsSettingsCustomItem::TimeEditor),
                            hbTrId("txt_mailips_setlabel_day_start_time"));

                    insertContentItem(IpsServices::ReceptionDayStartTime, formItemData);
                    formItemData->setContentWidgetData(QString("heading"),
                        hbTrId("txt_mailips_dialog_heading_start_time"));
                    mReceivingScheduleGroupItem->appendChild(formItemData);
                    mDataForm.addConnection(formItemData, SIGNAL(timeChanged(QTime)),
                        this, SLOT(startTimeModified(QTime)));

                }

                // Update data
                QVariant startTime;
                mSettingsManager.readSetting(item, startTime);
                int startHour(startTime.toInt() / 60);
                int startMinute(startTime.toInt() % 60);
                formItemData->setContentWidgetData("time", QTime(startHour, startMinute));
                break;
            }

            // 5. Day end time
            case IpsServices::ReceptionDayEndTime:
            {
                // If not exist, create one
                if (!formItemData) {
                    formItemData = new CpSettingFormItemData(
                        static_cast<HbDataFormModelItem::DataItemType> (NmIpsSettingsCustomItem::TimeEditor),
                            hbTrId("txt_mailips_setlabel_day_end_time"));

                    insertContentItem(IpsServices::ReceptionDayEndTime, formItemData);
                    formItemData->setContentWidgetData(QString("heading"),
                        hbTrId("txt_mailips_dialog_heading_end_time"));
                    mReceivingScheduleGroupItem->appendChild(formItemData);
                }

                // Update data
                QVariant endTime;
                mSettingsManager.readSetting(item, endTime);
                int endHour(endTime.toInt() / 60);
                int endMinute(endTime.toInt() % 60);
                formItemData->setContentWidgetData("time", QTime(endHour, endMinute));
                mDataForm.addConnection(formItemData, SIGNAL(timeChanged(QTime)),
                    this, SLOT(endTimeModified(QTime)));
                break;
            }

            // 6. Refresh mail interval
            case IpsServices::ReceptionRefreshPeriodDayTime:
            {
                // If not exist, create one
                if (!formItemData) {
                    formItemData = new CpSettingFormItemData(
                        HbDataFormModelItem::RadioButtonListItem,
                            hbTrId("txt_mailips_setlabel_refresh_mail"));

                    mReceivingScheduleGroupItem->appendChild(formItemData);
                    insertContentItem(IpsServices::ReceptionRefreshPeriodDayTime, formItemData);
                    QStringList refreshMailItems;
                    refreshMailItems << hbTrId("txt_mailips_setlabel_val_keep_uptodate")
                                     << hbTrId("txt_mailips_setlabel_val_every_15_minutes")
                                     << hbTrId("txt_mailips_setlabel_val_every_1_hour")
                                     << hbTrId("txt_mailips_setlabel_val_every_4_hours");
                    formItemData->setContentWidgetData(QString("items"), refreshMailItems);

                    QList<QVariant> refreshMailItemValues;
                    refreshMailItemValues << 5
                                          << 15
                                          << 60
                                          << 240;
                    QVariant value(refreshMailItemValues);
                    formItemData->setData(HbDataFormModelItem::DescriptionRole + 1, value);

                    mDataForm.addConnection(formItemData, SIGNAL(itemSelected(int)),
                        this, SLOT(refreshPeriodModified(int)));
                }

                // Update data
                QVariant interval;
                mSettingsManager.readSetting(item, interval);
                QHash<int,int> refreshPeriod;
                refreshPeriod[5] = 0;
                refreshPeriod[15] = 1;
                refreshPeriod[60] = 2;
                refreshPeriod[240] = 3;
                formItemData->setContentWidgetData(QString("selected"),
                    refreshPeriod.value(interval.toInt()));
                break;
            }
            default:
            {
                // Unknown/unhandled SettingItem.
                break;
            }
        }
    }
}

/*!
    Creates the setting items under the Serverinfo group items user authentication.
*/
void NmIpsSettingsHelper::createServerInfoGroupDynamicItems()
{   
    HbDataFormModelItem *item = mContentItems.value(IpsServices::SMTPAuthentication);
    int insertIndex = mServerInfoGroupItem->indexOf(item) + 1;
        
    // Username
    QVariant username;
    mSettingsManager.readSetting(IpsServices::OutgoingLoginName, username);
    CpSettingFormItemData *usernameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_username"));
    insertContentItem(IpsServices::OutgoingLoginName, usernameItem);
    usernameItem->setContentWidgetData(QString("text"), username);
    mDataForm.addConnection(usernameItem, SIGNAL(editingFinished()),
                            this, SLOT(saveOutgoingUserName()));
    mDataForm.addConnection(usernameItem, SIGNAL(textChanged(QString)),
                            this, SLOT(outgoingUserNameTextChange(QString)));
    mServerInfoGroupItem->insertChild(insertIndex, usernameItem);

    // Password
    QVariant password;
    mSettingsManager.readSetting(IpsServices::OutgoingPassword, password);
    CpSettingFormItemData *passwordItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_password"));
    insertContentItem(IpsServices::OutgoingPassword, passwordItem);
    passwordItem->setContentWidgetData(QString("text"), password);
    passwordItem->setContentWidgetData(QString("echoMode"), HbLineEdit::PasswordEchoOnEdit);
    mDataForm.addConnection(passwordItem, SIGNAL(editingFinished()),
                            this, SLOT(saveOutgoingPassword()));
    mServerInfoGroupItem->insertChild(insertIndex + 1, passwordItem);
    
    mServerInfoDynamicItemsVisible = true;
}

/*!
    Saves the My Name value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveMyName()
{
    if (mCurrentLineEditChanged) {
        HbDataFormModelItem *item = mContentItems.value(IpsServices::EmailAlias);
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
	bool ok = true;
    if (mCurrentLineEditChanged) {
        HbDataFormModelItem *item = mContentItems.value(IpsServices::MailboxName);
        QVariant data = item->contentWidgetData(QString("text"));
        // Only save mailbox name if it's length is greater than zero. CEmailAccounts does not
        // allow zero-length mailbox names.
        if (data.toString().length() > 0) {
            ok = mSettingsManager.writeSetting(IpsServices::MailboxName, data);

            // If failed, try again when the plugin is set to offline
            if (!ok) {
                NMLOG("NmIpsSettingsHelper::saveMailboxName failed");
            	emit goOffline(mSettingsManager.mailboxId());
				mEmitOnline = true;
				ok = mSettingsManager.writeSetting(IpsServices::MailboxName, data);
			}

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
		emit goOffline(mSettingsManager.mailboxId());
		mEmitOnline = true;
		HbDataFormModelItem *item = mContentItems.value(IpsServices::EmailAddress);
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
    Saves the Incoming username value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveIncomingUserName()
{
    if (mCurrentLineEditChanged) {
		emit goOffline(mSettingsManager.mailboxId());
		mEmitOnline = true;
		HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingLoginName);
        QVariant username = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::IncomingLoginName, username);
        // Outgoing username needs to be updated if it is set as Same as Incoming.
        item = mContentItems.value(IpsServices::SMTPAuthentication);
        QVariant selected = item->contentWidgetData(QString("selected"));
        if (selected.toInt() == IpsServices::EMailAuthSameAsIncoming) {
            mSettingsManager.writeSetting(IpsServices::OutgoingLoginName, username);
        }
    }
    mCurrentLineEditChanged = false;
}

/*!
    Saves the Outgoing username value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveOutgoingUserName()
{
    if (mCurrentLineEditChanged) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingLoginName);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::OutgoingLoginName, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::incomingUserNameTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/

void NmIpsSettingsHelper::outgoingUserNameTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Saves the Incoming password value into database
*/
void NmIpsSettingsHelper::saveIncomingPassword()
{
	emit goOffline(mSettingsManager.mailboxId());
	mEmitOnline = true;
	HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingPassword);
    QVariant password = item->contentWidgetData(QString("text"));
    mSettingsManager.writeSetting(IpsServices::IncomingPassword, password);
    // Outgoing password needs to be updated if it is set as Same as Incoming.
    item = mContentItems.value(IpsServices::SMTPAuthentication);
    QVariant selected = item->contentWidgetData(QString("selected"));
    if (selected.toInt() == IpsServices::EMailAuthSameAsIncoming) {
        mSettingsManager.writeSetting(IpsServices::OutgoingPassword, password);
    }
}

/*!
    Saves the Outgoing password value into database
*/
void NmIpsSettingsHelper::saveOutgoingPassword()
{
    emit goOffline(mSettingsManager.mailboxId());
    mEmitOnline = true;
    HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingPassword);
    QVariant data = item->contentWidgetData(QString("text"));
    mSettingsManager.writeSetting(IpsServices::OutgoingPassword, data);
}
/*!
    Saves the Reply to value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveReplyTo()
{
    if (mCurrentLineEditChanged) {
        HbDataFormModelItem *item = mContentItems.value(IpsServices::ReplyAddress);
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
    Shows the confirmation dialog for user.
*/
void NmIpsSettingsHelper::deleteButtonPress()
{
    if(!mDeleteConfirmationDialog) {
        mDeleteConfirmationDialog = 
            new HbMessageBox(HbMessageBox::MessageTypeQuestion);
        mDeleteConfirmationDialog->setText(
            hbTrId("txt_mail_dialog_do_you_want_to_delete_the_mailbox"));
        mDeleteConfirmationDialog->setTimeout(HbMessageBox::NoTimeout);
    }
    mDeleteConfirmationDialog->open(this, SLOT(handleMailboxDelete(HbAction *)));
}

/*!
    Deletes mailbox and displays the proper notes.
*/
void NmIpsSettingsHelper::handleMailboxDelete(HbAction *action)
{
    if (action == mDeleteConfirmationDialog->primaryAction()) {
        
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = false;
    
        QVariant mailboxName;
        mSettingsManager.readSetting(IpsServices::MailboxName, mailboxName);
    
        // Display the progress note. Before display the note, remove the cancel
        // button.
        HbProgressDialog progressNote(HbProgressDialog::WaitDialog);
        progressNote.setText(hbTrId("txt_common_info_deleting"));
        action = progressNote.primaryAction();
        progressNote.removeAction(action);
        progressNote.delayedShow();
    
        if (!mSettingsManager.deleteMailbox()) {
            // The mailbox was deleted successfully. Hide the progress note and
            // display the "mailbox deleted" dialog.
            progressNote.close();
            
            if (!mDeleteInformationDialog) {
                mDeleteInformationDialog = 
                    new HbMessageBox(HbMessageBox::MessageTypeInformation);
                mDeleteInformationDialog->setText(
                    hbTrId("txt_mail_dpophead _1_deleted").arg(mailboxName.toString()));
                mDeleteInformationDialog->setTimeout(HbMessageBox::NoTimeout);
            }
            mDeleteInformationDialog->open(this, SLOT(handleMailboxDeleteUpdate(HbAction *)));            
            
        } else {
            // Failed to delete the mailbox!
            progressNote.close();
        }
    }
}

/*!
    Handels the event after the mailbox delete information dialog has been dismissed.
*/
void NmIpsSettingsHelper::handleMailboxDeleteUpdate(HbAction *action)
{
    Q_UNUSED(action);
    // Emit the signal to update the UI.
    emit mailboxListChanged(mSettingsManager.mailboxId(),
                            NmSettings::MailboxDeleted);
}

/*!
    Saves the selected receiving schedule setting.
    \param index Selected receiving schedule.
*/
void NmIpsSettingsHelper::receivingScheduleChange(int index)
{
    mSettingsManager.writeSetting(IpsServices::ReceptionActiveProfile, QVariant(index));

    // Remove text property to avoid old data to be copied from model to widget.
    HbDataFormModelItem *item = contentItem(IpsServices::ReceptionWeekDays);
    if (item) {
        QHash<QString, QVariant> data = item->contentWidgetData();
        data.remove("text");
        item->setData(HbDataFormModelItem::PropertyRole, data);
    }

    // Read receiving schedule dynamic group item values and
    // make a decision based on those if item should be visible or not.
    const int dynamicGroupItemsCount(
        sizeof(NmIpsSettingsReceivingSchedule) / sizeof(NmIpsSettingsReceivingSchedule[0]));

    for (int itemIndex(0); itemIndex < dynamicGroupItemsCount; ++itemIndex) {
        QVariant setting;
        // Read setting value from active profile
        IpsServices::SettingItem item(NmIpsSettingsReceivingSchedule[itemIndex]);
        mSettingsManager.readSetting(item, setting);

        // If setting value is not valid (-1=N/A) then remove setting item.
        int value = setting.toInt();
        if (value != -1) {
            createOrUpdateReceivingScheduleGroupDynamicItem(item);
        } else {
            deleteReceivingScheduleGroupDynamicItem(item);
        }
    }

    updateShowMailInMailbox();
}

/*!
    Returns state if online should be emited.
    \return true / false.
*/
bool NmIpsSettingsHelper::isOffline()
{
    return mEmitOnline;
}

/*!
    Updates the show mail in mailbox setting value based on the active profile.
*/
void NmIpsSettingsHelper::updateShowMailInMailbox()
{
    QVariant value;
    mSettingsManager.readSetting(IpsServices::ReceptionInboxSyncWindow, value);
    QHash<int,int> syncWindows;
    syncWindows[50] = 0;
    syncWindows[100] = 1;
    syncWindows[500] = 2;
    syncWindows[0] = 3;

    HbDataFormModelItem *item = mContentItems.value(IpsServices::ReceptionInboxSyncWindow);
    if (item) {
        item->setContentWidgetData(QString("selected"), syncWindows.value(value.toInt()));
    }
}

/*!
    Deletes the dynamic setting items under the receiving schedule group item.
*/
void NmIpsSettingsHelper::deleteReceivingScheduleGroupDynamicItem(IpsServices::SettingItem item)
{
    HbDataFormModelItem *formItem = mContentItems.value(item);
    if (formItem) {
        mDataForm.removeAllConnection(formItem);
        int index(mReceivingScheduleGroupItem->indexOf(formItem));
        if (index >= 0) {
            mReceivingScheduleGroupItem->removeChild(index);
        }
        mContentItems.remove(item);
    }
}

/*!
    Deletes the dynamic setting items under the serverinfo group item.
*/
void NmIpsSettingsHelper::deleteServerInfoGroupDynamicItems()
{
    if(mServerInfoDynamicItemsVisible) {
        HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingLoginName);
        mDataForm.removeConnection(item, SIGNAL(editingFinished()),
            this, SLOT(saveOutgoingUserName()));
        mDataForm.removeConnection(item, SIGNAL(textChanged(QString)),
            this, SLOT(outgoingUserNameTextChange(QString)));
        int index = mServerInfoGroupItem->indexOf(item);
        mServerInfoGroupItem->removeChild(index);
        mContentItems.remove(IpsServices::OutgoingLoginName);

        item = mContentItems.value(IpsServices::OutgoingPassword);
        mDataForm.removeConnection(item, SIGNAL(editingFinished()),
            this, SLOT(saveOutgoingPassword()));
        index = mServerInfoGroupItem->indexOf(item);
        mServerInfoGroupItem->removeChild(index);
        mContentItems.remove(IpsServices::OutgoingPassword);
        mServerInfoDynamicItemsVisible = false;
    }
}

/*!
    Saves the incoming mailserver value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveIncomingMailServer()
{
    if (mCurrentLineEditChanged) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingMailServer);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::IncomingMailServer, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::incomingMailServerTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}
/*!
    Saves the outgoing mailserver value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveOutgoingMailServer()
{
    if (mCurrentLineEditChanged) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingMailServer);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::OutgoingMailServer, data);
    }
    mCurrentLineEditChanged = false;
}

/*!
    Sets the edit changed flag to indicate that user has edited the field

    \param text Reference to the text value in the line edit box.
*/
void NmIpsSettingsHelper::outgoingMailServerTextChange(const QString &text)
{
    Q_UNUSED(text);
    mCurrentLineEditChanged = true;
}

/*!
    Saves the incoming port value into database if user has changed the value. If the user wish to
    define the port, a input dialog is shown.
    
    \param index Used to determine if the default value or a user defined value should be written
*/
void NmIpsSettingsHelper::incomingPortChange(int index)
{
    if (mRadioButtonPreviousIndex != index && index == IpsServices::NmIpsSettingsDefault) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        int port = mSettingsManager.determineDefaultIncomingPort();
        mSettingsManager.writeSetting(IpsServices::IncomingPort, port);
    } else if (index == IpsServices::NmIpsSettingsUserDefined) {
        showIncomingPortInputDialog();
    }
}

/*!
    Show a input dialog for allowing the user to specify a incoming port.
*/
void NmIpsSettingsHelper::showIncomingPortInputDialog()
{    
    // User can open the dialog multiple times, so delete the old ones first.
    delete mIncomingPortInputDialog;
    mIncomingPortInputDialog = 0;
    delete mIncomingPortInputValidator;
    mIncomingPortInputValidator = 0;
    
    mIncomingPortInputDialog = new HbInputDialog();
    mIncomingPortInputDialog->setInputMode(HbInputDialog::IntInput);
    QVariant currentPort;
    mSettingsManager.readSetting(IpsServices::IncomingPort, currentPort);
    mIncomingPortInputValidator = new HbValidator();
    mIncomingPortInputValidator->addField(new QIntValidator(0, 65535, 0), 
                        HbStringUtil::convertDigits(QString::number(currentPort.toInt())));
    mIncomingPortInputDialog->setValidator(mIncomingPortInputValidator);
    mIncomingPortInputDialog->setPromptText(
        hbTrId("txt_mailips_setlabel_incoming_port_user_defined"));
    
    mIncomingPortInputDialog->open(this, SLOT(handleUserDefinedIncomingPortInput(HbAction *)));
}

/*!
   Handels the saving of the port new value.
*/
void NmIpsSettingsHelper::handleUserDefinedIncomingPortInput(HbAction *action)
{
    if (action == mIncomingPortInputDialog->primaryAction()) {
        QVariant newPort = mIncomingPortInputDialog->value();
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        mSettingsManager.writeSetting(IpsServices::IncomingPort, newPort);
    } else {
        //set selected value back if user canceled.
        HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingPort);
        item->setContentWidgetData(QString("selected"), mRadioButtonPreviousIndex);
    }
}

/*!
    Used for getting the index to display in the port radio button list 
    
    \return index Used to set the selected value
*/
int NmIpsSettingsHelper::getCorrectPortRadioButtonIndex(int currentPort)
{
    int index = 0;
    int port = mSettingsManager.determineDefaultIncomingPort();
    if (port == currentPort) {
        index = IpsServices::NmIpsSettingsDefault;
    } else {
        index = IpsServices::NmIpsSettingsUserDefined;
    }
    return index;
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::incomingPortPress(const QModelIndex &index)
{
    Q_UNUSED(index);
    HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingPort);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!
    Saves the incoming secure connection value into database if user has changed the value.
*/
void NmIpsSettingsHelper::incomingSecureConnectionItemChange(int index)
{
    if (mRadioButtonPreviousIndex != index) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        switch (index) {
            case IpsServices::EMailStartTls: // On (Start TLS)
                mSettingsManager.writeSetting(IpsServices::IncomingSecureSockets, true);
                mSettingsManager.writeSetting(IpsServices::IncomingSSLWrapper, false);
                break;

            case IpsServices::EMailSslTls: // On (SSL/TLS)
                mSettingsManager.writeSetting(IpsServices::IncomingSecureSockets, false);
                mSettingsManager.writeSetting(IpsServices::IncomingSSLWrapper, true);
                break;

            case IpsServices::EMailSecurityOff: // Off
                mSettingsManager.writeSetting(IpsServices::IncomingSecureSockets, false);
                mSettingsManager.writeSetting(IpsServices::IncomingSSLWrapper, false);
                break;

        	 default:
                break;
        }
        // Update incoming port value only if default incoming port used
        HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingPort);
        QVariant data = item->contentWidgetData(QString("selected"));
        // Default incoming port selected
        if (data.toInt() == IpsServices::NmIpsSettingsDefault) {
			int port = mSettingsManager.determineDefaultIncomingPort();
			mSettingsManager.writeSetting(IpsServices::IncomingPort, port);
        }
    }
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::incomingSecureConnectionPress(const QModelIndex &index)
{
	Q_UNUSED(index);
	HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingSecureSockets);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!
    Used for getting the index to display in the secure connection radio button list 
    
    \return index Used to set the selected value
*/
int NmIpsSettingsHelper::getCorrectSecureRadioButtonIndex(QVariant secureSockets, 
                                                          QVariant secureSSLWrapper)
{
    IpsServices::TIpsSetDataSecurityTypes securityType = IpsServices::EMailStartTls;
    // secureSockets == True
    if (secureSockets.toBool()) { 
        securityType = IpsServices::EMailStartTls;
    }
    // secureSockets == False & secureSSLWrapper == True
    else if (secureSSLWrapper.toBool()) {
        securityType = IpsServices::EMailSslTls;
    }
    // secureSockets == False & secureSSLWrapper == False
    else {
        securityType = IpsServices::EMailSecurityOff;
    }
    return securityType;
}

/*!
    Saves the folderpath value into database if user has changed the value.
*/
void NmIpsSettingsHelper::folderPathChange(int index)
{
    if (mRadioButtonPreviousIndex != index && index == IpsServices::NmIpsSettingsDefault) {
        // Empty string sets the folder path to default.
        mSettingsManager.writeSetting(IpsServices::FolderPath, QString(""));
    } else if (index == IpsServices::NmIpsSettingsUserDefined) {
        showFolderPathInputDialog();       
    }
}

/*!
    Show a input dialog for allowing the user to specify a folder path.
*/
void NmIpsSettingsHelper::showFolderPathInputDialog()
{   
    // User can open the dialog multiple times, so delete the old one first.
    delete mFolderPathInputDialog;
    mFolderPathInputDialog = 0;
    
    QVariant folderPath;
    mSettingsManager.readSetting(IpsServices::FolderPath, folderPath);
    
    mFolderPathInputDialog = new HbInputDialog();
    mFolderPathInputDialog->setInputMode(HbInputDialog::TextInput);
    mFolderPathInputDialog->setPromptText(hbTrId("txt_mailips_setlabel_folder_path_user_defined"));
    mFolderPathInputDialog->setValue(folderPath.toString());
    
    mFolderPathInputDialog->open(this, SLOT(handleUserDefinedFolderPathInput(HbAction *)));
}

/*!
   Handels the saving of the folder path new value.
*/
void NmIpsSettingsHelper::handleUserDefinedFolderPathInput(HbAction *action)
{
    HbDataFormModelItem *item = mContentItems.value(IpsServices::FolderPath);
    
    if (action == mFolderPathInputDialog->primaryAction()) { 
        QVariant newFolderPath = mFolderPathInputDialog->value();
        mSettingsManager.writeSetting(IpsServices::FolderPath, newFolderPath);
        //set selected index to default if user inputed empty string.
        if (newFolderPath.toString().isEmpty()) {
            item->setContentWidgetData(QString("selected"), IpsServices::NmIpsSettingsDefault);
        }
    } else {
        //set selected value back if user canceled.
        item->setContentWidgetData(QString("selected"), mRadioButtonPreviousIndex);
    }
}

/*!
    Sets the previous index value to indicate that user has edited the field.

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::folderPathPress(const QModelIndex &index)
{
    Q_UNUSED(index);
    HbDataFormModelItem *item = mContentItems.value(IpsServices::FolderPath);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!
    Handles weekdays modifications.
*/
void NmIpsSettingsHelper::receivingWeekdaysModified()
{
    HbDataFormModelItem *item = mContentItems.value(IpsServices::ReceptionWeekDays);
    QVariant widgetData = item->contentWidgetData(QString("selectedItems"));
    QList<QVariant> selectedIndexes = widgetData.value< QList<QVariant> >();
    QVariant itemData = item->data(HbDataFormModelItem::HbDataFormModelItem::DescriptionRole + 1);
    QList<QVariant> itemValues = itemData.value< QList<QVariant> >();

    int days(0);
    foreach (QVariant selectedIndex, selectedIndexes) {
        days |= itemValues.at(selectedIndex.toInt()).toInt();
    }

    handleReceivingScheduleSettingChange(IpsServices::ReceptionWeekDays, days);

    // Every weekday selected.
    if (days == 0x7f) {
        item->setContentWidgetData(QString("text"),
            hbTrId("txt_mailips_setlabel_download_images_val_every_day"));
    }
}

/*!
    Handles start time modifications.
	
    \param time Modified start time.
*/
void NmIpsSettingsHelper::startTimeModified(QTime time)
{
    int startTime(time.hour()*60 + time.minute());
    handleReceivingScheduleSettingChange(IpsServices::ReceptionDayStartTime, startTime);
}

/*!
    Handles refresh period modifications.

    \param index selected item index.
*/
void NmIpsSettingsHelper::refreshPeriodModified(int index)
{
    HbDataFormModelItem* item = mContentItems.value(IpsServices::ReceptionRefreshPeriodDayTime);
    QVariant itemData = item->data(HbDataFormModelItem::HbDataFormModelItem::DescriptionRole + 1);
    int selectedValue = itemData.value< QList< QVariant > >().at(index).toInt();
    handleReceivingScheduleSettingChange(IpsServices::ReceptionRefreshPeriodDayTime, selectedValue);
}

/*!
    Handles end time modifications.

    \param time Modified start time.
*/
void NmIpsSettingsHelper::endTimeModified(QTime time)
{
    int endTime(time.hour()*60 + time.minute());
    handleReceivingScheduleSettingChange(IpsServices::ReceptionDayEndTime, endTime);
}

/*!
    Used for getting the index to display in the inbox path radio button list 
    
    \return index Used to set the selected value
*/
int NmIpsSettingsHelper::getCorrectInboxPathRadioButtonIndex(QVariant folderPath)
{
    int index(0);
    if (folderPath.toString().isEmpty()) {
        index = IpsServices::NmIpsSettingsDefault;
    } else {
        index = IpsServices::NmIpsSettingsUserDefined;
    }
    return index;
}

/*!
    Handles receiving schedule item value modifications.
    Takes care of creating 'user defined' mode, coping values from active profile to
    user defined mode, storing changed value and selecting 'user defined' mode.

    \param settingItem Changed setting item.
    \param settingValue Setting item's value.
*/
void NmIpsSettingsHelper::handleReceivingScheduleSettingChange(
    IpsServices::SettingItem settingItem, const QVariant &settingValue)
{
    // Check what was currently active sync mode.
    QVariant setting;
    mSettingsManager.readSetting(IpsServices::ReceptionActiveProfile, setting);
    int activeProfile(setting.toInt());
    if (activeProfile != NmIpsSettingsReceivingScheduleUserDefinedProfile) {
        // Create user defined mode if needed.
        emit createUserDefinedMode();

        // Copy all settings from currently active mode to user defined mode
        copyReceivingScheduleSettingsFromActiveProfile(
            NmIpsSettingsReceivingScheduleUserDefinedProfile);

        // select 'user defined' mode
        mSettingsManager.writeSetting(IpsServices::ReceptionActiveProfile,
            NmIpsSettingsReceivingScheduleUserDefinedProfile);
    }

    // store selected setting
    mSettingsManager.writeSetting(settingItem, settingValue);

    // update selection
    HbDataFormModelItem *syncProfile = contentItem(IpsServices::ReceptionActiveProfile);
    syncProfile->setContentWidgetData(QString("currentIndex"), 3);
}

/*!
    Copies receiving schedule settings from currently active profile to given profile.

    \param profileMode Mode where receiving schedule settings from active profile are copied to.
*/
void NmIpsSettingsHelper::copyReceivingScheduleSettingsFromActiveProfile(int profileMode)
{
    // Read receiving schedule dynamic group item values and
    // make a decision based on those if item should be visible or not.
    const int dynamicGroupItemsCount(
        sizeof(NmIpsSettingsReceivingSchedule) / sizeof(NmIpsSettingsReceivingSchedule[0]));

    NmIpsExtendedSettingsManager &extendedSettingsManager =
        mSettingsManager.extendedSettingsManager();

    for (int itemIndex(0); itemIndex < dynamicGroupItemsCount; ++itemIndex) {
        QVariant setting;
        // Read setting value from active profile
        IpsServices::SettingItem item(NmIpsSettingsReceivingSchedule[itemIndex]);
        mSettingsManager.readSetting(item, setting);

        // write settings to user defined profile.
        extendedSettingsManager.writeSetting(profileMode, item, setting);
    }
}
/*!
    Saves the outgoing port value into database if user has changed the value. If the user wish to
    define the port, a input dialog is shown.

    \param index Used to determine if the default value or a user defined value should be written
*/
void NmIpsSettingsHelper::outgoingPortChange(int index)
{
    if (mRadioButtonPreviousIndex != index && index == IpsServices::NmIpsSettingsDefault) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        int port = mSettingsManager.determineDefaultOutgoingPort();
        mSettingsManager.writeSetting(IpsServices::OutgoingPort, port);
    } else if (index == IpsServices::NmIpsSettingsUserDefined) {
        showOutgoingPortInputDialog();
    }
}

/*!
    Show a input dialog for allowing the user to specify a outgoing port.

*/
void NmIpsSettingsHelper::showOutgoingPortInputDialog()
{
    // User can open the dialog multiple times, so delete the old ones first.
    delete mOutgoingPortInputDialog;
    mOutgoingPortInputDialog = 0;
    delete mOutgoingPortInputValidator;
    mOutgoingPortInputValidator = 0;
    
    mOutgoingPortInputDialog = new HbInputDialog();
    mOutgoingPortInputDialog->setInputMode(HbInputDialog::IntInput);
    QVariant currentPort;
    mSettingsManager.readSetting(IpsServices::OutgoingPort, currentPort);
    mOutgoingPortInputValidator = new HbValidator();
    mOutgoingPortInputValidator->addField(new QIntValidator(0, 65535, 0), 
                        HbStringUtil::convertDigits(QString::number(currentPort.toInt())));
    mOutgoingPortInputDialog->setValidator(mOutgoingPortInputValidator);
    mOutgoingPortInputDialog->setPromptText(
        hbTrId("txt_mailips_setlabel_incoming_port_user_defined"));
    
    mOutgoingPortInputDialog->open(this, SLOT(handleUserDefinedOutgoingPortInput(HbAction *)));
}

/*!
    Handels the saving of the port new value.
*/
void NmIpsSettingsHelper::handleUserDefinedOutgoingPortInput(HbAction *action)
{
    if (action == mOutgoingPortInputDialog->primaryAction()) {
        QVariant newPort = mOutgoingPortInputDialog->value();
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        mSettingsManager.writeSetting(IpsServices::OutgoingPort, newPort);
    } else {
        //set selected value back if user canceled.
        HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingPort);
        item->setContentWidgetData(QString("selected"), mRadioButtonPreviousIndex);
    }
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param index Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::outgoingPortPress(const QModelIndex &index)
{
    Q_UNUSED(index);
    HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingPort);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!
    Used for getting the index to display in the outgoing port radio button list

    \return index Used to set the selected value
*/
int NmIpsSettingsHelper::getCorrectOutgoingPortRadioButtonIndex(int currentPort)
{
    int index = 0;
    int port = mSettingsManager.determineDefaultOutgoingPort();
    if (port == currentPort) {
        index = IpsServices::NmIpsSettingsDefault;
    } else {
        index = IpsServices::NmIpsSettingsUserDefined;
    }
    return index;
}

/*!
    Used for getting the index to display in the outgoing authentication radio button list 
    
    \return index Used to set the selected value
*/
int NmIpsSettingsHelper::getCorrectOutgoingAuthenticationRadioButtonIndex()
{
    int index(IpsServices::EMailAuthNone);
    QVariant temp;
    bool outgoingAuthentication;
    QString outgoingLoginName;
    QString outgoingPassword;
    QString incomingLoginName;
    QString incomingPassword;
    mSettingsManager.readSetting(IpsServices::SMTPAuthentication, temp);
    outgoingAuthentication = temp.toBool();
    mSettingsManager.readSetting(IpsServices::OutgoingLoginName, temp);
    outgoingLoginName = temp.toString();
    mSettingsManager.readSetting(IpsServices::OutgoingPassword, temp);
    outgoingPassword = temp.toString();
    mSettingsManager.readSetting(IpsServices::IncomingLoginName, temp);
    incomingLoginName = temp.toString();
    mSettingsManager.readSetting(IpsServices::IncomingPassword, temp);
    incomingPassword = temp.toString();
    
    if (outgoingAuthentication) {
        if (outgoingLoginName == incomingLoginName && outgoingPassword == incomingPassword) {
            index = IpsServices::EMailAuthSameAsIncoming;    
        } else {
            index = IpsServices::EMailAuthUserAuthentication;
        }
    }
    return index;
}

/*!
    Saves the outgoing secure connection value into database if user has changed the value.
*/
void NmIpsSettingsHelper::outgoingSecureConnectionItemChange(int index)
{
    if (mRadioButtonPreviousIndex != index) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        switch (index) {
            case IpsServices::EMailStartTls: // On (Start TLS)
                mSettingsManager.writeSetting(IpsServices::OutgoingSecureSockets, true);
                mSettingsManager.writeSetting(IpsServices::OutgoingSSLWrapper, false);
                break;

            case IpsServices::EMailSslTls: // On (SSL/TLS)
                mSettingsManager.writeSetting(IpsServices::OutgoingSecureSockets, false);
                mSettingsManager.writeSetting(IpsServices::OutgoingSSLWrapper, true);
                break;

            case IpsServices::EMailSecurityOff: // Off
                mSettingsManager.writeSetting(IpsServices::OutgoingSecureSockets, false);
                mSettingsManager.writeSetting(IpsServices::OutgoingSSLWrapper, false);
                break;

        	 default:
                break;
        }
        // Update outgoing port value only if default outgoing port used
        HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingPort);
        QVariant data = item->contentWidgetData(QString("selected"));
        // Default outgoing port selected
        if (data.toInt() == IpsServices::NmIpsSettingsDefault) {
			int port = mSettingsManager.determineDefaultOutgoingPort();
			mSettingsManager.writeSetting(IpsServices::OutgoingPort, port);
        }
    }
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::outgoingSecureConnectionPress(const QModelIndex &index)
{
	Q_UNUSED(index);
	HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingSecureSockets);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!
    Saves the outgoing authetication value into database if user has changed the value and
    updates dynamic group items.
*/
void NmIpsSettingsHelper::outgoingAuthenticationChange(int index)
{
    if (mRadioButtonPreviousIndex != index) {
        QVariant loginName;
        QVariant loginPassword;
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        switch (index) {
            case IpsServices::EMailAuthNone: // No authentication
                deleteServerInfoGroupDynamicItems();
                mSettingsManager.writeSetting(IpsServices::SMTPAuthentication, false);
                break;

            case IpsServices::EMailAuthSameAsIncoming: // Same as Incoming
                deleteServerInfoGroupDynamicItems();
                mSettingsManager.writeSetting(IpsServices::SMTPAuthentication, true);
              
                mSettingsManager.readSetting(IpsServices::IncomingLoginName, loginName);
                mSettingsManager.writeSetting(IpsServices::OutgoingLoginName, loginName);
                
                mSettingsManager.readSetting(IpsServices::IncomingPassword, loginPassword);
                mSettingsManager.writeSetting(IpsServices::OutgoingPassword, loginPassword);
                break;

            case IpsServices::EMailAuthUserAuthentication: // User authentication
                mSettingsManager.writeSetting(IpsServices::SMTPAuthentication, true);
                createServerInfoGroupDynamicItems();
                break;
                
             default:
                break;
        }
    }
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::outgoingAuthenticationPress(const QModelIndex &index)
{
    Q_UNUSED(index);
    HbDataFormModelItem *item = mContentItems.value(IpsServices::SMTPAuthentication);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}
