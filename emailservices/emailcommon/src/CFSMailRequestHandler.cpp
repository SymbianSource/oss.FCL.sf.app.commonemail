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
* Description:  plugin request handler
*
*/


#include <nmcommonheaders.h>
#include "emailtrace.h"
#include "CFSMailRequestHandler.h"
#include "CFSMailRequestObserver.h"


// ================= MEMBER FUNCTIONS ==========================================  
// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CFSMailRequestHandler
// -----------------------------------------------------------------------------
 CFSMailRequestHandler::CFSMailRequestHandler() : iRequestId(0)
{
    FUNC_LOG;

    // store pointer to TLS
    TInt err = Dll::SetTls(static_cast<TAny*>(this));

    // reset 
    iPendingRequests.Reset();
    iPluginList.Reset();
  
    iTempDirName =NULL;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::~CFSMailRequestHandler
// -----------------------------------------------------------------------------
 CFSMailRequestHandler::~CFSMailRequestHandler()
{
    FUNC_LOG;
    TRAP_IGNORE( CancelAllRequestsL() );
    RemoveAllRequests();
    iPendingRequests.Reset();
    iPluginList.ResetAndDestroy();
    
    if(iTempDirName)
        {
        delete iTempDirName;
        // close file server connection
        iFs.Close();        
        }
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::NewLC
// -----------------------------------------------------------------------------
 CFSMailRequestHandler* CFSMailRequestHandler::NewLC( 
                  RPointerArray<CImplementationInformation>& aPluginInfo,
                  RPointerArray<CFSMailPlugin>& aPlugins )
{
    FUNC_LOG;
    CFSMailRequestHandler* pluginHandler = new (ELeave) CFSMailRequestHandler();
    CleanupStack:: PushL(pluginHandler);
    pluginHandler->ConstructL( aPluginInfo, aPlugins );
    return pluginHandler;
} 

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::NewL
// -----------------------------------------------------------------------------
 CFSMailRequestHandler* CFSMailRequestHandler::NewL( 
                  RPointerArray<CImplementationInformation>& aPluginInfo,
                  RPointerArray<CFSMailPlugin>& aPlugins )
{
    FUNC_LOG;
    CFSMailRequestHandler* pluginHandler =  
          CFSMailRequestHandler::NewLC( aPluginInfo, aPlugins );
    CleanupStack:: Pop(pluginHandler);
    return pluginHandler;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::ConstructL
// -----------------------------------------------------------------------------
void CFSMailRequestHandler::ConstructL( 
                RPointerArray<CImplementationInformation> /*aPluginInfo*/,
                RPointerArray<CFSMailPlugin> /*aPlugins*/ )
{
    FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetPluginByUid
// -----------------------------------------------------------------------------
 CFSMailPlugin* CFSMailRequestHandler::GetPluginByUid( TFSMailMsgId aObjectId )
{
    FUNC_LOG;

    for(TInt i=0;i<iPluginList.Count();i++)
        {
        if(iPluginList[i]->iPluginId.iUid == aObjectId.PluginId().iUid)
            {
            return iPluginList[i]->iPlugin;
            }
        }
    return NULL;
}
  
// -----------------------------------------------------------------------------
// CFSMailRequestHandler::InitAsyncRequestL
// -----------------------------------------------------------------------------
 TFSPendingRequest CFSMailRequestHandler::InitAsyncRequestL( 
                          TUid aPluginId,
                          MFSMailRequestObserver& aOperationObserver )
{
    FUNC_LOG;
    TFSPendingRequest newRequest;
    for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestStatus != TFSPendingRequest::EFSRequestPending)
            {
            iPendingRequests[i].iPluginId = aPluginId;
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestPending;
            iPendingRequests[i].iObserver->SetUserObserver(aOperationObserver);     
            newRequest = iPendingRequests[i];
            return newRequest;
            }
        }

    newRequest.iRequestId = iPendingRequests.Count();
    newRequest.iPluginId = aPluginId;
    newRequest.iRequestStatus = TFSPendingRequest::EFSRequestPending;     
    CFSMailRequestObserver* observer = 
            CFSMailRequestObserver::NewL(*this, aOperationObserver);
    newRequest.iObserver = observer;
    iPendingRequests.Append(newRequest);
      
    return newRequest;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CompleteRequest
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CompleteRequest( TInt aRequestId )
{
    FUNC_LOG;
    for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestId == aRequestId)
            {
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestComplete;
            break;
            }
        }
}
  
// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CancelRequestL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CancelRequestL( TInt aRequestId )
{
    FUNC_LOG;
    for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestId == aRequestId 
            && iPendingRequests[i].iRequestStatus == TFSPendingRequest::EFSRequestPending)
            {
            TFSMailMsgId pluginId(iPendingRequests[i].iPluginId,0);
            CFSMailPlugin* plugin = GetPluginByUid(pluginId);
            plugin->CancelL(aRequestId);
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestCancelled;
            break;
            }
        }
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CancelAllRequestsL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CancelAllRequestsL( )
{
    FUNC_LOG;
    for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestStatus == TFSPendingRequest::EFSRequestPending)
            {
            TFSMailMsgId pluginId(iPendingRequests[i].iPluginId,0);
            CFSMailPlugin* plugin = GetPluginByUid(pluginId);
            plugin->CancelL(iPendingRequests[i].iRequestId);
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestCancelled;
            }
        }
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::AddPluginL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::AddPluginL( TUid aPluginId, CFSMailPlugin* aPlugin )
{
    FUNC_LOG;

    CFSMailPluginData* pluginData = new (ELeave) CFSMailPluginData;
    pluginData->iPluginId = aPluginId;
    pluginData->iPlugin = aPlugin;
    iPluginList.Append(pluginData);       
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::RemoveAllRequests
// -----------------------------------------------------------------------------
void CFSMailRequestHandler::RemoveAllRequests()
{
    FUNC_LOG;
    for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        delete iPendingRequests[i].iObserver;
        iPendingRequests[i].iObserver = NULL;
        }
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetTempFileL( )
// -----------------------------------------------------------------------------
// <qmail>
 RFile CFSMailRequestHandler::GetTempFileL( TFSMailMsgId aContentId, TFileName aFileName )
// </qmail>
{
    FUNC_LOG;
    
    // get temp dir path
    // <qmail>
    if(!iTempDirName)
    // </qmail>
        {
        // open file server connection
        User::LeaveIfError(iFs.Connect());
        iFs.CreatePrivatePath(EDriveC);
        iTempDirName = HBufC::NewL(20);
        TPtr ptr = iTempDirName->Des();
        iFs.PrivatePath(ptr);        
        }
    
    // create temp file
    RFile file;
    aFileName.Append(iTempDirName->Des());
    aFileName.AppendNum(aContentId.PluginId().iUid);
    aFileName.AppendNum(aContentId.Id());
    file.Replace(iFs,aFileName,EFileWrite);
    return file;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CleanTempDirL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CleanTempDirL( )
{
    FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetTempDirL
// -----------------------------------------------------------------------------
 TDesC& CFSMailRequestHandler::GetTempDirL( )
{
    FUNC_LOG;
    return *iTempDirName;
}

