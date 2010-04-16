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
    return EmailAddressPattern.exactMatch(emailAddress);
}

/*!
  Generates a display string from an NmAddress object.
*/
QString NmUtilities::addressToDisplayName( const NmAddress &address )
{
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
int NmUtilities::openFile(RFile &file)
{
    int ret(NmNotFoundError);
    XQSharableFile sf;
    sf.setHandle(file);
    XQApplicationManager aiwMgr;
    XQAiwRequest *request(NULL);
    request = aiwMgr.create(sf);  
    // Create request for the sharable file
    if (request)
    {
        // Set request arguments
        QList<QVariant> args;
        args << qVariantFromValue(sf);  
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
    if (string.length() <= length) {
        return string;
    }

    QString padding = "...";

    return string.mid(0, length - padding.length()) + padding;
}

