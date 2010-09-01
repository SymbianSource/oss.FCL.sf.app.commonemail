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
* Description:  Exported utilities for FS Email
*
*/


#ifndef FREESTYLEEMAILEXPORTEDUTILITIES_H_
#define FREESTYLEEMAILEXPORTEDUTILITIES_H_

/**
*  CleanupResetAndDestroyClose
*
*  Use CleanupResetAndDestroyClosePushL to push RPointerArray items
*   on the cleanup stack. Use CleanupStack::PopAndDestroy( &ptrarray )
*   to resetanddestroy + close the pointer array.
*
*/
template<typename T> class CleanupResetAndDestroyClose
    {
public:
    inline static void PushL( T& aRef );
private:
    static void Close( TAny *aPtr );
    };

template<typename T> inline void CleanupResetAndDestroyClosePushL( T& aRef );

template<typename T> inline void CleanupResetAndDestroyClose<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &Close, &aRef ) );
    }

template<typename T> void CleanupResetAndDestroyClose<T>::Close( TAny *aPtr )
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    static_cast<T*>(aPtr)->Close();
    }

template<typename T> inline void CleanupResetAndDestroyClosePushL( T& aRef )
    {
    CleanupResetAndDestroyClose<T>::PushL( aRef );
    }

#endif /*FREESTYLEEMAILEXPORTEDUTILITIES_H_*/
