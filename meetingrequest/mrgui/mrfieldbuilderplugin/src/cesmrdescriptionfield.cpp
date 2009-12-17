/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR Description (summary) field implementation
 *
*/


#include "emailtrace.h"
#include <calentry.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
//<cmail>
#include "cesmrurlparserplugin.h"
//</cmail>

#include "cesmrdescriptionfield.h"
#include "mesmrlistobserver.h"
#include "cesmreditor.h"
#include "cesmrrichtextlink.h"
#include "cesmrlayoutmgr.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrfeaturesettings.h"
#include "cesmrglobalnote.h"

namespace{ // codescanner::namespace
const TInt KFieldInnerShrink(3);
}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRDescriptionField::NewL
// ---------------------------------------------------------------------------
//
CESMRDescriptionField* CESMRDescriptionField::NewL( )
    {
    FUNC_LOG;
    CESMRDescriptionField* self = new (ELeave) CESMRDescriptionField;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::~CESMRDescriptionField
// ---------------------------------------------------------------------------
//
CESMRDescriptionField::~CESMRDescriptionField( )
    {
    FUNC_LOG;
    delete iFrameBgContext;
    
    delete iLocationLink;
    
    delete iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::InitializeL()
    {
    FUNC_LOG;
    iDescription->SetFontL( iLayout->Font (iCoeEnv, iFieldId ), iLayout );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry ( );
    HBufC* summary = entry.DescriptionL().AllocLC ( );

    // externalize is done only when there is text and its NOT the default one.
    if ( summary->Length ( )> 0 && iDescription->DefaultText().Compare (*summary )!= 0 )
        {
        iDescription->ClearSelectionAndSetTextL ( *summary );
        if ( FeaturesL().FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            StoreLinkL( aEntry );
            }
        }

    CleanupStack::PopAndDestroy ( summary );

    // this is needed to be re-called here, otherwise the CEikEdwin
    // does not get correctly instantiated with default text
    iDescription->FocusChanged(EDrawNow);
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* buf = iDescription->GetTextInHBufL();

    if( buf )
        {
        CleanupStack::PushL( buf );

        if( iDescription->DefaultText().Compare( *buf ) != 0 )
            {
            CCalEntry& entry = aEntry.Entry();
                        
            HBufC* newBuf = AddLinkToTextL( *buf );
            if( newBuf )
                {
                CleanupStack::PopAndDestroy( buf );
                buf = newBuf;
                CleanupStack::PushL( buf );
                }
            entry.SetDescriptionL( *buf );
            }
        CleanupStack::PopAndDestroy( buf );
        }
    else if( iLocationLink )
        {
        CCalEntry& entry = aEntry.Entry();
        entry.SetDescriptionL( iLocationLink->Value() );
        }
    else
        {
        CCalEntry& entry = aEntry.Entry();
        entry.SetDescriptionL( KNullDesC );
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ExpandedHeight
// ---------------------------------------------------------------------------
//
TInt CESMRDescriptionField::ExpandedHeight( ) const
    {
    FUNC_LOG;
    return iSize.iHeight;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRDescriptionField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    return iDescription->OfferKeyEventL( aEvent, aType );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRDescriptionField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    iSize = aSize;
    iSize.iHeight -= KEditorDifference;

    if (iLayout->CurrentFontZoom() == EAknUiZoomSmall ||
        iLayout->CurrentFontZoom() == EAknUiZoomVerySmall)
        {
        iSize.iHeight -= KEditorDifference;
        }

    if ( iObserver )
        {
        iObserver->ControlSizeChanged ( this );
        }

    if( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iDescription->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::HandleEdwinEventL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType)
	{
    FUNC_LOG;
    if ( aEdwin == iDescription
    		&& aEventType == EEventTextUpdate )
        {
        HBufC* text = iDescription->GetTextInHBufL();
        if ( text )
            {
            CleanupStack::PushL( text );
            TInt textLength( text->Length() );
            if ( iDescription->GetLimitLength() <= textLength )
            	{
            	NotifyEventAsyncL( EESMRCmdSizeExceeded );
            	}            
            CleanupStack::PopAndDestroy( text );
            }
        }
	}

// ---------------------------------------------------------------------------
// CESMRDescriptionField::CESMRDescriptionField
// ---------------------------------------------------------------------------
//
CESMRDescriptionField::CESMRDescriptionField() :
    iSize( TSize( 0, 0 ))
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect(TPoint(iDescription->Position()), iDescription->Size());
    TRect inner(rect);
    inner.Shrink( KFieldInnerShrink, KFieldInnerShrink );
    TRect outer(rect);

    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrInput, 
                                                                outer, 
                                                                inner, 
                                                                EFalse );

    iFrameBgContext->SetParentContext( iBackground );
    iDescription->SetSkinBackgroundControlContextL( iFrameBgContext );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::PositionChanged()
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    if( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iDescription->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::ConstructL()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldDescription );
    SetExpandable( );
    iDescription = CESMREditor::NewL( this, 1, KTextLimit,
                                    CEikEdwin::EResizable |
                                    CEikEdwin::EAvkonEditor );
    iDescription->SetEdwinSizeObserver( this );
    iDescription->SetEdwinObserver( this );

    HBufC* buf = StringLoader::LoadLC( R_QTN_MEET_REQ_DETAILS_FIELD );
    iDescription->SetDefaultTextL( buf ); // ownership transferred
    CleanupStack::Pop( buf );

    iBackground = AknsDrawUtils::ControlContext( this );

    CESMRField::ConstructL( iDescription );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::GetMinimumVisibleVerticalArea
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aLower = iDescription->CurrentLineNumber() * iDescription->RowHeight();
    aUpper = aLower - iDescription->RowHeight();
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::ListObserverSet()
    {
    FUNC_LOG;
    iDescription->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::StoreLinkL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::StoreLinkL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TInt command( EESMRCmdDisableWaypointIcon );
    CESMRUrlParserPlugin* urlParser = CESMRUrlParserPlugin::NewL();
    CleanupStack::PushL( urlParser );
    TPtrC urlPointer;
    TInt position;
    position = urlParser->FindLocationUrl( aEntry.Entry().DescriptionL(), 
                                           urlPointer );
    CleanupStack::PopAndDestroy( urlParser );
    if ( position >= KErrNone )
        {
        command = EESMRCmdEnableWaypointIcon;
        HBufC* description =
            HBufC::NewL( aEntry.Entry().DescriptionL().Length() );
        CleanupStack::PushL( description );
        TPtr descriptionPointer( description->Des() );
        descriptionPointer.Append( aEntry.Entry().DescriptionL() );
        descriptionPointer.Delete( position, urlPointer.Length() );
        descriptionPointer.TrimLeft();
        if ( description->Length() != 0 )
            {
            iDescription->ClearSelectionAndSetTextL( *description );
            }
        else
            {
            iDescription->ClearSelectionAndSetTextL( iDescription->DefaultText() );
            }
        CleanupStack::PopAndDestroy( description );

        if ( iLocationLink )
            {
            delete iLocationLink;
            iLocationLink = NULL;
            }
        HBufC* showOnMapBuf =
            StringLoader::LoadLC( R_MEET_REQ_LINK_SHOW_ON_MAP );        
        iLocationLink = CESMRRichTextLink::NewL( position, 
                                     showOnMapBuf->Length(), 
                                     urlPointer,
                                     CESMRRichTextLink::ETypeLocationUrl,
                                     CESMRRichTextLink::ETriggerKeyOk );
        CleanupStack::PopAndDestroy( showOnMapBuf );        
        }
    
    NotifyEventL( command );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::AddLinkToTextL
// ---------------------------------------------------------------------------
//
HBufC* CESMRDescriptionField::AddLinkToTextL( const TDesC& aBuf )
    {
    FUNC_LOG;
    if( iLocationLink )
        {
        TInt newTextLen = aBuf.Length() + iLocationLink->Value().Length() + 1;
        HBufC* retBuf = HBufC::NewL( newTextLen );
        
        retBuf->Des().Append( iLocationLink->Value() );
        _LIT( KParagraphSeparator, "\x2029" ); // unicode paragraph separator
        // add the separator after url
        retBuf->Des().Append( KParagraphSeparator );
        retBuf->Des().Append( aBuf );

        return retBuf;
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::FeaturesL
// ---------------------------------------------------------------------------
//
CESMRFeatureSettings& CESMRDescriptionField::FeaturesL()
    {
    FUNC_LOG;
    if ( !iFeatures )
        {
        iFeatures = CESMRFeatureSettings::NewL();
        }
    
    return *iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::ExecuteGenericCommandL( 
		TInt aCommand )
	{
    FUNC_LOG;
	if ( EESMRCmdSizeExceeded == aCommand )
		{
		CESMRGlobalNote::ExecuteL ( 
				CESMRGlobalNote::EESMRCannotDisplayMuchMore );
		
		HBufC* text = iDescription->GetTextInHBufL();
		CleanupDeletePushL( text );
		if ( text )
			{    
	    	TInt curPos = iDescription->CursorPos();    	
	    	if( curPos > iDescription->GetLimitLength() - 1 )
	    		curPos = iDescription->GetLimitLength() - 1;
	    	HBufC* newText = 
	    		text->Des().Mid( 0, iDescription->GetLimitLength() - 1 ).AllocLC();    	
	    	
	    	iDescription->SetTextL ( newText );
	    	CleanupStack::PopAndDestroy( newText );
	    	newText = NULL;

	    	iDescription->SetCursorPosL (curPos, EFalse );
	    	iDescription->HandleTextChangedL();
	    	iDescription->UpdateScrollBarsL();
	    	SetFocus(ETrue);
			}
		CleanupStack::PopAndDestroy( text );	
		}
	}


// EOF
