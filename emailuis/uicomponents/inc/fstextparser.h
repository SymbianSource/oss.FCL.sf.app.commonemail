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
* Description:  Parser class for parsing CFsRichText
*
*/


#ifndef C_FSTEXTPARSER_H
#define C_FSTEXTPARSER_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>
#include <badesca.h>
// <cmail> SF
#include <alf/alfenv.h>
// </cmail>
#include "fstextureobserver.h"


class CFsRichText;
class MFsRichTextObject;
class CFsSmileyParser;
class TRichTextExapandArea;
class TRichTextHotSpotArea;
class CFsTextureLoader;
class CFsTextStyleManager;
class CEikonEnv;

class TTextureIndex
	{
	public:
		TInt iPositionInText;
		TInt iTextureId;
	};

class CFsTextParser: public CBase
    {
    public:
        /**
        * NewL( CFsRichText& aText, CHuiEnv& aEnv );
        * 
        * @param aText
        * @param aEnv
        */
        static CFsTextParser* NewL( 
                CFsRichText& aText, 
                CAlfEnv& aEnv, 
                CFsTextureLoader* aTextureLoader );
        
        /**
        * ~CFsTextParser();
        * 
        * 
        */
        ~CFsTextParser();

        /**
        * SetParsingPosition( TInt aPosition );
        * 
        * @param aPosition
        */
        TBool SetParsingPosition( TInt aPosition );
        
        /**
        * MoveParsingPosition( TInt aMoveParsingPositionBy);
        * 
        * @param aMoveParsingPositionBy
        */
        TBool MoveParsingPosition( TInt aMoveParsingPositionBy);
        
        /**
        * GetNextObjectL();
        * 
        * @param 
        */
        MFsRichTextObject * GetNextObjectL();
        
        /**
        * ParseTextL();
        * 
        * @param 
        */
        TBool ParseTextL();
        
        /**
        * IsNextObject() const;
        * 
        * 
        */
        TBool IsNextObject() const;
        
        /**
        * MoveBack();
        * 
        * 
        */
        void MoveBack();
        
        /**
        * SetSmileyParser(CFsSmileyParser* aSmileyParser);
        * 
        * @param aSmileyParser
        */
        void SetSmileyParser(CFsSmileyParser* aSmileyParser);
        
        /**
        * SetExpandStatusL(TInt aIdOfExpand, TBool aStatus);
        * 
        * @param aIdOfExpand
        * @param aStatus
        */
        TBool SetExpandStatusL(TInt aIdOfExpand, TBool aStatus);
        
        /**
        * GetRichTextL(TInt aBeginIndex, TInt aEndIndex);
        * 
        * @param aBeginIndex
        * @param aEndIndex
        */
        CFsRichText* GetRichTextL(TInt aBeginIndex, TInt aEndIndex);
        
        /**
        * GetEndIndexOfExpandAreaL(TInt aId);
        * 
        * @param aId
        */
        TInt GetEndIndexOfExpandAreaL(TInt aId);
        
        /**
        * GetStartIndexOfExpandAreaL(TInt aId);
        * 
        * @param aId
        */
        TInt GetStartIndexOfExpandAreaL(TInt aId);
        
        /**
        * GetBodyIndexOfExpandAreaL(TInt aId);
        * 
        * @param aId
        */
        TInt GetBodyIndexOfExpandAreaL(TInt aId);
        
        /**
        * GetEndIndexOfHotSpotAreaL(TInt aId);
        * 
        * @param aId
        */
        TInt GetEndIndexOfHotSpotAreaL(TInt aId, TBool aOriginal = EFalse);
        
        /**
        * GetStartIndexOfHotSpotAreaL(TInt aId);
        * 
        * @param aId
        */
        TInt GetStartIndexOfHotSpotAreaL(TInt aId, TBool aOriginal = EFalse );
        
        /**
        * SetSizeOfSmile(TSize aSizeOfSmiley);
        * 
        * @param aSizeOfSmiley
        */
        void SetSizeOfSmile(TSize aSizeOfSmiley);
        
        /**
        * SetTextStylemanager(CFsTextStyleManager* aTextStyleManager);
        * 
        * @param aTextStyleManager
        */
        void SetTextStylemanager(CFsTextStyleManager* aTextStyleManager);
        
        /**
        * CutWord(TInt aBeginOfWord, TInt aNewLengthOfWord);
        * 
        * @param aBeginOfWord
        * @param aNewLengthOfWord
        */
        void CutWord(TInt aBeginOfWord, TInt aNewLengthOfWord);

        /**
        * GetTextL(TInt aStart, TInt aEnd, TInt& aStyle);
        * 
        * @param aStart
        * @param aEnd
        * @param aStyle
        */
        HBufC* GetTextL(TInt aStart, TInt aEnd) const;
        
        /**
        * GetTextureL(TInt aPos);
        * 
        * @param aPos
        */
        CFsTexture& GetTextureL(TInt aPos);
        
        /**
        * GetParagraphDirection(TInt aPosInText);
        * 
        * @param aPosInText
        */
        TBidiText::TDirectionality GetParagraphDirection(TInt aPosInText);
        
        /**
        * GetParagraphDirection(TInt aPosInText);
        * 
        * @param aPosInText
        */
        TBool SetTextDirection(TInt aStartPosInText, TInt aEndPosInText, 
        				TBidiText::TDirectionality aDirection);
        				
        
        
    
        				
    private:
        /**
        * CFsTextParser( CFsRichText& aText, CHuiEnv& aEnv );
        * 
        * @param aText
        * @param aEnv
        */
        CFsTextParser( 
                CFsRichText& aText, 
                CAlfEnv& aEnv, 
                CFsTextureLoader* aTextureLoader );
        
        /**
        * IsWordPartOfExpandArea(TInt aStartPosition, TInt aEndPosition);
        * 
        * @param aStartPosition
        * @param aEndPosition
        */
        TInt IsWordPartOfExpandArea(TInt aStartPosition, TInt aEndPosition);
        
        /**
        * ConstructL();
        * 
        * 
        */
        void ConstructL();
        
        /**
        * IsPartOfHotSpot(TInt aIndex);
        * 
        * @param aIndex
        */
        TBool IsPartOfHotSpot(TInt aIndex);
        
        /**
        * IsWordPartOfHotSpotArea(TInt aStartPosition, TInt aEndPosition);
        * 
        * @param aStartPosition
        * @param aEndPosition
        */
        TInt IsWordPartOfHotSpotArea(TInt aStartPosition, TInt aEndPosition);
        
        /**
        * ConvertBitmapL(CPicture* aPicture, CFbsBitmap*& aBitmap);
        * 
        * @param aPicture
        * @param aBitmap
        */
        void ConvertBitmapL(CPicture* aPicture, CFbsBitmap*& aBitmap);
        
        /**
        * IsFieldInrange();
        * 
        * 
        */
        TBool IsFieldInrange(TInt aStartPosition, TInt aRange);
        
        /**
         * IsTexturePosition(TInt aPos, TInt &aId)
         * 
         */
        TBool IsTexturePosition(TInt aPos, TInt &aId);
        
        /**
         * IsNewLinePresent(TInt aPosition, TInt aLength)
         * 
         */
        TInt IsNewLinePresent(TInt aPosition, TInt aLength);
        
		/*
		 * FindBlokOfText();
		 * 
		 */
        void FindBlokOfText();
        
        /*
         * FindNextBlokOfText();
         * 
         */
        void FindNextBlokOfText();
        
    private:
        CFsRichText& iText;  
        TInt iPosition;
        TInt iLastPosition;
        TInt iTypeOfLastObject;
        CFsSmileyParser* iSmileyParser;
        RArray<TRichTextExapandArea> iExpandArray;
        RArray<TRichTextHotSpotArea> iHotSpotArray;
        RArray<TInt> iNewLineArray;
        RArray<TBidiText::TDirectionality> iLineDirection;
        CFsTextureLoader* iTextureLoader;
        CAlfEnv& iEnv;
        
        CFsTextStyleManager* iTextStyleManager;
        
        TBool iIsWordTooLong;
        TInt iNewLengthOfWord;
        
        TBool iIsSetSizeOfSmiley;
        TSize iSizeOfSmiley;
        CEikonEnv* iEikon;
        RArray<TInt> iFieldLocation;
        RArray<TTextureIndex> iTextureIndex;
        
        //new
        TInt iLastWordPosition;
        TInt iLastLength;
        
        
        
        //variables used in GetNetxObject
        TCharFormat iGNOCharFormat;
        TPtrC iGNOSomeText;
        
        TCharFormat iGNOCharFormatOfNextBlock;
        TPtrC iGNOSomeTextOfNextBlock;
        TInt iGNONextPosition;
        //end of variables used in GetNetxObject
        
        TInt iLastGetCharsPos;
        TInt iLastGetCharsLength;
        TBool iGetCharsUsed;
        
        TInt iLastGetCharsPosOfNextBlock;
        TInt iLastGetCharsLengthOfNextBlock;
        TBool iGetCharsUsedOfNextBlock;
        
        TInt iNewLineArrayIndex;
        TInt iLastNewLinePositionIndex;
        
    };

#endif // C_FSTEXTPARSER_H

