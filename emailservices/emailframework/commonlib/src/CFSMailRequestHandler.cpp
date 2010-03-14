/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "emailtrace.h"
#include "cfsmailrequesthandler.h"
#include "cfsmailrequestobserver.h"


// ================= MEMBER FUNCTIONS ==========================================    
// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CFSMailRequestHandler
// -----------------------------------------------------------------------------
EXPORT_C CFSMailRequestHandler::CFSMailRequestHandler() : iRequestId(0)
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
EXPORT_C CFSMailRequestHandler::~CFSMailRequestHandler()
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
EXPORT_C CFSMailRequestHandler* CFSMailRequestHandler::NewLC( 
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
EXPORT_C CFSMailRequestHandler* CFSMailRequestHandler::NewL( 
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
EXPORT_C CFSMailPlugin* CFSMailRequestHandler::GetPluginByUid(TFSMailMsgId aObjectId)
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
EXPORT_C TFSPendingRequest CFSMailRequestHandler::InitAsyncRequestL( 
                                                    TUid aPluginId,
                                                    MFSMailRequestObserver& aOperationObserver)
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
EXPORT_C void CFSMailRequestHandler::CompleteRequest( TInt aRequestId )
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
EXPORT_C void CFSMailRequestHandler::CancelRequestL( TInt aRequestId )
    {
    FUNC_LOG;
        for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestId == aRequestId &&
            iPendingRequests[i].iRequestStatus == TFSPendingRequest::EFSRequestPending)
            {
            TFSMailMsgId pluginId(iPendingRequests[i].iPluginId,0);
            if(CFSMailPlugin* plugin = GetPluginByUid(pluginId))
                {
                plugin->CancelL(aRequestId);
                }
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestCancelled;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CancelAllRequestsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailRequestHandler::CancelAllRequestsL( )
    {
    FUNC_LOG;
        for(TInt i=0;i<iPendingRequests.Count();i++)
        {
            if(iPendingRequests[i].iRequestStatus == TFSPendingRequest::EFSRequestPending)
                {
                TFSMailMsgId pluginId(iPendingRequests[i].iPluginId,0);
                if(CFSMailPlugin* plugin = GetPluginByUid(pluginId))
                    {
                    plugin->CancelL(iPendingRequests[i].iRequestId);
                    }
                iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestCancelled;
                }
        }
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::AddPluginL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailRequestHandler::AddPluginL( TUid aPluginId, CFSMailPlugin* aPlugin )
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
RFile CFSMailRequestHandler::GetTempFileL(TFSMailMsgId aContentId, TFileName aFileName)
    {
    FUNC_LOG;
    
    // get temp dir path
    if(iTempDirName == NULL)
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
EXPORT_C void CFSMailRequestHandler::CleanTempDirL( )
    {
    FUNC_LOG;
       
    /* list existing temp files
    CDir* dir = NULL;
    User::LeaveIfError(iFs.GetDir(*iTempDirName, KEntryAttNormal, ESortNone, dir));
    
    // delete all temp files
    CDir files = *dir;
    for(TInt i=0;i<files.Count();i++)
        {
        iFs.Delete(files[i].iName);
        }
    
    delete dir; */
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetTempDirL
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailRequestHandler::GetTempDirL( )
    {
    FUNC_LOG;
    return *iTempDirName;
    }

