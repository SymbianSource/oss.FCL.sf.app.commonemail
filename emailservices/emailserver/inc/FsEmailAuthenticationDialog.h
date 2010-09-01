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
* Description:  Source file for FS Email authentication notifier plugin
*
*/



#ifndef C_FSEMAILAUTHENTICATIONDIALOG_H
#define C_FSEMAILAUTHENTICATIONDIALOG_H

// INCLUDES
#include <e32base.h>
#include <AknQueryDialog.h>

// FORWARD DECLARATION
class CFsEmailAuthenticationPlugin;


// CLASS DECLARATION
/**
 * Class implementing Authentication (username and password) dialog
 */
class CFsEmailAuthenticationDialog : public CAknTextQueryDialog
    {
public:
    /**
    * NewL function
    * @param aStatus       active object's status variable
    * @param aMailboxName  mailbox name
    * @param aPassword     password
    * return CFsEmailAuthenticationDialog*
    */
    static CFsEmailAuthenticationDialog* NewL( TRequestStatus& aStatus,
                                               TDesC& aMailboxName,
                                               TDes& aPassword );

    /**
    * ~CFsEmailAuthenticationDialog destructor
    * @param    -
    */
    ~CFsEmailAuthenticationDialog();

private:
    /**
    * CFsEmailAuthenticationDialog default constructor
    * @param aStatus       active object's status variable
    * @param aMailboxName  mailbox name
    * @param aPassword     password
    */
    CFsEmailAuthenticationDialog( TRequestStatus& aStatus,
                                  TDesC& aMailboxName,
                                  TDes& aPassword );

    /**
    * OkToExitL destructor
    * @param  aButtonId button exit id
    * @return TBool exit or no
    */
    virtual TBool OkToExitL( TInt aButtonId );

private:
    /**
    * PreLayoutDynInitL function
    * @param    -
    */
    virtual void PreLayoutDynInitL();

private:
    
    TRequestStatus* iRequest;
    
    TDesC& iMailboxName;

    };


#endif // C_FSEMAILAUTHENTICATIONDIALOG_H

// End of File
