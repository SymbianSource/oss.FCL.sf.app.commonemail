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
* Description:  Data structure class for tree list component
*
*/


#ifndef C_FSRICHTEXT_H
#define C_FSRICHTEXT_H

#include <txtrich.h>

class CRichText;
class CFsTexture;

class TRichTextExapandArea
    {   
    public:
    TInt iStartIndex;
    TInt iLength;
    TInt iCaptionLength;
    TBool iIsExpand;
    };

class TRichTextHotSpotArea
    {
    public:
    TInt iOriginalStartIndex;
    TInt iStartIndex;
    TInt iLength;
    TInt iOriginalLength;
    TInt iId;
    };

class TRichTextPictureArea
    {
    public:
    TInt iPictureIndex;
    TFileName iFileName;
    };

class CMyPicture :public CPicture
    {
    public:  
        IMPORT_C CMyPicture( TSize aSize, CFbsBitmap& aBitmap );
        IMPORT_C CMyPicture( TSize aSize, CFbsBitmap& aBitmap, CFbsBitmap& aBitmapMask );
        
        IMPORT_C ~CMyPicture();

    public: 
        IMPORT_C void Draw( CGraphicsContext& aGc,
                           const TPoint& aTopLeft,
                           const TRect& aClipRect,
                           MGraphicsDeviceMap* aMap ) const;
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        IMPORT_C void SetOriginalSizeInTwips( TSize aSize );

        IMPORT_C void GetOriginalSizeInTwips( TSize& aSize ) const;

        IMPORT_C CFbsBitmap* GetBitmap();
        
        IMPORT_C CFbsBitmap* GetBitmapMask();
       

    protected:    

        TSize iSizeInTwips; 
        CFbsBitmap* iBitmap; 
        CFbsBitmap* iBitmapMask; 
    };





class CFsRichText: public CRichText
    {
    public:
    
    IMPORT_C static CFsRichText* NewL(
            const CParaFormatLayer* aGlobalParaLayer, 
            const CCharFormatLayer* aGlobalCharLayer,
            TDocumentStorage aStorage = ESegmentedStorage,
            TInt aDefaultTextGranularity = EDefaultTextGranularity,
            TParaType aParaType = EMultiPara);
                                    
    IMPORT_C static CFsRichText* NewL(
            const CParaFormatLayer* aGlobalParaLayer, 
            const CCharFormatLayer* aGlobalCharLayer,
            const CStyleList& aStyleList,
            TDocumentStorage aStorage = ESegmentedStorage,
            TInt aDefaultTextGranularity = EDefaultTextGranularity,
            TParaType aParaType = EMultiPara);
                                    
    IMPORT_C static CFsRichText* NewL(
            const CStreamStore& aStore, 
            TStreamId aStreamId,
            const CParaFormatLayer* aGlobalParaLayer, 
            const CCharFormatLayer* aGlobalCharLayer,
            MTextFieldFactory* aFieldFactory = NULL,
            TDocumentStorage aStorage = ESegmentedStorage);
                                    
    IMPORT_C static CFsRichText* NewL(
            const CStreamStore& aStore, 
            TStreamId aStreamId,
            const CParaFormatLayer* aGlobalParaLayer, 
            const CCharFormatLayer * aGlobalCharLayer,
            MPictureFactory* aPictureFactory, 
            MRichTextStoreResolver* aStoreResolver,
            MTextFieldFactory* aFieldFactory = NULL,
            TDocumentStorage aStorage = ESegmentedStorage);
    
    IMPORT_C ~CFsRichText();

    IMPORT_C TBool SetExpandL(TInt aStartIndex, TInt aLength, TInt aCaptionLength);
    IMPORT_C TInt GetNumberOfExpandAreas();
    IMPORT_C TRichTextExapandArea GetExpandAreaL(TInt aIndex);
    
    IMPORT_C TBool SetHotSpotL(TInt aStartIndex, TInt aLength, TInt aId = 0);
    IMPORT_C TInt GetNumberOfHotSpotAreas();
    IMPORT_C TRichTextHotSpotArea GetHotSpotAreaL(TInt aIndex);
    
    IMPORT_C TBool SetPictureL(TFileName aFileName, TInt aIndex);
    IMPORT_C TInt GetNumberOfPicture();
    // <cmail>
    IMPORT_C TRichTextPictureArea GetPictureAreaL(TInt aIndex);
    // </cmail>
    
    IMPORT_C void Reset();
    
    
    private:
    
    CFsRichText(const CParaFormatLayer* aGlobalParaLayer, 
            const CCharFormatLayer* aGlobalCharLayer,
            CStyleList* aStyleList = NULL);
    
    private:
    RArray<TRichTextExapandArea> iExpandAreaArray;
    RArray<TRichTextHotSpotArea> iHotSpotAreaArray;
    RArray<TRichTextPictureArea> iPictureAreaArray;
    
    };
    
#endif //C_FSRICHTEXT_H
