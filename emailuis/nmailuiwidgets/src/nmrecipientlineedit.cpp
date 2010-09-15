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
    NM_FUNCTION;
    
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(handleTextChanged(QString)));
}


/*!
   Destructor
*/
NmRecipientLineEdit::~NmRecipientLineEdit()
{
    NM_FUNCTION;
}


/*!
   Get the emailaddress list generated from the content of the lineedit.
*/
QList<NmAddress> NmRecipientLineEdit::emailAddressList()
{
    NM_FUNCTION;
    
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
   This Slot appends the selected contacts to the end of the lineedit content.    
*/
void NmRecipientLineEdit::addSelectedContacts(const QVariant &selectedContacts)
{
    NM_FUNCTION;
    
    // If user selected contact
    if (!selectedContacts.isNull()) {        

        // If the lineedit is not empty and if there is no ";" or "; " at the end,
        // add a delimiter("; ") at the end.
        if (this->text().length() != 0 && !(this->text().endsWith(Semicolon)) && 
            !(this->text().endsWith(Delimiter))){
            
            // Move cursor to the end of the lineedit.
            this->setCursorPosition(this->text().length());       
            QTextCursor textCursor(this->textCursor());
            // Append delimiter("; ") to the end of the lineedit
            textCursor.insertText(Delimiter);
        }
        
        CntServicesContactList contactList = qVariantValue<CntServicesContactList>(selectedContacts);

        // Loop through all the selected contacts.
        for (int i = 0; i < contactList.count(); ++i) {
            QString contactName = contactList[i].mDisplayName;
            QString contactEmailAddress = contactList[i].mEmailAddress;
            
            // If this contact has no name, use it's emailaddress as the display name
            if(contactName.isEmpty()) {	
                // Move cursor to the end of the lineedit.
                this->setCursorPosition(this->text().length());       
                QTextCursor textCursor(this->textCursor());
                // Append contactEmailAddress to the end of the lineedit
                textCursor.insertText(contactEmailAddress);
            }
            // If this contact has name, use the name as the display name
            else {
                // Handle a rare case: there are contacts has same name but different emailaddress.
                for (int i = 0; i != mRecipientsAddedFromContacts.count(); ++i) {
                    if (mRecipientsAddedFromContacts.at(i).displayName() == contactName &&
                  	    mRecipientsAddedFromContacts.at(i).address() != contactEmailAddress) {
                        // Differentiate this contact by supplying it's emailaddress
                        contactName.append("<");
                        contactName.append(contactEmailAddress);
                        contactName.append(">");
                    }
                }
                
                // Move cursor to the end of the lineedit.
                this->setCursorPosition(this->text().length());       
                QTextCursor textCursor(this->textCursor());
                // Append contactName to the end of the lineedit
                textCursor.insertText(contactName);
            }
                
            QTextCursor textCursor(this->textCursor());
            // Append delimiter("; ")
            textCursor.insertText(Delimiter);
            
            // Form the contact into Qmail NmAddress format.
            NmAddress contact;
            contact.setAddress(contactEmailAddress);
            contact.setDisplayName(contactName);
            
            // Add this NmAddress formated contact into mRecipientsAddedFromContacts.
            mRecipientsAddedFromContacts.append(contact);
        } 
    }
    else {
        //Request returned NULL 
        NM_COMMENT("ContactsPicker request returned NULL.");
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
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
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
   Generate a list of all the email addresses from the content of the lineedit.
*/
void NmRecipientLineEdit::generateEmailAddressList()
{   
    NM_FUNCTION;
    
    // Remove whitespace from the start and the end of the lineedit content. 
    QString contentOfLineedit = (this->text()).trimmed();
    
    // Split the lineedit content by semicolon(";").
    QStringList itemsOfLineeditContent = contentOfLineedit.split(Semicolon, QString::SkipEmptyParts);
        
    // Loop through all the items of the lineedit content.
    for (int i = 0; i != itemsOfLineeditContent.count(); ++i) {
        // Remove whitespace from the start and the end of the item.
        QString itemInLineedit = itemsOfLineeditContent.at(i).trimmed();

        // Get the count of the recipients added from Contacts.
        int countOfRecipientsAddedFromContacts = mRecipientsAddedFromContacts.count();
        
        // If there is recipient added from Contacts.
        if (countOfRecipientsAddedFromContacts > 0) {
            QStringList listOfAddedContactsName;
            QStringList listOfAddedContactsAddress;
            
            // Loop through all the recipients added from Contacts.
            for (int j = 0; j != countOfRecipientsAddedFromContacts; ++j) {
                NmAddress contact = mRecipientsAddedFromContacts.at(j);          
                listOfAddedContactsName.append(contact.displayName());
                listOfAddedContactsAddress.append(contact.address());
            }
                
            int indexInAddedContactsName = listOfAddedContactsName.indexOf(itemInLineedit);
            int indexInAddedContactsAddress = listOfAddedContactsAddress.indexOf(itemInLineedit);
            
            // If this itemInLineedit matches the name of one added contact.
            if (indexInAddedContactsName >= 0) {
                // Add the recipient into mEmailAddressList.
                mEmailAddressList.append(mRecipientsAddedFromContacts.at(indexInAddedContactsName));  
            }
            // If this itemInLineedit matches the emailaddress of one added contact.
            else if (indexInAddedContactsAddress >= 0) { 
                // Add the recipient into mEmailAddressList.
                mEmailAddressList.append(mRecipientsAddedFromContacts.at(indexInAddedContactsAddress));  
            }
            // This itemInLineedit is not added from Contacts
            else { 
                // Form the item into NmAddress format.
                NmAddress recipient;
                recipient.setAddress(itemInLineedit);
                // There is no display name info available, so leave display name empty.
                recipient.setDisplayName(QString()); 
                // Add this NmAddress formated lineedit item into mEmailAddressList.
                mEmailAddressList.append(recipient);  
            }
        }
        else { // There is no recipient is added from Contacts
            // Form the item into NmAddress format.
            NmAddress recipient;
            recipient.setAddress(itemInLineedit);
            // There is no display name info available, so leave display name emapty.
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
    NM_FUNCTION;
    
    Q_UNUSED(text);
    mNeedToGenerateEmailAddressList = true;
}

/*
 * If recipient is added from Contacts by Contacts, we need to add it to the list.
 */
void NmRecipientLineEdit::addContacts(QList<NmAddress *> contacts)
{
    foreach (NmAddress *nmContact,contacts) {
        if (nmContact && nmContact->displayName().length() > 0) {
            mRecipientsAddedFromContacts.append(*nmContact);
            mNeedToGenerateEmailAddressList = true;
        }
    }
}
