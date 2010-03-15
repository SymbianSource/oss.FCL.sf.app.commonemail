/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Library to handle resource loading when 
*              CCoeEnv is not available.
*/

#ifndef __BASEPLUGINRESOURCELOADER_H__
#define __BASEPLUGINRESOURCELOADER_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>

/**
 * Library to handle resource loading when CCoeEnv is not available.
 */
class CResourceLoader : public CBase
    {
public:

    /**
     * A constructor.
     * @param aName a resource name
     * @return created object.
     */
    IMPORT_C static CResourceLoader* NewL( const TDesC& aName );

    /**
     * Destructor
     */
    virtual ~CResourceLoader();

    /**
    * @return reference to opened file session
    */
    IMPORT_C RFs& Fs();
    
    /**
     * @param aResourceId a resource key
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* LoadLC(TInt aResourceId );

    /**
     * @param aReader a reader
     * @param aResourceId a resource key
     * @return pointer to resource buffer placed on leave stack
     */
    IMPORT_C HBufC8* CreateResourceReaderLC( TResourceReader& aReader,TInt aResourceId ) const;
    
private:

    /**
    * A constructor
    */
    CResourceLoader();

    /**
    * A 2nd phase constructor
    * @param aName a HTML macro, e.g. lt
    */
    void ConstructL( const TDesC& aName );

private:

    RFs iFs;
    RResourceFile iResFile;
    };


#endif  // __BASEPLUGINRESOURCELOADER_H__
