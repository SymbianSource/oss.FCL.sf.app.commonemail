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
* Description:  FsSmileyDictionary's implementation
*
*/


#ifndef C_FSSMILEYDICTIONARY_H
#define C_FSSMILEYDICTIONARY_H

#include <e32base.h>

class TSmiley
    {
    public:
        TSmiley():iText(NULL)
            {
            
            }
            
        ~TSmiley()
            {
            delete iText;
            }
    public:
        HBufC* iText;
        TFileName iFileName;
    };

class CFsSmileyDictionary: public CBase
    {   
    public:    
        /**
        * NewL();
        * 
        * 
        */                         
        IMPORT_C static CFsSmileyDictionary* NewL();
        
        /**
        * ~CFsSmileyDictionary();
        * 
        * 
        */
        IMPORT_C ~CFsSmileyDictionary();
        
        /**
        * AddNewSmileyL(TSmiley* aSmiley);
        * 
        * @param aSmiley
        */
        void AddNewSmileyL(TSmiley* aSmiley);
        
        /**
        * AddNewSmileyL(const TDesC& aText, TFileName aFileName);
        * 
        * @param aText
        * @param aFileNamext
        */
        IMPORT_C void AddNewSmileyL(const TDesC& aText, TFileName aFileName);
        
        /**
        * CheckDictionaryL(TBuf<1> someCharacter);
        * 
        * @param someCharacter
        */
        TBool CheckDictionaryL(TBuf<1> someCharacter);
        
        /**
        * ResetL();
        * 
        * 
        */
        void ResetL();
        
        /**
        * IsSmiley();
        * 
        * 
        */
        TBool IsSmiley();
        
        /**
        * GetSmileyIndex();
        * 
        * 
        */
        TInt GetSmileyIndex();
        
        /**
        * GetSmileyLength(TInt aSmileyIndex);
        * 
        * @param aSmileyIndex
        */
        TInt GetSmileyLength(TInt aSmileyIndex);
        
        /**
        * GetSmileyFileName(TInt aSmileyIndex);
        * 
        * @param aSmileyIndex
        */
        TFileName GetSmileyFileName(TInt aSmileyIndex);
    
    private:
        /**
        * ConstructL();
        * 
        * 
        */
        void ConstructL();
        
        /**
        * CFsSmileyDictionary();
        * 
        * 
        */
        CFsSmileyDictionary();
    
    private:
        RArray<TSmiley*> iSmileysDictionary;
        TInt iLongestSmiley;
        TInt* iResults;
        TInt iSizeOfResultsTable;
        TInt iEndIndex;
        TInt iPositionOfCheckingLetter;
        TBool iSmileyFound;
        TInt iSmileyIndex;
    };
    
#endif //C_FSSMILEYDICTIONARY_H


