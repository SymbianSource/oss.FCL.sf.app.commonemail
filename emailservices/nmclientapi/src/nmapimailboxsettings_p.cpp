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
 *
 */

#include "nmapiheaders.h"


namespace EmailClientApi
{
NmApiMailboxSettingsPrivate::NmApiMailboxSettingsPrivate(QObject *parent) : 
    QObject(parent), 
    mFactory(NULL),
    mDeleteMailboxOperation(NULL)
{
    NM_FUNCTION;
    mFactory = NmApiDataPluginFactory::instance();
    Q_CHECK_PTR(mFactory);
}

NmApiMailboxSettingsPrivate::~NmApiMailboxSettingsPrivate()
{
    NM_FUNCTION;
    NmApiDataPluginFactory::releaseInstance(mFactory);
    if(mDeleteMailboxOperation && mDeleteMailboxOperation->isRunning()) {
        mDeleteMailboxOperation->cancelOperation();        
    }
}

bool NmApiMailboxSettingsPrivate::listMailboxIds(QList<quint64> &idList)
{
    NM_FUNCTION; 
    QList<NmId> mailboxIds;
    bool ret = false;
    idList.clear();
    
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        if (instance->listMailboxIds(mailboxIds) == KErrNone) {
            
            foreach (NmId boxId, mailboxIds) {
                        idList.append(boxId.id());
                    }
            
            ret = true;   
            }
        } 
     
    return ret;
}

bool NmApiMailboxSettingsPrivate::loadSettings(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    Q_UNUSED(mailboxId);
    Q_UNUSED(data);
    return false;
}

bool NmApiMailboxSettingsPrivate::saveSettings(const NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    Q_UNUSED(data);
    return false;
}

bool NmApiMailboxSettingsPrivate::createMailbox(
    const QString &mailboxType,
    NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    Q_UNUSED(mailboxType);
    Q_UNUSED(data);
    bool ret = false;
    
    /*QT_TRY {
        if (mailboxType==NmApiMailboxTypePop || mailboxType==NmApiMailboxTypeImap) {
            QScopedPointer<NmApiPopImapSettingsManager> popImapManager(new NmApiPopImapSettingsManager());  
            popImapManager->createMailbox(mailboxType, data);
            ret = true;
        }
        else {
            ret = false;
        }
        //TODO store cenrep stuff here
    }
    QT_CATCH(...){
        ret = false;
    }*/
    
    return ret;
}

bool NmApiMailboxSettingsPrivate::deleteMailbox(quint64 mailboxId)
{
    NM_FUNCTION;
    bool ret = false;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        if (mDeleteMailboxOperation && mDeleteMailboxOperation->isRunning()) {
            mDeleteMailboxOperation->cancelOperation();
        }
        mDeleteMailboxOperation = instance->deleteMailboxById(NmId(mailboxId));
        
        if (mDeleteMailboxOperation) {
            ret = true;
            connect(mDeleteMailboxOperation, 
                    SIGNAL(operationCompleted(int)), 
                    this, 
                    SIGNAL(mailboxDeleted(int)));
        }
    }
    return ret;
}

bool NmApiMailboxSettingsPrivate::populateDefaultSettings(
    const QString &mailboxType, NmApiMailboxSettingsData &data) 
{
    NM_FUNCTION;
    Q_UNUSED(mailboxType);
    Q_UNUSED(data);
    /*QScopedPointer<NmApiPopImapSettingsManager> popImapManager(new NmApiPopImapSettingsManager());  
    return popImapManager->populateDefaults(mailboxType, data);*/
    return true;
}

}// namespace
