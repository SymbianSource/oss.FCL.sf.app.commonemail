/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclvCardSender.
*
*/


#ifndef CPBKXRCLVCARDSENDER_H
#define CPBKXRCLVCARDSENDER_H

#include <e32base.h>

class CPbkContactEngine;
class CPbkContactItem;
class CBCardEngine;
class CSendUi;
class CEikonEnv;
class CRichText;
class CPbkxRclContactConverter;

/**
* Class that handles sending business card using CSendUI.
*
* @lib pbkxrclengine.lib
* @since 3.1
*/
class CPbkxRclvCardSender : public CBase
    {
public: // constructor and destructor

    /**
    * Creates new object.
    *
    * @param aContactEngine Handle to contact engine.
    * @return Created object.
    */
    static CPbkxRclvCardSender* NewL( CPbkContactEngine& aContactEngine );

    /**
    * Creates new object.
    *
    * Pointer to the created object is left in the cleanupstack.
    *
    * @param aContactEngine Handle to contact engine.
    * @return Created object.
    */
    static CPbkxRclvCardSender* NewLC( CPbkContactEngine& aContactEngine );

    /**
    * Destructor.
    */
    virtual ~CPbkxRclvCardSender();

public: // new methods

    /**
    * Send business card of the given contact item.
    *
    * @param aContactItem Contact item which is sent as business card.
    */
    void SendvCardL( 
            CPbkContactItem& aContactItem, 
            CPbkxRclContactConverter& aContactConverter );
    
private: // methods used internally

    /**
    * Creates temporary file of the contact item.
    *
    * @param aContactItem Contact item.
    */
    void CreateTempFileL( 
            CPbkContactItem& aContactItem, 
            CPbkxRclContactConverter& aContactConverter );

    /**
    * Destroys temporary file.
    */
    void DestroyTempFileL();

    /**
    * Creates rich text object of the temporary file.
    *
    * @return Created rich text object. Ownership is transferred.
    */
    CRichText* CreateRichTextFromFileLC();

    /**
    * Creates filter.
    */
    void CreateFilterL();

    //<cmail> getting temporary file name is not hard coded
    /**
    * Creates a temporary file name on device memory, in private 
    * folder of the process, which loads this dll.
    *
    * @param aFileName file name is returned here
    * @return error code
    */
    TInt GetTemporaryFileNameL( TFileName& aFileName, CPbkContactItem& aContactItem );
    //</cmail>

private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclvCardSender();

    /**
    * Second-phase constructor.
    *
    * @param aContactEngine Handle to contact engine.
    */
    void ConstructL( CPbkContactEngine& aContactEngine );

private: // data

    // Used to send the business card. Owned.
    CSendUi* iSendUi;

    // Card engine. Owned.
    CBCardEngine* iCardEngine;

    // Filter. Owned.
    CArrayFixFlat<TUid>* iFilter;
   
    // Eikon environment. Not owned.
    CEikonEnv* iEikEnv;
    //<cmail>
    // file name for temporary storage
    TFileName iTempFileName;
    //</cmail>
 
    };

#endif

