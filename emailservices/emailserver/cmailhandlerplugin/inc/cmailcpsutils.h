/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class to handle content publishing for widget
*
*/


#ifndef CMAILCPSUTILS_H
#define CMAILCPSUTILS_H

#include <memailmessagedata.h> // TEmailPriority

/**
 *  Utility methods for cps handler
 *  @lib fsmailserver.exe
 *  @since S60 v5.2
 */
NONSHARABLE_CLASS( TMailCpsUtils )
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */ 
    static TInt ResolveIcon( 
        TBool aUnread,
        TBool aCalMsg,
        TBool aAttas,
        EmailInterface::TEmailPriority aPrio,
        TBool aRe,
        TBool aFw );
    };

#endif  // CMAILCPSUTILS_H
