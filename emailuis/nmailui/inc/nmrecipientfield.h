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
* Description: Email Address Field Widget
*
*/

#ifndef NMRECIPIENTFIELD_H_
#define NMRECIPIENTFIELD_H_

#include <hbwidget.h>

class QGraphicsLinearLayout;
class HbLabel;
class NmRecipientLineEdit;
class HbPushButton;
class HbIconItem;
#ifdef Q_OS_SYMBIAN
class XQAiwRequest;
#endif

class NmRecipientField : public HbWidget
{
    Q_OBJECT
public:
    NmRecipientField(
    HbLabel *label,
    NmRecipientLineEdit *edit,
    HbPushButton *button,
    QGraphicsItem *parent = 0);
    NmRecipientField(const QString &labelString = "", QGraphicsItem *parent = 0);
    virtual ~NmRecipientField();
    qreal height();
    void createConnections();

signals:
    void textChanged(const QString &text);
    void cursorPositionChanged(int oldPos, int newPos);
    void editingFinished();
    void selectionChanged();
    void launchContactsPickerButtonClicked();

public:
    NmRecipientLineEdit *editor() const;
    const QString text() const;

public slots:
    void setText(const QString &text);
#ifdef Q_OS_SYMBIAN
    void launchContactsPicker();
#endif

private:
    QGraphicsLinearLayout *mLayoutHorizontal;
    HbLabel *mLabel;                  
    NmRecipientLineEdit *mRecipientsEditor;
    HbPushButton *mLaunchContactsPickerButton;       
    HbIconItem *mAddButtonIcon;
    bool mOwned;
};

#endif   // NMRECIPIENTFIELD_H_
