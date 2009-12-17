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
* Description:  Implementation of the class CFsMtmUi
*
*/


// INCLUDE FILES
// Messaging includes
#include "emailtrace.h"
#include <msvuids.h>
#include <msvids.h>
#include <eikappui.h>
#include <eikenv.h>
#include <coeaui.h>
#include <data_caging_path_literals.hrh>

#include "FreestyleEmailUiConstants.h"
#include "fsmtmsconstants.h"
#include "cfsuimtm.h"
#include "cfssendashelper.h"

// ---------------------------------------------------------------------------
// CFsMtmUi::NewL
// Exported factory function
// ---------------------------------------------------------------------------
//
EXPORT_C CFsMtmUi* CFsMtmUi::NewL( CBaseMtm& aBaseMtm, 
    CRegisteredMtmDll& aRegisteredMtmDll)
    {
    FUNC_LOG;
    
    CFsMtmUi* self = new(ELeave) CFsMtmUi( aBaseMtm, aRegisteredMtmDll );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CFsMtmUi
// ---------------------------------------------------------------------------
//
CFsMtmUi::CFsMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    : CBaseMtmUi(aBaseMtm, aRegisteredMtmDll )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::~CFsMtmUi
// ---------------------------------------------------------------------------
//
CFsMtmUi::~CFsMtmUi()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::ConstructL
// ---------------------------------------------------------------------------
//
void CFsMtmUi::ConstructL()
    {
    FUNC_LOG;

    CBaseMtmUi::ConstructL();
    
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CancelL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::CancelL( TRequestStatus& /*aStatus*/, 
    const CMsvEntrySelection& /*aSelection*/ )
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL; //Statisfies the compiler
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::GetResourceFileName
// Resource file loading 
// ---------------------------------------------------------------------------
//
void CFsMtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    FUNC_LOG;
    TFileName dllFileName;
    Dll::FileName( dllFileName );      
    TParse parse;
    parse.Set( KFsMtmUiResourceFile, &KDC_MTM_RESOURCE_DIR, &dllFileName );
    aFileName= parse.FullName();   

    
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::OpenL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::OpenL( TRequestStatus& aStatus )
    {
    FUNC_LOG;
    
   	CEikAppUi* appUi = CEikonEnv::Static()->EikAppUi();
    CCoeAppUi* coeAppUi = static_cast<CCoeAppUi*>( appUi );
    TVwsViewId viewId = TVwsViewId( KFSEmailUiUid, MailListId );
	TMailListActivationData tmp;
	
    TMsvId accountId = iBaseMtm.Entry().Entry().iServiceId;	
    CMsvEntry* accountEntry = CMsvEntry::NewL( Session(), accountId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ));	
    CleanupStack::PushL( accountEntry );
    
	tmp.iMailBoxId = TFSMailMsgId( accountEntry->Entry().MtmData1(), accountEntry->Entry().MtmData2() );
    
   
	TPckgBuf<TMailListActivationData> pkgOut( tmp );
	coeAppUi->ActivateViewL( viewId, KStartListWithFolderId, pkgOut);

    CleanupStack::PopAndDestroy( accountEntry );
	
    
    TBufC8<1> buf;  
    return CMsvCompletedOperation::NewL( Session(), Type(), buf, 
            KMsvLocalServiceIndexEntryId, aStatus, KErrNone ); 
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::OpenL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::OpenL( TRequestStatus& aStatus, 
    const CMsvEntrySelection& /*aSelection*/ )
    {
    FUNC_LOG;

    return OpenL(aStatus);
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CloseL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::CloseL(TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CloseL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::CloseL( TRequestStatus& /*aStatus*/, 
    const CMsvEntrySelection& /*aSelection*/ )
    {
    FUNC_LOG;
    User::Leave(KErrNotSupported); 
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CreateL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::CreateL( const TMsvEntry& aEntry, 
    CMsvEntry& /*aParent*/, TRequestStatus& aStatus )
    {
    FUNC_LOG;
    // INFO: By doing this the accountEntry will get further details that are 
    // required to launch the respective accounts in email UI.
    TMsvId accountId = aEntry.iServiceId;
    CMsvEntry* accountEntry = CMsvEntry::NewL( Session(), accountId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ));
    CleanupStack::PushL( accountEntry );
    

    CFsSendAsHelper* sendAsHelper = CFsSendAsHelper::NewL();
    CleanupStack::PushL( sendAsHelper );

    // Send Plugin Id and Mailbox Id for Launching.
    TUint pluginId = accountEntry->Entry().MtmData1();
    TUint id = accountEntry->Entry().MtmData2();
    sendAsHelper->LaunchUiL( pluginId, id, ETrue, EFalse );

    CleanupStack::PopAndDestroy( sendAsHelper );
    CleanupStack::PopAndDestroy( accountEntry );
    TBufC8<1> buf;  
    
    //Complete ourselves.
    return CMsvCompletedOperation::NewL( Session(), Type(), buf, 
        KMsvLocalServiceIndexEntryId, aStatus, KErrNone );
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::EditL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::EditL( TRequestStatus& aStatus )
    {
    FUNC_LOG;

    TUid type = iBaseMtm.Entry().Entry().iType;
	//__ASSERT_DEBUG(type==KUidMsvMessageEntry || type==KUidMsvServiceEntry,
	//		Panic(ETextMtmUiWrongEntryType));
    TMsvId accountId = iBaseMtm.Entry().Entry().iServiceId;
    CMsvEntry* accountEntry = CMsvEntry::NewL( Session(), accountId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ));
    CleanupStack::PushL( accountEntry );    
    TUint puginId = accountEntry->Entry().MtmData1();
    TUint id = accountEntry->Entry().MtmData2();
    CFsSendAsHelper* sendAsHelper = CFsSendAsHelper::NewL();
    CleanupStack::PushL( sendAsHelper );
    if ( type == KUidMsvMessageEntry )
		{	    
	    // FS UI when - Createmessage -> email -> FS Email account    
	    sendAsHelper->LaunchUiL( puginId, id, EFalse, EFalse );
		}
	else
		{
	    // Settings when - Messages -> Options->Email Settings -> Email account.
	    sendAsHelper->LaunchUiL( puginId, id, EFalse, ETrue );
		}
    CleanupStack::PopAndDestroy( sendAsHelper );
    CleanupStack::PopAndDestroy( accountEntry );
    TBufC8<1> buf;
    
    //Complete ourselves.
    return CMsvCompletedOperation::NewL( Session(), Type(), buf, 
        KMsvLocalServiceIndexEntryId, aStatus, KErrNone );
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::EditL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::EditL( TRequestStatus& /*aStatus*/, 
    const CMsvEntrySelection& /*aSelection*/ )
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::ViewL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::ViewL( TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::ViewL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::ViewL( TRequestStatus& /*aStatus*/, 
    const CMsvEntrySelection& /*aSelection*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::DeleteFromL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::DeleteFromL( const CMsvEntrySelection& /*aSelection*/,
                                       TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CopyToL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::CopyToL( const CMsvEntrySelection& /*aSelection*/, 
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::MoveToL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::MoveToL( const CMsvEntrySelection& /*aSelection*/, 
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::CopyFromL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::CopyFromL( const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aTargetId*/, TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::MoveFromL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::MoveFromL( const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aTargetId*/, TRequestStatus& /*aStatus*/ )
// Moves entries in selection from current context to aTargetId id    
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported ); 
    
    return NULL;     
    }

//
//    MTM-specific functionality
//
// ---------------------------------------------------------------------------
// CFsMtmUi::InvokeSyncFunctionL
// ---------------------------------------------------------------------------
//
void CFsMtmUi::InvokeSyncFunctionL( TInt /*aFunctionId*/, 
    const CMsvEntrySelection& /*aSelection*/, TDes8& /*aParameter*/ )
    {
    FUNC_LOG;

    User::Leave(KErrNotSupported); 
    
    }

// ---------------------------------------------------------------------------
// CFsMtmUi::InvokeAsyncFunctionL
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::InvokeAsyncFunctionL( TInt /*aFunctionId*/, 
    const CMsvEntrySelection& /*aSelection*/, 
    TRequestStatus& /*aCompletionStatus*/, TDes8& /*aParameter*/ )
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported); 
    
    return NULL;     
    }

//
//    Message responding
//
// ---------------------------------------------------------------------------
// CFsMtmUi::ReplyL
// Reply to message - no UI support
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::ReplyL( TMsvId /*aDestination*/, 
    TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/ )
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    return NULL;     

    }

// ---------------------------------------------------------------------------
// CFsMtmUi::ForwardL
// Forwarded message - no UI support
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmUi::ForwardL( TMsvId /*aDestination*/, 
    TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/ )
    {
    FUNC_LOG;
    
    User::Leave(KErrNotSupported);
    
    return NULL;     
    }

//
//    Progress information
//
// ---------------------------------------------------------------------------
// CFsMtmUi::GetProgress
// ---------------------------------------------------------------------------
//
TInt CFsMtmUi::GetProgress( const TDesC8& /*aProgress*/, 
    TBuf<EProgressStringMaxLen>& /*aReturnString*/, 
    TInt& /*aTotalEntryCount*/, TInt& /*aEntriesDone*/,
    TInt& /*aCurrentEntrySize*/, TInt& /*aCurrentBytesTrans*/) const
    {
    FUNC_LOG;
    return KErrNone;     
    }

// End of file

