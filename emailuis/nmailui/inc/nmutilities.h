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

/*!
	UI utilities class
*/
class NmUtilities
{    
public:
    enum NmAddressValidationType {
        ValidAddress,
        InvalidAddress,
        Default
    };
    
    static void getRecipientsFromMessage( const NmMessage &message, 
        QList<NmAddress> &recipients,
        NmAddressValidationType type = Default );
    
    static bool isValidEmailAddress( const QString &emailAddress );
    
    static QString addressToDisplayName( const NmAddress &address );
    
    static bool parseEmailAddress( const QString &emailAddress, NmAddress &address );
    
    static QString cleanupDisplayName( const QString &displayName );     

    static int openFile(QFile &file);
    
    static int openFile(RFile &file);
    
    static QString truncate( const QString &string, int length );
};

#endif /* NMUTILITIES_H_ */
