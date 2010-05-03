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

#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hblineedit.h>
#include <cpsettingformitemdata.h>
#include <qplugin.h>
#include <QCoreApplication>
#include <QTranslator>
#include <QScopedPointer>
#include <QLocale>
#include <HbStringUtil>

#include "nmipssettingsplugin.h"
#include "nmipssettingshelper.h"
#include "nmipssettingsmanagerbase.h"
#include "nmipssettingsmanagerfactory.h"
#include "nmipssettingitems.h"
#include "nmcommon.h"

#include "nmipssettingscustomitem.h"
#include "nmipssettingslabeledcombobox.h"

const QString NmIpsSettingsComboItems("comboItems");
const QString NmIpsSettingsLabelTexts("labelTexts");
const QString NmIpsSettingsItems("items");

/*!
    \class NmIpsSettingsPlugin
    \brief The class implements NmSettingsPluginInterface which is an interface
           for NMail settings plug-ins.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsPlugin.
*/
NmIpsSettingsPlugin::NmIpsSettingsPlugin()
: mSettingsHelper(0),
  mSettingsManager(0),
  mTranslator(0)
{
    QScopedPointer<QTranslator> translator(new QTranslator());

#ifdef Q_OS_SYMBIAN
    QString lang = QLocale::system().name();
    QString appName = "mailips_";
    QString path = "Z:/resource/qt/translations/";
#else
    QString lang;
    QString appName = "mailips";
    QString path = ":/translations";
#endif

    translator->load(appName + lang, path);

    QCoreApplication::installTranslator(translator.data());
    mTranslator = translator.take();

}

/*!
    Destructor of NmIpsSettingsPlugin.
*/
NmIpsSettingsPlugin::~NmIpsSettingsPlugin()
{
    delete mSettingsManager;
    delete mSettingsHelper;
    delete mTranslator;
}

/*!
    Populates the form items into model.
    \param model Reference to the model.
    \param form Reference to the form.
    \param mailboxId Id of the mailbox.
    \return <code>true</code> if plug-in contains certain mailbox otherwise <code>false</code>
*/
bool NmIpsSettingsPlugin::populateModel(HbDataFormModel &model,
    HbDataForm &form, const NmId &mailboxId)
{
    // Store model and form pointers.
    mModel = &model;
    mForm = &form;

    // populateModel is called each time when a new settings view is created and this
    // plugin is destructed only after the mail settings is exited, so
    // SettingsManager and SettingsHelper needs to be deleted.
    delete mSettingsManager;
    delete mSettingsHelper;
    mSettingsHelper = 0;
    mSettingsManager = NmIpsSettingsManagerFactory::createSettingManager(mailboxId);

    bool result(false);

    // This plugin is only used when the mailbox is a IMAP or POP3 account.
    // Settings manager object is valid if the mailboxId is IMAP or POP3 account.
    if (mSettingsManager) {
        // Create settings helper.
        mSettingsHelper = new NmIpsSettingsHelper(*mSettingsManager, form, model);

        connect(mSettingsHelper,
                SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                this, SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

        connect(mSettingsHelper, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                this, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

        connect(mSettingsHelper, SIGNAL(goOffline(const NmId &)),
                this, SIGNAL(goOffline(const NmId &)));

        connect(mSettingsHelper, SIGNAL(createUserDefinedMode()),
                this, SLOT(createUserDefinedMode()));

        // Add items to the model.
        initGroupItems();
        result = true;
    }
    
    return result;
}

/*!
    This slot can be used in cases when plugin needs to handle stuff before settingsview is closed.
*/
void NmIpsSettingsPlugin::aboutToClose()
{
	if (mSettingsHelper->isOffline()) {
		emit goOnline(mSettingsManager->mailboxId());
	}
}

/*!
    Appends the group items to the group item.
    \param model Reference to the model.
    \param form Reference to the form.
*/
void NmIpsSettingsPlugin::initGroupItems()
{
    HbDataFormModelItem *rootItem = mModel->invisibleRootItem();

    HbDataFormModelItem *preferencesItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_preferences"),
                                   rootItem);
    initPreferenceItems(*preferencesItem);

    HbDataFormModelItem *receivingScheduleItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_receiving_schedule"),
                                   rootItem);
    mSettingsHelper->setReceivingScheduleGroupItem(receivingScheduleItem);
    initReceivingScheduleItems(*receivingScheduleItem);

    HbDataFormModelItem *userInfoItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_user_info"),
                                   rootItem);
    initUserInfoItems(*userInfoItem);

    HbDataFormModelItem *serverInfoItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_server_info"),
                                   rootItem);
    initServerInfoItems(*serverInfoItem);

    // Create the delete mailbox button.
    HbDataFormModelItem::DataItemType buttonItem =
        static_cast<HbDataFormModelItem::DataItemType>(
            HbDataFormModelItem::CustomItemBase + 2);

    HbDataFormModelItem *deleteMailboxButtonItem =
        mModel->appendDataFormItem(buttonItem, QString(), rootItem);
    deleteMailboxButtonItem->setContentWidgetData(QString("text"),
                                                  hbTrId("txt_mailips_button_delete_mailbox"));

    mForm->addConnection(deleteMailboxButtonItem, SIGNAL(clicked()),
                         mSettingsHelper, SLOT(deleteButtonPress()));

}

/*!
    Appends the preferences items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initPreferenceItems(HbDataFormModelItem &item) const
{
    // My Name
    QVariant myName;
    mSettingsManager->readSetting(IpsServices::EmailAlias, myName);
    CpSettingFormItemData *myNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_my_name"));
    mSettingsHelper->insertContentItem(IpsServices::EmailAlias, myNameItem);
    myNameItem->setContentWidgetData(QString("text"), myName);
    mForm->addConnection(myNameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveMyName()));
    mForm->addConnection(myNameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(myNameTextChange(QString)));
    item.appendChild(myNameItem);

    // Mailbox Name
    QVariant mailboxName;
    mSettingsManager->readSetting(IpsServices::MailboxName, mailboxName);
    CpSettingFormItemData *mailboxNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_mailbox_name"));
    mSettingsHelper->insertContentItem(IpsServices::MailboxName, mailboxNameItem);
    mailboxNameItem->setContentWidgetData(QString("text"), mailboxName);
    mForm->addConnection(mailboxNameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveMailboxName()));
    mForm->addConnection(mailboxNameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(mailboxNameTextChange(QString)));
    item.appendChild(mailboxNameItem);
}

/*!
    Appends the receiving schedule items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initReceivingScheduleItems(HbDataFormModelItem &item)
{
    // Active sync profile

    // Append custom item to the prototype list.
    NmIpsSettingsCustomItem *prototype = new NmIpsSettingsCustomItem();
    QList<HbAbstractViewItem *> protoTypeList = mForm->itemPrototypes();
    protoTypeList.append(prototype);
    mForm->setItemPrototypes(protoTypeList);

    // 1. Active sync profile
    HbDataFormModelItem *infoItem = new HbDataFormModelItem(
        static_cast<HbDataFormModelItem::DataItemType>(NmIpsSettingsCustomItem::LabeledComboBox),
        hbTrId("txt_mailips_setlabel_selected_mode"));

    mSettingsHelper->insertContentItem(IpsServices::ReceptionActiveProfile, infoItem);
    item.appendChild(infoItem);

    QStringList modeList;
    modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_keep_upto")          // 0
             << hbTrId("txt_mailips_setlabel_selected_mode_val_save_energy")        // 1
             << hbTrId("txt_mailips_setlabel_selected_mode_val_fetch_manua");       // 2

    QVariant userDefineMode;
    mSettingsManager->readSetting(IpsServices::ReceptionUserDefinedProfile, userDefineMode);


    // If 'user defined' mode is defined then add it to combo items.
    if (userDefineMode.toInt()) {
        modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_user_define");   // 3
    }
    infoItem->setContentWidgetData(NmIpsSettingsComboItems, modeList);

    QStringList infoList;
    infoList << hbTrId("txt_mailips_list_the_mailbox_is_uptodate_during")           // 0
             << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_every_15")        // 1
             << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_only_by");        // 2

    // If 'user defined' mode is defined then add 'user defined' explanation to label list.
    if (userDefineMode.toInt()) {
        infoList << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_as_defin");   // 3
    }
    infoItem->setContentWidgetData(NmIpsSettingsLabelTexts, infoList);

    QVariant profileIndex;
    mSettingsManager->readSetting(IpsServices::ReceptionActiveProfile, profileIndex);
    infoItem->setContentWidgetData(QString("currentIndex"), profileIndex);

    // 2. Show mail in inbox
    QVariant mailInInbox;
    mSettingsManager->readSetting(IpsServices::ReceptionInboxSyncWindow, mailInInbox);

    CpSettingFormItemData *showMailInInboxItem =
        new CpSettingFormItemData(HbDataFormModelItem::RadioButtonListItem, hbTrId(
            "txt_mailips_setlabel_show_mail_in_inbox"));

    mSettingsHelper->insertContentItem(IpsServices::ReceptionInboxSyncWindow, showMailInInboxItem);

    QStringList showMailItems;
    showMailItems << HbStringUtil::convertDigits("50")
                  << HbStringUtil::convertDigits("100")
                  << HbStringUtil::convertDigits("500")
                  << hbTrId("txt_mailips_setlabel_val_all");
    

    QList<QVariant> showMailItemValues;
    showMailItemValues << 50
                       << 100
                       << 500
                       << 0;
    
    QVariant value(showMailItemValues);
    showMailInInboxItem->setData(HbDataFormModelItem::DescriptionRole + 1, value);
    showMailInInboxItem->setContentWidgetData(NmIpsSettingsItems, showMailItems);
    showMailInInboxItem->setEnabled(true);
    item.appendChild(showMailInInboxItem);

    // Active sync profile connection
    mForm->addConnection(infoItem, SIGNAL(currentIndexChanged(int)),
        mSettingsHelper, SLOT(receivingScheduleChange(int)));

    mForm->addConnection(showMailInInboxItem, SIGNAL(itemSelected(int)),
        this, SLOT(showMailInInboxModified(int)));

    // Must be called manually here, because the signal->slot connection set above using
    // HbDataForm::addConnection() is actually established AFTER the properties have first been
    // set to the widget, causing the first currentIndexChanged signal not to reach
    // NmIpsSettingsHelper::receivingScheduleChange().
    mSettingsHelper->receivingScheduleChange(profileIndex.toInt());

}


/*!
    Appends the user info items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initUserInfoItems(HbDataFormModelItem &item) const
{
    // Mail address
    QVariant mailAddress;
    mSettingsManager->readSetting(IpsServices::EmailAddress, mailAddress);
    CpSettingFormItemData *mailAddressItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_mail_address"));
    mSettingsHelper->insertContentItem(IpsServices::EmailAddress, mailAddressItem);
    mailAddressItem->setContentWidgetData(QString("text"), mailAddress);
    mForm->addConnection(mailAddressItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveMailAddress()));
    mForm->addConnection(mailAddressItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(mailAddressTextChange(QString)));
    item.appendChild(mailAddressItem);

    // Username (Never visible in Yahoo!)
    QVariant username;
    mSettingsManager->readSetting(IpsServices::IncomingLoginName, username);
    CpSettingFormItemData *usernameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_username"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingLoginName, usernameItem);
    usernameItem->setContentWidgetData(QString("text"), username);
    mForm->addConnection(usernameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveUserName()));
    mForm->addConnection(usernameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(userNameTextChange(QString)));
    item.appendChild(usernameItem);

    // Password
    QVariant password;
    mSettingsManager->readSetting(IpsServices::IncomingPassword, password);
    CpSettingFormItemData *passwordItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_password"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingPassword, passwordItem);
    passwordItem->setContentWidgetData(QString("text"), password);
    passwordItem->setContentWidgetData(QString("echoMode"), HbLineEdit::PasswordEchoOnEdit);
    mForm->addConnection(passwordItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(savePassword()));
    item.appendChild(passwordItem);

    // Reply to address
    QVariant replyToAddress;
    mSettingsManager->readSetting(IpsServices::ReplyAddress, replyToAddress);
    CpSettingFormItemData *replyToItem = new CpSettingFormItemData(
       HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_reply_to_address"));
    mSettingsHelper->insertContentItem(IpsServices::ReplyAddress, replyToItem);
    replyToItem->setContentWidgetData(QString("text"), replyToAddress);
    mForm->addConnection(replyToItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveReplyTo()));
    mForm->addConnection(replyToItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(replyToTextChange(QString)));
    item.appendChild(replyToItem);
}

/*!
    Appends the server info items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initServerInfoItems(HbDataFormModelItem &item) const
{
    // 1. Incoming mail server address
    QVariant incomingMailServer;
    mSettingsManager->readSetting(IpsServices::IncomingMailServer, incomingMailServer);
    CpSettingFormItemData *incomingMailServerItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_incoming_mail_server"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingMailServer, incomingMailServerItem);
    incomingMailServerItem->setContentWidgetData(QString("text"), incomingMailServer);
    mForm->addConnection(incomingMailServerItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveIncomingMailServer()));
    mForm->addConnection(incomingMailServerItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(incomingMailServerTextChange(QString)));
    item.appendChild(incomingMailServerItem);
    
    // 2. Incoming Secure connection
    QVariant secureSockets;
    QVariant secureSSLWrapper;
    mSettingsManager->readSetting(IpsServices::IncomingSecureSockets, secureSockets);
    mSettingsManager->readSetting(IpsServices::IncomingSSLWrapper, secureSSLWrapper);
    CpSettingFormItemData *incomingSecureConnectionItem =
        new CpSettingFormItemData(HbDataFormModelItem::RadioButtonListItem, hbTrId(
            "txt_mailips_setlabel_incoming_security"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingSecureSockets, incomingSecureConnectionItem);
    QStringList showSCItems;
    showSCItems << hbTrId("txt_mailips_setlabel_security_val_on_starttls")
                << hbTrId("txt_mailips_setlabel_security_val_on_ssltls")
                << hbTrId("txt_mailips_setlabel_security_val_off");

    incomingSecureConnectionItem->setContentWidgetData(QString("items"), showSCItems);
    int incomingSecureConnectionItemIndex = 
        mSettingsHelper->getCorrectSecureRadioButtonIndex(secureSockets, secureSSLWrapper);
    incomingSecureConnectionItem->setContentWidgetData(QString("selected"), 
                                                       incomingSecureConnectionItemIndex);
    mForm->addConnection(incomingSecureConnectionItem, SIGNAL(itemSelected(int)),    		  
    		mSettingsHelper, SLOT(incomingSecureConnectionItemChange(int)));
    mForm->addConnection(incomingSecureConnectionItem, SIGNAL(pressed(const QModelIndex &)),    		  
    		mSettingsHelper, SLOT(incomingSecureConnectionPressed(const QModelIndex &)));
    item.appendChild(incomingSecureConnectionItem);
    
    // 3. Incoming mail server port
    QVariant incomingPort;
    mSettingsManager->readSetting(IpsServices::IncomingPort, incomingPort);
    CpSettingFormItemData *incomingPortItem =
        new CpSettingFormItemData(HbDataFormModelItem::RadioButtonListItem, hbTrId(
            "txt_mailips_setlabel_incoming_port"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingPort, incomingPortItem);    
    mForm->addConnection(incomingPortItem, SIGNAL(itemSelected(int)),
                         mSettingsHelper, SLOT(incomingPortChange(int)));
    mForm->addConnection(incomingPortItem, SIGNAL(pressed(const QModelIndex &)),              
                         mSettingsHelper, SLOT(incomingPortPressed(const QModelIndex &)));
    QStringList incomingPortItems;
    incomingPortItems << hbTrId("txt_mailips_setlabel_incoming_port_default")
                      << hbTrId("txt_mailips_setlabel_incoming_port_user_defined");
    incomingPortItem->setContentWidgetData(QString("items"), incomingPortItems);
    int incomingPortItemIndex = 
        mSettingsHelper->getCorrectPortRadioButtonIndex(incomingPort.toInt()); 
    incomingPortItem->setContentWidgetData(QString("selected"), incomingPortItemIndex);
    item.appendChild(incomingPortItem);    
    
    // 4. Outgoing mail server address
    QVariant outgoingMailServer;
    mSettingsManager->readSetting(IpsServices::OutgoingMailServer, outgoingMailServer);
    CpSettingFormItemData *outgoingMailServerItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_outgoing_mail_server"));
    mSettingsHelper->insertContentItem(IpsServices::OutgoingMailServer, outgoingMailServerItem);
    outgoingMailServerItem->setContentWidgetData(QString("text"), outgoingMailServer);
    mForm->addConnection(outgoingMailServerItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveOutgoingMailServer()));
    mForm->addConnection(outgoingMailServerItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(outgoingMailServerTextChange(QString)));
    item.appendChild(outgoingMailServerItem);
    
    // 5. Outgoing Secure connection
    QVariant outgoingSecureSockets;
    QVariant outgoingSSLWrapper;
    mSettingsManager->readSetting(IpsServices::OutgoingSecureSockets, outgoingSecureSockets);
    mSettingsManager->readSetting(IpsServices::OutgoingSSLWrapper, outgoingSSLWrapper);
    CpSettingFormItemData *outgoingSecureConnectionItem =
        new CpSettingFormItemData(HbDataFormModelItem::RadioButtonListItem, hbTrId(
            "txt_mailips_setlabel_outgoing_security"));
    mSettingsHelper->insertContentItem(IpsServices::OutgoingSecureSockets, outgoingSecureConnectionItem);
    QStringList outgoingShowSCItems;
    outgoingShowSCItems << hbTrId("txt_mailips_setlabel_security_val_on_starttls")
                << hbTrId("txt_mailips_setlabel_security_val_on_ssltls")
                << hbTrId("txt_mailips_setlabel_security_val_off");

    outgoingSecureConnectionItem->setContentWidgetData(QString("items"), outgoingShowSCItems);
    int outgoingSecureConnectionItemIndex = 
        mSettingsHelper->getCorrectSecureRadioButtonIndex(outgoingSecureSockets, 
														  outgoingSSLWrapper);
    outgoingSecureConnectionItem->setContentWidgetData(QString("selected"), 
                                                       outgoingSecureConnectionItemIndex);
    mForm->addConnection(outgoingSecureConnectionItem, SIGNAL(itemSelected(int)),    		  
    		mSettingsHelper, SLOT(outgoingSecureConnectionItemChange(int)));
    mForm->addConnection(outgoingSecureConnectionItem, SIGNAL(pressed(const QModelIndex &)),    		  
    		mSettingsHelper, SLOT(outgoingSecureConnectionPressed(const QModelIndex &)));
    item.appendChild(outgoingSecureConnectionItem);

    // 6. Outgoing mail server port
    QVariant outgoingPort;
    mSettingsManager->readSetting(IpsServices::OutgoingPort, outgoingPort);
    CpSettingFormItemData *outgoingPortItem =
        new CpSettingFormItemData(HbDataFormModelItem::RadioButtonListItem, hbTrId(
            "txt_mailips_setlabel_outgoing_port"));
    mSettingsHelper->insertContentItem(IpsServices::OutgoingPort, outgoingPortItem);    
    mForm->addConnection(outgoingPortItem, SIGNAL(itemSelected(int)),
                         mSettingsHelper, SLOT(outgoingPortChange(int)));
    mForm->addConnection(outgoingPortItem, SIGNAL(pressed(const QModelIndex &)),              
                         mSettingsHelper, SLOT(outgoingPortPressed(const QModelIndex &)));
    QStringList outgoingPortItems;
    outgoingPortItems << hbTrId("txt_mailips_setlabel_incoming_port_default")
                      << hbTrId("txt_mailips_setlabel_incoming_port_user_defined");
    outgoingPortItem->setContentWidgetData(QString("items"), outgoingPortItems);
    int outgoingPortItemIndex = 
        mSettingsHelper->getCorrectOutgoingPortRadioButtonIndex(outgoingPort.toInt()); 
    outgoingPortItem->setContentWidgetData(QString("selected"), outgoingPortItemIndex);
    item.appendChild(outgoingPortItem);
    
    // 8. Folder path
    // This item is only shown for IMAP4 account.
    if (mSettingsManager->accountType() == IpsServices::EMailImap) {
        QVariant folderPath;
        mSettingsManager->readSetting(IpsServices::FolderPath, folderPath);
        CpSettingFormItemData *folderPathItem =
            new CpSettingFormItemData(HbDataFormModelItem::RadioButtonListItem, hbTrId(
                "txt_mailips_setlabel_folder_path"));
        mSettingsHelper->insertContentItem(IpsServices::FolderPath, folderPathItem);    
        mForm->addConnection(folderPathItem, SIGNAL(itemSelected(int)),
                             mSettingsHelper, SLOT(inboxPathChange(int)));
        mForm->addConnection(folderPathItem, SIGNAL(pressed(const QModelIndex &)),              
                             mSettingsHelper, SLOT(inboxPathPressed(const QModelIndex &)));
        QStringList folderPathItems;
        folderPathItems << hbTrId("txt_mailips_setlabel_folder_path_val_default")
                       << hbTrId("txt_mailips_setlabel_folder_path_user_defined");
        folderPathItem->setContentWidgetData(QString("items"), folderPathItems);
        int folderPathItemIndex = mSettingsHelper->getCorrectInboxPathRadioButtonIndex(folderPath);
        folderPathItem->setContentWidgetData(QString("selected"), folderPathItemIndex);
        item.appendChild(folderPathItem);
    }
}

/*!
    Creates user defined mode if not already exist.
*/
void NmIpsSettingsPlugin::createUserDefinedMode()
{
    QVariant userDefineMode;
    mSettingsManager->readSetting(IpsServices::ReceptionUserDefinedProfile, userDefineMode);

    // If user defined mode do not already exist, create it
    if (!userDefineMode.toInt()) {
        // Add 'user defined' mode to combobox
        HbDataFormModelItem *syncProfile =
            mSettingsHelper->contentItem(IpsServices::ReceptionActiveProfile);

        QVariant contentWidgetData = syncProfile->contentWidgetData(NmIpsSettingsComboItems);
        QStringList modeList = contentWidgetData.value<QStringList>();
        modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_user_define");

        // Add 'user defined' explanation text to label text
        contentWidgetData = syncProfile->contentWidgetData(NmIpsSettingsLabelTexts);
        QStringList infoList = contentWidgetData.value<QStringList>();
        infoList << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_as_defin");

        // disconnect
        mForm->removeConnection(syncProfile, SIGNAL(currentIndexChanged(int)),
            mSettingsHelper, SLOT(receivingScheduleChange(int)));

        syncProfile->setContentWidgetData(NmIpsSettingsComboItems, modeList);
        syncProfile->setContentWidgetData(NmIpsSettingsLabelTexts, infoList);

        // reconnect
        mForm->addConnection(syncProfile, SIGNAL(currentIndexChanged(int)),
            mSettingsHelper, SLOT(receivingScheduleChange(int)));

        // Mark that user defined mode exists
        userDefineMode.setValue(1);
        mSettingsManager->writeSetting(IpsServices::ReceptionUserDefinedProfile, userDefineMode);
    }
}

/*!

*/
void NmIpsSettingsPlugin::showMailInInboxModified(int index)
{
    HbDataFormModelItem* item = mSettingsHelper->contentItem(IpsServices::ReceptionInboxSyncWindow);
    QVariant itemData = item->data(HbDataFormModelItem::HbDataFormModelItem::DescriptionRole + 1);
    int selectedValue = itemData.value< QList< QVariant > >().at(index).toInt();
    mSettingsHelper->handleReceivingScheduleSettingChange(
        IpsServices::ReceptionInboxSyncWindow, selectedValue);
}

Q_EXPORT_PLUGIN2(nmipssettings, NmIpsSettingsPlugin);
