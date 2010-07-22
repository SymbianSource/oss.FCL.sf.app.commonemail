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

#ifndef NMAPIPOPIMAPSETTINGSMANAGER_H
#define NMAPIPOPIMAPSETTINGSMANAGER_H

#include <nmapimailboxsettingsdata.h>

namespace EmailClientApi {

class NmApiPopImapSettingsManager 
{
public:       
    NmApiPopImapSettingsManager();
    virtual ~NmApiPopImapSettingsManager();
    bool populateDefaults(const QString &mailboxType, NmApiMailboxSettingsData &data);

private:
    void populateImapDefs(NmApiMailboxSettingsData &data);
    void populatePopDefs(NmApiMailboxSettingsData &data);
    
};
}//namespace

#endif // NMAPIPOPIMAPSETTINGSMANAGER_H
