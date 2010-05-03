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
  mDynamicItemsVisible(false),
  mRadioButtonPreviousIndex(0)
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
    HbDataFormModelItem *value)
{
    mContentItems.insert(key, value);
}

/*!

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
                        new CpSettingFormItemData(HbDataFormModelItem::MultiselectionItem,
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
                                 << hbTrId("txt_mailips_setlabel_download_images_val_sun")
                                 << hbTrId("txt_mailips_setlabel_download_images_val_every_day");

                    QList<QVariant> weekdayItemValues;
                    weekdayItemValues << 0x01
                                      << 0x02
                                      << 0x04
                                      << 0x08
                                      << 0x10
                                      << 0x20
                                      << 0x40
                                      << 0x7F;

                    formItemData->setData(HbDataFormModelItem::HbDataFormModelItem::DescriptionRole + 1,
                        weekdayItemValues);
                    formItemData->setContentWidgetData(QString("items"), weekdayItems);
                    formItemData->setEnabled(false);
//                    mDataForm.addConnection(formItemData, SIGNAL(finished(HbAction *)),
//                        this, SLOT(receivingWeekdaysModified(HbAction *)));
                }

                // Update data
                QVariant value;
                mSettingsManager.readSetting(item, value);
                QList<QVariant> selectedDays;
                selectedDays.clear();
                int days(value.toInt());
                if (days == 0x7f) {
                   // Every weekday selected
                   selectedDays.append(7);
                } else {
                   // 0-6 Individual weekdays selected
                   for (int i=0; i<=6; ++i) {
                       if (days & (1 << i)) {
                           selectedDays.append(i);
                       }
                   }
                }
                formItemData->setContentWidgetData(QString("selectedItems"), selectedDays);
                break;
            }

            // 4. Day start time
            case IpsServices::ReceptionDayStartTime:
            {
                // If not exist, create one
                if (!formItemData) {
                    QString format(timeFormat());
                    formItemData = new CpSettingFormItemData(
                        static_cast< HbDataFormModelItem::DataItemType >(NmIpsSettingsCustomItem::TimeEditor),
                            hbTrId("txt_mailips_setlabel_day_start_time"));

                    insertContentItem(IpsServices::ReceptionDayStartTime, formItemData);
                    formItemData->setEnabled(true);
                    formItemData->setContentWidgetData(QString("displayFormat"), format);
                    mReceivingScheduleGroupItem->appendChild(formItemData);
                    mDataForm.addConnection(formItemData, SIGNAL(editingFinished()),
                        this, SLOT(startTimeModified()));

                }

                // Update data
                QVariant startTime;
                mSettingsManager.readSetting(item, startTime);
                QDateTime startDateTime(QDate(2010, 1, 1), QTime(startTime.toInt(),0));
                formItemData->setContentWidgetData("dateTime", startDateTime);
                break;
            }

            // 5. Day end time
            case IpsServices::ReceptionDayEndTime:
            {
                // If not exist, create one
                if (!formItemData) {
                    QString format(timeFormat());
                    formItemData = new CpSettingFormItemData(
                        static_cast<HbDataFormModelItem::DataItemType> (NmIpsSettingsCustomItem::TimeEditor),
                            hbTrId("txt_mailips_setlabel_day_end_time"));

                    insertContentItem(IpsServices::ReceptionDayEndTime, formItemData);
                    formItemData->setEnabled(true);
                    formItemData->setContentWidgetData(QString("displayFormat"), format);
                    mReceivingScheduleGroupItem->appendChild(formItemData);
                }

                // Update data
                QVariant endTime;
                mSettingsManager.readSetting(item, endTime);
                QDateTime endDateTime(QDate(2010, 1, 1), QTime(endTime.toInt(),0));
                formItemData->setContentWidgetData("dateTime", endDateTime);
                mDataForm.addConnection(formItemData, SIGNAL(editingFinished()),
                    this, SLOT(endTimeModified()));
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
    if (mCurrentLineEditChanged) {
        HbDataFormModelItem *item = mContentItems.value(IpsServices::MailboxName);
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
    Saves the Username value into database if user has changed the value.
*/
void NmIpsSettingsHelper::saveUserName()
{
    if (mCurrentLineEditChanged) {
		emit goOffline(mSettingsManager.mailboxId());
		mEmitOnline = true;
		HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingLoginName);
        QVariant data = item->contentWidgetData(QString("text"));
        mSettingsManager.writeSetting(IpsServices::IncomingLoginName, data);
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
	emit goOffline(mSettingsManager.mailboxId());
	mEmitOnline = true;
	HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingPassword);
    QVariant data = item->contentWidgetData(QString("text"));
    mSettingsManager.writeSetting(IpsServices::IncomingPassword, data);
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

        HbMessageBox infoDialog(HbMessageBox::MessageTypeInformation);
        infoDialog.setText(
            hbTrId("txt_mail_dpophead _1_deleted").arg(mailboxName.toString()));
        infoDialog.setModal(true);
        HbAction *action = infoDialog.exec();

        // Emit the signal to update the UI.
        emit mailboxListChanged(mSettingsManager.mailboxId(),
                                NmSettings::MailboxDeleted);
    } else {
        // Failed to delete the mailbox!
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
    Returns locale specific formatting string for hours and minutes.
    \return Time format string.
*/
QString NmIpsSettingsHelper::timeFormat()
{
    // Set the correct time format string based on locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    QString timeFormat("h:mm ap");
    if (HbExtendedLocale::Time24 == locale.timeStyle()) {
        timeFormat = QString("h:mm");
    }
    // Set the separator between hours and minutes.
    timeFormat.replace(QString(":"), locale.timeSeparator(1));

    return timeFormat;
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
        QVariant port; 
        mSettingsManager.readSetting(IpsServices::IncomingPort, port);
        bool changed = false;
        int changedPort = showIncomingPortInputDialog(port.toInt(), changed);       
        if (changed) {
            emit goOffline(mSettingsManager.mailboxId());
            mEmitOnline = true;
            mSettingsManager.writeSetting(IpsServices::IncomingPort, changedPort);
        } else {
            //set selected value back if user canceled.
            HbDataFormModelItem *item = mContentItems.value(IpsServices::IncomingPort);
            item->setContentWidgetData(QString("selected"), mRadioButtonPreviousIndex);
        }
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
    Show a input dialog for allowing the user to specify a incoming port 
    \param int The current port stored in the settings  
    \param bool Reference used for marking if the value really changed
    \return int The user defined port
*/
int NmIpsSettingsHelper::showIncomingPortInputDialog(int currentPort, bool &changed)
{    
    QVariant newPort;

    HbInputDialog inputDialog;
    inputDialog.setInputMode(HbInputDialog::IntInput);
    HbValidator *validator = new HbValidator();
    validator->addField(new QIntValidator(0, 65535, 0), 
                        HbStringUtil::convertDigits(QString::number(currentPort)));
    inputDialog.setValidator(validator);
    inputDialog.setPromptText(hbTrId("txt_mailips_setlabel_incoming_port_user_defined"));
    
    HbAction *action = inputDialog.exec();     
    if (action == inputDialog.primaryAction()) {
        newPort = inputDialog.value();
        changed = true;
    }  
    delete validator;   
    
    return newPort.toInt();
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::incomingPortPressed(const QModelIndex &index)
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
                mSettingsManager.writeSetting(IpsServices::IncomingSecureSockets, ETrue);
                mSettingsManager.writeSetting(IpsServices::IncomingSSLWrapper, EFalse);
                break;

            case IpsServices::EMailSslTls: // On (SSL/TLS)
                mSettingsManager.writeSetting(IpsServices::IncomingSecureSockets, EFalse);
                mSettingsManager.writeSetting(IpsServices::IncomingSSLWrapper, ETrue);
                break;

            case IpsServices::EMailSecurityOff: // Off
                mSettingsManager.writeSetting(IpsServices::IncomingSecureSockets, EFalse);
                mSettingsManager.writeSetting(IpsServices::IncomingSSLWrapper, EFalse);
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
void NmIpsSettingsHelper::incomingSecureConnectionPressed(const QModelIndex &index)
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
void NmIpsSettingsHelper::inboxPathChange(int index)
{
    if (mRadioButtonPreviousIndex != index && index == IpsServices::NmIpsSettingsDefault) {
        // Empty string sets the folder path to default.
        mSettingsManager.writeSetting(IpsServices::FolderPath, QString(""));
    } else if (index == IpsServices::NmIpsSettingsUserDefined) {
        bool changed = false;
        HbDataFormModelItem *item = mContentItems.value(IpsServices::FolderPath);
        QString changedPath = showFolderPathInputDialog(changed);       
        if (changed) {
            mSettingsManager.writeSetting(IpsServices::FolderPath, changedPath);
            //set selected index to default if user inputed empty string.
            if (changedPath.isEmpty()) {
                item->setContentWidgetData(QString("selected"), IpsServices::NmIpsSettingsDefault);
            }
        } else {
            //set selected value back if user canceled.
            item->setContentWidgetData(QString("selected"), mRadioButtonPreviousIndex);
        }
    }
}

/*!
    Show a input dialog for allowing the user to specify a folder path 
    \param bool Reference used for marking if the value really changed
    \return QString The user defined folder path
*/
QString NmIpsSettingsHelper::showFolderPathInputDialog(bool &changed)
{    
    QVariant folderPath;
    mSettingsManager.readSetting(IpsServices::FolderPath, folderPath);
    
    HbInputDialog inputDialog;
    inputDialog.setInputMode(HbInputDialog::TextInput);
    inputDialog.setPromptText(hbTrId("txt_mailips_setlabel_inbox_path_user_defined"));
    inputDialog.setValue(folderPath.toString());
    
    HbAction *action = inputDialog.exec();     
    if (action == inputDialog.primaryAction()) { 
        folderPath = inputDialog.value();
        changed = true;
    }

    return folderPath.toString();
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param text Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::inboxPathPressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    HbDataFormModelItem *item = mContentItems.value(IpsServices::FolderPath);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!

*/
void NmIpsSettingsHelper::receivingWeekdaysModified(HbAction *action)
{
    Q_UNUSED(action)
    HbDataFormModelItem *item = mContentItems.value(IpsServices::ReceptionWeekDays);
    QVariant widgetData = item->contentWidgetData(QString("selectedItems"));
    QList< QVariant > selectedItems = widgetData.value< QList< QVariant > >();

    foreach (QVariant selection, selectedItems) {
        int value = selection.toInt();
    }
}

/*!

*/
void NmIpsSettingsHelper::startTimeModified()
{
    HbDataFormModelItem *item = mContentItems.value(IpsServices::ReceptionDayStartTime);
    QVariant data = item->contentWidgetData(QString("dateTime"));
    QDateTime dateTime = data.value< QDateTime >();
    int startHour(dateTime.time().hour());
    handleReceivingScheduleSettingChange(IpsServices::ReceptionDayStartTime, startHour);
}

/*!

*/
void NmIpsSettingsHelper::refreshPeriodModified(int index)
{
    HbDataFormModelItem* item = mContentItems.value(IpsServices::ReceptionRefreshPeriodDayTime);
    QVariant itemData = item->data(HbDataFormModelItem::HbDataFormModelItem::DescriptionRole + 1);
    int selectedValue = itemData.value< QList< QVariant > >().at(index).toInt();
    handleReceivingScheduleSettingChange(IpsServices::ReceptionRefreshPeriodDayTime, selectedValue);
}

/*!

*/
void NmIpsSettingsHelper::endTimeModified()
{
    HbDataFormModelItem *item = mContentItems.value(IpsServices::ReceptionDayEndTime);
    QVariant data = item->contentWidgetData(QString("dateTime"));
    QDateTime dateTime = data.value< QDateTime >();
    int endHour(dateTime.time().hour());
    handleReceivingScheduleSettingChange(IpsServices::ReceptionDayEndTime, endHour);
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
        QVariant port;
        mSettingsManager.readSetting(IpsServices::OutgoingPort, port);
        bool changed = false;
        int changedPort = showOutgoingPortInputDialog(port.toInt(), changed);
        if (changed) {
            emit goOffline(mSettingsManager.mailboxId());
            mEmitOnline = true;
            mSettingsManager.writeSetting(IpsServices::OutgoingPort, changedPort);
        } else {
            //set selected value back if user canceled.
            HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingPort);
            item->setContentWidgetData(QString("selected"), mRadioButtonPreviousIndex);
        }
    }
}

/*!
    Sets the previous index value to indicate that user has edited the field

    \param index Reference to the item in the radio button list.
*/
void NmIpsSettingsHelper::outgoingPortPressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingPort);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

/*!
    Show a input dialog for allowing the user to specify a outgoing port
    \param int The current port stored in the settings
    \param bool Reference used for marking if the value really changed
    \return int The user defined port
*/
int NmIpsSettingsHelper::showOutgoingPortInputDialog(int currentPort, bool &changed)
{
    QVariant newPort;

    HbInputDialog inputDialog;
    inputDialog.setInputMode(HbInputDialog::IntInput);
    HbValidator *validator = new HbValidator();
    validator->addField(new QIntValidator(0, 65535, 0), 
                        HbStringUtil::convertDigits(QString::number(currentPort)));
    inputDialog.setValidator(validator);
    inputDialog.setPromptText(hbTrId("txt_mailips_setlabel_incoming_port_user_defined"));

    HbAction *action = inputDialog.exec();
    if (action == inputDialog.primaryAction()) {
        newPort = inputDialog.value();
        changed = true;
    }
    delete validator;

    return newPort.toInt();
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
    Saves the outgoing secure connection value into database if user has changed the value.
*/
void NmIpsSettingsHelper::outgoingSecureConnectionItemChange(int index)
{
    if (mRadioButtonPreviousIndex != index) {
        emit goOffline(mSettingsManager.mailboxId());
        mEmitOnline = true;
        switch (index) {
            case IpsServices::EMailStartTls: // On (Start TLS)
                mSettingsManager.writeSetting(IpsServices::OutgoingSecureSockets, ETrue);
                mSettingsManager.writeSetting(IpsServices::OutgoingSSLWrapper, EFalse);
                break;

            case IpsServices::EMailSslTls: // On (SSL/TLS)
                mSettingsManager.writeSetting(IpsServices::OutgoingSecureSockets, EFalse);
                mSettingsManager.writeSetting(IpsServices::OutgoingSSLWrapper, ETrue);
                break;

            case IpsServices::EMailSecurityOff: // Off
                mSettingsManager.writeSetting(IpsServices::OutgoingSecureSockets, EFalse);
                mSettingsManager.writeSetting(IpsServices::OutgoingSSLWrapper, EFalse);
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
void NmIpsSettingsHelper::outgoingSecureConnectionPressed(const QModelIndex &index)
{
	Q_UNUSED(index);
	HbDataFormModelItem *item = mContentItems.value(IpsServices::OutgoingSecureSockets);
    QVariant data = item->contentWidgetData(QString("selected"));
    mRadioButtonPreviousIndex = data.toInt();
}

