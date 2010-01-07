/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mockup Container store content manager.
*
*/
#ifndef __CONTAINERSTORECONTENTMANAGER_H__
#define __CONTAINERSTORECONTENTMANAGER_H__


#include <f32file.h>


NONSHARABLE_CLASS( CContainerStoreContentManager ) : CBase
    {
public:

    CContainerStoreContentManager();

    void ReplaceFileWithFileL(
        RFile& aSource,
        const TDesC& aTarget );
    
    void PrependBufferAndMoveFileL(
        const TDesC& aSource,
        const TDesC& aTarget,
        const TDesC8& aPrepend );
    
    TInt TransferContentFile(
        TContainerId aId, 
        const TDesC& aContentPath );
    
    void SetLeaveAfter( TInt aLeaveAfter );
    
    void SimulateLeaveL();

private:
    TInt iLeaveAfter;
    };
		
#endif // __CONTAINERSTORECONTENTMANAGER_H__
