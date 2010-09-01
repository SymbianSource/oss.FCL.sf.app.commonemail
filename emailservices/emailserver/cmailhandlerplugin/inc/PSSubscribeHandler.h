/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*       Interface for handling P&S subscribe events
*/

#ifndef __PSSUBSCRIBEHANDLER_H__
#define __PSSUBSCRIBEHANDLER_H__

/**
*  Callback interface for CPSSubscriber.
*/
class MPSSubscribeHandler
    {
    public:
		/**
        * Through this method the Property subscribers notify of Property changes
        * @param aCategory The category of the property to be observed
		* @param aKey The subkey of the property to be observed
		*/
        virtual void HandlePropertyChangedL( const TUid& aCategory, TInt aKey ) = 0;
    };

#endif //__PSSUBSCRIBEHANDLER_H__
