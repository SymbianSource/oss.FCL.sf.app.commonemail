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
* Description: 
*     Main plugin class
*
*/


#include "emailtrace.h"
#include <AlwaysOnlineManagerClient.h>
#include <SendUiConsts.h>
#include <msvids.h>
#include <msvuids.h>

// becuase of RD_IPS_AO_PLUGIN flag, can be removed
// when flag is removed
#include "ipsplgsosbaseplugin.hrh"

#include "IpsSosAOImapPopLogic.h"
#include "IpsSosAOMboxLogic.h"
#include "IpsSosAOEMNResolver.h"
#include "ipssetdataapi.h"



#include "ipsplgcommon.h"

//const TInt KAOSmtpStartDelaySeconds = 310;
const TInt KIpsSosAOImapPopLogicDefGra = 1;
//const TInt KIpsSosAoImapPopLogicEventGra = 2;

// ----------------------------------------------------------------------------
// class CIpsSosAOImapPopLogic
// ----------------------------------------------------------------------------
//
CIpsSosAOImapPopLogic::CIpsSosAOImapPopLogic( CMsvSession& aSession ) 
    : iSession( aSession ), iMailboxLogics( KIpsSosAOImapPopLogicDefGra ),
    iNoNWOpsAllowed( EFalse )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOImapPopLogic::~CIpsSosAOImapPopLogic()
    {
    FUNC_LOG;
    iMailboxLogics.ResetAndDestroy();
   // iActiveEvents.ResetAndDestroy();
    delete iEmnResolver;
    delete iDataApi;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOImapPopLogic* CIpsSosAOImapPopLogic::NewL( CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSosAOImapPopLogic* self = new (ELeave) CIpsSosAOImapPopLogic(
            aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::ConstructL()
    {
    FUNC_LOG;
    iEmnResolver = CIpsSosAOEMNResolver::NewL();
    iDataApi = CIpsSetDataApi::NewL( iSession );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOImapPopLogic::HandleAOServerCommandL( 
    TInt aCommand, TDesC8* aParameters )
    
    {
    
    FUNC_LOG;
    TInt result = KErrNone;

    TInt mailboxId = KErrNotFound;
            
    // Make sure that the parameter length matches Id length
    if ( aParameters && aParameters->Length() == sizeof( TMsvId ) )
        {
        TPckgBuf<TMsvId> paramPack;
        paramPack.Copy( *aParameters );
        
        // Get the mailbox id from the packet
        mailboxId = paramPack();
        }
    
    switch ( aCommand )
        {
        case EAOManagerPluginStart:
            RefreshMailboxListL( );
            if ( !iNoNWOpsAllowed )
                {
                SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStart );
                }
            break;
            
        case EAOManagerPluginStop:
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStop );
            // Return value does not matter, plugin will be stopped anyway.
            break;

        case EAOManagerNWOpsNotAllowed:
            iNoNWOpsAllowed = ETrue;
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStop );
            break;
            
        case EAOManagerNWOpsAllowed:
            iNoNWOpsAllowed = EFalse;
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStart );
            break;
            
        case EAOManagerStartedRoaming:
            SendCommandToMailboxesL( 
                    CIpsSosAOMBoxLogic::ECommandStartedRoaming );
            break;
            
        case EAOManagerStoppedRoaming:
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStoppedRoaming );
            break;
            
        case EAOManagerDiskSpaceAboveCritical:
            if ( !iNoNWOpsAllowed )
                {
                SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStart );
                }
            break;
            
        case EAOManagerDiskSpaceBelowCritical:
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStop );
            break;
        case EAOManagerSuicideQuery:
            // Always tell to server, that we don't 
            // want to make a suicide.
            result = EFalse;
            break;
        // Mailbox handling
        case EAOManagerMailboxAgentSuspend:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandStop );
            break;
        case EAOManagerMailboxAgentResume:
            RefreshMailboxListL( );
            if ( !iNoNWOpsAllowed )
                {
                SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandStart );
                }
            break;
        case EAOManagerMailboxAgentRemove:
            StopAndRemoveMailboxL( mailboxId );
            //cancel all
            break;
        case EAOManagerEMNReceived:
            if ( aParameters )
                {
                HandleEMNMessageL( *aParameters );
                }
            break;
        case EAOManagerMailboxAgentUpdateMailWhileConnected:
            // Not supported atm, was in old plugin Imap-idle
            break;
        case EAOManagerQueryState:
            result = QueryMailboxStatus( *aParameters );
            break;
        case EAOManagerCancelAllAndDisconnect:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandCancelAndDisconnect );
            break;
        case EAOManagerCancelAllAndDoNotDisconnect:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandCancelDoNotDiconnect );
            break;
        case EAOManagerSuspend:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandSuspend );
            break;
        case EAOManagerContinue:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandContinue );
            break;
        case EAOManagerDoNotDisconnect:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandDoNotDisconnect );
            break;
        // error in CActiveScheduler
        // EAOManagerAOSchdulerError not handled because 
        // there's no badly behaving active objects
        case EAOManagerAOSchdulerError:
        default:    
            result = KErrNotSupported;
            break;
        }
   
    
    // </cmail>
    return result;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
void CIpsSosAOImapPopLogic::HandleMsvSessionEventL(
// <cmail> RD_IPS_AO_PLUGIN flag removed
     MMsvSessionObserver::TMsvSessionEvent aEvent, 
     TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/ )
    {
    FUNC_LOG;
    switch( aEvent )
        {
        case MMsvSessionObserver::EMsvEntriesCreated:
            break;
        case MMsvSessionObserver::EMsvEntriesChanged:
            {
            TMsvId parent = (*(TMsvId*) (aArg2));
            //we check that parent is the root. if not, it cannot be a 
            //event from service, thus can't be from connection change..
            if ( parent == KMsvRootIndexEntryId )
                {
                const CMsvEntrySelection* selection = 
                    static_cast<CMsvEntrySelection*>( aArg1 );
                
                TMsvEntry tEntry;
                TMsvId service;
                if ( selection->Count() )
                    {
                    iSession.GetEntry( selection->At(0), service, tEntry );
                    }
                
                if ( !tEntry.Connected() )
                    {
                    SendCommandToSpecificMailboxL( 
                            tEntry.Id(), 
                            CIpsSosAOMBoxLogic::ECommandClearDoNotDisconnect );
                    }
                }
            }
            break;
        case MMsvSessionObserver::EMsvEntriesDeleted:
            // NOTE: if mailbox is deleted somewhere else than ips plugin
            // in here need to put logic for removing corresponding 
            // mailboxlogic object
        case MMsvSessionObserver::EMsvEntriesMoved:
        default:
            break;
        };
    // </cmail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::SendCommandToMailboxesL( 
        TInt aCommand )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iMailboxLogics.Count(); i++ )
        {
        iMailboxLogics[i]->SendCommandL( aCommand );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::StopAndRemoveMailboxL( TMsvId aMailboxId )
    {
    FUNC_LOG;
    TInt index = GetMailboxLogicIndex( aMailboxId );
    
    if ( index != KErrNotFound )
        {
        SendCommandToSpecificMailboxL( aMailboxId, 
                CIpsSosAOMBoxLogic::ECommandStop );
        delete iMailboxLogics[index];
        iMailboxLogics.Remove( index );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::SendCommandToSpecificMailboxL( 
        TMsvId aMailboxId, 
        TInt aCommand )
    {
    FUNC_LOG;
    TInt index = GetMailboxLogicIndex( aMailboxId );
    
    if ( index != KErrNotFound )
        {
        iMailboxLogics[index]->SendCommandL( aCommand ); // faulty CS warning
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::HandleEMNMessageL(
    const TDesC8& aParameters )
    {
    FUNC_LOG;
    TEMNElement elements;

    if ( iEmnResolver->ParameterDispatchTEMNElement( 
        aParameters, elements ) == KErrNone )
        {
        
        TInt index = iEmnResolver->FindEMNMailbox( 
                elements.mailbox, iMailboxLogics );
        
        if ( index != KErrNotFound )
            {
            CIpsSosAOMBoxLogic* logic = iMailboxLogics[index];
            //<cmail>
			if ( !logic->FirstEMNReceived() )
                {
                logic->SetFirstEMNReceived();
                }
            if ( !iNoNWOpsAllowed && 
                 !logic->IsMailboxRoamingStoppedL() )
            //</cmail>
                {
                SendCommandToSpecificMailboxL( 
                    logic->GetMailboxId(), // faulty CS warning
                    CIpsSosAOMBoxLogic::ECommandStartSync );
                }
            else
                {
                //<cmail>
                logic->SetEmnReceivedFlagL( ETrue );
                //</cmail>
                }
            logic = NULL;
            }
/*<cmail>
        else
            {
            }
</cmail>*/
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::RefreshMailboxListL( )
    {
    FUNC_LOG;
    CMsvEntry* cEntry = iSession.GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    
    CMsvEntrySelection* childEntries = cEntry->ChildrenWithMtmL( KSenduiMtmPop3Uid );
    CleanupStack::PushL( childEntries );
    UpdateLogicArrayL( *childEntries );
    CleanupStack::PopAndDestroy( childEntries );
    
    childEntries = cEntry->ChildrenWithMtmL( KSenduiMtmImap4Uid );
    CleanupStack::PushL( childEntries );
    UpdateLogicArrayL( *childEntries );
    CleanupStack::PopAndDestroy( childEntries );
    CleanupStack::PopAndDestroy( cEntry );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::UpdateLogicArrayL( 
        const CMsvEntrySelection& aChilds )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < aChilds.Count(); i++ )
        {
        TBool found = EFalse;
        TMsvId mboxId = aChilds.At(i);
        for ( TInt j = 0; !found && j < iMailboxLogics.Count(); j++  )
            {
            if ( iMailboxLogics[j]->GetMailboxId() == mboxId )
                {
                found = ETrue;
                }
            }
        
        CIpsSetDataExtension* extSet = CIpsSetDataExtension::NewLC();
        TRAPD( error, iDataApi->LoadExtendedSettingsL( 
                mboxId, *extSet ) );
        if ( error == KErrNone  )
            {
            if ( !found  && ( extSet->AlwaysOnlineState() != EMailAoOff || 
                    extSet->EmailNotificationState() != EMailEmnOff || 
                    !extSet->FirstEmnReceived() )
                     )
                {
                CIpsSosAOMBoxLogic* newLogic = CIpsSosAOMBoxLogic::NewL( 
                        iSession, mboxId );
                CleanupStack::PushL( newLogic );
                iMailboxLogics.AppendL( newLogic );
                CleanupStack::Pop( newLogic );
                }
            else if ( found && extSet->AlwaysOnlineState() == EMailAoOff &&
                    extSet->EmailNotificationState() == EMailEmnOff &&
                    extSet->FirstEmnReceived() )
                {
                StopAndRemoveMailboxL( mboxId );
                }
            }
        CleanupStack::PopAndDestroy( extSet );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOImapPopLogic::QueryMailboxStatus( TDesC8& aParameter )
    {
    FUNC_LOG;
    TPckgBuf<RMessage2> param;
    param.Copy( aParameter );
    // get ipc message from param
    RMessage2 msg = param();
    TPckgBuf<TMsvId> mboxBuf;
    msg.Read(0, mboxBuf);
    
    TMsvId mbox = mboxBuf();
    
    TInt error = KErrNone;
    
    TInt index = GetMailboxLogicIndex( mbox );


    if ( index != KErrNotFound )
        {
        TInt state = iMailboxLogics[index]->GetCurrentState(); // faulty CS warning

        if ( state == CIpsSosAOMBoxLogic::EStateSyncOngoing
          || state == CIpsSosAOMBoxLogic::EStateFetchOngoing )
            {
            TPckgBuf<TInt> event( EIpsAOPluginStatusSyncStarted );
            msg.Write(1, event );
            }
        else
            {
            TPckgBuf<TInt> event( EIpsAOPluginStatusSyncCompleted );
            msg.Write(1, event );
            }
        }
    else
        {
        error = KErrNotFound;
        }
    
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOImapPopLogic::GetMailboxLogicIndex( TMsvId aMailboxId )
    {
    FUNC_LOG;
    TInt index = KErrNotFound;
    for ( TInt i = 0; 
            index == KErrNotFound && i < iMailboxLogics.Count(); i++ )
        {
        if ( iMailboxLogics[i]->GetMailboxId() == aMailboxId )
            {
            index = i;
            }
        }
    return index;
    }

// End of file

