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
* Description:  handles interface to CenRep settings
*
*/


#ifndef EMAILWIDGETSETTINGSMAILBOXES_H_
#define EMAILWIDGETSETTINGSMAILBOXES_H_

#include "CFSMailCommon.h"

class CFSMailClient;

/**
 * 
 *  @lib
 *  @since
 */
NONSHARABLE_CLASS( CEmailWidgetSettingsMailboxes )
    {
public:
    /**
     * Two-phased constructor.
     */
    static CEmailWidgetSettingsMailboxes* NewL();

    /**
    * Destructor.
    */
    virtual ~CEmailWidgetSettingsMailboxes();

    /**
     * 
     * @param 
     */
    void GetMailboxIdL(TInt aNum, TFSMailMsgId& aId );
    
    /**
     * 
     * @param 
     */	
    void GetMailboxNameL( TInt aNum, TDes& aMailboxName );

    /**
     * 
     */
    TInt MailboxCount();    

private:
    /**
     * Constructor
     * @param aMailClient reference to CFSMailClient class. Mailserver offers
     *        this for all handlers in order to prevent parallel instantiation
     */
    CEmailWidgetSettingsMailboxes();

    void ConstructL();

private: // data
    /**
     * Mail client reference.
     */
    CFSMailClient* iMailClient;
    };

#endif /*EMAILWIDGETSETTINGSMAILBOXES_H_*/
