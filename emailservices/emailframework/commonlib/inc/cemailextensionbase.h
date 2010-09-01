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
* Description:  Meeting Request calendar info object extension
*
*/

#ifndef CEMAILEXTENSIONBASE_H
#define CEMAILEXTENSIONBASE_H

#include <e32base.h>

/**
* Base class for extensions. Instances of subclasses are requested from
* CExtendableEmail::ExtensionL() with specific extension UID.
* When client has ended using an extension it must be released with
* CExtendableEmail::ReleaseExtension().
* @since s60 v5.0
*/
class CEmailExtension : public CBase
{

public: 
    IMPORT_C ~CEmailExtension();
protected:    
    IMPORT_C CEmailExtension( const TUid& aUid );
public:
    
    /**         
    * returns extension uid
    * @return   uid of the extension.
    */
    IMPORT_C TUid Uid() const;

public: // for internal use
                           
    /**
    * Decrement reference count.
    * @return new value of ref count
    */                           
    TUint DecRef();
    
    /**
    * Increment reference count.
    */                           
    void IncRef();
        
private:

    
    // extension interface UID defined by specific interface.
    const TUid iUid;    
    
    // Reference count is needed because more than one client may request
    // same extension (e.g. UI and protocol plugin).
    TUint iRefCount;
};

/**
* helper class for managing extensions. In most cases needed only
* by CExtendableEmail when extensions are created/released. Proto
* @since s60 v5.0
*/
class TEmailExtensions
{
public:

    /**
     * c++ constructor
     */
    IMPORT_C TEmailExtensions();
    
    /**
     * Destructor
     */
    IMPORT_C ~TEmailExtensions();
                       
    /**
     * Returns extension index in iExtensions
     @ return extension index or KErrNotFound
     */
    IMPORT_C TInt FindExtension( const TUid& aUid ) const;
    
    /**
     * Returns extension by index or panics if invalid index is given
     * @param aIndex index in iExtensions
     * @panic EEmailExtensionIndexOutOfRange if index is out of range
     * @return extension
     */
    IMPORT_C CEmailExtension* Extension( const TInt aIndex ) const;
    
    /**
     * Adds extension to iExtensions array and increments its ref count by one.
     *
     * @param aExtenstion   extension to add
     * @leave KErrArgument  if aExtension is NULL
     */
    IMPORT_C void AddL( CEmailExtension* aExtension );

    /**
     * Removes extension from iExtensions
     * @param aExtenstion extension to remove
     */
    IMPORT_C void Remove( const CEmailExtension* aExtension );
    
private:    
    // array of extensions
    RPointerArray<CEmailExtension> iExtensions;
};

/**                 
* Classes that support email extension mechanism by returning extensions by
* uid should have this as base class.
*
* @since s60 v5.0
*/                 
class CExtendableEmail : public CBase
{
public:
    /**
    * Decrements extension reference count and deletes if count reaches 0.
    * Derived class must call this if it overrides this method.
    * @param aExtension extension to release
    */
    IMPORT_C virtual void ReleaseExtension( CEmailExtension* aExtension );

    /**
    * Requests extension. Default implementation performs lookup only and
    * derived class should implement actual instantiation because it knows
    * extensions it supports. Derived class must always call this to ensure
    * that extension is not created twice.
    *
    * @param aExtension extension to release
    */
    IMPORT_C virtual CEmailExtension* ExtensionL( const TUid& aInterfaceUid );

protected:    
    // extension containment
    TEmailExtensions iExtensions;
};

#endif // CEMAILEXTENSIONBASE_H
