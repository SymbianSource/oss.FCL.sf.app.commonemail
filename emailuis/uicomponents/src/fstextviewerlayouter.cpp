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
* Description:  Visual aligning class
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "emailtrace.h"
#include <gdi.h>
#include <w32std.h>
// <cmail> SF
#include <alf/alfenv.h>
#include <alf/alftextstyle.h>
#include <alf/alftextstylemanager.h>
// </cmail>
#include <AknLayoutFont.h>
#include <AknFontAccess.h> 
#include <AknBidiTextUtils.h>
// <cmail>
#include <alf/alftextvisual.h>
// </cmail>

#include "fstextviewervisualizer.h"

#include "fstextviewerlayouter.h"
#include "fsrichtextobject.h"
#include "fstextstylemanager.h"
#include "fsrichtext.h"
#include "fsrichtexttext.h"
#include "fsrichtextsmiley.h"
#include "fsrichtextpicture.h"
#include "fstextviewercoordinator.h"
#include "fstextviewervisualizerdata.h"
#include "fstextparser.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerLayouter* CFsTextViewerLayouter::NewL( CAlfEnv* aEnv )
    {
    FUNC_LOG;
    CFsTextViewerLayouter* self = 
        new (ELeave) CFsTextViewerLayouter( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerLayouter
// ---------------------------------------------------------------------------
CFsTextViewerLayouter::~CFsTextViewerLayouter()
    {
    FUNC_LOG;
    iCurrentLineArray.Close();
    }

// ---------------------------------------------------------------------------
// MeasureBidiTextBoundsWidth
// ---------------------------------------------------------------------------
TInt CFsTextViewerLayouter::MeasureBidiTextBoundsWidth(
    const CFont& aFont,
    const TDesC& aText,
    CFont::TMeasureTextInput::TFlags aOrder)
    {
    FUNC_LOG;
//    return AknBidiTextUtils::MeasureTextBoundsWidth( aFont, aText, aOrder);
    CFont::TMeasureTextInput input; 
    input.iFlags = aOrder;
    CFont::TMeasureTextOutput output; 
    
    TInt textAdvance = aFont.MeasureText( aText, &input, &output );
	
	TRect bounds = output.iBounds;
	bounds.iTl.iX = Min(bounds.iTl.iX, 0);
	bounds.iBr.iX = Max(bounds.iBr.iX, textAdvance);    
    return bounds.Width();
    }


// ---------------------------------------------------------------------------
// CalculateSizeL
// ---------------------------------------------------------------------------
TSize CFsTextViewerLayouter::CalculateSizeL( MFsRichTextObject* aObject )
    {
    FUNC_LOG;
    TSize retVal;

    switch ( aObject->GetType() )
        {
        case MFsRichTextObject::EFsText:
            {
            if ( static_cast<CFsRichTextText*>(aObject)->GetHotSpot() && static_cast<CFsRichTextText*>(aObject)->GetText().Length() > 1 )
                {
                CAlfTextVisual* tmp = CAlfTextVisual::AddNewL( *iControl, iVisualizer->BackgroundLayout() );
                tmp->SetTextL( static_cast<CFsRichTextText*>(aObject)->GetText() );
                tmp->SetTextStyle( static_cast<CFsRichTextText*>(aObject)->GetStyleId() );
                tmp->SetAlign( EAlfAlignHLeft, EAlfAlignVTop );
                tmp->SetPadding( 0 );
//                retVal = tmp->SubstringExtents( 0, static_cast<CFsRichTextText*>(aObject)->GetText().Length() - 1 ).Size();
                retVal = tmp->TextExtents();
                // <cmail> Platform layout change
                //retVal.iWidth -= 4;
                // </cmail> Platform layout change
                tmp->RemoveAndDestroyAllD();
                break;
                }
            
            CFont* font;
            User::LeaveIfNull( iScreenDev );
        
            CAlfTextStyle* textStyle(
                iEnv->TextStyleManager().TextStyle(
                    static_cast<CFsRichTextText*>(aObject)->GetStyleId() ) );

            HBufC* typeface( textStyle->TypefaceNameL() );
            CleanupStack::PushL( typeface );
            TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );
            if ( textStyle->IsBold() )
				{
				fontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
				}
            if ( textStyle->IsItalic() )
            	{
            	fontSpec.iFontStyle.SetPosture( EPostureItalic );
            	}

            User::LeaveIfError(
                iScreenDev->GetNearestFontInTwips(
                    font, fontSpec ) );

//            TAknFontSpecification aknFs(EAknFontCategoryPrimary, fontSpec, iScreenDev);
//            aknFs.SetTextPaneHeight( iScreenDev->VerticalTwipsToPixels(fontSpec.iHeight));
//            aknFs.SetTextPaneHeightIsDesignHeight( ETrue );
//            font = 
//                AknFontAccess::CreateLayoutFontFromSpecificationL( 
//                    *iScreenDev, 
//                    fontSpec.iTypeface,
//                    aknFs ); 
            

//            retVal = TSize(
//                    font->TextWidthInPixels(
//                        static_cast<CFsRichTextText*>(aObject)->GetText() ), 
//                    font->FontMaxHeight() );
            
            CleanupStack::PopAndDestroy( typeface );
            TSize retVal1( MeasureBidiTextBoundsWidth( *font, static_cast<CFsRichTextText*>(aObject)->GetText(), CFont::TMeasureTextInput::EFVisualOrder), font->FontMaxHeight() );

            retVal = retVal1;
    
            const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( font );
            if ( layoutFont )
            {
                retVal.iHeight = font->HeightInPixels();
                TInt textPaneHeight = layoutFont->TextPaneHeight();
                TInt textPaneTopToBaseline = layoutFont->TextPaneTopToBaseline();
        
                retVal.iHeight += textPaneHeight - textPaneTopToBaseline;
                retVal.iHeight += 1;
                }
            else
                {
                retVal.iHeight = Max(retVal.iHeight, font->HeightInPixels());
                retVal.iHeight += 5; // the best approximation - fails on big (>=72) fonts
                }
            iScreenDev->ReleaseFont( font );
//            if ( static_cast<CFsRichTextText*>(aObject)->GetHotSpot() && static_cast<CFsRichTextText*>(aObject)->GetText().Length() > 1 )
//                {
//                retVal.iWidth += 3;
//                }
            break;
            }
        case MFsRichTextObject::EFsTexture:
            {
            retVal = static_cast<CFsRichTextPicture*>
                (aObject)->GetTextureSize();
            retVal.iWidth += 3;
            break;
            }
        case MFsRichTextObject::EFsSmiley:
            {
            if ( iSmileyEnabled )
                {
                retVal = static_cast<CFsRichTextSmiley*>
                     (aObject)->GetPictureObject()->GetTextureSize();
                }
            else
                {
                CFont* font;
                CFsRichTextText* object = 
                    static_cast<CFsRichTextSmiley*>
                     (aObject)->GetTextObject();
                User::LeaveIfNull(iScreenDev);
        
                CAlfTextStyle* textStyle(
                    iEnv->TextStyleManager().TextStyle(
                        object->GetStyleId() ) );

                HBufC* typeface( textStyle->TypefaceNameL() );
                CleanupStack::PushL( typeface );
                TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );

                User::LeaveIfError(
                    iScreenDev->GetNearestFontInTwips( font, fontSpec ) );

                retVal = TSize(
                        font->TextWidthInPixels( object->GetText() ), 
                        font->FontMaxHeight() );
                iScreenDev->ReleaseFont( font );
                CleanupStack::PopAndDestroy( typeface );
                break;
                }
            }
        default:
            {
            break;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// FindMaxLengthL
// ---------------------------------------------------------------------------
TInt CFsTextViewerLayouter::FindMaxLengthL( MFsRichTextObject* aObject )
    {
    FUNC_LOG;
    TInt retVal = 0;
    if ( aObject->GetType() == MFsRichTextObject::EFsText )
        {
        CFont* font;
        User::LeaveIfNull( iScreenDev );
            
        CAlfTextStyle* textStyle(
            iEnv->TextStyleManager().TextStyle(
                static_cast<CFsRichTextText*>( aObject )->GetStyleId() ) );

        HBufC* typeface( textStyle->TypefaceNameL() );
        CleanupStack::PushL( typeface );
        TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );

        User::LeaveIfError(
            iScreenDev->GetNearestFontInTwips(
                font, fontSpec ) );
        
        retVal = font->TextCount( static_cast<CFsRichTextText*>(aObject)->GetText(), iDisplaySize.iWidth ) - 1;
        iScreenDev->ReleaseFont( font );
        CleanupStack::PopAndDestroy( typeface );
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// PrepareRangedVisL
// ---------------------------------------------------------------------------
TFsRangedVisual* CFsTextViewerLayouter::PrepareRangedVisL( 
                        MFsRichTextObject* aObject,
                        CAlfLayout* aLayout )
    {
    FUNC_LOG;
    TFsRangedVisual* retVal = new (ELeave) TFsRangedVisual;
    retVal->iVisual = NULL;
    retVal->iHotspot = EFalse;
    retVal->iEmbed = EFalse;

    switch ( aObject->GetType() )
        {
        case MFsRichTextObject::EFsText:
            {
            retVal->iStart = aObject->GetBeginOfObject();
            retVal->iEnd = aObject->GetEndOfObject();
            retVal->iIsActive = ETrue;
            retVal->iLayout = aLayout;
            retVal->iIsText = ETrue;
            retVal->iTextColor = 
                static_cast<CFsRichTextText*>(aObject)->GetTextColor();
            retVal->iTextStyleId = static_cast<CFsRichTextText*>(aObject)->GetStyleId();
            retVal->iDirection = 
                static_cast<CFsRichTextText*>(aObject)->GetTextDirection();
            retVal->iSmiley = EFalse;
            break;
            }
        case MFsRichTextObject::EFsTexture:
            {
            retVal->iStart = aObject->GetBeginOfObject(); 
            retVal->iEnd = aObject->GetEndOfObject();
            retVal->iIsActive = ETrue;
            retVal->iLayout = aLayout;
            retVal->iIsText = EFalse;
            retVal->iSmiley = EFalse;
            break;
            }
        case MFsRichTextObject::EFsSmiley:
            {
            if ( iSmileyEnabled )
                {
                retVal->iStart = aObject->GetBeginOfObject(); 
                retVal->iEnd = aObject->GetEndOfObject();
                retVal->iIsActive = ETrue;
                retVal->iLayout = aLayout;
                retVal->iIsText = EFalse;
                retVal->iTextColor = 
                    static_cast<CFsRichTextSmiley*>(aObject)->GetTextObject()->GetTextColor();
                retVal->iTextStyleId = 
                    static_cast<CFsRichTextSmiley*>(aObject)->GetTextObject()->GetStyleId();
                }
            else
                {
                retVal->iStart = aObject->GetBeginOfObject(); 
                retVal->iEnd = aObject->GetEndOfObject();
                retVal->iIsActive = ETrue;
                retVal->iLayout = aLayout;
                retVal->iIsText = ETrue;
                retVal->iTextColor = 
                    static_cast<CFsRichTextSmiley*>(aObject)->GetTextObject()->GetTextColor();
                retVal->iTextStyleId = 
                    static_cast<CFsRichTextSmiley*>(aObject)->GetTextObject()->GetStyleId();
                }
            retVal->iSmiley = ETrue;
            break;
            }
        case MFsRichTextObject::EFsNewLine:
            {
            break;
            }
        default:
            {
            break;
            }
        }

    if ( retVal->iIsActive )
        {
        if ( aObject->GetEmbeded() )
            {
            retVal->iId = aObject->GetIdOfEmbeded();
            retVal->iEmbed = ETrue;
            }
        if ( aObject->GetHotSpot() )
            {
            retVal->iHotspotType = aObject->GetTypeOfHotSpot();
            retVal->iId = aObject->GetIdOfHotSpot();
            retVal->iHotspot = ETrue;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// SetDisplaySize
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetDisplaySize( TSize aDisplaySize )
    {
    FUNC_LOG;
    iDisplaySize = aDisplaySize;
    }

// ---------------------------------------------------------------------------
// AppendObjectL
// ---------------------------------------------------------------------------
TBool CFsTextViewerLayouter::AppendObjectL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TBool aRegisterSelects )
    {
    FUNC_LOG;
    TRect objectRect;
    TRect naturalRect;
    TSize objectSize = CalculateSizeL( aObject );
    
    if ( objectSize.iWidth > iDisplaySize.iWidth )
        {
        if ( iLowestPicPosition > iCurrentLineOffset )
            {
            iCurrentLineOffset = iLowestPicPosition;
            ConstructLineL();
            }
        if ( aObject->GetType() != MFsRichTextObject::EFsText )
            {
            // Resizing object to fit display width
            const TInt KScalingFactor = 65536;
            const TInt KTwo = 2;
            
            TInt resizeFactor = KScalingFactor * iDisplaySize.iWidth /
                                objectSize.iWidth;
            TInt height = objectSize.iHeight;
            
            objectSize.iWidth = iDisplaySize.iWidth;
            
            height = height * resizeFactor + KScalingFactor / KTwo;
            objectSize.iHeight = height / KScalingFactor;
            }
        else
            {
            aParser->CutWord(
                aObject->GetBeginOfObject(),
                FindMaxLengthL( aObject ) );
            return EFalse;
            }
        }
    
    TFsRangedVisual* currentVisual;
    currentVisual = PrepareRangedVisL( aObject, aLayout );
    if ( !aRegisterSelects )
        {
        currentVisual->iEmbed = EFalse;
        }
    
    if ( iMainDirection == TBidiText::ELeftToRight )
        {
        objectRect = TRect( TPoint( iLastCol, iCurrentLineOffset ), objectSize );
        }
    else
        {        
        objectRect = TRect( TPoint( iLastCol - objectSize.iWidth, iCurrentLineOffset ), objectSize );
        }
    iNewLine = EFalse;
    naturalRect = objectRect;
    if ( iHelper->NextFreeCol( objectRect, iMainDirection ) )
        {
        ConstructLineL();
        delete currentVisual;
        currentVisual = NULL;
        AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
        }
    else
        {
        if ( objectRect != naturalRect )
            {
            currentVisual->iMoved = ETrue;
            }
        else 
            {
            currentVisual->iMoved = EFalse;
            }
        
        if ( iMainDirection == TBidiText::ELeftToRight )
            {
            currentVisual->iRect = objectRect;
            iLastCol = objectRect.iBr.iX;
            iCurrentLineArray.Append( currentVisual );
            }
        else
            {
            currentVisual->iRect = objectRect;
            iLastCol = objectRect.iTl.iX;
            iCurrentLineArray.Append( currentVisual );
            }
        }
    if ( objectRect.Height() > iLineHeight && aObject->GetType() != MFsRichTextObject::EFsTexture )
        {
        iLineHeight = objectRect.Height();
        }
    else if ( aObject->GetType() == MFsRichTextObject::EFsTexture || aObject->GetHotSpot() )
        {
        iHelper->RegisterObstacleL( objectRect );
        if ( objectRect.iBr.iY > iLowestPicPosition )
            {
            iLowestPicPosition = objectRect.iBr.iY;
            }
        }
//    if ( text )
//    	{
//    	delete text;
//    	}
    return ETrue;
    }

// ---------------------------------------------------------------------------
// InsertObjectL
// ---------------------------------------------------------------------------
TBool CFsTextViewerLayouter::InsertObjectL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TBool aRegisterSelects )
    {
    FUNC_LOG;
    return AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
    //<cmail> following lines are commented out to avoid warnings
    /*
    TBool retVal = EFalse;
    
    TSize objectSize = CalculateSizeL( aObject );
    if ( objectSize.iWidth > iDisplaySize.iWidth )
        {
        aParser->CutWord( 
            aObject->GetBeginOfObject(),
            FindMaxLengthL( aObject ) );
        return EFalse;
        }
    
    if ( ( objectSize.iWidth > iDisplaySize.iWidth - iLastCol && iMainDirection == TBidiText::ELeftToRight )
            || ( objectSize.iWidth > iLastCol && iMainDirection == TBidiText::ERightToLeft ) )
        {
        ConstructLineL();
        retVal = AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
        }
    else //if ( 0 == iCurrentLineArray.Count() )
        {
        retVal = AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
        }*/
    //</cmail>    
//    else
//        {
//        if ( iMainDirection == TBidiText::ELeftToRight )
//            {
////            retVal = ReorderLineFromLeftL( aObject, aLayout, aParser, objectSize, aRegisterSelects );
//            }
//        else 
//            {
////            retVal = ReorderLineFromRightL( aObject, aLayout, aParser, objectSize, aRegisterSelects );
//            }
//        }
    //<cmail> following lines are commented out to avoid warnings
    //return retVal;
    //</cmail>
    }

// ---------------------------------------------------------------------------
// ConstructLineL
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::ConstructLineL( TInt aHeight )
    {
    FUNC_LOG;
    TInt i = 0;
    TInt dY;
    RPointerArray<TFsRangedVisual> finalLine;
    
    if ( 0 == iLineHeight )
        {
        iLineHeight = aHeight;
        }
    
    for ( i = 0; i < iCurrentLineArray.Count(); ++i )
        {
        iCurrentLineArray[i]->iLineNumber = iLines;
        if ( iCurrentLineArray[i]->iIsText )
            {
            dY = iLineHeight - iCurrentLineArray[i]->iRect.Height();
            iCurrentLineArray[i]->iRect.iTl.iY += dY;
            iCurrentLineArray[i]->iRect.iBr.iY += dY;
            }
        if ( i > 0
                && iCurrentLineArray[i]->iIsText 
                && finalLine[finalLine.Count() - 1]->iIsText
//                && iCurrentLineArray[i]->iDirection == finalLine[finalLine.Count() - 1]->iDirection
//                && iCurrentLineArray[i]->iDirection == iMainDirection
                && iCurrentLineArray[i]->iTextStyleId == finalLine[finalLine.Count() - 1]->iTextStyleId
                && iCurrentLineArray[i]->iTextColor == finalLine[finalLine.Count() - 1]->iTextColor
                && ( !iCurrentLineArray[i]->iHotspot 
                        ||
                        iCurrentLineArray[i]->iId == finalLine[finalLine.Count() - 1]->iId
                        )
//                && !finalLine[finalLine.Count() - 1]->iHotspot
                && !iCurrentLineArray[i]->iMoved
//                && !iCurrentLineArray[i]->iEmbed
//                && !finalLine[finalLine.Count() - 1]->iEmbed                
                 )
            
            {
            JoinRangedVisuals( finalLine[finalLine.Count() - 1], iCurrentLineArray[i] );
            delete iCurrentLineArray[i];
            iCurrentLineArray[i] = NULL;
            }
        else
            {
            finalLine.Append( iCurrentLineArray[i] );
            }
        }
    
    for ( i = 0; i < finalLine.Count(); ++i )
        {
        iNavigator->AppendVisualL( finalLine[i] );
        }
    finalLine.Close();
    
    if ( iMainDirection == TBidiText::ELeftToRight )
        {
        iLastCol = iLeftMargin;
        }
    else 
        {
        iLastCol = iDisplaySize.iWidth + iLeftMargin;
        }
    
    iNewLine = ETrue;
    
    iCurrentLineArray.Reset();
    iCurrentLineOffset += iLineHeight;
    iLineHeight = 0;
    ++iLines;
    }

// ---------------------------------------------------------------------------
// JoinRangedVisuals
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::JoinRangedVisuals( TFsRangedVisual* aFirst, TFsRangedVisual* aSecond )
    {
    FUNC_LOG;
/*        if ( aFirst->iRect.iTl.iX > aSecond->iRect.iTl.iX )
            {
            aFirst->iRect.iTl.iX = aSecond->iRect.iTl.iX;
            }
        else 
            {
            aFirst->iRect.iBr.iX = aSecond->iRect.iBr.iX;
            }
            */
        TInt a = aFirst->iRect.iTl.iX;
        TInt b = aFirst->iRect.iBr.iX;
        TInt c = aSecond->iRect.iTl.iX;
        TInt d = aSecond->iRect.iBr.iX;

        TInt max = Max( Max(a, b), Max(d, c) );
        TInt min = Min( Min(a, b), Min(d, c) );
        aFirst->iRect.iTl.iX = min;
        aFirst->iRect.iBr.iX = max;
        aFirst->iStart = Min( aFirst->iStart, aSecond->iStart );
        aFirst->iEnd = Max( aFirst->iEnd, aSecond->iEnd );
    }

// ---------------------------------------------------------------------------
// GetCurrentLineOffset
// ---------------------------------------------------------------------------
TInt CFsTextViewerLayouter::GetCurrentLineOffset()
    {
    FUNC_LOG;
    return iCurrentLineOffset;
    }

// ---------------------------------------------------------------------------
// SetMargins
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetCurrentLineOffset( TInt aOffset )
    {
    FUNC_LOG;
    iCurrentLineOffset = aOffset;
    }

// ---------------------------------------------------------------------------
// SetMargins
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetMargins( const TInt aLeft, const TInt aRight )
    {
    FUNC_LOG;
    iLeftMargin = aLeft;
    iRightMargin = aRight;
    if ( iMainDirection == TBidiText::ELeftToRight )
        {
        iLastCol = iLeftMargin;
        }
    else 
        {
        iLastCol = iDisplaySize.iWidth + iLeftMargin;
        }
    }

// ---------------------------------------------------------------------------
// SetHelper
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetHelper( CFsTextViewerCoordinator* aHelper )
    {
    FUNC_LOG;
    iHelper = aHelper;
    }

// ---------------------------------------------------------------------------
// SetNavigator
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetNavigator( CFsTextViewerVisualizerData* aNavigator )
    {
    FUNC_LOG;
    iNavigator = aNavigator;
    }

// ---------------------------------------------------------------------------
// Lines
// ---------------------------------------------------------------------------
TInt CFsTextViewerLayouter::Lines()
    {
    FUNC_LOG;
    return iLines;
    }

// ---------------------------------------------------------------------------
// EnableSmileys
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::EnableSmileys( TBool aEnable )
    {
    FUNC_LOG;
    iSmileyEnabled = aEnable;
    }

// ---------------------------------------------------------------------------
// SetPosition
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetPosition( TInt aLine, TInt aLineOffset )
    {
    FUNC_LOG;
    iLines = aLine;
    iCurrentLineOffset = aLineOffset;
    }

// ---------------------------------------------------------------------------
// ChangeDirection
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::ChangeDirection( TBidiText::TDirectionality aDirection )
    {
    FUNC_LOG;
    iMainDirection = aDirection;
    if ( iMainDirection == TBidiText::ELeftToRight && iCurrentLineArray.Count() < 1 )
		{
		iLastCol = iLeftMargin;
		}
	else if ( iMainDirection == TBidiText::ERightToLeft && iCurrentLineArray.Count() < 1 )
		{
		iLastCol = iDisplaySize.iWidth + iLeftMargin;
		}
    }

// ---------------------------------------------------------------------------
// SetDirection
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::SetDirection( TBidiText::TDirectionality aDirection )
    {
    FUNC_LOG;
    iMainDirection = aDirection;
    if ( iMainDirection == TBidiText::ELeftToRight )
        {
        iLastCol = iLeftMargin;
        }
    else 
        {
        iLastCol = iDisplaySize.iWidth + iLeftMargin;
        }
    }

// ---------------------------------------------------------------------------
// GetDirection
// ---------------------------------------------------------------------------
TBidiText::TDirectionality CFsTextViewerLayouter::GetDirection()
    {
    FUNC_LOG;
    return iMainDirection;
    }

// ---------------------------------------------------------------------------
// IsNewLine
// ---------------------------------------------------------------------------
TBool CFsTextViewerLayouter::IsNewLine()
    {
    FUNC_LOG;
    if ( iCurrentLineArray.Count() < 1 )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerLayouter::ConstructL()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// CFsTextViewerLayouter
// ---------------------------------------------------------------------------
CFsTextViewerLayouter::CFsTextViewerLayouter( CAlfEnv* aEnv )
    {
    FUNC_LOG;
    iScreenDev = CCoeEnv::Static()->ScreenDevice();
    iEnv = aEnv;
    iLines = 0;
    iLowestPicPosition = 0;
    iMainDirection = TBidiText::ELeftToRight;
    }

// ---------------------------------------------------------------------------
// ReorderLineFromLeftL
// ---------------------------------------------------------------------------
TBool CFsTextViewerLayouter::ReorderLineFromLeftL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TSize aObjectSize, TBool aRegisterSelects )
    {
    FUNC_LOG;
//    TBool retVal = EFalse;
//    TInt i = iCurrentLineArray.Count() - 1;
//    TInt dX = aObjectSize.iWidth;
//    TInt moveFrom = 0;
//    TRect translatedObject;
//    TRect origRect;
//    while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
//        {
//        translatedObject = iCurrentLineArray[i]->iRect;
////        if ( iCurrentLineArray[i]->iIsText || iCurrentLineArray[i]->iSmiley )
////            {
//            iHelper->RemoveRect( iCurrentLineArray[i]->iRect );
//            translatedObject.iTl.iX += dX;
//            translatedObject.iBr.iX += dX;
//            origRect = translatedObject;
//            if ( iHelper->NextFreeCol( translatedObject, iMainDirection ) )
//                {
//                i = -100;
//                }
//            if ( origRect != translatedObject )
//                {
//                iCurrentLineArray[i]->iMoved = ETrue;
//                }
////            }
//        moveFrom = i;
//        --i;
//        }
//    if ( -101 == i )
//        {
//        ConstructLineL();
//        retVal = AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
//        }
//    else 
//        {
//        TFsRangedVisual* currentVisual; 
//        currentVisual = PrepareRangedVisL( aObject, aLayout );
//        if ( !aRegisterSelects )
//            {
//            currentVisual->iEmbed = EFalse;
//            }
//        currentVisual->iRect = TRect( iCurrentLineArray[moveFrom]->iRect.iTl, aObjectSize );
//        iCurrentLineArray.Insert( currentVisual, moveFrom );
//        for ( i = moveFrom + 1; i < iCurrentLineArray.Count(); ++i )
//            {
//            if ( iCurrentLineArray[i]->iIsText || iCurrentLineArray[i]->iSmiley )
//                {
//                iCurrentLineArray[i]->iRect.iTl.iX += dX;
//                iCurrentLineArray[i]->iRect.iBr.iX += dX;
//                }
//            }
//        if ( currentVisual->iRect.Height() > iLineHeight && aObject->GetType() != MFsRichTextObject::EFsTexture )
//            {
//            iLineHeight = currentVisual->iRect.Height();
//            }
//        }
//    return retVal;
    TBool retVal = EFalse;
    TInt dX = aObjectSize.iWidth;
    TInt moveFrom = 0;
    TRect translatedObject;
    TFsRangedVisual* visual = NULL;
    TInt maxCol = 0;
    // <cmail>TInt tmp = 0;
    
    TInt i = iCurrentLineArray.Count() - 1;
    while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
        {
        if ( !iCurrentLineArray[i]->iIsText )
            {
            iHelper->RemoveRect( iCurrentLineArray[i]->iRect );
            }
        --i;
        }
    i = iCurrentLineArray.Count() - 1;
    while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
        {
        translatedObject = TRect(iCurrentLineArray[i]->iRect.iTl, iCurrentLineArray[i]->iRect.iBr );
        visual = iCurrentLineArray[i];
            translatedObject.iTl.iX += dX;
            translatedObject.iBr.iX += dX;
        // <cmail> tmp = translatedObject.iTl.iX;
        moveFrom = i;
        if ( iHelper->NextFreeCol( translatedObject, TBidiText::ELeftToRight ) )
                {
                i = -100;
                }
        --i;
        }
    if ( -101 == i )
        {
        i = iCurrentLineArray.Count() - 1;
        while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
            {
            if ( !iCurrentLineArray[i]->iIsText )
                {
                translatedObject = TRect(iCurrentLineArray[i]->iRect.iTl, iCurrentLineArray[i]->iRect.iBr );
                translatedObject.iTl.iX += dX;
                translatedObject.iBr.iX += dX;
                iHelper->RegisterObstacleL( iCurrentLineArray[i]->iRect );
                }
            --i;
            }
        ConstructLineL();
        retVal = AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
        }
    else 
        {
        TFsRangedVisual* currentVisual; 
        currentVisual = PrepareRangedVisL( aObject, aLayout );
        currentVisual->iMoved = EFalse;
        if ( !aRegisterSelects )
            {
            currentVisual->iEmbed = EFalse;
            }
        
        currentVisual->iRect = TRect( TPoint( iCurrentLineArray[moveFrom]->iRect.iTl.iX, iCurrentLineArray[moveFrom]->iRect.iTl.iY ), aObjectSize );
        iCurrentLineArray.Insert( currentVisual, moveFrom );
        
        if ( aObject->GetType() == MFsRichTextObject::EFsTexture )
            {
            iHelper->RegisterObstacleL( currentVisual->iRect );
            }
        if ( currentVisual->iRect.Height() > iLineHeight && aObject->GetType() != MFsRichTextObject::EFsTexture )
            {
            iLineHeight = currentVisual->iRect.Height();
            }
        
        for ( i = moveFrom + 1; i < iCurrentLineArray.Count(); ++i )
                {
//            if ( iCurrentLineArray[i]->iIsText || iCurrentLineArray[i]->iSmiley )
//                {
                visual = iCurrentLineArray[i];
                iCurrentLineArray[i]->iRect.iTl.iX += dX;
                iCurrentLineArray[i]->iRect.iBr.iX += dX;
                iHelper->NextFreeCol( iCurrentLineArray[i]->iRect, TBidiText::ELeftToRight );
                if ( !(visual->iIsText) )
                    {
                    iHelper->RegisterObstacleL( visual->iRect );
                }
               if ( visual->iRect.iBr.iX > maxCol )
            {
                   maxCol = visual->iRect.iBr.iX;
                   }
            }
        }
    iLastCol = maxCol;
    return retVal;
    }

// ---------------------------------------------------------------------------
// ReorderLineFromRightL
// ---------------------------------------------------------------------------
TBool CFsTextViewerLayouter::ReorderLineFromRightL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TSize aObjectSize, TBool aRegisterSelects )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    TInt dX = aObjectSize.iWidth;
    TInt moveFrom = 0;
    TRect translatedObject;
    TFsRangedVisual* visual = NULL;
    
    TInt i = iCurrentLineArray.Count() - 1;
    while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
        {
        if ( !iCurrentLineArray[i]->iIsText )
            {
            iHelper->RemoveRect( iCurrentLineArray[i]->iRect );
            }
        --i;
        }
    i = iCurrentLineArray.Count() - 1;
    while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
            {
        translatedObject = TRect(iCurrentLineArray[i]->iRect.iTl, iCurrentLineArray[i]->iRect.iBr );
        visual = iCurrentLineArray[i];
            translatedObject.iTl.iX -= dX;
            translatedObject.iBr.iX -= dX;
        moveFrom = i;
        if ( iHelper->NextFreeCol( translatedObject, TBidiText::ERightToLeft ) )
                {
                i = -100;
                }
        --i;
        }
    if ( -101 == i  )
        {
        i = iCurrentLineArray.Count() - 1;
        while ( i >= 0 && iCurrentLineArray[i]->iDirection != iMainDirection )
            {
            if ( !iCurrentLineArray[i]->iIsText )
                {
                translatedObject = TRect(iCurrentLineArray[i]->iRect.iTl, iCurrentLineArray[i]->iRect.iBr );
                translatedObject.iTl.iX -= dX;
                translatedObject.iBr.iX -= dX;
                iHelper->RegisterObstacleL( iCurrentLineArray[i]->iRect );
                }
            --i;
            }
        ConstructLineL();
        retVal = AppendObjectL( aObject, aLayout, aParser, aRegisterSelects );
        }
    else 
        {
        TFsRangedVisual* currentVisual; 
        currentVisual = PrepareRangedVisL( aObject, aLayout );
        currentVisual->iMoved = EFalse;
        if ( !aRegisterSelects )
            {
            currentVisual->iEmbed = EFalse;
            }
        
        currentVisual->iRect = TRect( TPoint( iCurrentLineArray[moveFrom]->iRect.iBr.iX - dX, iCurrentLineArray[moveFrom]->iRect.iTl.iY ), aObjectSize );
        iCurrentLineArray.Insert( currentVisual, moveFrom );
        
        if ( aObject->GetType() == MFsRichTextObject::EFsTexture )
                {
            iHelper->RegisterObstacleL( currentVisual->iRect );
            }
        if ( currentVisual->iRect.Height() > iLineHeight && aObject->GetType() != MFsRichTextObject::EFsTexture )
            {
            iLineHeight = currentVisual->iRect.Height();
            }
        
        for ( i = moveFrom + 1; i < iCurrentLineArray.Count(); ++i )
            {
//            if ( iCurrentLineArray[i]->iIsText || iCurrentLineArray[i]->iSmiley )
//                {
                visual = iCurrentLineArray[i];
                iCurrentLineArray[i]->iRect.iTl.iX -= dX;
                iCurrentLineArray[i]->iRect.iBr.iX -= dX;
                iHelper->NextFreeCol( iCurrentLineArray[i]->iRect, TBidiText::ERightToLeft );
                if ( !(visual->iIsText) )
                    {
                    iHelper->RegisterObstacleL( visual->iRect );
                    }
            }
        }
    return retVal;
    }

