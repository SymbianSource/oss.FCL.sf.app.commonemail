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
* Description: This file implements class CFsSmileyDictionary.
*
*/
#include "emailtrace.h"
#include "fssmileydictionary.h"

EXPORT_C CFsSmileyDictionary* CFsSmileyDictionary::NewL()
    {
    FUNC_LOG;
    CFsSmileyDictionary* self = new(ELeave)CFsSmileyDictionary();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
EXPORT_C CFsSmileyDictionary::~CFsSmileyDictionary()
    {
    FUNC_LOG;
    for(int i = 0 ; i < iSmileysDictionary.Count() ; ++i)
        {
        TSmiley *smiley = iSmileysDictionary[i];
        delete smiley;
        }
      
    delete []iResults;
    iSmileysDictionary.Close();    
    }

void CFsSmileyDictionary::AddNewSmileyL(TSmiley* aSmiley)
    {
    FUNC_LOG;
    iSmileysDictionary.AppendL(aSmiley);
    if(aSmiley->iText->Length() > iLongestSmiley)
        {
        iLongestSmiley = aSmiley->iText->Length();
        }
        
    iEndIndex = iSmileysDictionary.Count();
    }   
    
EXPORT_C void CFsSmileyDictionary::AddNewSmileyL(
        const TDesC& aText, 
        TFileName aFileName)
    {
    FUNC_LOG;
    TSmiley* smiley = new(ELeave)TSmiley;
    
    smiley->iText = aText.AllocL();
    smiley->iFileName = aFileName;
    
    iSmileysDictionary.AppendL(smiley);
    if(smiley->iText->Length() > iLongestSmiley)
        {
        iLongestSmiley = smiley->iText->Length();
        }
        
    iEndIndex = iSmileysDictionary.Count();
    }

TBool CFsSmileyDictionary::CheckDictionaryL(TBuf<1> someCharacter) 
    {
    FUNC_LOG;
    TBool retVal = ETrue;
    TInt newEndPosition = 0;
    
    //poprzednia litera nie byla smileyem
    if( -1 == iPositionOfCheckingLetter )
        {
        ResetL();
        iPositionOfCheckingLetter = 0;
        }
    
    for(int i = 0 ; i < iEndIndex ; ++i)
        {
        TSmiley* smiley = iSmileysDictionary[iResults[i]];
        
        if(smiley->iText->Length() > iPositionOfCheckingLetter 
                && 0 == someCharacter.Mid(0, 1)
                  .Compare(smiley->iText->Mid(iPositionOfCheckingLetter, 1)))
            {
            iResults[newEndPosition++] = iResults[i];
            }
        }
    
    if(0 == newEndPosition)
        {
        retVal = EFalse;
        
        //sprawdzic co sie dzialo
        if(0 == iPositionOfCheckingLetter)
            {
            //ani jedna literka nie przeszla
            }
        else
            {
            //jakies literki przeszly  
            for(int i = 0 ; i < iEndIndex ; ++i)
                {
                TSmiley* smiley = iSmileysDictionary[iResults[i]];
                
                if(smiley->iText->Length() == iPositionOfCheckingLetter)
                    {
                    iSmileyFound = ETrue;
                    iSmileyIndex = iResults[i];
                    }
                }  
            }
        iPositionOfCheckingLetter = -1;
        }
    else
        {
        iEndIndex = newEndPosition; 
        ++iPositionOfCheckingLetter; 
        }
    
    return retVal;
    }

TInt CFsSmileyDictionary::GetSmileyLength(TInt aSmileyIndex)
    {
    FUNC_LOG;
    return iSmileysDictionary[aSmileyIndex]->iText->Length();
    }
    
TFileName CFsSmileyDictionary::GetSmileyFileName(TInt aSmileyIndex)
    {
    FUNC_LOG;
    TFileName retVal;
    if ( aSmileyIndex >= 0 && aSmileyIndex < iSmileysDictionary.Count() )
        {
        retVal = iSmileysDictionary[aSmileyIndex]->iFileName;
        }
    return retVal; 
    }

TInt CFsSmileyDictionary::GetSmileyIndex()
    {
    FUNC_LOG;
    return iSmileyIndex;
    }

TBool CFsSmileyDictionary::IsSmiley()
    {
    FUNC_LOG;
    return iSmileyFound;
    }
    
void CFsSmileyDictionary::ResetL()
    {
    FUNC_LOG;
    if(0 != iSmileysDictionary.Count() && 
       (!iResults || 
       iSizeOfResultsTable != iSmileysDictionary.Count()))
        {
        delete []iResults;
        
        iSizeOfResultsTable = iSmileysDictionary.Count();
        
        iResults = new(ELeave)TInt[iSizeOfResultsTable];
        }
        
    for(TInt i = 0 ; i < iSizeOfResultsTable ; ++i)
        {
        iResults[i] = i;
        }
            
    iEndIndex = iSizeOfResultsTable;
    iSmileyFound = EFalse;
    }
    
void CFsSmileyDictionary::ConstructL()
    {
    FUNC_LOG;
    
    }
        
CFsSmileyDictionary::CFsSmileyDictionary():iPositionOfCheckingLetter(-1)
    {
    FUNC_LOG;
        
    }


