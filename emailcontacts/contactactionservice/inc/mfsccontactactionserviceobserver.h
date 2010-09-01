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
* Description:  Declaration of interface MFscContactActionServiceObserver.
*
*/


#ifndef M_FSCCONTACTACTIONSERVICE_H
#define M_FSCCONTACTACTIONSERVICE_H

/**
 *  Contact action service observer.
 *  Defines an interface for contact action service observer.
 *
 *  @since S60 3.1
 */
class MFscContactActionServiceObserver
    {
    
public:
    
    /**
     * Called when QueryActionsL method is complete.
     */
    virtual void QueryActionsComplete() = 0;
        
    /**
     * Called when QueryActionsL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void QueryActionsFailed( TInt aError ) = 0;
    
    /**
     * Called when ExecuteL method is complete.
     */
    virtual void ExecuteComplete() = 0;
       
    /**
     * Called when ExecuteL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void ExecuteFailed( TInt aError ) = 0;
    
public:
    
    /**
     * Destructor.
     */
    virtual ~MFscContactActionServiceObserver() {}
    
    };

#endif // M_FSCCONTACTACTIONSERVICE_H
