/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CFsTextViewer
*
*/


#include "emailtrace.h"
#include "fsrichtext.h"
#include "fsrichtextfields.h"
#include "fstexture.h"
#include <eikenv.h>
#include <fbs.h>

// ---------------------------------------------------------------------------
// CMyPicture
// ---------------------------------------------------------------------------
EXPORT_C CMyPicture::CMyPicture( TSize aSize, CFbsBitmap& aBitmap )
    : iSizeInTwips(aSize), iBitmap(&aBitmap), iBitmapMask(NULL)
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CMyPicture
// ---------------------------------------------------------------------------
EXPORT_C CMyPicture::CMyPicture( 
                TSize aSize, CFbsBitmap& aBitmap, CFbsBitmap& aBitmapMask )
    : iSizeInTwips(aSize), iBitmap(&aBitmap), iBitmapMask(&aBitmapMask)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ~CMyPicture
// ---------------------------------------------------------------------------
EXPORT_C CMyPicture::~CMyPicture()
    {
    FUNC_LOG;
    delete iBitmap;  
    delete iBitmapMask;    
    }
// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------
EXPORT_C void CMyPicture::Draw( CGraphicsContext& /*aGc*/,
                             const TPoint& /*aTopLeft*/,
                             const TRect& /*aClipRect*/,
                             MGraphicsDeviceMap* /*aMap*/ ) const
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
EXPORT_C void CMyPicture::ExternalizeL( RWriteStream& /*aStream*/ ) const
    {
    FUNC_LOG;
    }
        
// ---------------------------------------------------------------------------
// SetOriginalSizeInTwips
// ---------------------------------------------------------------------------
EXPORT_C void CMyPicture::SetOriginalSizeInTwips( TSize aSize )
    {
    FUNC_LOG;
    iSizeInTwips = aSize;
    }

// ---------------------------------------------------------------------------
// GetOriginalSizeInTwips
// ---------------------------------------------------------------------------
EXPORT_C void CMyPicture::GetOriginalSizeInTwips( TSize& aSize ) const
    {
    FUNC_LOG;
    aSize = iSizeInTwips;
    }

// ---------------------------------------------------------------------------
// GetBitmap
// ---------------------------------------------------------------------------
EXPORT_C CFbsBitmap* CMyPicture::GetBitmap()
    {
    FUNC_LOG;
    return iBitmap;
    }
    
// ---------------------------------------------------------------------------
// GetBitmapMask
// ---------------------------------------------------------------------------
EXPORT_C CFbsBitmap* CMyPicture::GetBitmapMask()
    {
    FUNC_LOG;
    return iBitmapMask;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsRichText* CFsRichText::NewL(const CParaFormatLayer* aGlobalParaLayer,
                       const CCharFormatLayer* aGlobalCharLayer,
                       TDocumentStorage aStorage,
                       TInt aDefaultTextGranularity,
                       TParaType aParaType)
    {
    FUNC_LOG;
    CFsRichText* self = 
        new(ELeave) CFsRichText(aGlobalParaLayer, aGlobalCharLayer);
    CleanupStack::PushL(self);
    self->ConstructL(aStorage, aDefaultTextGranularity, aParaType);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsRichText* CFsRichText::NewL(const CParaFormatLayer* aGlobalParaLayer,
                       const CCharFormatLayer* aGlobalCharLayer,
                       const CStyleList& aStyleList,
                       TDocumentStorage aStorage,
                       TInt aDefaultTextGranularity,
                       TParaType aParaType)
    {
    FUNC_LOG;
    CFsRichText* self = new(ELeave) CFsRichText(
            aGlobalParaLayer, 
            aGlobalCharLayer, 
            CONST_CAST(CStyleList*, &aStyleList));
    CleanupStack::PushL(self);
    self->ConstructL(aStorage, aDefaultTextGranularity, aParaType);
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsRichText* CFsRichText::NewL(const CStreamStore& aStore,
                       TStreamId aStreamId,
                       const CParaFormatLayer* aGlobalParaLayer,
                       const CCharFormatLayer* aGlobalCharLayer,
                       MTextFieldFactory* aFieldFactory,
                       TDocumentStorage aStorage)
    {
    FUNC_LOG;
    CFsRichText* self = 
        new(ELeave) CFsRichText(aGlobalParaLayer, aGlobalCharLayer);
    CleanupStack::PushL(self);
    self->ConstructL(aStore, aStreamId, NULL, NULL, aFieldFactory, aStorage);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsRichText* CFsRichText::NewL(const CStreamStore& aStore, 
                       TStreamId aStreamId,
                       const CParaFormatLayer* aGlobalParaLayer, 
                       const CCharFormatLayer* aGlobalCharLayer,
                       MPictureFactory* aPictureFactory, 
                       MRichTextStoreResolver* aStoreResolver,
                       MTextFieldFactory* aFieldFactory,
                       TDocumentStorage aStorage)

    {
    FUNC_LOG;
    CFsRichText* self = 
        new(ELeave) CFsRichText(aGlobalParaLayer, aGlobalCharLayer);
    CleanupStack::PushL(self);
    self->ConstructL(
            aStore, 
            aStreamId, 
            aPictureFactory, 
            aStoreResolver, 
            aFieldFactory, 
            aStorage);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsRichText()
// ---------------------------------------------------------------------------
EXPORT_C CFsRichText::~CFsRichText()
    {
    FUNC_LOG;
    iExpandAreaArray.Close();
    iHotSpotAreaArray.Close();
    iPictureAreaArray.Close();
    }
    
// ---------------------------------------------------------------------------
// CFsRichText
// ---------------------------------------------------------------------------
CFsRichText::CFsRichText(const CParaFormatLayer* aGlobalParaLayer, 
                 const CCharFormatLayer* aGlobalCharLayer,
                 CStyleList* aStyleList)
            :CRichText(aGlobalParaLayer, aGlobalCharLayer, aStyleList)
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// SetPictureL
// ---------------------------------------------------------------------------
EXPORT_C TBool CFsRichText::SetPictureL(TFileName aFileName, TInt aIndex)
    {
    FUNC_LOG;
    TBool retVal = ETrue;    
    
    TRichTextPictureArea richTextPictureArea;
    
    richTextPictureArea.iFileName = aFileName;
    richTextPictureArea.iPictureIndex = aIndex;
    
    iPictureAreaArray.AppendL(richTextPictureArea);
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// SetHotSpotL
// ---------------------------------------------------------------------------
EXPORT_C TBool CFsRichText::SetHotSpotL(TInt aStartIndex, TInt aLength, TInt aId)
    {
    FUNC_LOG;
    TBool retVal = ETrue;
    
    for(TInt i = 0 ; i < iHotSpotAreaArray.Count() ; ++i)
        {
        if((aStartIndex >= iHotSpotAreaArray[i].iStartIndex && 
            aStartIndex < iHotSpotAreaArray[i].iStartIndex + iHotSpotAreaArray[i].iLength) ||
            (aStartIndex + aLength >= iHotSpotAreaArray[i].iStartIndex && 
            aStartIndex + aLength < iHotSpotAreaArray[i].iStartIndex + iHotSpotAreaArray[i].iLength))
            {
            retVal = EFalse;
            }
        }
    
    if( retVal )
        { 
        TRichTextHotSpotArea richTextHotSpotArea;
        
        richTextHotSpotArea.iStartIndex = aStartIndex;
        richTextHotSpotArea.iOriginalStartIndex = aStartIndex;
        richTextHotSpotArea.iLength = aLength;
        richTextHotSpotArea.iOriginalLength = aLength;
        richTextHotSpotArea.iId = aId;
        
        iHotSpotAreaArray.AppendL(richTextHotSpotArea);
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// SetExpandL
// ---------------------------------------------------------------------------
EXPORT_C TBool CFsRichText::SetExpandL(TInt aStartIndex, TInt aLength, TInt aCaptionLength)
    {
    FUNC_LOG;
    TBool retVal = ETrue;
    
    for(TInt i = 0 ; i < iExpandAreaArray.Count() ; ++i)
        {
        if((aStartIndex >= iExpandAreaArray[i].iStartIndex && 
            aStartIndex < iExpandAreaArray[i].iStartIndex + iExpandAreaArray[i].iLength) ||
            (aStartIndex + aLength >= iExpandAreaArray[i].iStartIndex && 
            aStartIndex + aLength < iExpandAreaArray[i].iStartIndex + iExpandAreaArray[i].iLength))
            {
            retVal = EFalse;
            }
        }
    
    if(retVal)
        { 
        TRichTextExapandArea richTextExpandArea;
        
        richTextExpandArea.iStartIndex = aStartIndex;
        richTextExpandArea.iLength = aLength;
        richTextExpandArea.iCaptionLength = aCaptionLength;
        richTextExpandArea.iIsExpand = EFalse;
        
        iExpandAreaArray.AppendL(richTextExpandArea);
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetNumberOfExpandAreas
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsRichText::GetNumberOfExpandAreas()
    {
    FUNC_LOG;
    return iExpandAreaArray.Count();
    }
    
// ---------------------------------------------------------------------------
// GetNumberOfPicture
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsRichText::GetNumberOfPicture()
    {
    FUNC_LOG;
    return iPictureAreaArray.Count();
    }

// ---------------------------------------------------------------------------
// GetNumberOfHotSpotAreas
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsRichText::GetNumberOfHotSpotAreas()
    {
    FUNC_LOG;
    return iHotSpotAreaArray.Count();
    }

// ---------------------------------------------------------------------------
// GetExpandAreaL
// ---------------------------------------------------------------------------
EXPORT_C TRichTextExapandArea CFsRichText::GetExpandAreaL(TInt aIndex)
    {
    FUNC_LOG;
    if(aIndex > iExpandAreaArray.Count())
        {
        User::Leave(KErrArgument);
        }
        
    return iExpandAreaArray[aIndex];
    }

// ---------------------------------------------------------------------------
// GetHotSpotAreaL
// ---------------------------------------------------------------------------
EXPORT_C TRichTextHotSpotArea CFsRichText::GetHotSpotAreaL(TInt aIndex)
    {
    FUNC_LOG;
    if(aIndex > iHotSpotAreaArray.Count())
        {
        User::Leave(KErrArgument);
        }
        
    return iHotSpotAreaArray[aIndex];
    }

// ---------------------------------------------------------------------------
// GetPictureArea
// ---------------------------------------------------------------------------
// <cmail>
EXPORT_C TRichTextPictureArea CFsRichText::GetPictureAreaL(TInt aIndex)
// </cmail>
    {
    FUNC_LOG;
    if(aIndex > iPictureAreaArray.Count())
        {
        User::Leave(KErrArgument);
        }
        
    return iPictureAreaArray[aIndex];    
    }

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
EXPORT_C void CFsRichText::Reset()
    {
    FUNC_LOG;
    CRichText::Reset();
    iExpandAreaArray.Reset();
    iHotSpotAreaArray.Reset();
    iPictureAreaArray.Reset();
    }

