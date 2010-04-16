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
* Description: nmrecipientlineedit.h
*
*/

#ifndef NMRECIPIENTLINEEDIT_H_
#define NMRECIPIENTLINEEDIT_H_

#include <nmailuiwidgetsdef.h>
#include <nmhtmllineedit.h>
#include <nmaddress.h>

class QGraphicsItem;


class NMAILUIWIDGETS_EXPORT NmRecipientLineEdit : public NmHtmlLineEdit
{
    Q_OBJECT
    
public:
    NmRecipientLineEdit(QGraphicsItem *parent = 0);
    virtual ~NmRecipientLineEdit();

    // Get the emailaddress list generated from the content of the lineedit.
    QList<NmAddress> emailAddressList();
	
public slots:
#ifdef Q_OS_SYMBIAN
    void insertSelectedContacts(const QVariant &selectedContacts);
#endif
    void insertText(const QString &text);

protected:	// from HbLineEdit
    void keyPressEvent(QKeyEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);

private:
    // Generate emailaddress list from the content of the lineedit.
    void generateEmailAddressList();
	
private slots:    
    void handleTextChanged(const QString &text);

private:
    // mEmailAddressList contains all emailaddress that appear in the lineedit and 
    // all emailaddress corresponding to the name inserted from PhoneBook,
    // including invalid emailaddress user typed or invalid emailaddress picked from PhoneBook.
    // Emailaddress validation check is conducted in nmeditorview.
    QList<NmAddress> mEmailAddressList; 
	
    QList<NmAddress> mContactsSelectedFromPhoneBook;
	
    // Flag "need to generate mEmailAddressList" is used whenever editing made after 
    // the emailaddress validation check failed.
    bool mNeedToGenerateEmailAddressList;
};

#endif   // NMRECIPIENTLINEEDIT_H_
