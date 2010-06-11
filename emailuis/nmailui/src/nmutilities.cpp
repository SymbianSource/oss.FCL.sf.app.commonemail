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
* Description: UI utilities class
*
*/

#include "nmuiheaders.h"

static const int NmMegabyte = 1048576;
static const int NmShortInterval = 1000; // 1 sec

// taken from http://www.regular-expressions.info/email.html
static const QRegExp EmailAddressPattern("[A-Za-z\\d!#$%&'*+/=?^_`{|}~-]+"
                                         "(?:"
                                         "\\."
                                         "[A-Za-z\\d!#$%&'*+/=?^_`{|}~-]+"
                                         ")*"
                                         "@"
                                         "(?:"
                                         "[A-Za-z\\d]"
                                         "(?:"
                                         "[A-Za-z\\d-]*[A-Za-z\\d]"
                                         ")?"
                                         "\\."
                                         ")+"
                                         "[A-Za-z\\d]"
                                         "(?:"
                                         "[A-Za-z\\d-]*[A-Za-z\\d]"
                                         ")?"
                                        );

/*!
   Gets valid, invalid or all the recipients from a message
 */
void NmUtilities::getRecipientsFromMessage( const NmMessage &message,
    QList<NmAddress> &recipients,
    NmAddressValidationType type )
{
    NM_FUNCTION;
    
    // validate TO addresses
    QList<NmAddress> toRecipients = message.envelope().toRecipients();
    int recipientCount = toRecipients.count();

    for (int i = 0; i < recipientCount; ++i) {
        bool validAddress = isValidEmailAddress(toRecipients.at(i).address());

        if (type == Default ||
            type == ValidAddress && validAddress ||
            type == InvalidAddress && !validAddress) {
            recipients.append(toRecipients.at(i));
        }
    }

    // validate CC addresses
    QList<NmAddress> ccRecipients = message.envelope().ccRecipients();
    recipientCount = ccRecipients.count();

    for (int i = 0; i < recipientCount; ++i) {
        bool validAddress = isValidEmailAddress(ccRecipients.at(i).address());

        if (type == Default ||
            type == ValidAddress && validAddress ||
            type == InvalidAddress && !validAddress) {
            recipients.append(ccRecipients.at(i));
        }
    }

    // validate BCC addresses
    QList<NmAddress> bccRecipients = message.envelope().bccRecipients();
    recipientCount = bccRecipients.count();

    for (int i = 0; i < recipientCount; ++i) {
        bool validAddress = isValidEmailAddress(bccRecipients.at(i).address());

        if (type == Default ||
            type == ValidAddress && validAddress ||
            type == InvalidAddress && !validAddress) {
            recipients.append(bccRecipients.at(i));
        }
    }
}

/*!
  Validates the given string against an email address pattern.
*/
bool NmUtilities::isValidEmailAddress( const QString &emailAddress )
{
    NM_FUNCTION;
    
    return EmailAddressPattern.exactMatch(emailAddress);
}

/*!
  Generates a display string from an NmAddress object.
*/
QString NmUtilities::addressToDisplayName( const NmAddress &address )
{
    NM_FUNCTION;
    
    QString emailAddress = address.address();
    QString displayName = address.displayName();

    QString ret;
    if (displayName.length() > 0 && displayName != emailAddress) {
        ret = displayName + " <" + emailAddress + ">";
    }
    else {
        ret = emailAddress;
    }
    return ret;
}

/*!
  Returns an NmAddress object that is parsed from a display name string.
*/
bool NmUtilities::parseEmailAddress( const QString &emailAddress, NmAddress &address )
{
    NM_FUNCTION;
    
    bool foundAddress = false;

    QRegExp rx(EmailAddressPattern);
    // locate the email address in the string
    int pos = rx.indexIn(emailAddress);
    if (pos != -1) {
        // extract the email address...
        int matchedLen = rx.matchedLength();
        QString addr = emailAddress.mid(pos, matchedLen);
        address.setAddress(addr);

        // ...and use the rest as display name
        QString name = emailAddress.left(pos) + emailAddress.mid(pos + matchedLen);
        address.setDisplayName(cleanupDisplayName(name));

        foundAddress = true;
    }

    return foundAddress;
}

/*!
  Cleans up display name by stripping extra characters from the beginning and end of the string.
*/
QString NmUtilities::cleanupDisplayName( const QString &displayName )
{
    NM_FUNCTION;
    
    // find the first and last position that is NOT one of the characters below
    QRegExp rx("[^\\s\"<>]");
    int firstPos = std::max(rx.indexIn(displayName), 0);
    int lastPos = rx.lastIndexIn(displayName);

    if (lastPos < 0) {
        lastPos = displayName.length() - 1;
    }

    return displayName.mid(firstPos, lastPos - firstPos + 1);
}

/*!
  Opens file specified by QFile handle. Usually used by editor
  for opening added attachments
*/
int NmUtilities::openFile(QFile &file)
{
    NM_FUNCTION;
    
    int ret(NmNotFoundError);
    XQApplicationManager aiwMgr;
    XQAiwRequest *request(NULL);
    request = aiwMgr.create(file);
    // If request is created then there is a handler for that file
    if (request) {
         // Set request arguments
         QList<QVariant> args;
         args << file.fileName();
         request->setArguments(args);
         // Send the request, ownership of request is transferred
         bool res = request->send();
         if (res) {
             // Request ok, set error status.
             ret = NmNoError;
         }
    }
    delete request;
    return ret;
}


/*!
  Opens file specified by RFile handle. Usually used by viewer
  for opening attachments from message store as RFiles
*/
int NmUtilities::openFile(XQSharableFile &file)
{
    NM_FUNCTION;
    
    int ret(NmNotFoundError);
    XQApplicationManager aiwMgr;
    XQAiwRequest *request(NULL);
    request = aiwMgr.create(file);  
    // Create request for the sharable file
    if (request)
    {
        // Set request arguments
        QList<QVariant> args;
        args << qVariantFromValue(file);  
        request->setArguments(args);
        // Send the request, ownership of request is transferred
        bool res = request->send();
        if (res) {
        // Request ok, set error status.
        ret = NmNoError;
        }
    }
    return ret;
}

/*!
 * Truncate a string to a specific length. If length is less than
 * the string, ellipses are added to the end of the string.
 */
QString NmUtilities::truncate( const QString &string, int length )
{
    NM_FUNCTION;
    
    if (string.length() <= length) {
        return string;
    }

    QString padding = "...";

    return string.mid(0, length - padding.length()) + padding;
}

/*!
 * Shows an error note. Used by at least editor and viewer classes.
 * 
 */
void NmUtilities::displayErrorNote(QString noteText)
{
    NM_FUNCTION;
    
	HbNotificationDialog *note = new HbNotificationDialog();
	
	note->setIcon(HbIcon(QLatin1String("note_warning")));
	note->setTitle(noteText);
	note->setTitleTextWrapping(Hb::TextWordWrap);
	note->setDismissPolicy(HbPopup::TapAnywhere);
	note->setAttribute(Qt::WA_DeleteOnClose);
	note->setSequentialShow(false);

	note->show();
}

/*!
    Function returns localized attachment size string based
    on attachment size in bytes
 */
QString NmUtilities::attachmentSizeString(const int sizeInBytes)
{
    NM_FUNCTION;
    
    qreal sizeMb = (qreal)sizeInBytes / (qreal)NmMegabyte;
    if (sizeMb < 0.1) {
        // 0.1 Mb is the minimum size shown for attachment
        sizeMb = 0.1;
    }
    return QString().sprintf("(%.1f Mb)", sizeMb); // Use loc string when available    
}

/*!
    Displays a note with Yes/No buttons. Note has no timeout, i.e. it has to be dismissed manually.
    Returns pointer to dialog so that caller can take ownership and handle deletion.
    Parameter 'receiver' is the object and 'member' is the slot where user selection is passed. 
*/
HbMessageBox* NmUtilities::displayQuestionNote(
    QString noteText, QObject* receiver, const char* member)
{
    NM_FUNCTION;
    
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    messageBox->setText(noteText);
    messageBox->setTimeout(HbMessageBox::NoTimeout); // Note has to be dismissed manually
    messageBox->open(receiver, member);
    return messageBox;
}

/*!
 * displays an error note with no buttons. Note dismisses itself after NmShortInterval.
 */
void NmUtilities::displayWarningNote(QString noteText)
{
    NM_FUNCTION;
    
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeWarning);
    messageBox->setText(noteText);
    messageBox->setTimeout(NmShortInterval);
    messageBox->clearActions(); // gets rid of buttons from the note
    messageBox->setModal(false);
    messageBox->setBackgroundFaded(false);
    messageBox->show();

    delete messageBox;
}

/*!
    Function returns localized "Original message" header
    in html format based on envelope
*/
QString NmUtilities::createReplyHeader(const NmMessageEnvelope &env)
{
    NM_FUNCTION;
    
    QString ret = "<html><body><br><br>";
    // Append "----- Original message ----" text
    ret+=hbTrId("txt_mail_editor_reply_original_msg");                  
    // Append sender
    ret+="<br>";
    ret+=hbTrId("txt_mail_editor_reply_from");               
    ret+=" ";        
    if (env.sender().displayName().length()){
        ret+=env.sender().displayName();
    }
    else{
        ret+=env.sender().address();
    }   
    // Append sent time
    ret+="<br>";
    ret+=hbTrId("txt_mail_editor_reply_sent");   
    ret+=" ";  
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDate sentLocalDate = env.sentTime().toLocalTime().date();
    ret+=locale.format(sentLocalDate, r_qtn_date_usual);   
    // Append to recipients
    const QList<NmAddress> &toList = env.toRecipients();
    if (toList.count()){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_to"); 
        ret+=" ";    
        for (int i=0;i<toList.count();i++){
            if (toList[i].displayName().length()){
                ret+=toList[i].displayName();
            }
            else{
                ret+=toList[i].address();
            }
            if (i!=toList.count()-1){
                ret+=";";          
            }
        }    
    }
    // Append cc recipients
    const QList<NmAddress> &ccList = env.ccRecipients();
    if (ccList.count()){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_cc"); 
        ret+=" ";         
        for (int i=0;i<ccList.count();i++){
            if (ccList[i].displayName().length()){
                ret+=ccList[i].displayName();
            }
            else{
                ret+=ccList[i].address();
            }
            if (i!=toList.count()-1){
                ret+=";";          
            }
        }    
    }
    // Append subject if there is subject to display
    if (env.subject().length()){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_subject"); 
        ret+=" ";    
        ret+=env.subject();    
    }
    // Append priority if it is other than normal  
    if (env.priority()!=NmMessagePriorityNormal){   
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_importance"); 
        ret+=" ";    
        if (env.priority()==NmMessagePriorityLow){
            ret+=hbTrId("txt_mail_editor_reply_importance_low");         
        }
        else {
            ret+=hbTrId("txt_mail_editor_reply_importance_high"); 
        }
    }    
    ret+="<br></body></html>";
    return ret;
}
