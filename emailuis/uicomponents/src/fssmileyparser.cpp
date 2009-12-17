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
* Description: This file implements class CFsSmileyParser.
*
*/

#include "emailtrace.h"
#include "fssmileyparser.h"
#include "fssmileydictionary.h"

CFsSmileyParser* CFsSmileyParser::NewL()
    {
    FUNC_LOG;
    CFsSmileyParser* self = new(ELeave)CFsSmileyParser();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CFsSmileyParser::~CFsSmileyParser()
    {
    FUNC_LOG;
    }

void CFsSmileyParser::ConstructL()
    {
    FUNC_LOG;
    
    }
        
CFsSmileyParser::CFsSmileyParser()
    {
    FUNC_LOG;
        
    }

TInt CFsSmileyParser::GetSmileyLength(TInt aSmileyIndex)
    {
    FUNC_LOG;
    return iSmileyDictionary->GetSmileyLength(aSmileyIndex);
    }
    
TFileName CFsSmileyParser::GetSmileyFileName(TInt aSmileyIndex)
    {
    FUNC_LOG;
    return iSmileyDictionary->GetSmileyFileName(aSmileyIndex);
    }

TInt CFsSmileyParser::GetSmileyIndex()
    {
    FUNC_LOG;
    return iSmileyDictionary->GetSmileyIndex();
    }

TBool CFsSmileyParser::IsPartOfSmileyL(TBuf<1> someCharacter)
    {
    FUNC_LOG;
    return iSmileyDictionary->CheckDictionaryL(someCharacter);
    }
   
TBool CFsSmileyParser::IsSmiley()
    {
    FUNC_LOG;
    return iSmileyDictionary->IsSmiley();
    }

void CFsSmileyParser::SetSmileyDictionary(
        CFsSmileyDictionary* aSmileyDictionary)
    {
    FUNC_LOG;
    iSmileyDictionary = aSmileyDictionary;
    }


