/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  InitTestCase test module.
*
*/



#ifndef SEARCH_TESTS_H
#define SEARCH_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CSearchTests : public CAsyncTestCaseBase, public MMsgStoreSearchClient
    {
    public:  // Constructors and destructor

        static CSearchTests* NewL( CStifLogger* aLog );
        
        virtual ~CSearchTests();

    private:
        
        CSearchTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
        // inherited from MMsgStoreSearchClient        
        virtual void MatchFound( CMsgStorePropertyContainer*& aMessage );   
        virtual void SearchCompleted();

        void ValidateSearchResults( RArray<TMsgStoreId>& aExpectedMatches, TBool aIgnoreExtra = EFalse );
        
        void ReplaceMessageContentWithFileL( CMsgStoreMessage* aMessage, const TDesC& KFilename );
        
        void SetSubjectAndAddressL( CMsgStoreMessage* aMessage, 
                                   const TDesC& aSubject, 
                                   const TDesC& aDisplayName, 
                                   const TDesC& aAddress, 
                                   const TDesC8& aAddressType );
        
    private:
        
        CMsgStoreMailBox*   iSessionToCancelSearch;
        TBool               iSearchCompleted;
        RArray<TMsgStoreId> iMatches;
        RPointerArray<CMsgStoreMessage> iMessages;
        RMsgStoreSearchCriteria*        iSearchCriteria;
    };

#endif      // SEARCH_TESTS_H

// End of File
