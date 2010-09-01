/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  email internal extension base class
*
*/

#include "cemailextensionbase.h"
#include "emailtrace.h"

/**
*
*/
enum TEmailFwPanic {
    EEmailExtensionIndexOutOfRange
    };

_LIT( KEmailExtensionPanic, "EmailFw" );
    
void Panic( TEmailFwPanic aPanic )
    {
    FUNC_LOG;
    User::Panic( KEmailExtensionPanic, aPanic );
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CEmailExtension::CEmailExtension( const TUid& aUid ) : 
    iUid( TUid::Uid(aUid.iUid ) )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CEmailExtension::~CEmailExtension()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CEmailExtension::Uid() const
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CEmailExtension::DecRef()
    {
    if ( iRefCount )
        {
        iRefCount--;
        }
    return iRefCount;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailExtension::IncRef()
    {
    ++iRefCount;
    }
        
// ---------------------------------------------------------------------------
// deletes extension and removes it from extension array
// ---------------------------------------------------------------------------
//
EXPORT_C void CExtendableEmail::ReleaseExtension( CEmailExtension* aExtension )
    {
    FUNC_LOG;
    if ( !aExtension->DecRef() )
        {
        iExtensions.Remove( aExtension );
        delete aExtension;
        }
    }
    
// ---------------------------------------------------------------------------
// Finds and returns extension
// ---------------------------------------------------------------------------
//
EXPORT_C CEmailExtension* CExtendableEmail::ExtensionL( const TUid& aInterfaceUid )
    {
    FUNC_LOG;
    TInt index = iExtensions.FindExtension( aInterfaceUid );
    CEmailExtension* ext = NULL;
    if ( index != KErrNotFound )
        {
        ext = iExtensions.Extension( index );
        ext->IncRef();
        }
    return ext;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
EXPORT_C TEmailExtensions::~TEmailExtensions()
    {
    iExtensions.Close();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TEmailExtensions::TEmailExtensions() : iExtensions( 1 )
    {
    }

// ---------------------------------------------------------------------------
// returns index of extension in extension array
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TEmailExtensions::FindExtension( const TUid& aUid ) const
    {
    TInt index = KErrNotFound;
    for ( TInt i = 0; i < iExtensions.Count(); i++ )
        {        
        const CEmailExtension* tested = iExtensions[i];
        if ( aUid == tested->Uid() )
            {
            index = i;
            break;
            }
        }
    return index;
    }

// ---------------------------------------------------------------------------
// Returns extension by index. Panics if index is out of range.
// ---------------------------------------------------------------------------
//
EXPORT_C CEmailExtension* TEmailExtensions::Extension( const TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex>=0 && aIndex < iExtensions.Count(),
        Panic( EEmailExtensionIndexOutOfRange ) );
    return iExtensions[aIndex];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void TEmailExtensions::AddL( CEmailExtension* aExtension )
    {
    FUNC_LOG;
    if ( !aExtension )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PushL( aExtension );
    iExtensions.AppendL( aExtension );    
    CleanupStack::Pop();
    aExtension->IncRef();
    }

// ---------------------------------------------------------------------------
// Removes extension from array
// ---------------------------------------------------------------------------
//
EXPORT_C void TEmailExtensions::Remove( 
    const CEmailExtension* aExtension )
    {
    FUNC_LOG;
    const TInt index( FindExtension( aExtension->Uid() ) );
    if ( index != KErrNotFound )    
        {
        iExtensions.Remove( index );
        }    
    }

// End of file    
