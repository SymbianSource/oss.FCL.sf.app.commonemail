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
* Description:  Implementation of the class CFsUiDataMtm.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <MtmExtendedCapabilities.hrh>
#include <msvuids.h>
#include <msvids.h>
#include <mtmuids.h>
#include <coemain.h>    
#include <AknIconUtils.h>
#include <data_caging_path_literals.hrh>

#include <fsuidatamtm.mbg>
#include "fsmtmsuids.h" 
#include <fsuidatamtm.rsg>
#include "fsmtmsconstants.h"
#include "cfsuidatamtm.h"

// FREESTYLE EMAIL FRAMEWORK INCLUDES
#include "cfsmailcommon.h"
#include "cfsmailclient.h"
#include "cfsmailbox.h"


#include <gulicon.h>
#include "mfsmailbrandmanager.h"

// GLOBAL EXPORTS.

// ---------------------------------------------------------------------------
// Panic
// ---------------------------------------------------------------------------
//
GLDEF_C void Panic(TInt aPanic)
    {

    User::Panic(_L("FsUiMtms"), aPanic);
    
    }

// ---------------------------------------------------------------------------
// NewMtmUiDataLayerL
// ---------------------------------------------------------------------------
//
EXPORT_C CBaseMtmUiData* NewMtmUiDataLayerL( 
    CRegisteredMtmDll& aRegisteredDll )
    // Factory function
    {
    return CFsUiDataMtm::NewL( aRegisteredDll );
    }

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CFsUiDataMtm
// ---------------------------------------------------------------------------
//
CFsUiDataMtm::CFsUiDataMtm( CRegisteredMtmDll& aRegisteredMtmDll ):
    CBaseMtmUiData( aRegisteredMtmDll )
{
    FUNC_LOG;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::ConstructL
// ---------------------------------------------------------------------------
//
void CFsUiDataMtm::ConstructL()
{
    FUNC_LOG;
    CBaseMtmUiData::ConstructL();
    // No need to allocated as ConstructL will alocate and baseclass itself delets
    // memory allocated.
    // iIconArrays = new (ELeave) CArrayPtrSeg<CBitmapArray>(2); 
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CFsUiDataMtm
// ---------------------------------------------------------------------------
//
CFsUiDataMtm::~CFsUiDataMtm()
{
    FUNC_LOG;
    
    iMailboxIds.Close();
    
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CFsUiDataMtm* CFsUiDataMtm::NewL( 
    CRegisteredMtmDll& aRegisteredMtmDll )
{
    FUNC_LOG;

    CFsUiDataMtm* self = new(ELeave) CFsUiDataMtm( aRegisteredMtmDll );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::PopulateArraysL
// Initialises bitmaps and function information.
// ---------------------------------------------------------------------------
//
void CFsUiDataMtm::PopulateArraysL()
{
    FUNC_LOG;
    
    // Populate function array
    ReadFunctionsFromResourceFileL( R_FS_FUNCTION_ARRAY ); 

    // Add the icon information here.
    TFileName dllFileName;
    Dll::FileName( dllFileName );      
    TParse parse;
    parse.Set( KFsUiDataMtmUdBitmapFile, &KDC_BITMAP_DIR, &dllFileName );
    TFileName bitmapFileName= parse.FullName();     
    
    // Support for White label branding added
    CFSMailClient* mailClient = CFSMailClient::NewL();
    CleanupClosePushL(*mailClient);
    MFSMailBrandManager& brandManager = mailClient->GetBrandManagerL();
    RPointerArray<CFSMailBox> mailboxes;
    CleanupResetAndDestroyClosePushL( mailboxes );
    TFSMailMsgId id;

    mailClient->ListMailBoxes( id, mailboxes );

    iIconArrays->Reset();
    iMailboxIds.Reset();

    for ( TInt i = 0; i <= mailboxes.Count(); i++ ) // Last round is for setting the default mb icon at the end of the list
        {
        CFbsBitmap* bitmap(0);
        CFbsBitmap* bitmapMask(0);
        CArrayPtrFlat<CFbsBitmap>* array = new (ELeave) CArrayPtrFlat<CFbsBitmap>( 2 );
        CleanupStack::PushL( array ); // +2 array
        array->SetReserveL( 2 ); // AppendLs will not LEAVE
        
        CGulIcon* brandedIcon( NULL );
        TInt err(KErrNone);

        if ( i < mailboxes.Count() ) // Do not execute for the last round
            {
            TRAP( err, brandedIcon = brandManager.GetGraphicL( EFSMailboxIcon, mailboxes[i]->GetId() ) );
            }
        if ( err == KErrNone && brandedIcon )
            {
            bitmap = brandedIcon->Bitmap();
            bitmapMask = brandedIcon->Mask();
            brandedIcon->SetBitmapsOwnedExternally( ETrue );
            CleanupStack::PushL( bitmap );
            array->AppendL( bitmap );
            CleanupStack::Pop( bitmap );
            bitmap = 0;
            CleanupStack::PushL( bitmapMask );
            array->AppendL( bitmapMask );
            CleanupStack::Pop( bitmapMask );
            bitmapMask = 0;
            iIconArrays->AppendL( array );
            iMailboxIds.Append( (mailboxes[i]->GetId()).Id() );
            }
        else
            {
            // Icon was not found from the branding manager or last round for the default icon
            TRAPD( err, AknIconUtils::CreateIconL( bitmap, bitmapMask, bitmapFileName,
                    EMbmFsuidatamtmQgn_prop_cmail_inbox_small, EMbmFsuidatamtmQgn_prop_cmail_inbox_small + 1 ));
            if( err != KErrNone )
                {
                }
            CleanupStack::PushL( bitmap );
            array->AppendL( bitmap );
            CleanupStack::Pop( bitmap );
            bitmap = 0;
            CleanupStack::PushL( bitmapMask );
            array->AppendL( bitmapMask );
            CleanupStack::Pop( bitmapMask );
            bitmapMask = 0;
            // INFO: This is not working for some reason, otherwise above code is not required.
            // CreateBitmapsL(2, KFsUiDataMtmUdBitmapFile, EMbmFsuidatamtmFsmailbox, 
            // EMbmFsuidatamtmLastElement);
            iIconArrays->AppendL( array );
            iMailboxIds.Append(0);
            }
        if ( brandedIcon )
            {
            delete brandedIcon;
            }
        CleanupStack::Pop( array );  
        }
    
    CleanupStack::PopAndDestroy( &mailboxes );
    CleanupStack::PopAndDestroy( mailClient );

}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CFsUiDataMtm
// ---------------------------------------------------------------------------
//
void CFsUiDataMtm::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, 
    TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
{
    FUNC_LOG;
    //For now don't do anything.
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::GetResourceFileName
// From CBaseMtmUiData: Gets the resource file name associated with MTM Ui 
// Data
// ---------------------------------------------------------------------------
//
void CFsUiDataMtm::GetResourceFileName(TFileName &aFileName) const
{
    FUNC_LOG;
    
    TFileName dllFileName;
    Dll::FileName( dllFileName );      
    TParse parse;
    parse.Set( KFsUiDataMtmUdResourceFile, &KDC_MTM_RESOURCE_DIR, &dllFileName );
    aFileName= parse.FullName();   
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::ContextIcon
// ---------------------------------------------------------------------------
//
const CBaseMtmUiData::CBitmapArray& CFsUiDataMtm::ContextIcon( 
    const TMsvEntry& aContext, TInt /*aStateFlags*/ ) const
{
    FUNC_LOG;
    for ( TInt i = 0; i < iIconArrays->Count() - 1; i++ )
        {
        if ( iMailboxIds[i] == aContext.iMtmData2 )
            {       
            return *(iIconArrays->At( i ));
            }
        }
    return *(iIconArrays->At( iIconArrays->Count() - 1)); // Default icon
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::MtmSpecificFunctions
// ---------------------------------------------------------------------------
//
const CArrayFix<CBaseMtmUiData::TMtmUiFunction>& CFsUiDataMtm::MtmSpecificFunctions() const
{
    FUNC_LOG;
    return *iMtmSpecificFunctions;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanCancelL
// Tests if the current operation on the entry can be cancelled.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanCancelL(
    const TMsvEntry &/*aContext*/, TInt &aReasonResourceId ) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanCloseEntryL()
// Tests if a close operation is supported.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanCloseEntryL( const TMsvEntry &/*aContext*/, 
    TInt &/*aReasonResourceId*/ ) const
{
    FUNC_LOG;
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanCopyMoveFromEntryL
// Tests whether a copy from or move from operation is supported.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/, TInt& aReasonResourceId ) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanCopyMoveToEntryL
// Tests if a copy from or move to operation is supported.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/, TInt& aReasonResourceId ) const
{
    FUNC_LOG;
    
    //ActiveSync Does not support Copying or Moving a Mail into the Inbox.
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanCreateEntryL
// Tests if an entry can be created as a child.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanCreateEntryL( const TMsvEntry& aParent,  
    TMsvEntry &aNewEntry, TInt &aReasonResourceId ) const
{
    FUNC_LOG;
    if ((aNewEntry.iMtm == KUidMsgValTypeFsMtmVal) && 
        (aNewEntry.iType == KUidMsvServiceEntry) )
        {
        aReasonResourceId=0;
        // --- Can create services if they are off root ---
        return (aParent.Id() == KMsvRootIndexEntryIdValue);
        }
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanDeleteFromEntryL
// Tests if a delete operation is supported.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanDeleteFromEntryL( const TMsvEntry &/*aContext*/, 
    TInt &aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanDeleteServiceL
// Tests if the service entry can be deleted.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanDeleteServiceL(
    const TMsvEntry& /*aService*/, TInt& aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanEditEntryL( )
// Tests if the entry can be edited.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanEditEntryL( const TMsvEntry &/*aContext*/, 
    TInt &aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanForwardEntryL
//
// Tests if an entry can be forwarded.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanForwardEntryL( const TMsvEntry &/*aContext*/, 
    TInt &aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId=R_FS_NOT_SUPPORTED;

    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanOpenEntryL
// Tests if an entry can be opened.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanOpenEntryL( const TMsvEntry& /*aContext*/, 
    TInt& aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanReplyToEntryL
// Tests if an entry can be replied to.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanReplyToEntryL( const TMsvEntry& /*aContext*/, 
    TInt& aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanViewEntryL
// Tests if an entry can be viewed.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanViewEntryL( const TMsvEntry &/*aContext*/, 
    TInt &aReasonResourceId) const
{
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
}

// ---------------------------------------------------------------------------
// CFsUiDataMtm::CanUnDeleteFromEntryL
// Tests if the entry can be undeleted.
// ---------------------------------------------------------------------------
//
TBool CFsUiDataMtm::CanUnDeleteFromEntryL( const TMsvEntry&/*aContext*/, 
    TInt &aReasonResourceId) const
    {
    FUNC_LOG;
    
    aReasonResourceId = R_FS_NOT_SUPPORTED;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CFsUiDataMtm::OperationSupportedL
// Tests if an MTM-specific operation is appropriate to the specified entry.
// ---------------------------------------------------------------------------
//
TInt CFsUiDataMtm::OperationSupportedL(
    TInt aOperationId, const TMsvEntry &/*aContext*/) const
    {
    FUNC_LOG;
    
    switch(aOperationId)
        {
        case KMtmUiFunctionSendAs:
            return KErrNone;  // Operation *IS* supported
        default:
            break;
        }
    return R_FS_NOT_SUPPORTED;
    }
// ---------------------------------------------------------------------------
// CFsUiDataMtm::QueryCapability
// Queries if the MTM supports a particular capability, specified by a UID.
// ---------------------------------------------------------------------------
//
TInt CFsUiDataMtm::QueryCapability( TUid aCapability, TInt &aResponse) const
    {
    FUNC_LOG;

    RProcess process;//SCANNER
    TSecureId sid = process.SecureId();
    TInt rc = KErrNone;
    switch ( aCapability.iUid )
        {
        // These are defined in mtmdef.hrh in epoc32\include
        case KUidMtmQueryMaxBodySizeValue:          
            aResponse = KMaxTInt;
            break;

        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse=KMaxTInt;
            break;

        case KUidMtmQuerySupportedBodyValue:
            aResponse = KMtm7BitBody | KMtm8BitBody | KMtm16BitBody;
            break;

        case KUidMsvMtmQuerySupportEditor:
            aResponse=ETrue;
            break;

        // --- Supported non-valued capabilities ---
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQueryCanCreateNewMsgValue: //0x10008b24 - 268471076
        // INFO - Because of one of these Freestyle is coming under
        // Writemessga ->Shortmessage, Multimediamesage, .. Freestyle
        // When Freestyle Install is not yet executed but MTM resouce file 
        // is compiled.               
        // Solution - Compare with sid.iID EAknsMinorQgnMenuMceLst = 0x100058C5
        // defined in aknsconstants.hrh.

            //Disabling SendAs for Calendar is removed.
            aResponse=ETrue;
            break;

        default:
            rc = KErrNotSupported;
     }

    return rc;
    }
// ---------------------------------------------------------------------------
// CFsUiDataMtm::StatusTextL
// Provides status text for the sending state of the entry.
// ---------------------------------------------------------------------------
//
HBufC* CFsUiDataMtm::StatusTextL( const TMsvEntry& /*aContext*/ ) const
    {
    FUNC_LOG;

    return HBufC::NewL(0);
    }

// End of file

