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
* Description:  Declaration of interface MFscContactActionPluginObserver.
*
*/


#ifndef M_FSCCONTACTACTIONPLUGIN_H
#define M_FSCCONTACTACTIONPLUGIN_H

/**
 *  Contact Action Plugin observer.
 *  Defines an interface for contact action plugin observer.
 *
 *  @since S60 3.1
 */
class MFscContactActionPluginObserver
    {
    
public:
    
    /**
     * Called when PriorityForContactSetL method is complete.
     * 
     * @param aPriority retrieved priority.
     */
    virtual void PriorityForContactSetComplete( TInt aPriority ) = 0;
    
    /**
     * Called when PriorityForContactSetL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void PriorityForContactSetFailed( TInt aError ) = 0;
    
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
    virtual ~MFscContactActionPluginObserver() {}
    
    };

#endif // M_FSCCONTACTACTIONPLUGIN_H
