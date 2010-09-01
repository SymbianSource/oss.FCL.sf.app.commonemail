/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CFsEmailMessageQueryDialog
*
*/


#ifndef C_FSEMAILMESSAGEQUERYDIALOG_H
#define C_FSEMAILMESSAGEQUERYDIALOG_H

// INCLUDES
#include <e32base.h>
#include <aknmessagequerydialog.h>

//<cmail>
#include "fsmailserverconst.h"
//</cmail>

// FORWARD DECLARATION
class CFsEmailMessageQueryPlugin;


// CLASS DECLARATION
/**
 * Class implementing MessageQuery (username and password) dialog
 */
class CFsEmailMessageQueryDialog : public CAknMessageQueryDialog
    {
public:
    /**
    * NewL function
    * @param aStatus    active object's status variable
    */
    
    static CFsEmailMessageQueryDialog* NewLC( TRequestStatus& aStatus,
                                              TDesC& aMailboxName,
                                              TFsEmailNotifierSystemMessageType aMessageType,
                                              const TDesC& aCustomMessageText = KNullDesC );

    /**
    * ~CFsEmailMessageQueryDialog destructor
    * @param    -
    */
    ~CFsEmailMessageQueryDialog();

private: // From base class CAknMessageQueryDialog
    /**
    * OkToExitL destructor
    * @param  aButtonId button exit id
    * @return TBool exit or no
    */
    virtual TBool OkToExitL( TInt aButtonId );

private: // Own functions
    /**
    * CFsEmailMessageQueryDialog default constructor
    * @param aStatus    active object's status variable
    */
    CFsEmailMessageQueryDialog( TRequestStatus& aStatus,
                                TDesC& aMailboxName,
                                TFsEmailNotifierSystemMessageType aMessageType );

    void PrepareDialogLC( const TDesC& aCustomMessageText = KNullDesC );

    TBool GetResourceIdsL();
    void SetMessageTextAndCbaToDialogL();
    void SetMessageTextAndCbaToDialogL( const TDesC& aText );
    void SetDialogHeadingL();
    
private:

    TRequestStatus* iRequest;
    
    TFsEmailNotifierSystemMessageType iMessageType;
    TDesC& iMailboxName;
    TInt iMsgTxtResourceId;     // Message text resource id
    TInt iCbaResourceId;        // Cba buttons resource id
    TBool iAddMailboxNameToText;

    };


#endif // C_FSEMAILMESSAGEQUERYDIALOG_H

// End of File
