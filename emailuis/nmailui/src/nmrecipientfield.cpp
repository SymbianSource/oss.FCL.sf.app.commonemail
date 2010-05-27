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

static const double Un = 6.66;
static const double Margin = 2 * Un;
static const int MaxRows = 10000;
static const double LabelFieldWidth = 12.5 * Un;
static const double ButtonWidth = 9.5 * Un;
static const double FieldHeight = 5 * Un;
static const char *ContactsServiceName = "com.nokia.services.phonebookservices";
static const char *ContactsInterfaceName = "Fetch";
static const char *ContactsOperationName = "fetch(QString,QString,QString)";

/*!
   widget is created using AD/docml
*/
NmRecipientField::NmRecipientField(const QString &labelString, QGraphicsItem *parent):
    HbWidget(parent),
    mLabel(NULL),
    mRecipientsEditor(NULL),
    mLaunchContactsPickerButton(NULL),
    mOwned(true)
{
    //construct default ui.    
    mLayoutHorizontal = new QGraphicsLinearLayout(Qt::Horizontal);

    mLabel = new HbLabel(labelString);
    if (mLabel) {
        mLayoutHorizontal->addItem(mLabel);
        mLabel->setPreferredWidth(LabelFieldWidth);
        mLabel->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
    }

    mRecipientsEditor = new NmRecipientLineEdit();
    if (mRecipientsEditor) {
    	mLayoutHorizontal->addItem(mRecipientsEditor);
        mRecipientsEditor->setMaxRows(MaxRows);
        mRecipientsEditor->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
        mRecipientsEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mRecipientsEditor->setMinimumHeight(FieldHeight);
        mRecipientsEditor->setPreferredHeight(FieldHeight);
        mRecipientsEditor->setMaximumHeight(FieldHeight);
    }

    mLaunchContactsPickerButton = new HbPushButton();
    if (mLaunchContactsPickerButton) {
    	mLayoutHorizontal->addItem(mLaunchContactsPickerButton);
    	mLayoutHorizontal->setAlignment(mLaunchContactsPickerButton, Qt::AlignTop);
        mLaunchContactsPickerButton->setPreferredHeight(FieldHeight);
        mLaunchContactsPickerButton->setPreferredWidth(ButtonWidth);
        mLaunchContactsPickerButton->setMaximumHeight(FieldHeight);

        mLaunchContactsPickerButton->setIcon(NmIcons::getIcon(NmIcons::NmIconContacts));
    }

    mLayoutHorizontal->setContentsMargins(0, 0, 0, 0);
    // Set the spacing between the line edit  and the Add button to
    mLayoutHorizontal->setItemSpacing(1, Un);
    // Set the spacing between the label and the line edit to 0.0
    mLayoutHorizontal->setItemSpacing(0, 0.0);
    
    this->setLayout(mLayoutHorizontal);

    createConnections();
}

/*!
   Creates connections for this class items
*/
void NmRecipientField::createConnections()
{
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
    if (mOwned)
    {
        if (mLaunchContactsPickerButton) {
            delete mLaunchContactsPickerButton;
            mLaunchContactsPickerButton = 0;
        }
        if (mRecipientsEditor) {
            delete mRecipientsEditor;
            mRecipientsEditor = 0;
        }
        if (mLabel) {
            delete mLabel;
            mLabel = 0;
        }
    }
}

/*!
   Widget height
*/
qreal NmRecipientField::height()
{
    return mRecipientsEditor->geometry().height() + Margin;
}

/*!
   editor
*/
NmRecipientLineEdit *NmRecipientField::editor() const
{
    return mRecipientsEditor;
}


/*!
   LineEdit contents
*/
const QString NmRecipientField::text() const
{
    return mRecipientsEditor->text();
}


/*!
   Slot setText
*/
void NmRecipientField::setText(const QString &newText)
{
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
        NMLOG("XQApplicationManager: failed creating fecth contactspicker request.");
	    return;
    }

    // Send request
    if (!launchContactsPickerRequest->send()) {
       //Failed sending request 
       NMLOG("XQApplicationManager: failed sending request.");
    }
        
    delete launchContactsPickerRequest;
    launchContactsPickerRequest = 0;
}
#endif
