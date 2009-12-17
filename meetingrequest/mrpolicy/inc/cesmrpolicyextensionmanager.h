/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manager class for policy extensions

*
*/


#ifndef CESMRPOLICYEXTENSIONMANAGER_H

#define CESMRPOLICYEXTENSIONMANAGER_H



#include <e32base.h>

#include <barsc.h>      // RResourceFile



class CCoeEnv;



/**

 * Utility class for holding extension resource info

 */

NONSHARABLE_CLASS( CESMRExtensionResourceFile ): public CBase

    {

    public:

        static CESMRExtensionResourceFile* NewL( const TDesC& aPath, RFs& aFs );

        ~CESMRExtensionResourceFile();

    

    public: // interface

        TInt ResId() const;

        RResourceFile& ResFile();

    

    private: // implementation

        CESMRExtensionResourceFile(RFs& aFs);

        void ConstructL( const TDesC& aPath );

        

    private: // data

        RFs& iFsSession;

        RResourceFile iResFile;

        TInt iResId;

    };



/**

 * Utility class to manage policy extension's information

 */

NONSHARABLE_CLASS( CESMRPolicyExtensionManager ): public CBase

    {

    public: // C'tor and d'tor

        static CESMRPolicyExtensionManager* NewL( CCoeEnv& aCoeEnv );

        ~CESMRPolicyExtensionManager();

    

    public: // Interface

        /**

         * Resolves all extension policy files and opens those 

         * files. Reads the id of the policy.

         */

        void ReadResourcesFromExtensionsL();

        /**

         * If resource with aResourceId not found, return NULL.

         * ReadResourcesFromExtensionsL() should have been called before 

         * this. Otherwise resources are not loaded.

         */

        HBufC8* ReadBufferL( TInt aResourceId );

        

        CESMRExtensionResourceFile* ExtensionResourceFile( TInt aResourceId );

    

    private: // implementation

        CESMRPolicyExtensionManager( CCoeEnv& aCoeEnv );

        void ConstructL();

        

    private: // data        

        // Ref:

        CCoeEnv& iCoeEnv;

        // Own: array for the opened resource files

        RPointerArray<CESMRExtensionResourceFile> iResourceFiles;

    };



#endif // CESMRPOLICYEXTENSIONMANAGER_H

