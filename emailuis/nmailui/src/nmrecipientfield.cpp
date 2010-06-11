/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiheaders.h"

static const QString ContactsServiceName = "com.nokia.services.phonebookservices";
static const QString ContactsInterfaceName = "Fetch";
static const QString ContactsOperationName = "fetch(QString,QString,QString)";

static const int MaxRows = 10000;

/*!
   widget is created using AD/docml
*/
NmRecipientField::NmRecipientField(
        QObject *parent, HbDocumentLoader &docLoader, const QString &objPrefix):
    QObject(parent),
    mDocumentLoader(docLoader),
    mObjectPrefix(objPrefix),
    mLabel(NULL),
    mRecipientsEditor(NULL),
    mLaunchContactsPickerButton(NULL)
{
    NM_FUNCTION;
    
    // Load the widgets from nmeditorview.docml. The names match to the definitions in that docml.
    mWidget = qobject_cast<HbWidget *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Field"));

    mLabel = qobject_cast<HbLabel *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Label"));

    mRecipientsEditor = qobject_cast<NmRecipientLineEdit *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Edit"));
    mRecipientsEditor->setMaxRows(MaxRows);

    mLaunchContactsPickerButton = qobject_cast<HbPushButton *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Button"));

    mLaunchContactsPickerButton->setIcon(NmIcons::getIcon(NmIcons::NmIconContacts));

    createConnections();
}

/*!
   Creates connections for this class items
*/
void NmRecipientField::createConnections()
{
    NM_FUNCTION;
    
    connect(mRecipientsEditor, SIGNAL(textChanged(const QString &)),
        this, SIGNAL(textChanged(const QString &)));
    connect(mRecipientsEditor, SIGNAL(cursorPositionChanged(int, int)),
        this, SIGNAL(cursorPositionChanged(int, int)));
    connect(mRecipientsEditor, SIGNAL(editingFinished()),
        this, SIGNAL(editingFinished()));
    connect(mRecipientsEditor, SIGNAL(selectionChanged()),
        this, SIGNAL(selectionChanged()));
    connect(mLaunchContactsPickerButton, SIGNAL(pressed()),
            this, SIGNAL(launchContactsPickerButtonClicked()));

#ifdef Q_OS_SYMBIAN
    connect(mLaunchContactsPickerButton, SIGNAL(pressed()), this, SLOT(launchContactsPicker()));
#endif
}


/*!
   Destructor
*/
NmRecipientField::~NmRecipientField()
{
    NM_FUNCTION;
}

/*!
   Widget height
*/
qreal NmRecipientField::height()
{
    NM_FUNCTION;
    
    return mWidget->geometry().height();
}

/*!
   editor
*/
NmRecipientLineEdit *NmRecipientField::editor() const
{
    NM_FUNCTION;
    
    return mRecipientsEditor;
}


/*!
   LineEdit contents
*/
const QString NmRecipientField::text() const
{
    NM_FUNCTION;
    
    return mRecipientsEditor->text();
}


/*!
   Slot setText
*/
void NmRecipientField::setText(const QString &newText)
{
    NM_FUNCTION;
    
    if (newText != mRecipientsEditor->text()) {
        mRecipientsEditor->setText(newText);
        emit textChanged(newText);
    }
}


#ifdef Q_OS_SYMBIAN
/*!
   This Slot launches the contacts-picker
*/
void NmRecipientField::launchContactsPicker()
{
    NM_FUNCTION;
    
    XQApplicationManager mAppmgr;
    XQAiwRequest *launchContactsPickerRequest;
    
    bool isEmbeded = true;
    launchContactsPickerRequest = mAppmgr.create(ContactsServiceName, ContactsInterfaceName, 
                                                 ContactsOperationName, isEmbeded);
    
    if (launchContactsPickerRequest) {
        connect(launchContactsPickerRequest, SIGNAL(requestOk(QVariant)),
                mRecipientsEditor, SLOT(insertSelectedContacts(QVariant)));
    }
    else {
        // Failed creating request 
        NM_ERROR(1,"XQApplicationManager: failed creating fecth contactspicker request");
	    return;
    }

    QVariantList args; 
    args << "Non-filtered multi-fetch";
    args << KCntActionAll; 
    args << KCntFilterDisplayAll; 
    launchContactsPickerRequest->setArguments(args); 
    
    // Send request
    if (!launchContactsPickerRequest->send()) {
       //Failed sending request 
       NM_ERROR(1,"XQApplicationManager: failed sending request");
    }
        
    delete launchContactsPickerRequest;
    launchContactsPickerRequest = 0;
}
#endif
