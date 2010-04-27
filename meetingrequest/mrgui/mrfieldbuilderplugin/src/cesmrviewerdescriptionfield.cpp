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
* Description:  Meeting request description field
 *
*/

#include "cesmrviewerdescriptionfield.h"

#include "esmrfieldbuilderdef.h"
#include "cesmrrichtextviewer.h"
#include "cesmrrichtextlink.h"
#include "mesmrlistobserver.h"
#include "cesmrlocationplugin.h"
#include "esmrconfig.hrh"
#include "cesmrfeaturesettings.h"
#include "cesmrfieldcommandevent.h"
#include "nmrlayoutmanager.h"

//<cmail>
#include "cesmrurlparserplugin.h"
//</cmail>
#include <calentry.h>
#include <AknUtils.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <avkon.rsg>
#include <txtrich.h>

#include "emailtrace.h"

// Unnamed namespace for local definitions and functions
namespace { // codescanner::namespace

const TInt KMargin (5);

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerDescriptionField* CESMRViewerDescriptionField::NewL( )
    {
    FUNC_LOG;
    CESMRViewerDescriptionField* self = new (ELeave) CESMRViewerDescriptionField;
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop (self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::~CESMRViewerDescriptionField()
// ---------------------------------------------------------------------------
//
CESMRViewerDescriptionField::~CESMRViewerDescriptionField( )
    {
    FUNC_LOG;
    delete iLocationPlugin;
    delete iFeatures;
    delete iUrlParser;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::InitializeL()
    {
    FUNC_LOG;
    TAknLayoutText layoutText =
        NMRLayoutManager::GetLayoutText( Rect(), NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    iRichTextViewer->SetFontL( layoutText.Font() );
    iRichTextViewer->ApplyLayoutChangesL();
    
    if ( IsFocused() )
        {
        iRichTextViewer->FocusChanged( EDrawNow );
        }

    iRichTextViewer->HandleTextChangedL();
    iRichTextViewer->SetEventQueue( iEventQueue );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TPtrC text = aEntry.Entry().DescriptionL ( );
    if( text.Length() == 0 )
        {
        iObserver->HideControl( FieldId() );
        }
    else if ( FeaturesL().FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
        {
        AddShowOnMapLinkL( aEntry );
        }
    else
        {
        TPtrC text = aEntry.Entry().DescriptionL();
        iRichTextViewer->SetTextL( &text, ETrue );
        iRichTextViewer->SetMargins( KMargin );
        }
    iDisableRedraw = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRViewerDescriptionField::OfferKeyEventL(
        const TKeyEvent& aEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;
    response = iRichTextViewer->OfferKeyEventL ( aEvent, aType );

    //track up and down events to change MSK
    if ( aType == EEventKey )
        {
        if ( aEvent.iCode == EKeyUpArrow || aEvent.iCode == EKeyDownArrow )
            {
            if ( !iRichTextViewer->GetSelectedLink() )
                {
                // Restore default middle softkey if field does not contain link
                RestoreMiddleSoftKeyL();
                }
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::SetContainerWindowL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::SetContainerWindowL( const CCoeControl& aContainer )
    {
    CESMRField::SetContainerWindowL( aContainer );
    iRichTextViewer->SetContainerWindowL( aContainer );
    iRichTextViewer->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::MinimumSize()
// ---------------------------------------------------------------------------
//
TSize CESMRViewerDescriptionField::MinimumSize()
    {
    TRect rect( Parent()->Rect() );
    TRect listRect =
        NMRLayoutManager::GetLayoutRect( rect, NMRLayoutManager::EMRLayoutListArea ).Rect();
    TRect fieldRect =
        NMRLayoutManager::GetFieldLayoutRect( listRect, 1 ).Rect();
    TRect rowRect =
        NMRLayoutManager::GetFieldRowLayoutRect( fieldRect, 1 ).Rect();
    TRect viewerRect =
        NMRLayoutManager::GetLayoutText( rowRect, NMRLayoutManager::EMRTextLayoutMultiRowTextEditor ).TextRect();

    fieldRect.Resize( 0, iSize.iHeight - viewerRect.Height() );
    fieldRect.SetWidth( Parent()->Rect().Width() );
    return fieldRect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::SizeChanged()
    {
    // Store iRichTextViewer original width.
    TInt richTextViewerWidth = iRichTextViewer->Size().iWidth;
    TRect rect( Rect() );
    rect = NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ).Rect();

    TRect viewerRect =
        NMRLayoutManager::GetLayoutText(
                rect,
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor ).TextRect();

    TRect bgRect( viewerRect.iTl,
            TSize( viewerRect.Width(), iSize.iHeight ) );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );

    // Setting Font for the rich text viewer
    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
                Rect(),
                NMRLayoutManager::EMRTextLayoutTextEditor );

    // Failures are ignored. 
    TRAP_IGNORE( 
            // Try setting font 
            iRichTextViewer->SetFontL( text.Font() );
            // Try applying changes
            iRichTextViewer->ApplyLayoutChangesL();
            );

    if ( iRichTextViewer->Size().iWidth != richTextViewerWidth )
        {
        // Most of this case is screen orientation, in this case we need to 
        // Record the index of focusing link, after updating link array, then 
        // reset the focusing to original one.
        TInt focusingIndex = iRichTextViewer->GetFocusLink();
        if ( KErrNotFound != focusingIndex )
            {
            iRichTextViewer->SetFocusLink( focusingIndex );
            //wake up current contact menu selection by calling this
            iRichTextViewer->FocusChanged(ENoDrawNow);
            }
        } 
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerDescriptionField::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerDescriptionField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iRichTextViewer;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::HandleEdwinSizeEventL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDescriptionField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    iSize = aSize;

    if ( iObserver && iDisableRedraw )
        {
        iObserver->ControlSizeChanged ( this );
        }

    return iDisableRedraw;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::HandleRichTextLinkSelection
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDescriptionField::HandleRichTextLinkSelection(
        const CESMRRichTextLink* aLink )
    {
    TBool result = EFalse;
    if ( aLink &&
         aLink->Type() == CESMRRichTextLink::ETypeLocationUrl )
        {
        TRAPD( error, ShowLocationOnMapL( *aLink ) )
        if ( error )
            {
            iCoeEnv->HandleError( error );
            }
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::CESMRViewerDescriptionField()
// ---------------------------------------------------------------------------
//
CESMRViewerDescriptionField::CESMRViewerDescriptionField( )
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldDescription );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::ConstructL( )
    {
    FUNC_LOG;
    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    iRichTextViewer->SetEdwinSizeObserver( this );
    CESMRField::ConstructL( iRichTextViewer ); // ownership transferred
    iRichTextViewer->SetParent( this );
    iRichTextViewer->SetLinkObserver( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::GetCursorLineVerticalPos()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::GetCursorLineVerticalPos( TInt& aUpper, TInt& aLower )
    {
    FUNC_LOG;
    aLower = iRichTextViewer->CurrentLineNumber() * iRichTextViewer->RowHeight();
    aUpper = aLower - iRichTextViewer->RowHeight();
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::ListObserverSet()
    {
    FUNC_LOG;
    iRichTextViewer->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDescriptionField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    switch ( aCommand )
        {
        case EESMRCmdClipboardCopy:
            {
            iRichTextViewer->CopyCurrentLinkToClipBoardL();
            isUsed = ETrue;
            break;
            }
        case EAknSoftkeySelect:
            {
            iRichTextViewer->LinkSelectedL();
            isUsed = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    iRichTextViewer->SetFocus( aFocus );

    if ( FeaturesL().FeatureSupported(
            CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
        {
        if ( aFocus )
            {
            SetShowOnMapLinkMiddleSoftKeyL();
            }
        }

    if ( !aFocus )
        {
        //need to tell action menu that focus has changed
        iRichTextViewer->ResetActionMenuL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRDescriptionField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::HandleLongtapEventL(
        const TPoint& aPosition )
    {
    FUNC_LOG;
    
    if ( iRichTextViewer->Rect().Contains( aPosition ) )
        {
        if( iRichTextViewer->LinkSelectedL() )
        	{
			HandleTactileFeedbackL();        
        	}
        		
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::AddShowOnMapLinkL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::AddShowOnMapLinkL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    TPtrC urlPointer;
    TInt position;
    position = UrlParserL().FindLocationUrl( aEntry.Entry().DescriptionL(),
                                           urlPointer );

    // Location url is found and need to be replaced with show on map link
    if ( position >= KErrNone )
        {
        HBufC* showOnMapBuf =
            StringLoader::LoadLC( R_MEET_REQ_LINK_SHOW_ON_MAP,
                                  iCoeEnv );
        HBufC* description =
            HBufC::NewL( aEntry.Entry().DescriptionL().Length() + 1);
        CleanupStack::PushL( description );
        TPtr descriptionPointer( description->Des() );
        descriptionPointer.Append( aEntry.Entry().DescriptionL() );
        descriptionPointer.Replace( position,
                                    urlPointer.Length(),
                                    *showOnMapBuf );
        CESMRRichTextLink* showOnMapLink = CESMRRichTextLink::NewL( position,
                                             showOnMapBuf->Length(),
                                             urlPointer,
                                             CESMRRichTextLink::ETypeLocationUrl,
                                             CESMRRichTextLink::ETriggerKeyOk );
        CleanupStack::PushL( showOnMapLink );
        iRichTextViewer->SetTextL( description, ETrue );
        iRichTextViewer->InsertLinkL( showOnMapLink, 0 );
        CleanupStack::Pop( showOnMapLink );
        iRichTextViewer->HandleTextChangedL();
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( showOnMapBuf );

        StoreGeoValueL(
                aEntry.Entry(),
                urlPointer );
        }
    // No location url found. Other description text is added to field.
    else
        {
        TPtrC text = aEntry.Entry().DescriptionL();
        iRichTextViewer->SetTextL( &text, ETrue );
        }

    iRichTextViewer->SetMargins( KMargin );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::SetShowOnMapLinkMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::SetShowOnMapLinkMiddleSoftKeyL()
    {
    FUNC_LOG;
    const CESMRRichTextLink* link = iRichTextViewer->GetSelectedLink();
    if( link )
        {
        if ( link->Type() == CESMRRichTextLink::ETypeLocationUrl )
            {
            ChangeMiddleSoftKeyL(EESMRCmdShowOnMap, R_QTN_MSK_OPEN );
            }
        else
            {
            RestoreMiddleSoftKeyL();
            }

        SetMiddleSoftKeyVisible( ETrue );
        }
    else
        {
        RestoreMiddleSoftKeyL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::FeaturesL
// ---------------------------------------------------------------------------
//
CESMRFeatureSettings& CESMRViewerDescriptionField::FeaturesL()
    {
    FUNC_LOG;
    if ( !iFeatures )
        {
        iFeatures = CESMRFeatureSettings::NewL();
        }

    return *iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::LocationPluginL
// ---------------------------------------------------------------------------
//
CESMRLocationPlugin& CESMRViewerDescriptionField::LocationPluginL()
    {
    FUNC_LOG;
    if ( !iLocationPlugin )
        {
        iLocationPlugin = CESMRLocationPlugin::NewL();
        }

    return *iLocationPlugin;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::ShowLocationOnMapL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::ShowLocationOnMapL(
        const CESMRRichTextLink& aLink )
    {
    FUNC_LOG;

    if ( FeaturesL().FeatureSupported(
            CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
        {
        LocationPluginL().ShowOnMapL( aLink.Value() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::StoreGeoValueL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::StoreGeoValueL(
        CCalEntry& aCalEntry,
        const TDesC& aLocationUrl )
    {
    FUNC_LOG;

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
// CESMRViewerDescriptionField::UrlParserL
// ---------------------------------------------------------------------------
//
CESMRUrlParserPlugin& CESMRViewerDescriptionField::UrlParserL()
    {
    FUNC_LOG;

    if ( !iUrlParser )
        {
        iUrlParser = CESMRUrlParserPlugin::NewL();
        }

    return *iUrlParser;
    }

//EOF

