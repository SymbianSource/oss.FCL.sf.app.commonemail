/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     Email message related operations
 */

#include <nmapimessagemanager.h>
#include <nmapidatapluginfactory.h>
#include <nmdataplugininterface.h>
#include <nmapifolder.h>
#include <nmcommon.h>
#include "nmapimessagemanager_p.h"
#include "emailtrace.h"

namespace EmailClientApi
{

NmApiMessageManagerPrivate::NmApiMessageManagerPrivate(quint64 mailboxId,QObject *parent)
: QObject(parent),
  mState(NmApiMessageManagerPrivate::EIdle), mMailboxId(mailboxId)
{
    NM_FUNCTION;	
	
    mFactory = NmApiDataPluginFactory::instance();
    mPlugin = mFactory->plugin();    
    if(mPlugin) {
        connect(mPlugin,
                SIGNAL(messageEvent(NmMessageEvent,
                                    const NmId&,
                                    const QList<NmId>&,
                                    const NmId&)),
                this,
                SLOT(messageEventHandler(NmMessageEvent,
                                    const NmId&,
                                    const QList<NmId>&,
                                    const NmId&)));
    }				
    NmDataPluginInterface *interface = mFactory->interfaceInstance();
    interface->subscribeMailboxEvents(mailboxId);
	
}

NmApiMessageManagerPrivate::~NmApiMessageManagerPrivate()
{
    NM_FUNCTION;
    NmDataPluginInterface *interface = mFactory->interfaceInstance();
    interface->unsubscribeMailboxEvents(mMailboxId);
    NmApiDataPluginFactory::releaseInstance(mFactory);
}
    
/*!
 \fn moveMessages 
 \param messageIds Id list of source messages.
 \param sourceFolderId Id of the source folder.
 \param targetFolderId Id of the target folder.
 \return true if operation was successfully started.
 
 Starts async move operation for given messages.  
 Completion signalled with messagesMoved(int result).
 */
bool NmApiMessageManagerPrivate::moveMessages(const QList<quint64> messageIds,
											quint64 sourceFolder,
											quint64 targetFolder)
{
    NM_FUNCTION;
    Q_UNUSED(messageIds);
    Q_UNUSED(sourceFolder);
    Q_UNUSED(targetFolder);
    return false;
}
    
/*!
 \fn copyMessages 
 \param messageIds Id list of source messages.
 \param sourceFolder Id of the source folder.
 \param targetFolder Id of the target folder.
 \return true if operation was successfully started.
 
 Starts async copy operation for given messages.  
 Completion signalled with messagesCopied(int result).
 */
bool NmApiMessageManagerPrivate::copyMessages(const QList<quint64> messageIds,
                                            const quint64 sourceFolder,
											const quint64 targetFolder)
{
    NM_FUNCTION;
    bool ret = false;
    NmId targetFolderNmId(targetFolder);
	NmId sourceFolderNmId(sourceFolder);
	mTarget = targetFolderNmId;
	mMessages = messageIds;
	
	NmDataPluginInterface *interface = mFactory->interfaceInstance();
	
    if (interface) {
        mState = NmApiMessageManagerPrivate::ECopyPending;
        if (interface->copyMessages(mMailboxId,
                                  messageIds,
                                  sourceFolderNmId,
                                  targetFolderNmId)==0) {			
            ret = true;
        }        
    }
	mState = NmApiMessageManagerPrivate::EIdle;
	return ret;
}
  
/*!
 \fn messageEventHandler 
 \param event Event type.
 \param folder Folder id.
 \param messages Id list of messages involved.
 \param mailBox Id of mailbox.
  
 Handler for plugin originated messageEvent.
 */
void NmApiMessageManagerPrivate::messageEventHandler(NmMessageEvent event,
												const NmId &folder,
												const QList<NmId> &messages,
												const NmId &mailBox)
{	
	if(mMailboxId == mailBox &&
	   mTarget == folder) {	
        switch(event)
        {
            case NmMessageCreated: {
                if (mState==NmApiMessageManagerPrivate::ECopyPending) {
                    if(messages.count()==mMessages.count()) {
                        emit messagesCopied(0);
                    }
                    else {
                        emit messagesCopied(-1);
                    }
                }
                break;			
            }
            
            case NmMessageChanged: {
                break;		
            }
            
            case NmMessageDeleted: {            
                break;
            }
                
            case NmMessageFound: {
                break;
            }
            
            default: {
                break;
            }
        }
        mState = NmApiMessageManagerPrivate::EIdle;
	}
}
} //namespace


