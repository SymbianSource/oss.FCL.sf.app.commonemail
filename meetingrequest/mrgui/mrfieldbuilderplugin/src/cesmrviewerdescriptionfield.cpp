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
    iRichTextViewer->SetFontL ( layoutText.Font(), iLayout );
    if ( IsFocused() )
        {
        iRichTextViewer->FocusChanged( EDrawNow );
        }

    iRichTextViewer->HandleTextChangedL();
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
        iObserver->RemoveControl( FieldId() );
        }
    else if ( FeaturesL().FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
        {
        AddShowOnMapLinkL( aEntry );
        }
    else
        {
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
            if ( FeaturesL().FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                SetShowOnMapLinkMiddleSoftKeyL();
                }
            else
                {
                if ( iRichTextViewer->GetSelectedLink() )
                    {
                    SetMiddleSoftKeyVisible( ETrue );
                    }
                else
                    {
                    RestoreMiddleSoftKeyL();
                    }
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
    TRect rect( Rect() );
    rect = NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ).Rect();
    
    TRect viewerRect =
        NMRLayoutManager::GetLayoutText( 
                rect, 
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor ).TextRect();
    
    iRichTextViewer->SetRect( 
            TRect( viewerRect.iTl, 
                    TSize( viewerRect.Width(), iSize.iHeight ) ) );
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
// CESMRViewerDescriptionField::CESMRViewerDescriptionField()
// ---------------------------------------------------------------------------
//
CESMRViewerDescriptionField::CESMRViewerDescriptionField( )
    {
    FUNC_LOG;
    //do nothing
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
    iRichTextViewer->SetParent( this );
    
    SetFieldId( EESMRFieldDescription );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDescriptionField::GetMinimumVisibleVerticalArea()
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower)
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
void CESMRViewerDescriptionField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case EESMRCmdClipboardCopy:
            {
            iRichTextViewer->CopyCurrentLinkToClipBoardL();
            break;
            }
        case EESMRCmdShowOnMap:
            {
            if ( FeaturesL().FeatureSupported(
                 CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                const CESMRRichTextLink* link =
                    iRichTextViewer->GetSelectedLink();
                LocationPluginL().ShowOnMapL( link->Value() );
                }
            break;
            }
        case EAknSoftkeySelect:
            {
            iRichTextViewer->LinkSelectedL();
            break;
            }
        default:
            {
            break;
            }
        }
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
// CESMRViewerDescriptionField::AddShowOnMapLinkL
// ---------------------------------------------------------------------------
//
void CESMRViewerDescriptionField::AddShowOnMapLinkL( MESMRCalEntry& aEntry )
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
        command = EESMRCmdEnableWaypointIcon;
        }
    // No location url found. Other description text is added to field.
    else
        {
        TPtrC text = aEntry.Entry().DescriptionL();
        iRichTextViewer->SetTextL( &text, ETrue );
        }
    
    iRichTextViewer->SetMargins( KMargin );
    NotifyEventL( command );
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

//EOF

