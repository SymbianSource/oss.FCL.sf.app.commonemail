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
* Description: UI utilities class definition
*
*/

#ifndef NMUTILITIES_H_
#define NMUTILITIES_H_

#include <QObject>

class NmMessage;
class NmMessageEnvelope;
class NmAddress;
class XQSharableFile;
class HbMessageBox;

/*!
	UI utilities class
*/
class NmUtilities : public QObject
{
    Q_OBJECT
public:
    enum NmAddressValidationType {
        NmValidAddress,
        NmInvalidAddress,
        NmDefault
    };
    static void getRecipientsFromMessage( const NmMessage &message, 
        QList<NmAddress> &recipients,
        NmAddressValidationType type = NmDefault );
    static bool isValidEmailAddress( const QString &emailAddress );
    static QString addressToDisplayName( const NmAddress &address );
    static bool parseEmailAddress( const QString &emailAddress, NmAddress &address );
    static QString cleanupDisplayName( const QString &displayName );     
    static int openFile(XQSharableFile &file); 
    static QString truncate( const QString &string, int length );
    static QString attachmentSizeString(const int sizeInBytes);
    static void displayErrorNote(QString noteText); 
    static HbMessageBox *displayQuestionNote(QString noteText,
                                             QObject *receiver = 0,
                                             const char *member = 0);
   
    static HbMessageBox *displayWarningNote(QString noteText,
                                            QObject *receiver = 0,
                                            const char *member = 0);
    static QString createReplyHeader(const NmMessageEnvelope &env);
};

#endif /* NMUTILITIES_H_ */