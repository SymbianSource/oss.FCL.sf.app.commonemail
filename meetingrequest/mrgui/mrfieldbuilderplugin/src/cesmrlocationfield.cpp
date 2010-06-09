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
 *  Description : ESMR location field implementation
 *  Version     : %version: e002sa32#53.1.2 %
 *
 */


#include "cesmrlocationfield.h"
#include "esmrfieldbuilderdef.h"
#include "cesmreditor.h"
#include "cesmrfeaturesettings.h"
#include "esmrhelper.h"
#include "mesmrfieldevent.h"
#include "cesmrgenericfieldevent.h"
#include "cesmrglobalnote.h"
#include "cmrbutton.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrstyluspopupmenu.h"
#include "mesmrlistobserver.h"

#include <calentry.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>
#include <AknsBasicBackgroundControlContext.h>
#include <eikmfne.h>
#include <aknbutton.h>

#include "emailtrace.h"
#include "esmrconfig.hrh"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRLocationField::NewL
// ---------------------------------------------------------------------------
//
CESMRLocationField* CESMRLocationField::NewL()
    {
    FUNC_LOG;
    CESMRLocationField* self = new (ELeave) CESMRLocationField;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::CESMRLocationField
// ---------------------------------------------------------------------------
//
CESMRLocationField::CESMRLocationField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldLocation );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::ConstructL()
    {
    FUNC_LOG;

    iFieldButton = CMRButton::NewL( NMRBitmapManager::EMRBitmapLocation );
    iFieldButton->SetParent( this );

    iTitle = CMRLabel::NewL();
    iTitle->SetParent( this );
    HBufC* title = StringLoader::LoadLC(
            R_MEET_REQ_OPTIONS_ADD_LOCATION,
            iCoeEnv );
    iTitle->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    iLocation = CESMREditor::NewL( this,
                                   1,
                                   KMaxTextLength, //from ICAL spec.
                                   // do not change. other text fields do not
                                   // have this limitation
                                   CEikEdwin::EResizable |
                                   CEikEdwin::EAvkonEditor );

    CESMRField::ConstructL( iLocation ); //ownership transfered

    iLocation->SetEdwinSizeObserver( this );
    iLocation->SetEdwinObserver( this );
    iLocation->SetParent( this );

    HBufC* buf = StringLoader::LoadLC( R_QTN_MEET_REQ_LOCATION_FIELD, iCoeEnv );
    iLocation->SetDefaultTextL( buf ); // ownership is transferred
    CleanupStack::Pop( buf );

    // Setting background instead of theme skin
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );

    TRect initialisationRect( 0, 0, 0, 0 );
    iBgControlContext = CAknsBasicBackgroundControlContext::NewL(
                bitmapStruct.iItemId,
                initialisationRect,
                EFalse );

    iLocation->SetSkinBackgroundControlContextL( iBgControlContext );
    }


// ---------------------------------------------------------------------------
// CESMRLocationField::~CESMRLocationField
// ---------------------------------------------------------------------------
//
CESMRLocationField::~CESMRLocationField()
    {
    FUNC_LOG;
    delete iFieldButton;
    delete iWaypointIcon;
    delete iFeatures;
    delete iLocationText;
    delete iBgControlContext;
    delete iMenu;
    delete iTitle;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRLocationField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );

    TRect richTextRect =
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect textRect( NMRLayoutManager::GetLayoutText(
            richTextRect,
       NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );

    // Adjust field size so that there's room for expandable editor.
    richTextRect.Resize( 0, iSize.iHeight - textRect.Height() );

    // Add title area to the required size
    TSize titleSize( CESMRField::MinimumSize() );

    TSize completeFieldSize( titleSize );
    completeFieldSize.iHeight += richTextRect.Height();

    return completeFieldSize;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::InitializeL()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry();
    HBufC* location = entry.LocationL().AllocLC();

    // if there is no text available, don't overwrite the default text
    // with empty descriptor.
    if ( location->Length() > 0 )
        {
        iLocation->ClearSelectionAndSetTextL( *location );
        }

    CleanupStack::PopAndDestroy( location );

    // If the geo value has been set, the waypoint icon has to be shown on right
    // side of the location field
    CCalGeoValue* geoValue = aEntry.Entry().GeoValueL();
    TReal dummy;
    SetWaypointIconL( geoValue && geoValue->GetLatLong( dummy, dummy ) );

    // this is needed to be re-called here, otherwise the CEikEdwin
    // does not get correctly instantiated with default text
    iLocation->FocusChanged( EDrawNow );
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* loc = iLocation->GetTextInHBufL( );

    if ( loc )
        {
        CleanupStack::PushL( loc );

        // externalize the text only if it differs from the
        // default text. In other words, default text is not
        // externalized.
        HBufC* defaultText = StringLoader::LoadLC(
                R_QTN_MEET_REQ_LOCATION_FIELD,
                iCoeEnv );
        if ( defaultText->Compare( *loc ) != 0 )
            {
            CCalEntry& entry = aEntry.Entry();
            entry.SetLocationL( *loc );
            }
        else if ( !iWaypointIcon )
            {
            aEntry.Entry().ClearGeoValueL();
            }

        CleanupStack::PopAndDestroy( defaultText );
        CleanupStack::PopAndDestroy( loc );
        }
    else
        {
        CCalEntry& entry = aEntry.Entry();
        entry.SetLocationL( KNullDesC );
        entry.ClearGeoValueL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    if ( FeaturesL().FeatureSupported(
            CESMRFeatureSettings::EESMRUILocationFeatures ) )
        {
        if ( aFocus )
            {
            ChangeMiddleSoftKeyL( EAknSoftkeyContextOptions, R_QTN_MSK_OPEN );
            }
        else
            {
            if ( iLocationText )
                {
                delete iLocationText;
                iLocationText = NULL;
                }
            iLocationText = iLocation->GetTextInHBufL();

            HBufC* defaultText = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_LOCATION_FIELD,
                    iCoeEnv );

            if ( iLocationText && iLocationText->Compare( *defaultText ) != 0 )
                {
                // Create field change event
                CESMRGenericFieldEvent* event = CESMRGenericFieldEvent::NewLC(
                        this,
                        MESMRFieldEvent::EESMRFieldChangeEvent );

                // Add this field as parameter
                TInt fieldId = iFieldId;
                CESMRFieldEventValue* field = CESMRFieldEventValue::NewLC(
                        MESMRFieldEventValue::EESMRInteger,
                        &fieldId );
                event->AddParamL( field );
                CleanupStack::Pop( field );

                // Add new value as parameter
                CESMRFieldEventValue* value = CESMRFieldEventValue::NewLC(
                        CESMRFieldEventValue::EESMRString,
                        iLocationText );
                iLocationText = NULL; // Ownership transferred to value
                // Encapsulate value, so receiver gets the field value type
                event->AddParamL( value, ETrue );
                CleanupStack::Pop( value );

                // Send event
                NotifyEventAsyncL( event );
                CleanupStack::Pop( event );
                }
            CleanupStack::PopAndDestroy( defaultText );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRLocationField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    TBool reDraw( EFalse );

    if( iSize != aSize )
        {
        // Let's save the required size for the iLocation
        iSize = aSize;

        if ( iObserver && aEdwin == iLocation )
           {
           iObserver->ControlSizeChanged( this );
           reDraw = ETrue;
           }

        if( iLocation->LineCount() != iLineCount )
            {
            // Line count has changed, the whole component needs
            // to be redrawn
            DrawDeferred();
            iLineCount = iLocation->LineCount();
            }
        }

    return reDraw;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::HandleEdwinEventL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType)
    {
    FUNC_LOG;
    if ( aEdwin == iLocation && aEventType == EEventTextUpdate )
        {
        TInt textLength( iLocation->TextLength() );

        if ( iLocation->GetLimitLength() <= textLength )
            {
            NotifyEventAsyncL( EESMRCmdSizeExceeded );
            }
        else if ( iWaypointIcon && textLength == 0 )
            {
            SetWaypointIconL( EFalse );
            NotifyEventL( EESMRCmdDisableWaypointIcon );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::GetCursorLineVerticalPos
// ---------------------------------------------------------------------------
//
void CESMRLocationField::GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower)
    {
    aLower = iLocation->CurrentLineNumber() * iLocation->RowHeight();
    aUpper = aLower - iLocation->RowHeight();
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRLocationField::ListObserverSet()
    {
    FUNC_LOG;
    iLocation->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ListObserverSet
// ---------------------------------------------------------------------------
//
CESMRFeatureSettings& CESMRLocationField::FeaturesL()
    {
    FUNC_LOG;
    if ( !iFeatures )
        {
        iFeatures = CESMRFeatureSettings::NewL();
        }

    return *iFeatures;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::SetWaypointIconL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::SetWaypointIconL( TBool aEnabled )
    {
    delete iWaypointIcon;
    iWaypointIcon = NULL;

    if ( aEnabled )
        {
        iWaypointIcon = CMRImage::NewL(
                NMRBitmapManager::EMRBitmapLocationWaypoint, ETrue );
        iWaypointIcon->SetParent( this );
        }

    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRLocationField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if( iFieldButton )
        {
        ++count;
        }

    if ( iTitle )
        {
        ++count;
        }

    if( iLocation )
        {
        ++count;
        }

    if( iWaypointIcon )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRLocationField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;

    switch ( aInd )
        {
        case 0:
            return iFieldButton;
        case 1:
            return iTitle;
        case 2:
            return iLocation;
        case 3:
            return iWaypointIcon;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRLocationField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );

    TAknLayoutRect firstRowLayoutRect(
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect firstRowRect( firstRowLayoutRect.Rect() );

    TRect secondRowRect( firstRowRect );
    secondRowRect.Move( 0, firstRowRect.Height() );

    // Layout field button
    if( iFieldButton )
        {
        TAknWindowComponentLayout buttonLayout(
                NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutTextEditorIcon ) );
        AknLayoutUtils::LayoutControl(
                iFieldButton, firstRowRect, buttonLayout );
        }

    // Layout field title
    if( iTitle )
        {
        TAknTextComponentLayout editorLayout =
                NMRLayoutManager::GetTextComponentLayout(
                        NMRLayoutManager::EMRTextLayoutTextEditor );

        AknLayoutUtils::LayoutLabel( iTitle, firstRowRect, editorLayout );
        }

    TRect editorRect( 0, 0, 0, 0 );

    // Layout field editor
    if( iLocation )
        {
        TAknLayoutText editorLayoutText;

        if( iWaypointIcon )
           {
           editorLayoutText = NMRLayoutManager::GetLayoutText(
                   secondRowRect,
                   NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
           }
        else
           {
           editorLayoutText = NMRLayoutManager::GetLayoutText(
                   secondRowRect,
                   NMRLayoutManager::EMRTextLayoutTextEditor );
           }

        editorRect = editorLayoutText.TextRect();

        // Resize height according to actual height required by edwin.
        editorRect.Resize( 0, iSize.iHeight - editorRect.Height() );

        iLocation->SetRect( editorRect );

        // Try setting font. Failures are ignored.
        TRAP_IGNORE( iLocation->SetFontL( editorLayoutText.Font() ) );
        }

    // Layout the waypoint icon
    if ( iWaypointIcon )
        {
        TAknWindowComponentLayout iconLayout(
                NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
        AknLayoutUtils::LayoutImage( iWaypointIcon, secondRowRect, iconLayout );
        }

    // Layout field focus
    if( iLocation )
        {
        // Layouting focus for rich text editor area
        TRect bgRect( iLocation->Rect() );

        // Move focus rect so that it's relative to field's position.
        bgRect.Move( -Position() );
        SetFocusRect( bgRect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::HandleFieldEventL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::HandleFieldEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    if ( aEvent.Type() == MESMRFieldEvent::EESMRFieldCommandEvent )
        {
        TInt* command = static_cast< TInt* >( aEvent.Param( 0 ) );

        switch ( *command )
            {
            case EESMRCmdDisableWaypointIcon:
            case EESMRCmdEnableWaypointIcon:
                {
                SetWaypointIconL( *command == EESMRCmdEnableWaypointIcon );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CESMRLocationField::ExecuteGenericCommandL(
        TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    switch(aCommand)
        {
        case EESMRCmdSizeExceeded:
            {
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRCannotDisplayMuchMore );

            HBufC* text = iLocation->GetTextInHBufL();
            CleanupDeletePushL( text );
            if ( text )
                {
                TInt curPos = iLocation->CursorPos();
                if ( curPos > iLocation->GetLimitLength() - 1 )
                    curPos = iLocation->GetLimitLength() - 1;
                HBufC* newText =
                    text->Mid( 0, iLocation->GetLimitLength() - 1 ).AllocLC();

                iLocation->SetTextL ( newText );
                CleanupStack::PopAndDestroy( newText );
                newText = NULL;

                iLocation->SetCursorPosL( curPos, EFalse );
                iLocation->HandleTextChangedL();
                iLocation->UpdateScrollBarsL();
                SetFocus( ETrue );
                }
            CleanupStack::PopAndDestroy( text );
            isUsed = ETrue;
            break;
            }
        default:
            break;
        }
    return isUsed;
    }


// ---------------------------------------------------------------------------
// CESMRLocationField::HandleSingletapEventL
// ---------------------------------------------------------------------------
//
TBool CESMRLocationField::HandleSingletapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;
    TBool ret( EFalse );

    if( iTitle->Rect().Contains( aPosition ) ||
            iFieldButton->Rect().Contains( aPosition ) )
        {
		HandleTactileFeedbackL();
        NotifyEventL( EAknSoftkeyContextOptions );
        ret = ETrue;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::HandleLongtapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;

    if ( iLocation->Rect().Contains( aPosition ) )
        {
        if ( !iMenu )
            {
            iMenu = CMRStylusPopupMenu::NewL( *this );
            }
        RArray<CMRStylusPopupMenu::TMenuItem> items;
        CleanupClosePushL( items );

        HBufC* maps= NULL;
        HBufC* myLocations = NULL;

        if ( FeaturesL().FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            maps = StringLoader::LoadLC(
                    R_MEET_REQ_OPTIONS_ASSIGN_FROM_MAP,
                    iCoeEnv );
            CMRStylusPopupMenu::TMenuItem mapsItem(
                    *maps,
                    EESMRCmdAssignFromMap );
            items.AppendL( mapsItem );
            }

//#ifdef RD_USE_MYLOCATIONUI

        myLocations = StringLoader::LoadLC(
                R_MEET_REQ_OPTIONS_MY_LOCATIONS,
                iCoeEnv );

        CMRStylusPopupMenu::TMenuItem myLocationsItem(
                *myLocations,
                EMRCommandMyLocations );

        items.AppendL( myLocationsItem );

//#endif //RD_USE_MYLOCATIONUI

        if ( items.Count() == 1 )
            {
            // Only one option. Issue command directly
            NotifyEventL( items[ 0 ].iCommandId );
            }
        else if ( items.Count() > 1 )
            {
            iMenu->LaunchPopupL( items.Array(), aPosition );
            iMenuIsShown = ETrue;
            }

        if (myLocations )
            {
            CleanupStack::PopAndDestroy( myLocations );
            }

        if ( maps )
            {
            CleanupStack::PopAndDestroy( maps );
            }

        CleanupStack::PopAndDestroy( &items );
        }

    if( iTitle->Rect().Contains( aPosition ) ||
            iFieldButton->Rect().Contains( aPosition ) )
        {
		HandleTactileFeedbackL();
        NotifyEventL( EAknSoftkeyContextOptions );
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    if ( aResourceId == R_MR_EDITOR_ORGANIZER_MENU)
        {
        // Init assign options
        TInt numOptions( 1 ); // My Locations

        if ( FeaturesL().FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            ++numOptions;
            }

        #ifndef RD_USE_MYLOCATIONUI
        // Disable My Locations
        --numOptions;
        #endif // RD_USE_MYLOCATIONUI

        switch ( numOptions )
            {
            case 0:
                {
                aMenuPane->SetItemDimmed( EESMRCmdAddLocation, ETrue );
                break;
                }

            case 1:
                {
                // Replace Add location with actual command
                TInt pos( 0 );
                CEikMenuPaneItem* item =
                        aMenuPane->ItemAndPos( EESMRCmdAddLocation, pos );

                item->iData.iCascadeId = 0;

                if ( FeaturesL().FeatureSupported(
                        CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                    {
                    item->iData.iCommandId = EESMRCmdAssignFromMap;
                    StringLoader::Load(item->iData.iText,
                            R_MEET_REQ_OPTIONS_ASSIGN_FROM_MAP, iCoeEnv );
                    }

#ifdef RD_USE_MYLOCATIONUI
                else
                    {
                    item->iData.iCommandId = EMRCommandMyLocations;
                    StringLoader::Load( item->iData.iText,
                            R_MEET_REQ_OPTIONS_MY_LOCATIONS, iCoeEnv );
                    }
#endif //RD_USE_MYLOCATIONUI

                aMenuPane->SetItemDimmed( item->iData.iCommandId, EFalse );
                break;
                }
            default:
                {
                aMenuPane->SetItemDimmed( EESMRCmdAddLocation, EFalse );
                break;
                }
            }
        // Init Show/Search options
        TBool showOnMap( EFalse );
        TBool searchFromMap( EFalse );

        if ( iWaypointIcon )
            {
            // Coordinates available -> show on map
            showOnMap = ETrue;
            }
        else
            {
            HBufC* text = iLocation->GetTextInHBufL();
            if ( text && *text != iLocation->DefaultText() )
                {
                // Edited text in field -> enable search from map
                searchFromMap = ETrue;
                }
            delete text;
            }

        aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
        aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, !searchFromMap );
        }
    else if ( aResourceId == R_MR_EDITOR_LOCATION_MENU )
        {
        if ( !FeaturesL().FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            aMenuPane->SetItemDimmed( EESMRCmdAssignFromMap, ETrue );
            }
#ifndef RD_USE_MYLOCATIONUI
        aMenuPane->SetItemDimmed( EMRCommandMyLocations, ETrue );
#endif //RD_USE_MYLOCATIONUI
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::SetContainerWindowL(
        const CCoeControl& aContainer )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );

    iLocation->SetContainerWindowL( aContainer );
    iLocation->SetParent( this );

    iTitle->SetContainerWindowL( aContainer );
    iTitle->SetParent( this );

    iFieldButton->SetContainerWindowL(aContainer);
    iFieldButton->SetParent(this);
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRLocationField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    response = iLocation->OfferKeyEventL ( aEvent, aType );

    if ( aType == EEventKey &&
         ( aEvent.iScanCode != EStdKeyUpArrow &&
           aEvent.iScanCode != EStdKeyDownArrow ))
        {
        iLocation->DrawDeferred();
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::HandlePointerEventL
// ---------------------------------------------------------------------------
//
TBool CESMRLocationField::HandleRawPointerEventL( const TPointerEvent &aPointerEvent )
    {
    if ( !iMenuIsShown )
        {
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::ProcessCommandL( TInt aCommandId )
    {
    NotifyEventL( aCommandId );

    iMenuIsShown = EFalse;
    }
// EOF
