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
* Description: nmrecipientlineedit.cpp
* 
*/

#include "nmailuiwidgetsheaders.h"

static const QString Semicolon(";");
static const QString Delimiter("; ");
static const QRegExp CommaOrSemicolon("[,;]");


/*!
   Constructor
*/
NmRecipientLineEdit::NmRecipientLineEdit(QGraphicsItem *parent) 
    : NmHtmlLineEdit(parent),
    mNeedToGenerateEmailAddressList(true)
{
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(handleTextChanged(QString)));
}


/*!
   Destructor
*/
NmRecipientLineEdit::~NmRecipientLineEdit()
{
}


/*!
   Get the emailaddress list generated from the content of the lineedit.
*/
QList<NmAddress> NmRecipientLineEdit::emailAddressList()
{
    if (mNeedToGenerateEmailAddressList) {
        // Empty mEmailAddressList.
        mEmailAddressList.clear();
        // Generate mEmailAddressList from the lineedit content.
        generateEmailAddressList();					
        mNeedToGenerateEmailAddressList = false;
    }

    return mEmailAddressList;	
}


#ifdef Q_OS_SYMBIAN
/*!
   This Slot inserts the selected contacts from Contacts-picker into the lineedit cursor position.    
   "You shouldn't be able to convert the parameter selectedContacts into a QStringlist or QString,
   you need to convert selectedContacts into a CntServicesContactList." -- Comments from 
   Contacts-picker author Erkinheimo Joonas (Nokia-D/Espoo)
   Contacts-Picker should be working in TB 10.1 MCL wk16 release, 
   Custom metatypes problem will be fixed in wk16 by QtHighway.
*/
void NmRecipientLineEdit::insertSelectedContacts(const QVariant &selectedContacts)
{
    if (!selectedContacts.isNull()) {
        CntServicesContactList contactList;
        contactList = qVariantValue<CntServicesContactList>(selectedContacts);

        if (contactList.count() == 0) {	
            // String "No contact returned" will be replaced by a hbTrId.
            HbMessageBox note(tr("No contact returned"), HbMessageBox::MessageTypeInformation);
            note.setTimeout(HbMessageBox::NoTimeout);
            note.exec();
        }
        else {
            // Loop through all the contacts selected from Contacts application.
            for (int i = 0; i < contactList.count(); ++i) {
                QString contactEmailAddress = contactList[i].mEmailAddress;
                QString contactName = contactList[i].mDisplayName;

            // If this contact has no name.
            if(contactName.isEmpty()) {				
                // Generate custom keyevent for this contact's emailaddress.
                QKeyEvent contactEmailAddressKeyEvent(QEvent::KeyPress, Qt::Key_unknown, 
                		                              Qt::NoModifier, contactEmailAddress);
                // Forward this contactEmailAddressKeyEventt to base class to handle.
                NmHtmlLineEdit::keyPressEvent(&contactEmailAddressKeyEvent);
            }
            else {
                // Handle a rare case: there's another contact has same name 
                // but has different emailaddress.
                for (int i = 0; i != mContactsSelectedFromPhoneBook.count(); ++i) {
                    if (mContactsSelectedFromPhoneBook.at(i).displayName() == contactName &&
                    	mContactsSelectedFromPhoneBook.at(i).address() != contactEmailAddress) {
                        // Differentiate this contact's name by adding a * mark
                        contactName.append("*");
                    }
                }
                
                // Generate custom keyevent for this contact's name.
                QKeyEvent contactNameKeyEvent(QEvent::KeyPress, Qt::Key_unknown, Qt::NoModifier,
                                              contactName);
                // Forward this contactNameKeyEvent to base class to handle.
                NmHtmlLineEdit::keyPressEvent(&contactNameKeyEvent);
            }

            // Generate custom keyevent for Delimiter("; ").
            QKeyEvent delimiterKeyEvent(QEvent::KeyPress, Qt::Key_unknown, Qt::NoModifier, Delimiter);
            // Forward the delimiterKeyEvent to base class to handle.
            NmHtmlLineEdit::keyPressEvent(&delimiterKeyEvent);
			
            // Form the contact into Qmail NmAddress format.
            NmAddress contact;
            contact.setAddress(contactEmailAddress);
            contact.setDisplayName(contactName);
            
            // Add this NmAddress formated contact into mContactsSelectedFromPhoneBook.
            mContactsSelectedFromPhoneBook.append(contact);
            }
        }
    }
    else {
        //Request returned NULL 
        NMLOG("Request returned NULL.");
    }
        
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
#endif   // Q_OS_SYMBIAN


/*!
   keyPressEvent handles replacing user inputs "," or ";" from physical keyboard with "; "
   P.S. keyPressEvent can only catch QKeyEvent "," or ";" typed from physical keyboard,
   inputMethodEvent method handles user inputs "," or ";" from virtual keyboard.
*/
void NmRecipientLineEdit::keyPressEvent(QKeyEvent *keyEvent)
{
	bool eventHandled = false;
	
    if (keyEvent) {
        switch (keyEvent->key()) {    
        case Qt::Key_Comma:
        case Qt::Key_Semicolon:
            {
                QString textBeforeCursor = (this->text()).left(this->cursorPosition());
                
                // No action when the lineedit is empty or cursor is after a Delimiter("; ") 
                // or a Semicolon (";").
                if ((this->text()).isEmpty() || textBeforeCursor.endsWith(Delimiter)
                    || textBeforeCursor.endsWith(Semicolon)) {
                    keyEvent->ignore();
                    eventHandled = true;
                }
                else {
                    // Generate custom keyevent for Delimiter("; ") and 
                    // forward to the base class to handle.
                    QKeyEvent delimiterKeyEvent(keyEvent->type(), keyEvent->key(),
                                                keyEvent->modifiers(), Delimiter);
                    NmHtmlLineEdit::keyPressEvent(&delimiterKeyEvent);
                    eventHandled = true;
                }
            }
            break;

        default:
            NmHtmlLineEdit::keyPressEvent(keyEvent);
            eventHandled = true;
            break;      
        }
    }
    
    // If event is not handled, forward to the base class to handle.
    if (!eventHandled) {
        NmHtmlLineEdit::keyPressEvent(keyEvent);
    }
}


/*!
   inputMethodEvent handles replacing user inputs "," or ";" from virtual keyboard with "; ".
   P.S. keyPressEvent can only catch QKeyEvent "," or ";" typed from physical keyboard
*/
void NmRecipientLineEdit::inputMethodEvent(QInputMethodEvent *event)
{
	bool eventHandled = false;
	
    if (event) {
        QString eventText = event->commitString();

        if (!eventText.isEmpty() || event->replacementLength()) {
            // If typed charater from virtual keyboard is "," or ";"
            if (eventText.contains(CommaOrSemicolon)) {
                QString textBeforeCursor = (this->text()).left(this->cursorPosition());
				
                // No action when the lineedit is empty or cursor is after a Delimiter("; ") 
                // or Semicolon (";").
                if ((this->text()).isEmpty() || textBeforeCursor.endsWith(Delimiter)
                    || textBeforeCursor.endsWith(Semicolon)) {
                    event->ignore();
                    eventHandled = true;
                }
                else {
                    // Modify event with Delimiter("; ") and forward to the base class to handle.
                    event->setCommitString(Delimiter, event->replacementStart(),
                                           event->replacementLength()); 
                    NmHtmlLineEdit::inputMethodEvent(event);
                    eventHandled = true;
                }
            }
        }
    }

    // If event is not handled, forward to the base class to handle.
    if (!eventHandled) {
        NmHtmlLineEdit::inputMethodEvent(event);
    }
}

 
/*!
   Generate emailaddress list from the content of the lineedit.
*/
void NmRecipientLineEdit::generateEmailAddressList()
{   
    // Remove whitespace from the start and the end of the lineedit content. 
    QString contentOfLineedit = (this->text()).trimmed();
    
    // Split the lineedit content into individual items wherever a Semicolon(";") occurs, 
    // empty entries don't appear in the result.
    QStringList itemsOfLineeditContent = contentOfLineedit.split(Semicolon, QString::SkipEmptyParts);
        
    // Loop through all the items in the itemsOfLineeditContent list.
    for (int i = 0; i != itemsOfLineeditContent.count(); ++i) {
        // Remove whitespace from the start and the end of the item.
        QString itemInLineedit = itemsOfLineeditContent.at(i).trimmed();

        if (mContactsSelectedFromPhoneBook.count() > 0) {
            // Loop through all the elements in the mContactsSelectedFromPhoneBook list.
            for (int j = 0; j != mContactsSelectedFromPhoneBook.count(); ++j) {
                NmAddress contact = mContactsSelectedFromPhoneBook.at(j);		
                // If the item matches either the name or the emailaddress of this contact.
                if (itemInLineedit == contact.displayName() || itemInLineedit == contact.address()) {
                    // Add the contact into mEmailAddressList.
                    mEmailAddressList.append(contact);  
                }
                else {
                    // Form the item into Qmail NmAddress format.
                    NmAddress recipient;
                    recipient.setAddress(itemInLineedit);
                    // no display name info available, so don't us it
                    recipient.setDisplayName(QString()); 
                    // Add this NmAddress formated lineedit item into mEmailAddressList.
                    mEmailAddressList.append(recipient);  
                }
            }
        }
        else {
            // Form the item into Qmail NmAddress format.
            NmAddress recipient;
            recipient.setAddress(itemInLineedit);
            // no display name info available, so don't us it
            recipient.setDisplayName(QString()); 
            // Add this NmAddress formated lineedit item into mEmailAddressList.
            mEmailAddressList.append(recipient);  
        }
    }
}


/*!
   This Slot is called when the lineedit text changes.
*/
void NmRecipientLineEdit::handleTextChanged(const QString &text)
{
    Q_UNUSED(text);
    mNeedToGenerateEmailAddressList = true;
}
