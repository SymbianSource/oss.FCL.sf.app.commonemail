/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclQueryEditor.
*
*/


#ifndef CPBKXRCLQUERYEDITOR_H
#define CPBKXRCLQUERYEDITOR_H

#include <AknQueryDialog.h>

/**
*  Provides an UI that lets user to edit the search query.
*
*  @lib pbkxrclengine.lib
*  @since S60 3.1
*/
class CPbkxRclQueryEditor : public CAknTextQueryDialog
    {
public: // constructor and destructor

    /**
    * Two-phased constructor.
    *
    * @param aAccountName name of the remote lookup account
    * @param aQueryText query text
    * @return Created object.
    */
    static CPbkxRclQueryEditor* NewL(
        const TDesC& aAccountName,
        TDes& aQueryText );
    
    /**
    * Destructor.
    */
    virtual ~CPbkxRclQueryEditor();
    
public: // new methods

    /**
    * Execute UI that lets the user edit the query and returns the edited
    * query back to the user.
    *
    * @return ETrue if user selected Ok, EFalse otherwise.
    */
    TBool ExecuteDialogLD();

private: // methods used internally

    /**
    * Sets editor input mode based on user input.
    *
    * If there is at least one alpha character then text input mode is 
    * selected. Otherwise input mode is numerical.
    */
    void SetEditorInputModeL();

private: // constructors

    /**
    * Constructor.
    *
    * @param aQueryText Handle to query text.
    */
    CPbkxRclQueryEditor( TDes& aQueryText );

    /**
    * Second-phase constructor.
    *
    * @param aAccountName Account name used to create prompt text.
    */
    void ConstructL( const TDesC& aAccountName );
    
    };

#endif // CPBKXRCLQUERYEDITOR_H

