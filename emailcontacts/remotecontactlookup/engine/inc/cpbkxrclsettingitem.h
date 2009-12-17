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
* Description:  Definition of the class CPbkxRclResultI
*
*/


#ifndef CPBKXRCLSETTINGITEM_H
#define CPBKXRCLSETTINGITEM_H

#include <aknsettingitemlist.h>

class CPbkxRemoteContactLookupProtocolAccount;

/**
* Class for showing RCL account selection setting item.
*
* Selected value is written to central repository.
*/
class CPbkxRclSettingItem : public CAknEnumeratedTextPopupSettingItem
    {
public: // constructor and destructor
    
    /**
    * Creates new setting item.
    *
    * @return New setting item.
    */
    static CPbkxRclSettingItem* NewL();

    /**
    * Creates new setting item.
    *
    * Created item is left on cleanup stack.
    *
    * @return New setting item.
    */
    static CPbkxRclSettingItem* NewLC();

    /**
    * Destructor.
    */
    virtual ~CPbkxRclSettingItem();
    
public: // virtual methods from base class

    /**
    * @see CAknEnumeratedTextPopupSettingItem
    */
    virtual void EditItemL( TBool aCalledFromMenu );
    
private: // methods used internally

    /**
    * Creates all protocol adapters and asks all protocol accounts from
    * each.
    */
    void LoadProtocolAccountsL();

    /**
    * Initializes text arrays which are shown to the user.
    */
    void FillTextArraysL();

    /**
    * Reads account information from central repository and sets the correct
    * account as selected.
    */
    void SetSelectedAccountL();

    /**
    * Stores selected account information to central repository.
    */
    void StoreSettingL();
    
private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclSettingItem();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private: // data

    // Selected item index.
    TInt iSelectedItem;

    // Enumeration texts. Not owned.
    CArrayPtr<CAknEnumeratedText>* iEnumTexts;

    // Popped up texts. Not owned.
    CArrayPtr<HBufC>* iPoppedUpTexts;
    
    // Protocol accounts. Owned.
    RPointerArray<CPbkxRemoteContactLookupProtocolAccount> iProtocolAccounts;

    // Offset to resource file.
    TInt iResourceFileOffset;
    
    };

#endif

