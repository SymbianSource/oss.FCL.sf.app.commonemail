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
* Description:  Mockup Container store utils.
*
*/
#ifndef CONTAINERSTOREUTILS_H
#define CONTAINERSTOREUTILS_H


/**
 * 
 */
NONSHARABLE_CLASS( CContainerStoreUtils ) :
    public CBase
    
	{
public:
    
    static CContainerStoreUtils* NewL();
    
    void ConstructL();
    
    CContainerStoreUtils();
    
    ~CContainerStoreUtils();
    
    const TDesC& PrivatePath() const;
    
    RFs& FileSystem();
    
    void LeaveIfLowDiskSpaceL( TUint aBytesToWrite );
    
public:    
    RFs iFs;
    RBuf iPrivatePath;
	};

	
#endif // CONTAINERSTOREUTILS_H
