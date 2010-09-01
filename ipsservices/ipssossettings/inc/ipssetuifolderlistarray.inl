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
* Description:  class TArrayFolderItem inline functions
*
*/


inline TArrayFolderItem::TArrayFolderItem ( 
        TMsvId aId, TInt32 aParent, TInt32 aIndentLevel ) : 
        iId( aId ), iParent( aParent ), iIndentLevel( aIndentLevel ) 
    {
    FUNC_LOG;
    };
    
inline TArrayFolderItem::~TArrayFolderItem()
    {
    FUNC_LOG;
    };
    
inline TMsvId TArrayFolderItem::Id( void ) 
    { 
    return iId; 
    }; 
    
inline TInt32 TArrayFolderItem::Parent( void ) 
    { 
    return iParent; 
    }; 
    
inline TInt32 TArrayFolderItem::IndentLevel( void ) 
    { 
    return iIndentLevel; 
    };
