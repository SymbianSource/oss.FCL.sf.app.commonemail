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

#include "nmipssettingsplugin.h"
#include "nmipssettingshelper.h"
#include "nmipssettingsmanagerbase.h"
#include "nmipssettingsmanagerfactory.h"
#include "nmipssettingitems.h"
#include "nmcommon.h"

#include "nmipssettingscustomitem.h"
#include "nmipssettingslabeledcombobox.h"

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
        mSettingsHelper = new NmIpsSettingsHelper(*mSettingsManager);

        connect(mSettingsHelper, SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                this, SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

        connect(mSettingsHelper, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                this, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));
        // Add items to the model.
        initGroupItems();
        result = true;
    }
    return result;
}

/*!
    Slot called when individual setting items are shown.
    \param index. Index of the shown item.
*/
void NmIpsSettingsPlugin::itemShown(const QModelIndex& index)
{
    HbDataFormViewItem* item = static_cast<HbDataFormViewItem*>(mForm->itemByIndex(index));
    HbDataFormModelItem* modelItem = mModel->itemFromIndex(index);

    // Check item type
    if (modelItem == mReceivingScheduleSelectedItem) {

        // Populate the custom item's combobox and label data.
        NmIpsSettingsLabeledComboBox* box =
            static_cast<NmIpsSettingsLabeledComboBox*>(item->dataItemContentWidget());

        QStringList modeList;
        modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_keep_upto")      // 0
                 << hbTrId("txt_mailips_setlabel_selected_mode_val_save_energy")    // 1
                 << hbTrId("txt_mailips_setlabel_selected_mode_val_fetch_manua");   // 2

        QStringList infoList;
        infoList << hbTrId("txt_mailips_list_the_mailbox_is_uptodate_during")       // 0
                 << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_every_15")    // 1
                 << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_only_by");    // 2

        box->setItems(modeList, infoList);

        // Read and set the default value for the combobox and label
        QVariant value;
        if (mSettingsManager->readSetting(IpsServices::ReceptionActiveProfile, value)) {
            box->setCurrentIndex(value.toInt());
            }
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

    deleteMailboxButtonItem->setData(HbDataFormModelItem::KeyRole,
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

    // Instantiate the custom item and append it.
    HbDataFormModelItem *infoItem = new HbDataFormModelItem(
        static_cast<HbDataFormModelItem::DataItemType>(NmIpsSettingsCustomItem::LabeledComboBox),
        hbTrId("txt_mailips_setlabel_selected_mode"));
    item.appendChild(infoItem);

    // Store the pointer for use in itemShown.
    mReceivingScheduleSelectedItem = infoItem;

    // Connect the signals and slots.
    connect(mForm, SIGNAL(itemShown(const QModelIndex &)),
            this, SLOT(itemShown(const QModelIndex &)));
    mForm->addConnection(infoItem, SIGNAL(currentIndexChanged(int)),
                         mSettingsHelper, SLOT(receivingScheduleChange(int)));
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
    mSettingsManager->readSetting(IpsServices::LoginName, username);
    CpSettingFormItemData *usernameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_username"));
    mSettingsHelper->insertContentItem(IpsServices::LoginName, usernameItem);
    usernameItem->setContentWidgetData(QString("text"), username);
    mForm->addConnection(usernameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveUserName()));
    mForm->addConnection(usernameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(userNameTextChange(QString)));
    item.appendChild(usernameItem);

    // Password
    QVariant password;
    mSettingsManager->readSetting(IpsServices::Password, password);
    CpSettingFormItemData *passwordItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_password"));
    mSettingsHelper->insertContentItem(IpsServices::Password, passwordItem);
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
    Q_UNUSED(item);
}

Q_EXPORT_PLUGIN2(nmipssettings, NmIpsSettingsPlugin);
