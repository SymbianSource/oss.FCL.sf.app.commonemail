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

#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdocumentloader.h>

#include "nmmailboxsettingview.h"
#include "nmmailboxsettingsmanager.h"
#include "nmsettingsformcustomitems.h"


static const char *NMSETTINGUI_SETTING_VIEW_XML = ":/docml/nmmailboxsettingview.docml";
static const char *NMSETTINGUI_SETTING_VIEW_FORM = "mailboxSettingViewForm";


/*!
    \class NmMailboxSettingView
    \brief Setting view for the mailbox specific settings.

*/


// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmMailboxSettingView.

    Creates the setting form and form model that contain the setting items.
    Populates the form model items from the correct settings plug-in through
    NmMailboxSettingsManager.
*/
NmMailboxSettingView::NmMailboxSettingView(const NmId &mailboxId,
    const QString &mailboxName,
    NmMailboxSettingsManager& settingsManager,
    QGraphicsItem *parent)
    : CpBaseSettingView(0, parent),
      mForm(NULL),
      mModel(NULL),
      mMailboxId(mailboxId.id())
{
    NM_FUNCTION;
    
    setTitle(mailboxName);

    HbDocumentLoader documentLoader;
    bool documentLoaded = false;
    QObjectList objectList;
    objectList.append(this);

    documentLoader.setObjectTree(objectList);
    QObjectList widgetList =
        documentLoader.load(NMSETTINGUI_SETTING_VIEW_XML, &documentLoaded);

    if (documentLoaded && widgetList.count()) {
        // Get the form widget.
        mForm = qobject_cast<HbDataForm*>(
            documentLoader.findWidget(NMSETTINGUI_SETTING_VIEW_FORM));
    }

    if (mForm) {
        // Fix for dataform item recycling.
        mForm->setItemRecycling(false);

        // Set the form for the view.
        setWidget(mForm);

        // Make the custom items available.
        NmSettingsFormCustomItems *customItems = new NmSettingsFormCustomItems(mForm);
        QList<HbAbstractViewItem *> prototypes  = mForm->itemPrototypes();
        prototypes.append(customItems);
        mForm->setItemPrototypes(prototypes);

        // Set up the model.
        mModel = new HbDataFormModel();
        settingsManager.populateModel(*mModel, *mForm, mailboxId);
        mForm->setModel(mModel);
    }
}


/*!
    Destructor of NmMailboxSettingView.
*/
NmMailboxSettingView::~NmMailboxSettingView()
{
    NM_FUNCTION;
    
    if (mForm) {
        mForm->removeAllConnection();
        delete mForm;
    }

    delete mModel;
}


/*!
    Private slot for handling mailbox list event changes.

    \param mailboxId The ID of the mailbox of which list has been changed.
    \param type The type of the change.
*/
void NmMailboxSettingView::mailboxListChanged(const NmId &mailboxId,
    NmSettings::MailboxEventType type)
{
    NM_FUNCTION;
    
    Q_UNUSED(mailboxId);
    Q_UNUSED(type);

    HbAction *action = navigationAction();
    if (action) {
        action->activate(QAction::Trigger);
    }
}


/*!
    Private slot for handling mailbox property changes.

    \param mailboxId The ID of the mailbox of which property was changed.
    \param property The type of the property that changed.
    \param value The new value related to the change.
*/
void NmMailboxSettingView::mailboxPropertyChanged(const NmId &mailboxId,
                                                  QVariant property,
                                                  QVariant value)
{
    NM_FUNCTION;
    
    Q_UNUSED(mailboxId);

    switch (property.toInt()) {
        case NmSettings::MailboxName: {
            setTitle(value.toString());
            break;
        }
        default: {
            break;
        }
    }
}


// End of file.
