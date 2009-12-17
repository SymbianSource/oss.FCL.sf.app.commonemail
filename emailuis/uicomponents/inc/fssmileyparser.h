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
* Description:  FsSmileyParser's implementation
*
*/


#ifndef C_FSSMILEYPARSER_H
#define C_FSSMILEYPARSER_H

#include <e32base.h>

class CFsSmileyDictionary;

class CFsSmileyParser: public CBase
    {   
    public:   
        /**
        * NewL();
        * 
        * 
        */                          
        static CFsSmileyParser* NewL();
        
        /**
        * ~CFsSmileyParser();
        * 
        * 
        */
        ~CFsSmileyParser();
        
        /**
        * SetSmileyDictionary(CFsSmileyDictionary* aSmileyDictionary);
        * 
        * @param aSmileyDictionary
        */
        void SetSmileyDictionary(CFsSmileyDictionary* aSmileyDictionary);
        
        /**
        * IsPartOfSmileyL(TBuf<1> someCharacter);
        * 
        * @param someCharacter
        */
        TBool IsPartOfSmileyL(TBuf<1> someCharacter);
        
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
        * CFsSmileyParser();
        * 
        * 
        */
        CFsSmileyParser();
    
    private:
        CFsSmileyDictionary* iSmileyDictionary;
    };
    
#endif //C_FSSMILEYPARSER_H



