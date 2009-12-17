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
* Description:  Converting CPbkContactItem to MVPbkStoreContact
*
*/

#include "emailtrace.h"
#include <e32base.h> // CActiveSchedulerWait
#include <coemain.h>
#include <BCardEng.h> // Old VCard engine
#include <s32mem.h>   // Memory based streams
#include <s32strm.h>  // Stream
#include <CPbkContactEngine.h> // Old phonebook contact engine
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkVCardEng.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>

#include "cpbkxrclcontactconverter.h"

const TInt KGranularitySize = 128; // "Granularity" of dynamic buffer

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclContactConverter::CPbkxRclContactConverter()
// ---------------------------------------------------------------------------
//
CPbkxRclContactConverter::CPbkxRclContactConverter(
    CPbkContactEngine&   aContactEngine, 
    CVPbkContactManager& aContactManager, 
    MVPbkContactStore&   aContactStore ): 
    iContactEngine( aContactEngine ),
    iContactManager( aContactManager ),
    iContactStore( aContactStore )
    {
    FUNC_LOG;
    // No implementation required
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPbkxRclContactConverter::~CPbkxRclContactConverter()
    {
    FUNC_LOG;
    delete iCardEngine;
    delete iVPbkCardEngine;
    delete iWait;    
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactConverter* CPbkxRclContactConverter::NewLC()
// ---------------------------------------------------------------------------
//
CPbkxRclContactConverter* CPbkxRclContactConverter::NewLC(
    CPbkContactEngine&   aContactEngine, 
    CVPbkContactManager& aContactManager, 
    MVPbkContactStore&   aContactStore )
    {
    FUNC_LOG;
    CPbkxRclContactConverter* self = new (ELeave)CPbkxRclContactConverter( 
        aContactEngine, aContactManager, aContactStore );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CPbkxRclContactConverter* CPbkxRclContactConverter::NewL()
// ---------------------------------------------------------------------------
//
CPbkxRclContactConverter* CPbkxRclContactConverter::NewL(
    CPbkContactEngine&   aContactEngine, 
    CVPbkContactManager& aContactManager,
    MVPbkContactStore&   aContactStore )
    {
    FUNC_LOG;
    CPbkxRclContactConverter* self = CPbkxRclContactConverter::NewLC( 
        aContactEngine, aContactManager, aContactStore );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactConverter::ConstructL()
// ---------------------------------------------------------------------------
//
void CPbkxRclContactConverter::ConstructL()
    {
    FUNC_LOG;
    // Crete Old VCard engine
    iCardEngine = CBCardEngine::NewL( &iContactEngine );

    // New Virtual Phonebook VCard engine  
    iVPbkCardEngine = CVPbkVCardEng::NewL( iContactManager );

    iWait = new ( ELeave ) CActiveSchedulerWait();
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactConverter::ConvertContactL()
// ---------------------------------------------------------------------------
//
void CPbkxRclContactConverter::ConvertContactL( 
        CPbkContactItem& aContactItem, 
        RFscStoreContactList& aConvertedContact  )
    {
    FUNC_LOG;
    CBufFlat* buf = CBufFlat::NewL(KGranularitySize);
    RBufWriteStream bufWriteStream;
    bufWriteStream.Open(*buf);
    CleanupClosePushL(bufWriteStream);
     
    // Export ContactCard (ContactItem ) to VCard. Sync operation.
    iCardEngine->ExportBusinessCardL( bufWriteStream, aContactItem );
     
    CleanupStack::PopAndDestroy( &bufWriteStream );

    // Create ReadStrem for VPbk VCard engine and associate it with received buf
    RBufReadStream bufReadStream;
    bufReadStream.Open(*buf);
    CleanupClosePushL(bufReadStream);
    
    // Pass new ReadStream for VPbk VCard engine and receive MVPbkStoreContact object
    MVPbkContactOperationBase* oper = iVPbkCardEngine->ImportVCardL( 
            aConvertedContact, 
            iContactStore, 
            bufReadStream, 
            *this );
    CleanupDeletePushL( oper );   

    // Now wait until async ready
    if ( !(iWait->IsStarted()) )
        {
        iWait->Start();
        }   
    
    CleanupStack::PopAndDestroy(2, &bufReadStream); // bufReadStream, oper
    delete buf;
    }
// ---------------------------------------------------------------------------
// CPbkxRclContactConverter::ConvertAndExportContactL()
// ---------------------------------------------------------------------------
//
void CPbkxRclContactConverter::ConvertAndExportContactL( 
        CPbkContactItem& aContactItem, 
        RWriteStream& aStream )
    {
    FUNC_LOG;
    RFscStoreContactList convertedContact;
    CleanupClosePushL( convertedContact );
    
    ConvertContactL( aContactItem, convertedContact );
    CleanupDeletePushL( convertedContact[0] );
    
    MVPbkContactOperationBase* oper = iVPbkCardEngine->ExportVCardL(
            aStream, 
            *convertedContact[0], 
            *this);
    CleanupDeletePushL( oper ); 
    // Now wait until async ready
    iWait->Start();

    CleanupStack::PopAndDestroy(3, &convertedContact ); 
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactConverter::VPbkSingleContactOperationComplete()
// ---------------------------------------------------------------------------
//
void CPbkxRclContactConverter::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* /* aContact */ )
    {
    FUNC_LOG;
    // The aContact in the call back is always NULL.
    iLastError = KErrNone;
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactConverter::VPbkSingleContactOperationFailed()
// ---------------------------------------------------------------------------
//
void CPbkxRclContactConverter::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    FUNC_LOG;
    iLastError = aError;
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

