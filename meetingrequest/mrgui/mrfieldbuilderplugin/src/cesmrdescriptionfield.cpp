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
* Description:  ESMR Description (summary) field implementation
 *
*/

#include "cesmrdescriptionfield.h"
#include "mesmrlistobserver.h"
#include "cesmreditor.h"
#include "cesmrrichtextlink.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrfeaturesettings.h"
#include "cesmrglobalnote.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
//<cmail>
#include "cesmrurlparserplugin.h"
//</cmail>

#include <calentry.h>
#include <stringloader.h>
#include <esmrgui.rsg>
#include <aknutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <eikmfne.h>

// DEBUG
#include "emailtrace.h"

namespace // codescanner::namespace
    {
    /// Field's component count, iDescription
    const TInt KComponentCount( 1 );
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
    delete iLocationLink;
    delete iFeatures;
    delete iBgControlContext;
    delete iUrlParser;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::InitializeL()
    {
    FUNC_LOG;
    // Do nothing
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
    if ( summary->Length ( )> 0 &&
            iDescription->DefaultText().Compare (*summary )!= 0 )
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
    CCalEntry& entry = aEntry.Entry();
    
    if( buf )
        {
        CleanupStack::PushL( buf );

        if( iDescription->DefaultText().Compare( *buf ) != 0 )
            {
            // Edited text. Add link to description text
            HBufC* newBuf = AddLinkToTextL( *buf );
            if( newBuf )
                {
                CleanupStack::PopAndDestroy( buf );
                buf = newBuf;
                CleanupStack::PushL( buf );
                }
            entry.SetDescriptionL( *buf );
            }
        else if ( iLocationLink )
            {
            // Location link set
            entry.SetDescriptionL( iLocationLink->Value() );
            }
        else if ( entry.DescriptionL().Length() != 0 )
            {
            // Clear old description
            entry.SetDescriptionL( KNullDesC );
            }
        CleanupStack::PopAndDestroy( buf );
        }
    else if( iLocationLink )
        {
        entry.SetDescriptionL( iLocationLink->Value() );
        }
    else
        {
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
// CESMRDescriptionField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRDescriptionField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    response = iDescription->OfferKeyEventL( aEvent, aType );

    if ( aType == EEventKey &&
         ( aEvent.iScanCode != EStdKeyUpArrow &&
          aEvent.iScanCode != EStdKeyDownArrow ))
        {
        iDescription->DrawDeferred();
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRDescriptionField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    if ( aSize != iSize )
        {
        iSize = aSize;

        if ( iObserver )
            {
            iObserver->ControlSizeChanged ( this );
            }

        if( iDescription->LineCount() != iLineCount )
            {
            // Line count has changed, the whole component needs
            // to be redrawn
            DrawDeferred();

            // Also if new line count is less than previous one, we
            // need to redraw the parent also (this is the last field).
            // Otherwise the removed line will remain on the screen
            // until parent is redrawn.
            if( iDescription->LineCount() < iLineCount )
                {
                Parent()->DrawDeferred();
                }
            iLineCount = iDescription->LineCount();
            }
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::HandleEdwinEventL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::HandleEdwinEventL(
        CEikEdwin* aEdwin,TEdwinEvent aEventType)
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
CESMRDescriptionField::CESMRDescriptionField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldDescription );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::ConstructL()
    {
    FUNC_LOG;
    iDescription = CESMREditor::NewL( this, 1, KTextLimit,
                                    CEikEdwin::EResizable |
                                    CEikEdwin::EAvkonEditor );
    iDescription->SetEdwinSizeObserver( this );
    iDescription->SetEdwinObserver( this );
    CESMRField::ConstructL( iDescription );

    HBufC* buf = StringLoader::LoadLC( R_QTN_MEET_REQ_DETAILS_FIELD );
    iDescription->SetDefaultTextL( buf ); // ownership transferred
    CleanupStack::Pop( buf );

    // Setting background instead of theme skin
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );

    TRect initialisationRect( 0, 0, 0, 0 );
    iBgControlContext = CAknsBasicBackgroundControlContext::NewL(
                bitmapStruct.iItemId,
                initialisationRect,
                EFalse );

    iDescription->SetSkinBackgroundControlContextL( iBgControlContext );
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
// CESMRDescriptionField::GetCursorLineVerticalPos
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower)
    {
    aLower = iDescription->CurrentLineNumber() * iDescription->RowHeight();
    aUpper = aLower - iDescription->RowHeight();
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::StoreLinkL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::StoreLinkL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TPtrC urlPointer;
    TInt position;
    position = UrlParserL().FindLocationUrl( aEntry.Entry().DescriptionL(),
                                           urlPointer );

    if ( position >= KErrNone )
        {
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
            iDescription->ClearSelectionAndSetTextL(
                    iDescription->DefaultText() );
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

        StoreGeoValueL( aEntry.Entry(),
                        urlPointer );
        }
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
TBool CESMRDescriptionField::ExecuteGenericCommandL(
        TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if ( EESMRCmdSizeExceeded == aCommand )
        {
        CESMRGlobalNote::ExecuteL(
                CESMRGlobalNote::EESMRCannotDisplayMuchMore );

        HBufC* text = iDescription->GetTextInHBufL();
        CleanupDeletePushL( text );
        if ( text )
            {
            TInt curPos = iDescription->CursorPos();
            if( curPos > iDescription->GetLimitLength() - 1 )
                curPos = iDescription->GetLimitLength() - 1;
            HBufC* newText =
                text->Des().Mid( 0,
                        iDescription->GetLimitLength() - 1 ).AllocLC();

            iDescription->SetTextL ( newText );
            CleanupStack::PopAndDestroy( newText );
            newText = NULL;

            iDescription->SetCursorPosL (curPos, EFalse );
            iDescription->HandleTextChangedL();
            iDescription->UpdateScrollBarsL();
            SetFocus(ETrue);
            }
        CleanupStack::PopAndDestroy( text );
        isUsed = ETrue;
        }
    else if ( EESMRCmdDisableWaypointIcon == aCommand )
        {
        isUsed = ETrue;
        
        // Clear location link
        delete iLocationLink;
        iLocationLink = NULL;
        }
    
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRDescriptionField::CountComponentControls() const
    {
    return KComponentCount;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRDescriptionField::ComponentControl( TInt aInd ) const
    {
    if( aInd == 0 )
        {
        return iDescription;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::SizeChanged()
    {
    TRect rect( Rect() );

    // Optimization: Layouting only when necessary
    if( iFieldRect != rect )
        {
        rect = NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ).Rect();

        TAknLayoutText editorLayoutText = NMRLayoutManager::GetLayoutText(
                        rect,
                        NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );

        TRect editorRect = editorLayoutText.TextRect();

        TRect bgRect( editorRect );
        // Move focus rect so that it's relative to field's position.
        bgRect.Move( -Position() );
        if( iSize.iHeight > 0 )
            {
            bgRect.SetHeight( iSize.iHeight );
            }
        SetFocusRect( bgRect );

        iDescription->SetRect(
                TRect( editorRect.iTl,
                        TSize( editorRect.Width(), iSize.iHeight ) ) );

        // Set also correct font for description field
        TRAP_IGNORE( iDescription->SetFontL( editorLayoutText.Font() ) );

        iFieldRect = rect;
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRDescriptionField::MinimumSize()
    {
    TRect rect( Parent()->Rect() );
    TRect listRect =
        NMRLayoutManager::GetLayoutRect(
                rect,
                NMRLayoutManager::EMRLayoutListArea ).Rect();
    TRect fieldRect =
        NMRLayoutManager::GetFieldLayoutRect( listRect, 1 ).Rect();
    TRect rowRect =
        NMRLayoutManager::GetFieldRowLayoutRect( fieldRect, 1 ).Rect();
    TRect viewerRect =
        NMRLayoutManager::GetLayoutText(
                rowRect,
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor ).TextRect();

    fieldRect.Resize( 0, iSize.iHeight - viewerRect.Height() );

    return fieldRect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::SetContainerWindowL(
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iDescription->SetContainerWindowL( aContainer );

    iDescription->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::StoreGeoValueL
// ---------------------------------------------------------------------------
//
void CESMRDescriptionField::StoreGeoValueL(
        CCalEntry& aCalEntry,
        const TDesC& aLocationUrl )
    {
    TReal lat, lon;
    CCalGeoValue* geoVal = aCalEntry.GeoValueL();

    if ( !geoVal || ! geoVal->GetLatLong( lat, lon ) )
        {
        // GEO value not set. Convert URL
        geoVal = UrlParserL().CreateGeoValueLC( aLocationUrl );
        aCalEntry.SetGeoValueL( *geoVal );
        CleanupStack::Pop( geoVal );

        NotifyEventL( EESMRCmdEnableWaypointIcon );
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::UrlParserL
// ---------------------------------------------------------------------------
//
CESMRUrlParserPlugin& CESMRDescriptionField::UrlParserL()
    {
    if ( !iUrlParser )
        {
        iUrlParser = CESMRUrlParserPlugin::NewL();
        }

    return *iUrlParser;
    }

// EOF
