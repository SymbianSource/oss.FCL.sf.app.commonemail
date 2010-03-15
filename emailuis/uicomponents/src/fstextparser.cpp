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
* Description:  Implementation of class CFsTextParser
*
*/



//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alftextstylemanager.h>
// </cmail>
#include <eikenv.h>
#include <imagedata.h>
// <cmail>
#include <AknUtils.h>
// </cmail>
#include <fldinfo.h>

#include "fstextparser.h"
#include "fsrichtexttext.h"
#include "fsrichtextpicture.h"
#include "fsrichtextsmiley.h"
#include "fsrichtextnewline.h"
#include "fsrichtext.h"
#include "fsrichtextfields.h"
#include "fssmileyparser.h"
#include "fstextureloader.h"
#include "fstexture.h"
#include "fstextstylemanager.h"


#include "fbs.h"


CFsTextParser::CFsTextParser(
        CFsRichText& aText,
        CAlfEnv& aEnv,
        CFsTextureLoader* aTextureLoader):
    iText(aText),
    iTextureLoader(aTextureLoader),
    iEnv(aEnv),
    iTextStyleManager(NULL),
    iIsWordTooLong(EFalse),
    iIsSetSizeOfSmiley(EFalse),
    iGetCharsUsed(EFalse),
    iGetCharsUsedOfNextBlock(EFalse),
    iNewLineArrayIndex(0),
    iLastNewLinePositionIndex(0)
    {
    FUNC_LOG;

    }

CFsTextParser* CFsTextParser::NewL(
        CFsRichText& aText,
        CAlfEnv& aEnv,
        CFsTextureLoader* aTextureLoader)
    {
    FUNC_LOG;
    CFsTextParser * self = new(ELeave) CFsTextParser(
            aText,
            aEnv,
            aTextureLoader );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFsTextParser::~CFsTextParser()
    {
    FUNC_LOG;
    iExpandArray.Close();
    iHotSpotArray.Close();
    iFieldLocation.Close();
    iTextureIndex.Close();
    iNewLineArray.Close();
    iLineDirection.Close();

    delete iSmileyParser;
    }

void CFsTextParser::SetSizeOfSmile(TSize aSizeOfSmiley)
    {
    FUNC_LOG;
    iIsSetSizeOfSmiley = ETrue;
    iSizeOfSmiley = aSizeOfSmiley;
    }

void CFsTextParser::ConstructL()
    {
    FUNC_LOG;
    iEikon = CEikonEnv::Static();
    }

TBool CFsTextParser::SetParsingPosition( TInt aPosition )
    {
    FUNC_LOG;
    TBool retVal = ETrue;

    if( aPosition < 0 || aPosition >= iText.DocumentLength())
        {
        retVal = EFalse;
        }
    else
        {
        iPosition = aPosition;
        }

    return retVal;
    }

void CFsTextParser::MoveBack()
    {
    FUNC_LOG;
    iPosition = iLastPosition;
    }

TInt CFsTextParser::IsWordPartOfExpandArea(
        TInt aStartPosition,
        TInt aEndPosition )
    {
    FUNC_LOG;
    TInt retVal = -1;

    for(TInt i = 0 ; i < iExpandArray.Count() ; ++i)
        {
        if(iExpandArray[i].iStartIndex >= aStartPosition &&
            iExpandArray[i].iStartIndex < aEndPosition)
            {
            if(-1 == retVal)
                {
                retVal = i;
                }
            else
                {
                if ( iExpandArray[i].iStartIndex
                        < iExpandArray[retVal].iStartIndex )
                    {
                    retVal = i;
                    }
                }
            }
        else if ( aStartPosition >= iExpandArray[i].iStartIndex
                && aStartPosition
                  < iExpandArray[i].iStartIndex + iExpandArray[i].iLength )
            {
            if(-1 == retVal)
                {
                retVal = i;
                }
            else
                {
                if ( iExpandArray[i].iStartIndex
                        < iExpandArray[retVal].iStartIndex )
                    {
                    retVal = i;
                    }
                }
            }
        }
    return retVal;
    }


void CFsTextParser::CutWord(TInt aBeginOfWord, TInt aNewLengthOfWord)
    {
    FUNC_LOG;
    iIsWordTooLong = ETrue;
    iPosition = aBeginOfWord;
    iNewLengthOfWord = aNewLengthOfWord;
    }

// ---------------------------------------------------------------------------
// GetTextL
// ---------------------------------------------------------------------------
HBufC* CFsTextParser::GetTextL(TInt aStart, TInt aEnd) const
    {
    FUNC_LOG;
    HBufC* retVal = HBufC::NewL( aEnd - aStart + 1 );
//    TBuf<200> someText01;
    if(aStart < 0 || aEnd > iText.DocumentLength() || aEnd < aStart)
        {
        User::Leave(KErrArgument);
        }

//    someText01.Zero();
    TPtr ptr( retVal->Des() );
    ptr.Append( iText.Read(aStart, aEnd - aStart + 1) );
    while ( ptr.Length() < aEnd - aStart + 1 )
        {
        ptr.Append( iText.Read( aStart + ptr.Length(), aEnd - aStart + 1 - ptr.Length() ) );
        }
//    TPtrC retVal;
//    retVal.Set( someText01.Ptr(), someText01.Length() );
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetTextureL
// ---------------------------------------------------------------------------
CFsTexture& CFsTextParser::GetTextureL(TInt aPos)
    {
    FUNC_LOG;
    if(aPos > iText.DocumentLength())
        {
        User::Leave(KErrArgument);
        }

    const CTextField* field = iText.TextField(aPos);

    CFsTexture* retVal = NULL;

    TInt textureId;
    if(!IsTexturePosition(aPos, textureId))
    	{
		if(KFsRichTextPictureFieldUid == field->Type())
			{
			TPictureHeader header = iText.PictureHeader(aPos);

			CMyPicture* picture =
				static_cast<CMyPicture*>(header.iPicture.AsPtr() );

			CFbsBitmap* bitmap = picture->GetBitmap();
			CFbsBitmap* bitmapMask = picture->GetBitmapMask();

			CFsTexture& texture = (iTextureLoader->AppendBitmapL(bitmap, bitmapMask));

			TTextureIndex newTextureIndex;
			newTextureIndex.iPositionInText = aPos;
			newTextureIndex.iTextureId = texture.Id();

			iTextureIndex.Append(newTextureIndex);

			retVal = &texture;
			}
		else if(KFsRichTextSmileyFieldUid == field->Type())
			{

			TInt smileyIndex = ((CSmileyField*)(field))->GetSmileyIndex();


			TFileName fileName = iSmileyParser->GetSmileyFileName(smileyIndex);


			CFsTexture& texture = iTextureLoader->LoadTextureL( fileName );

			TTextureIndex newTextureIndex;
			newTextureIndex.iPositionInText = aPos;
			newTextureIndex.iTextureId = texture.Id();

			iTextureIndex.Append(newTextureIndex);

			retVal = &texture;
			}
		else
			{
			User::Leave(KErrArgument);
			}
    	}
    else
    	{
    	CFsTexture* texture = iTextureLoader->SearchById(textureId);

    	retVal = texture;
    	}

    return *retVal;
    }

TBool CFsTextParser::IsFieldInrange(TInt aStartPosition, TInt aRange)
    {
    FUNC_LOG;
    TInt siezOfArray = iFieldLocation.Count();

    TBool retVal = EFalse;

    TInt endPosition = aStartPosition + aRange;
    TInt pos;

    for(TInt i = 0 ; i < siezOfArray ; i++)
        {
        pos = iFieldLocation[i];

        if(aStartPosition <= pos && pos <= endPosition)
            {
            retVal = ETrue;
            i = siezOfArray;
            }
        }

    return retVal;
    }

void CFsTextParser::FindBlokOfText()
	{
	if(!iGetCharsUsed)
		{
		iText.GetChars(iGNOSomeText, iGNOCharFormat, iPosition);
		iLastGetCharsPos = iPosition;
		iLastGetCharsLength = iGNOSomeText.Length();
		iGetCharsUsed = ETrue;
		}
	else
		{
		if(iPosition >= iLastGetCharsPos && iPosition < (iLastGetCharsPos + iLastGetCharsLength))
			{
			iGNOSomeText.Set(iText.Read(
									iPosition,
									iLastGetCharsLength - (iPosition - iLastGetCharsPos)));
				
			iLastGetCharsPos = iPosition;
			iLastGetCharsLength = iGNOSomeText.Length();
			}
		else
			{
			if(iPosition == iLastGetCharsPosOfNextBlock)
				{
				iGNOSomeText.Set(iText.Read(iPosition,
							iLastGetCharsLengthOfNextBlock));
				
				iLastGetCharsPos = iPosition;
				iLastGetCharsLength = iGNOSomeText.Length();
				iGNOCharFormat = iGNOCharFormatOfNextBlock;
				}
			else
				{
				iText.GetChars(iGNOSomeText, iGNOCharFormat, iPosition);
				
				iLastGetCharsPos = iPosition;
				iLastGetCharsLength = iGNOSomeText.Length();
				}
			}
		}
	}

void CFsTextParser::FindNextBlokOfText()
	{	
	if(!iGetCharsUsedOfNextBlock)
		{
		iText.GetChars(iGNOSomeTextOfNextBlock, iGNOCharFormatOfNextBlock, iGNONextPosition );		
		iLastGetCharsPosOfNextBlock = iGNONextPosition;
		iLastGetCharsLengthOfNextBlock = iGNOSomeTextOfNextBlock.Length();
		iGetCharsUsedOfNextBlock = ETrue;
		}
	else
		{
		if(iLastGetCharsPosOfNextBlock == iGNONextPosition)
			{
			iGNOSomeTextOfNextBlock.Set(iText.Read(iGNONextPosition,
			iLastGetCharsLengthOfNextBlock));
			}
		else
			{
			iText.GetChars(iGNOSomeTextOfNextBlock, iGNOCharFormatOfNextBlock, iGNONextPosition );
			iLastGetCharsPosOfNextBlock = iGNONextPosition;
			iLastGetCharsLengthOfNextBlock = iGNOSomeTextOfNextBlock.Length();
			}
		}
	}

TInt CFsTextParser::IsNewLinePresent(TInt aPosition, TInt aLength)
	{	
		TInt retVal = -1;
		
		TInt index = 0;
		TInt newLineArrayCount = iNewLineArray.Count();
		
		if(iNewLineArray[iNewLineArrayIndex] < aPosition &&
				iNewLineArray[iNewLineArrayIndex] < (aPosition + aLength))
			{
			for(index = iNewLineArrayIndex ; index < newLineArrayCount - 1 ; ++index)
				{
				TInt alamakota= iNewLineArray[index];
				
				if(iNewLineArray[index] >= aPosition && 
						iNewLineArray[index] < (aPosition + aLength))
					{
					retVal = iNewLineArray[index] - aPosition;
					iNewLineArrayIndex = index;
					index = newLineArrayCount + 1;
					}
				else if(iNewLineArray[index] > aPosition &&
						iNewLineArray[index] > (aPosition + aLength))
					{
					retVal = -1;
					index = newLineArrayCount + 1;
					}
				}
			}
		else
			{
			for(index = iNewLineArrayIndex ; index >= 0  ; --index)
				{
				TInt alamakota= iNewLineArray[index];
							
				if(iNewLineArray[index] >= aPosition && 
						iNewLineArray[index] < (aPosition + aLength))
					{
					retVal = iNewLineArray[index] - aPosition;
					iNewLineArrayIndex = index;
					index = 0;
					}
				else if(iNewLineArray[index] > aPosition &&
						iNewLineArray[index] > (aPosition + aLength))
					{
					retVal = -1;
					index = 0;
					}
				}
			}
		
		return retVal;	
	}


MFsRichTextObject* CFsTextParser::GetNextObjectL()
    {
    MFsRichTextObject* textObject = NULL;
      
    TInt startPos = 0;
    TInt wordLength = 0;
    
    FindBlokOfText();
   
    iGNONextPosition = iPosition + iGNOSomeText.Length(); 
    
    if(iGNONextPosition  < iText.DocumentLength())
    	{
    	FindNextBlokOfText();
    		
    	if(iGNOCharFormat.IsEqual(iGNOCharFormatOfNextBlock))
    		{
    		TBool connect = ETrue;
    		
    		if(iGNOSomeTextOfNextBlock.Length() == 1)
    			{
    			TBuf<1> checkForSpecialChar;
    			_LIT(specialChar, "\xFFFC");
    			
    			iText.Extract(checkForSpecialChar, iGNONextPosition, 1);
    			
    			if(checkForSpecialChar.Right(1).Compare(specialChar) == 0)
    				{
    				connect = EFalse;
    				}
    			}
    		if(iGNOSomeText.Length() == 1)
    			{
    			TBuf<1> checkForSpecialChar;
    			_LIT(specialChar, "\xFFFC");
    			
    			iText.Extract(checkForSpecialChar, iPosition, 1);
    			    			
    			if(checkForSpecialChar.Right(1).Compare(specialChar) == 0)
    				{
    				connect = EFalse;
    				}
    			}
    		
    		if(connect)
    			{
    			TInt l1 = iGNOSomeText.Length();
    			TInt l2 = iGNOSomeTextOfNextBlock.Length();
    		
    			TBuf<121> someCharacter;
    		
    			TInt getTextLength = (l1 + l2) > 120 ? 120 : (l1 + l2);
    		
    			iText.Extract(someCharacter, iPosition, getTextLength);
    		
    			iGNOSomeText.Set(someCharacter.Left(someCharacter.Length()));
    			}
    		}
    	}
    
   	if(iPosition == 0 || iLastPosition != iLastWordPosition || iLastLength < 120)
   		{	
   		iText.GetWordInfo(
            iPosition, 
            startPos, 
            wordLength, 
            EFalse, 
            EFalse);    	
   		iLastLength = wordLength;
   		}
   		
   	if(iGNOSomeText.Length() > 100)
   		{
   		//iGNOSomeText.Set(iText.Read(iPosition,100));
   		iGNOSomeText.Set(iGNOSomeText.Mid(0, 100));
                    
        iLastLength -= 100;
        
        //wordLength = 100;
        //startPos = iPosition;
   		}
   	
   	
   	iLastWordPosition = iPosition;
   	   
   	if ( !wordLength )
   		{
   		wordLength = iNewLengthOfWord != 0 ? iNewLengthOfWord : 0;
   		startPos = iPosition;
   		}
   	   
   	TInt a1 = iGNOSomeText.Length();
   	TInt a2 = startPos + wordLength - iPosition + 1;
   	   
    if(a1 > a2)
        {
        //iGNOSomeText.Set(iText.Read(iPosition,startPos + wordLength - iPosition + 1));
        iGNOSomeText.Set(iGNOSomeText.Mid(0, startPos + wordLength - iPosition + 1));
        }
    
    /*_LIT(endOfFile, "\x2029");
    if(iGNOSomeText.Length() > 0 && iGNOSomeText.Right(1).Compare(endOfFile) == 0)
        {
        iGNOSomeText.Set(iGNOSomeText.Mid(0, iGNOSomeText.Length() - 1));
        }*/
    
    
    if(iIsWordTooLong)
        {
        iIsWordTooLong = EFalse;
        
        if(iGNOSomeText.Length() > iNewLengthOfWord)
            {
            iGNOSomeText.Set(iText.Read(
                    iPosition,
                    iNewLengthOfWord));
            }
        }
    
    //czy text jest elementem expanda
    TInt isWordPartOfExpandArea = IsWordPartOfExpandArea(iPosition, 
                                    iPosition + iGNOSomeText.Length() - 1);
    
    if(-1 != isWordPartOfExpandArea)
        {        
        // Profile
        if(iExpandArray.Count() > isWordPartOfExpandArea && 
            iExpandArray[isWordPartOfExpandArea].iStartIndex > iPosition &&
            iExpandArray[isWordPartOfExpandArea].iStartIndex < 
                            (iPosition + iGNOSomeText.Length() - 1))  
            {
            //w polowie slowa zaczyna sie expand - 
            //nalezy slowo uciac i zajac sie 
            //poczatkowa czescia, ktora nie jest expandem=
            iGNOSomeText.Set( iText.Read(
                iPosition,
                iExpandArray[isWordPartOfExpandArea].iStartIndex 
                  - iPosition ) );
                      
            isWordPartOfExpandArea = -1;
            }
        //wariant 3
        else if(iExpandArray[isWordPartOfExpandArea].iStartIndex + 
            iExpandArray[isWordPartOfExpandArea].iLength >= iPosition 
            && 
            iExpandArray[isWordPartOfExpandArea].iStartIndex + 
            iExpandArray[isWordPartOfExpandArea].iLength < 
            (iPosition + iGNOSomeText.Length() - 1))  
            {
            //poczatek slowa jest expandem 
            //a jego konie nie jest - ten koniec nalezy uciac
            iGNOSomeText.Set(iText.Read(
                        iPosition,
                        iExpandArray[isWordPartOfExpandArea].iStartIndex + 
                        iExpandArray[isWordPartOfExpandArea].iLength - 
                        iPosition));
            }
        }
        
    if(-1 != isWordPartOfExpandArea)
        {
        //Profile
        if(!iExpandArray[isWordPartOfExpandArea].iIsExpand)
            {
            if(iPosition >= 
                    iExpandArray[isWordPartOfExpandArea].iStartIndex + 
                    iExpandArray[isWordPartOfExpandArea].iCaptionLength)
                {              
                iLastPosition = iPosition;
                iPosition = 
                    iExpandArray[isWordPartOfExpandArea].iStartIndex + 
                        iExpandArray[isWordPartOfExpandArea].iLength; 
                
                //jak expand jest do konca textu;
                if(IsNextObject())
                    {
                    return GetNextObjectL();
                    }
                else
                    {
                    textObject = CFsRichTextNewLine::NewL();
                
                    textObject->SetBeginOfObject(iPosition);
                    textObject->SetEndOfObject(iPosition);
                    
                    iLastPosition = iPosition;
                    iPosition += 1;
                    
                    return textObject;
                    }
                }
            
            //expand nie jest rozwiniety dlatego 
            //mozemy przekazac tylko caption
            if(iExpandArray[isWordPartOfExpandArea].iStartIndex + 
                iExpandArray[isWordPartOfExpandArea].iCaptionLength > 
                  iPosition &&
                iExpandArray[isWordPartOfExpandArea].iStartIndex + 
                iExpandArray[isWordPartOfExpandArea].iCaptionLength < 
                iPosition + iGNOSomeText.Length())
                {
                iGNOSomeText.Set(iText.Read(
                        iPosition,
                        iExpandArray[isWordPartOfExpandArea].iStartIndex + 
                        iExpandArray[isWordPartOfExpandArea].iCaptionLength 
                        - iPosition));
                }
            }
//        if ( Profiling2( &iGNOSomeText, &isWordPartOfExpandArea, textObject ) )
//            {
//            return GetNextObjectL();
//            }
        }
    
    TInt isWordPartOfHotSpotArea =
        IsWordPartOfHotSpotArea(
                iPosition, 
                iPosition + iGNOSomeText.Length() - 1);
        
    if(-1 != isWordPartOfHotSpotArea 
            && iHotSpotArray.Count() > isWordPartOfHotSpotArea)
        {
        if(iHotSpotArray[isWordPartOfHotSpotArea].iStartIndex > iPosition &&
            iHotSpotArray[isWordPartOfHotSpotArea].iStartIndex < 
            (iPosition + iGNOSomeText.Length() - 1))  
            {
            //w polowie slowa zaczyna sie expand - 
            //nalezy slowo uciac i zajac sie 
            //poczatkowa czescia, ktora nie jest expandem=
            iGNOSomeText.Set( iText.Read(
                iPosition,
                iHotSpotArray[isWordPartOfHotSpotArea].iStartIndex 
                    - iPosition ) );
                          
            isWordPartOfHotSpotArea = -1;
            }
            //wariant 3
        else if(iHotSpotArray[isWordPartOfHotSpotArea].iStartIndex + 
            iHotSpotArray[isWordPartOfHotSpotArea].iLength >= iPosition && 
            iHotSpotArray[isWordPartOfHotSpotArea].iStartIndex + 
            iHotSpotArray[isWordPartOfHotSpotArea].iLength < 
            (iPosition + iGNOSomeText.Length() - 1))  
            {
            //poczatek slowa jest expandem 
            //a jego konie nie jest - ten koniec nalezy uciac
            iGNOSomeText.Set(iText.Read(
                iPosition,
                iHotSpotArray[isWordPartOfHotSpotArea].iStartIndex + 
                iHotSpotArray[isWordPartOfHotSpotArea].iLength - iPosition));
            } 
        }
    
    TFindFieldInfo info;
    
    TInt dx = iGNOSomeText.Length(); 
    
    if(iPosition + dx > iText.DocumentLength())
        {
        dx = iText.DocumentLength() - iPosition - 1;
        }
    
    //is new line in iGNOSomeText
    
    TInt positionOfNewLine = IsNewLinePresent(iPosition, iGNOSomeText.Length());
    TBool isNewLinePresent = EFalse;

    if( positionOfNewLine > -1 )
    	{
    	if(positionOfNewLine == 0)
    		{
    		isNewLinePresent = ETrue;
    		iGNOSomeText.Set(iGNOSomeText.Mid(0, 1));
    		}
    	else
    		{
    		iGNOSomeText.Set(iGNOSomeText.Mid(0, positionOfNewLine));
    		}
    	}
    
    if(IsFieldInrange(iPosition, dx) && iText.FindFields(info, iPosition, dx))
        {
        if(iPosition != info.iFirstFieldPos)
            {
            iGNOSomeText.Set(iText.Read(
                        iPosition, 
                        info.iFirstFieldPos - iPosition));
            textObject = static_cast<MFsRichTextObject*>
                (CFsRichTextText::NewL(iGNOSomeText, 0));
            CleanupStack::PushL(textObject);
            TInt styleId = iTextStyleManager->GetStyleIDL(iGNOCharFormat);
            ((CFsRichTextText*)textObject)->SetStyleId(styleId);
            
            textObject->SetBeginOfObject(iPosition);
            textObject->SetEndOfObject(iPosition + iGNOSomeText.Length() - 1);    
            
            ((CFsRichTextText*)textObject)->SetTextColor(
                iGNOCharFormat.iFontPresentation.iTextColor );
            
            iLastPosition = iPosition;
            iPosition += iGNOSomeText.Length();
            CleanupStack::Pop(textObject);
            }
        else
            {
            //pobrac fielda
            const CTextField* field = iText.TextField(iPosition);
            
            if(KFsRichTextPictureFieldUid == field->Type())
                {        
                TPictureHeader header = iText.PictureHeader(iPosition);
                
                CMyPicture* picture = 
                    static_cast<CMyPicture*>(header.iPicture.AsPtr() );
                
                textObject = CFsRichTextPicture::NewL();
                
                TSize sizeOfPicture = TSize( 0, 0 );
                picture->GetOriginalSizeInTwips( sizeOfPicture );
                
                static_cast< CFsRichTextPicture* >
                    (textObject)->SetTextureSize(sizeOfPicture);
                
                textObject->SetBeginOfObject(iPosition);
                textObject->SetEndOfObject(iPosition);
                
                iLastPosition = iPosition;
                iPosition += 1;
                }           
            else if(KFsRichTextSmileyFieldUid == field->Type())
                {
                TInt smileyIndex = ((CSmileyField*)(field))->GetSmileyIndex();
                
                iGNOSomeText.Set(iText.Read(
                            iPosition, 
                            iSmileyParser->GetSmileyLength(smileyIndex)));
                                  
                CFsRichTextText* smileyTextObject = 
                    CFsRichTextText::NewL(iGNOSomeText, 0);
                CleanupStack::PushL(smileyTextObject); 
                smileyTextObject->SetTextColor(
                        iGNOCharFormat.iFontPresentation.iTextColor );
                
                TInt styleId = iTextStyleManager->GetStyleIDL(iGNOCharFormat);
                ((CFsRichTextText*)smileyTextObject)->SetStyleId(styleId);
                
                TFileName fileName = 
                    iSmileyParser->GetSmileyFileName(smileyIndex);
                
                TInt textureId = 0;
                TSize texturesize = TSize( 0, 0 );
                if(IsTexturePosition(iPosition, textureId))
                	{
                	CFsTexture* texture = iTextureLoader->SearchById(textureId);
                	texturesize = texture->Texture().Size();
                	}
                else
                	{
                	CFsTexture& texture = iTextureLoader->LoadTextureL( fileName );
                	
                	TTextureIndex newTextureIndex;
                	newTextureIndex.iPositionInText = iPosition;
                	newTextureIndex.iTextureId = texture.Id();
                	
                	iTextureIndex.Append(newTextureIndex);
                	
                	texturesize = texture.Texture().Size();
                	}
                
                CFsRichTextPicture* smileyPictureObject = 
                    CFsRichTextPicture::NewL();
                CleanupStack::PushL(smileyPictureObject);
                if(iIsSetSizeOfSmiley)      
                    {
                    texturesize = iSizeOfSmiley;
                    }
                else if ( texturesize == TSize( 0, 0 ) )
                    {
                    TFrameInfo frameInfo;

                    CFsTextureLoader::GetFrameInfoL( 
                    		fileName,
                            frameInfo );
                    texturesize = frameInfo.iOverallSizeInPixels;
                    TSize texturesize = TSize( 10, 10 ); 
                    }

                static_cast< CFsRichTextPicture* >
                    (smileyPictureObject)->SetTextureSize( texturesize );
                
                textObject = CFsRichTextSmiley::NewL(
                        smileyTextObject, 
                        smileyPictureObject);
                
                CleanupStack::Pop(smileyPictureObject);
                CleanupStack::Pop(smileyTextObject);
                textObject->SetBeginOfObject(iPosition);
                textObject->SetEndOfObject(iPosition + iGNOSomeText.Length() - 1);
                
                iLastPosition = iPosition;
                iPosition += iGNOSomeText.Length();     
                }
            }
        }
    else
        {
        if(isNewLinePresent)
        	{
        	textObject = CFsRichTextNewLine::NewL();
                
            textObject->SetBeginOfObject(iPosition);
            textObject->SetEndOfObject(iPosition);
                
            iLastPosition = iPosition;
            iPosition += 1; 
        	}
        else
        	{
        	textObject = static_cast<MFsRichTextObject*>
                    (CFsRichTextText::NewL(iGNOSomeText, 0));
            CleanupStack::PushL(textObject); 
        	TInt styleId = iTextStyleManager->GetStyleIDL(iGNOCharFormat);
        	((CFsRichTextText*)textObject)->SetStyleId(styleId);
            
        	textObject->SetBeginOfObject(iPosition);
        	textObject->SetEndOfObject(iPosition + iGNOSomeText.Length() - 1);
            
        	((CFsRichTextText*)textObject)->SetTextColor(
            	iGNOCharFormat.iFontPresentation.iTextColor );    
            
        	TBidiText * bidiText = TBidiText::NewL(iGNOSomeText.Length() + 1, 1); 
        
        	TBool found = EFalse;
        
        	TBidiText::TDirectionality direction = bidiText->TextDirectionality(iGNOSomeText, &found );
        
        	// <cmail>
        	if ( !found )
        	    {
        	    direction = AknLayoutUtils::LayoutMirrored() ?
        	    	TBidiText::ERightToLeft : TBidiText::ELeftToRight;
        	    }
        	// </cmail>

        	static_cast<CFsRichTextText*>(textObject)->SetTextDirection(direction);
        
	        delete bidiText;
	        
	        TInt sizeOfSomeText = iGNOSomeText.Length();
	        TBool isWhiteSpace = ETrue;
	        for(TInt k = 0 ; k < sizeOfSomeText ; ++k)
	        	{
	        	if(iGNOSomeText[k] != ' ')
	        		{
	        		isWhiteSpace = EFalse;
	        		k = sizeOfSomeText + 1;
	        		}
	        	}
	            
	        textObject->SetIsWhiteSpace(isWhiteSpace);
	            
	        iLastPosition = iPosition;
	        iPosition += iGNOSomeText.Length();
	        
	        CleanupStack::Pop(textObject);
	        }     
        }
    
    if(-1 != isWordPartOfHotSpotArea)
        {
        textObject->SetHotSpot( ETrue );
        textObject->SetIdOfHotSpot(isWordPartOfHotSpotArea);
        textObject->SetTypeOfHotSpot(
                iHotSpotArray[isWordPartOfHotSpotArea].iId );
        }
    
    if(-1 != isWordPartOfExpandArea)
        {
        textObject->SetEmbeded( ETrue );
        textObject->SetIdOfEmbeded(isWordPartOfExpandArea);
        }
    
    return textObject;
    }

void CFsTextParser::SetTextStylemanager(
        CFsTextStyleManager* aTextStyleManager )
    {
    FUNC_LOG;
    iTextStyleManager = aTextStyleManager;
    }

void CFsTextParser::ConvertBitmapL(CPicture* aPicture, CFbsBitmap*& aBitmap)
    {
    FUNC_LOG;
    TSize size;
    aPicture->GetOriginalSizeInTwips(size);
    aBitmap = new (ELeave)CFbsBitmap;

    size.iWidth = 45;
    size.iWidth = 45;

    User::LeaveIfError(aBitmap->Create(size,EColor256));
    CleanupStack::PushL(aBitmap);

    //create an offscreen device and context
    CGraphicsContext* bitmapContext = NULL;
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(aBitmap);
    CleanupStack::PushL(bitmapDevice);
    User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
    CleanupStack::PushL(bitmapContext);

    aPicture->Draw(
            *bitmapContext,
            TPoint(0,0),
            TRect( TPoint(0,0),TPoint(45,45) ),
            bitmapDevice);

    CleanupStack::PopAndDestroy(bitmapContext);
    CleanupStack::PopAndDestroy(bitmapDevice);
    CleanupStack::Pop(aBitmap);
    }



TInt CFsTextParser::IsWordPartOfHotSpotArea(
        TInt aStartPosition,
        TInt aEndPosition )
    {
    FUNC_LOG;
    TInt retVal = -1;

    for(TInt i = 0 ; i < iHotSpotArray.Count() ; ++i)
        {
        /*
        TRichTextHotSpotArea test;
        test.iLength = iHotSpotArray[i].iLength;
        test.iStartIndex = iHotSpotArray[i].iStartIndex;
        */

        if(iHotSpotArray[i].iStartIndex >= aStartPosition &&
            iHotSpotArray[i].iStartIndex < aEndPosition)
            {
            if(-1 == retVal)
                {
                retVal = i;
                }
            else
                {
                if ( iHotSpotArray[i].iStartIndex
                        < iHotSpotArray[retVal].iStartIndex )
                    {
                    retVal = i;
                    }
                }
            }
        else if ( aStartPosition >= iHotSpotArray[i].iStartIndex
                && aStartPosition
                   < iHotSpotArray[i].iStartIndex
                      + iHotSpotArray[i].iLength )
            {
            if(-1 == retVal)
                {
                retVal = i;
                }
            else
                {
                if ( iHotSpotArray[i].iStartIndex
                        < iHotSpotArray[retVal].iStartIndex )
                    {
                    retVal = i;
                    }
                }
            }
        }
    return retVal;
    }




TBool CFsTextParser::IsNextObject() const
    {
    FUNC_LOG;

    TInt documentLength = iText.DocumentLength() - 1;
    TBool retVal = EFalse;

    if(documentLength < iPosition)
        {
        retVal = EFalse;
        }
    else
        {
        retVal = ETrue;
        }

    return retVal;
    }

TBool CFsTextParser::MoveParsingPosition( TInt aMoveParsingPositionBy )
    {
    FUNC_LOG;
    TBool retVal = ETrue;

    if( (iPosition + aMoveParsingPositionBy) < 0 ||
        (iPosition + aMoveParsingPositionBy) >= iText.DocumentLength())
        {
        retVal = EFalse;
        }
    else
        {
        iPosition += aMoveParsingPositionBy;
        }

    return retVal;
    }



TBool CFsTextParser::ParseTextL()
    {
    FUNC_LOG;
        TBool retVal = ETrue;

    TInt textLength = iText.DocumentLength();

    TBuf<1> someCharacter;

    _LIT(fourSpaces, "    ");
    _LIT(validNewLine, "\n");


    _LIT(newLine1, "\x2028");
    _LIT(newLine2, "\x2029");
    _LIT(newLine3, "\x85");
    _LIT(newLine4, "\x0D");

    _LIT(tabulator,"\x2007");

    for(TInt i = 0 ; i < iText.GetNumberOfExpandAreas() ; ++i)
        {
        iExpandArray.AppendL(iText.GetExpandAreaL(i));
        }

    for(TInt i = 0 ; i < iText.GetNumberOfHotSpotAreas() ; ++i)
        {
        iHotSpotArray.AppendL(iText.GetHotSpotAreaL(i));
        }

    textLength = iText.DocumentLength();
    for(TInt i = 0 ; i < textLength ; ++i)
        {
        iText.Extract(someCharacter, i, 1);

        if(someCharacter[0] == '\t')
            {
            iText.DeleteL(i,1);
            iText.InsertL(i,fourSpaces);

            textLength = iText.DocumentLength();

            for(TInt j = 0 ; j < iText.GetNumberOfExpandAreas() ; ++j)
                {
                if(iExpandArray[j].iStartIndex > i)
                    {
                    iExpandArray[j].iStartIndex += 3;
                    }
                }

            for(TInt j = 0 ; j < iText.GetNumberOfHotSpotAreas() ; ++j)
                {
                if(iHotSpotArray[j].iStartIndex <= i &&
                   iHotSpotArray[j].iStartIndex + iHotSpotArray[j].iLength > i)
                    {
                    iHotSpotArray[j].iLength += 3;
                    }
                if(iHotSpotArray[j].iStartIndex > i)
                    {
                    iHotSpotArray[j].iStartIndex += 3;
                    }
                }

            ++i;
            }
        else if(someCharacter.Right(1).Compare(tabulator) == 0)
            {
            iText.DeleteL(i,1);
            iText.InsertL(i,fourSpaces);

            textLength = iText.DocumentLength();
            ++i;
            }
        else if(someCharacter.Right(1).Compare(validNewLine) == 0)
        	{
        	TBuf<1> anotherCharacter;
        	if ( iText.DocumentLength() > ( i + 1 ) )
        		{
        		iText.Extract(anotherCharacter, i + 1, 1);

        		if( anotherCharacter.Right(1).Compare(newLine3) == 0 ||
        			anotherCharacter.Right(1).Compare(newLine4) == 0 )
        			{
        			iText.DeleteL( i, 2 );
        			iText.InsertL( i, validNewLine );
        			iText.InsertL( i, _L(" ") );

        			textLength = iText.DocumentLength();
        			}
        		}
        	}
        }

    textLength = iText.DocumentLength();
    TInt lastNewLine = 0;
    TPtrC someText;

    for(TInt i = 0 ; i < textLength ; ++i)
        {
        iText.Extract(someCharacter, i, 1);

        if(someCharacter.Right(1).Compare(validNewLine) == 0 ||
        		someCharacter.Right(1).Compare(newLine1) == 0 || 
        		someCharacter.Right(1).Compare(newLine2) == 0 ||
        		someCharacter.Right(1).Compare(newLine3) == 0 ||
        		someCharacter.Right(1).Compare(newLine4) == 0
                || i == textLength - 1 
                )  
            {
            iNewLineArray.AppendL(i);

            someText.Set(iText.Read(
                    lastNewLine,
                    i - lastNewLine));

            // <cmail>
            TInt partOfHotspotArea = IsWordPartOfHotSpotArea(
                    lastNewLine,
                    i);
            // </cmail>

            lastNewLine = i;

            TBidiText * bidiText = TBidiText::NewL(someText.Length() + 1, 1);

        	TBool found = EFalse;

        	TBidiText::TDirectionality direction = bidiText->TextDirectionality(someText, &found );

	        delete bidiText;

	        // <cmail>
	        if (!found || partOfHotspotArea != -1)
	            {
	            direction = AknLayoutUtils::LayoutMirrored() ? TBidiText::ERightToLeft : TBidiText::ELeftToRight;
	            }
	        // </cmail>

	        iLineDirection.AppendL(direction);
            }
        }


    for(int i = 0 ; i < textLength ; ++i)
        {
            iText.Extract(someCharacter, i, 1);

            if(!iSmileyParser->IsPartOfSmileyL(someCharacter))
                {
                if(iSmileyParser->IsSmiley())
                    {
                    TInt smileysIndex = iSmileyParser->GetSmileyIndex();
                    CTextField* field =
                        (CTextField*)new(ELeave)CSmileyField(smileysIndex);

                    TInt smileysLength =
                        iSmileyParser->GetSmileyLength(smileysIndex);
                    iText.InsertFieldL(
                            i-smileysLength,
                            field,
                            KFsRichTextSmileyFieldUid);

                    iFieldLocation.Append(i-smileysLength);
                    }
                }

            if(someCharacter[0] == CRichText::EPictureCharacter)
                {
                CTextField* field = (CTextField*)
                    new(ELeave)CPictureField(TSize(50,50));
                iText.InsertFieldL(i, field, KFsRichTextPictureFieldUid);

                iFieldLocation.Append(i);
                }
        }

    return retVal;
    }

TBool CFsTextParser::IsPartOfHotSpot(TInt aIndex)
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    for(TInt i = 0 ; i < iHotSpotArray.Count() ; ++i)
        {
        if ( aIndex > iHotSpotArray[i].iStartIndex
            && aIndex <
              iHotSpotArray[i].iStartIndex
              + iHotSpotArray[i].iLength
              - 1 )
            {
            retVal = ETrue;
            break;
            }
        }

    return retVal;
    }

void CFsTextParser::SetSmileyParser(CFsSmileyParser* aSmileyParser)
    {
    FUNC_LOG;
    iSmileyParser = aSmileyParser;
    }

TBool CFsTextParser::SetExpandStatusL(TInt aIdOfExpand, TBool aStatus)
    {
    FUNC_LOG;
    TBool retVal = ETrue;

    if(aIdOfExpand >= iExpandArray.Count())
        {
        User::Leave(KErrArgument);
        }

    iExpandArray[aIdOfExpand].iIsExpand = aStatus;

    return retVal;
    }

TInt CFsTextParser::GetEndIndexOfExpandAreaL(TInt aId)
    {
    FUNC_LOG;
    if(aId >= iExpandArray.Count())
        {
        User::Leave(KErrArgument);
        }

    return iExpandArray[aId].iStartIndex + iExpandArray[aId].iLength - 1;
    }

TInt CFsTextParser::GetStartIndexOfExpandAreaL(TInt aId)
    {
    FUNC_LOG;
    if(aId >= iExpandArray.Count())
        {
        User::Leave(KErrArgument);
        }

    return iExpandArray[aId].iStartIndex;
    }

TInt CFsTextParser::GetBodyIndexOfExpandAreaL(TInt aId)
    {
    FUNC_LOG;
    if(aId >= iExpandArray.Count())
        {
        User::Leave(KErrArgument);
        }

    return
        iExpandArray[aId].iStartIndex
        + iExpandArray[aId].iCaptionLength - 1;
    }

TInt CFsTextParser::GetEndIndexOfHotSpotAreaL(TInt aId, TBool aOriginal)
    {
    FUNC_LOG;
    if(aId >= iHotSpotArray.Count())
        {
        User::Leave(KErrArgument);
        }

    TInt retVal;

    retVal = aOriginal
        ? iHotSpotArray[aId].iOriginalStartIndex + iHotSpotArray[aId].iOriginalLength - 1
        : iHotSpotArray[aId].iStartIndex + iHotSpotArray[aId].iLength - 1;

    return retVal;
    }

TInt CFsTextParser::GetStartIndexOfHotSpotAreaL(TInt aId, TBool aOriginal)
    {
    FUNC_LOG;
    if(aId >= iHotSpotArray.Count())
        {
        User::Leave(KErrArgument);
        }

    TInt retVal = aOriginal
        ? iHotSpotArray[aId].iOriginalStartIndex
        : iHotSpotArray[aId].iStartIndex;

    return retVal;
    }

CFsRichText* CFsTextParser::GetRichTextL(TInt aBeginIndex, TInt aEndIndex)
    {
    FUNC_LOG;
    CFsRichText* fsRichText = CFsRichText::NewL(
            iEikon->SystemParaFormatLayerL(),
            iEikon->SystemCharFormatLayerL());

    TBuf<1> c;

    TCharFormat charFormat;
    TCharFormatMask charMask;

    if(aBeginIndex < 0 || aBeginIndex > iText.DocumentLength() ||
                aEndIndex < 0 || aEndIndex > iText.DocumentLength())
        {
        User::Leave(KErrArgument);
        }

    for(TInt i = aBeginIndex ; i < aEndIndex ; ++i )
        {
        iText.Extract( c, i, 1 );

        TRAPD(err, fsRichText->InsertL(i,c));
        if(err == KErrNone)
            {
            iText.GetSpecificCharFormat(charFormat,charMask,i);
            fsRichText->SetInsertCharFormatL(charFormat,charMask,i);
            }
        }

    return fsRichText;
    }

TBidiText::TDirectionality CFsTextParser::GetParagraphDirection(TInt aPosInText)
	{
    FUNC_LOG;
	/*if(aPosInText > iText.DocumentLength())
        {
        User::Leave(KErrArgument);
        }

    if(iNewLineArray.Count() <= 0 || (iNewLineArray.Count() != iLineDirection.Count()))
    	{
    	User::Leave(KErrArgument);
    	}*/

    // <cmail>
    TBidiText::TDirectionality retVal = AknLayoutUtils::LayoutMirrored() ? TBidiText::ERightToLeft : TBidiText::ELeftToRight;
    // </cmail>

    //TBool found = EFalse;

    TInt newLineArrayCount = iNewLineArray.Count();
    TInt lastNewLinePosition = 0;
    if( aPosInText > iNewLineArray[iLastNewLinePositionIndex] )
    	{		
			return iLineDirection[iLastNewLinePositionIndex + 1];
    	}
    for(TInt i = 0 ; i < newLineArrayCount ; ++i)	
    	{
    	if(lastNewLinePosition <= aPosInText && iNewLineArray[i] >= aPosInText)
    		{
    		retVal = iLineDirection[i];
    		iLastNewLinePositionIndex = i;
			break;
    		}

    		lastNewLinePosition = iNewLineArray[i];
    	}

    /*if(!found)
    	{
    	User::Leave(KErrArgument);
    	}*/

    return retVal;
	}

TBool CFsTextParser::SetTextDirection(TInt aStartPosInText, TInt aEndPosInText,
        				TBidiText::TDirectionality aDirection)
	{
    FUNC_LOG;
	TInt indexOfStart = 0;
	TInt indexOfEnd = 0;

	TInt tableCount = iNewLineArray.Count();

	for(TInt i = 0 ; i < tableCount ; ++i)
		{
		TInt ala = iNewLineArray[i];

		if(iNewLineArray[i] >= aStartPosInText)
			{
			indexOfStart = i;
			i = tableCount + 1;
			}
		}

	for(TInt i = indexOfStart ; i < tableCount ; ++i)
		{
		TInt ala = iNewLineArray[i];

		if(iNewLineArray[i] >= aEndPosInText)
			{
			indexOfEnd = i;
			i = tableCount + 1;
			}
		}

	if(indexOfEnd == indexOfStart)
		{
		iNewLineArray.Insert(aStartPosInText, indexOfStart);
		iNewLineArray.Insert(aEndPosInText, indexOfStart + 1);

		TBidiText::TDirectionality direction = iLineDirection[indexOfEnd];

		iLineDirection.Insert(direction, indexOfStart);
		iLineDirection.Insert(aDirection, indexOfStart + 1);
		}
	else
		{
		iNewLineArray.Insert(aEndPosInText, indexOfEnd);
		iNewLineArray.Insert(aStartPosInText, indexOfStart);

		for(TInt i = indexOfEnd ; i > indexOfStart ; --i)
			{
			iNewLineArray.Remove(i);
			}

		TBidiText::TDirectionality direction = iLineDirection[indexOfStart];

		iLineDirection.Insert(aDirection, indexOfEnd);
		iLineDirection.Insert(direction, indexOfStart);

		for(TInt i = indexOfEnd ; i > indexOfStart ; --i)
			{
			iLineDirection.Remove(i);
			}
		}
	if(0 == aStartPosInText)
		{
		iLineDirection[indexOfStart] = aDirection;
		}
	return ETrue;
	}

TBool CFsTextParser::IsTexturePosition(TInt aPos, TInt &aId)
	{
	TBool retVal = EFalse;

	TInt tableCount = iTextureIndex.Count();

	for(TInt i = 0 ; i < tableCount ; ++i)
		{
		TInt texturePosition = iTextureIndex[i].iPositionInText;

		if(aPos == texturePosition)
			{
			retVal = ETrue;
			aId = iTextureIndex[i].iTextureId;
			i = tableCount + 1;
			}
		}

	return retVal;
	}
