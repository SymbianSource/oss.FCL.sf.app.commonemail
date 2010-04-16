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
static const double LabelFieldWidth = 10 * Un + Un;
static const double ButtonWidth = 9.5 * Un;
static const double FieldHeight = 5 * Un;

/*!
   Constructor
*/
NmRecipientField::NmRecipientField(
    HbLabel *label,
    NmRecipientLineEdit *edit,
    HbPushButton *button,
    QGraphicsItem *parent):
    HbWidget(parent),
    mLabel(label),
    mRecipientsEditor(edit),
    mLaunchContactsPickerButton(button),
    mOwned(false)
#ifdef Q_OS_SYMBIAN
    ,mLaunchContactsPickerRequest(NULL)
#endif
{
    mLaunchContactsPickerButton->setIcon(NmIcons::getIcon(NmIcons::NmIconContacts));
    createConnections();
}


/*!
   Constructor for 'Cc:' and 'Bcc:' fields. This can be removed when groupBox content
   widget is created using AD/docml
*/
NmRecipientField::NmRecipientField(const QString &labelString, QGraphicsItem *parent):
    HbWidget(parent),
    mLabel(NULL),
    mRecipientsEditor(NULL),
    mLaunchContactsPickerButton(NULL),
    mOwned(true)
#ifdef Q_OS_SYMBIAN
    ,mLaunchContactsPickerRequest(NULL)
#endif
{
    mLayoutHorizontal = new QGraphicsLinearLayout(Qt::Horizontal, this);

    mLabel = new HbLabel(labelString);
    if (mLabel) {
        mLayoutHorizontal->addItem(mLabel);
        mLabel->setPreferredWidth(LabelFieldWidth);
        mLabel->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
        mLabel->setAlignment(Qt::AlignTop);
    }

    mRecipientsEditor = new NmRecipientLineEdit();
    if (mRecipientsEditor) {
    	mLayoutHorizontal->addItem(mRecipientsEditor);
        mRecipientsEditor->setMaxRows(MaxRows);
        mRecipientsEditor->setPreferredHeight(FieldHeight);
        mRecipientsEditor->setMinimumHeight(FieldHeight);
        mRecipientsEditor->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
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

    createConnections();
}

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

#ifdef Q_OS_SYMBIAN
    if (mLaunchContactsPickerRequest) {
        delete mLaunchContactsPickerRequest;
        mLaunchContactsPickerRequest = 0;
    }
#endif
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
    if (mLaunchContactsPickerRequest) {
        delete mLaunchContactsPickerRequest;
        mLaunchContactsPickerRequest = 0;
    }

    mLaunchContactsPickerRequest = new XQServiceRequest
	                                   ("com.nokia.services.phonebookservices.Fetch",
                                        "fetch(QString,QString,QString)", false);
    connect(mLaunchContactsPickerRequest, SIGNAL(requestCompleted(QVariant)),
            mRecipientsEditor, SLOT(insertSelectedContacts(QVariant)));

    // "Contacts" will be replaced by a hbTrId when it is ready
    *mLaunchContactsPickerRequest << tr("Contacts");
    *mLaunchContactsPickerRequest << KCntActionAll;
    *mLaunchContactsPickerRequest << KCntActionAll;

    QVariant returnValue;
    mLaunchContactsPickerRequest->send(returnValue);
}
#endif
