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
* Description: This file defines class CFSMailServerAutoStart.
*
*/


#ifndef C_FSMAILSERVERAUTOSTART_H
#define C_FSMAILSERVERAUTOSTART_H


#include <e32base.h>

class CFSMailServerAutoStart : public CBase
{
public: 
    static CFSMailServerAutoStart* NewL();

private: 
	CFSMailServerAutoStart();
    void ConstructL();

    void StartOneApplicationL( const TDesC& aAppName ) const;
    TBool IsProcessRunningL( const TDesC& aAppName ) const;
    
};


#endif // C_FSMAILSERVERAUTOSTART_H
