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
 *  Description : ESMR location field implementation
 *  Version     : %version: tr1sido#10 %
 *
 */

#include "emailtrace.h"
#include "cesmrlocationfield.h"

// SYSTEM INCLUDES
#include <calentry.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <esmrgui.rsg>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <data_caging_path_literals.hrh>

// INCLUDES
#include "esmrfieldbuilderdef.h"
#include "cesmreditor.h"
#include "cesmrfeaturesettings.h"
#include "esmrhelper.h"
#include "cesmrborderlayer.h"
#include "mesmrfieldevent.h"
#include "cesmrgenericfieldevent.h"
#include "cesmrlayoutmgr.h"
#include "cesmrglobalnote.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRLocationField::NewL
// ---------------------------------------------------------------------------
//
CESMRLocationField* CESMRLocationField::NewL( )
    {
    FUNC_LOG;
    CESMRLocationField* self = new (ELeave) CESMRLocationField;
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::~CESMRLocationField
// ---------------------------------------------------------------------------
//
CESMRLocationField::~CESMRLocationField( )
    {
    FUNC_LOG;
    delete iIcon;
    delete iWaypointIcon;
    delete iWaypointIconMask;
    delete iFrameBgContext;
    delete iFeatures;
    delete iLocationText;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::InitializeL()
    {
    FUNC_LOG;
    iLocation->SetFontL( iLayout->Font(iCoeEnv, iFieldId ), iLayout );
    AknLayoutUtils::OverrideControlColorL( *iLocation, 
                                            EColorControlBackground,
                                            iLayout->NormalTextBgColor() );
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
        HBufC* defaultText = StringLoader::LoadLC( R_QTN_MEET_REQ_LOCATION_FIELD );
        if ( defaultText->Compare( *loc ) != 0 )
            {
            CCalEntry& entry = aEntry.Entry();
            entry.SetLocationL( *loc );
            }

        CleanupStack::PopAndDestroy( defaultText );
        CleanupStack::PopAndDestroy( loc );
        }
    else
        {
        CCalEntry& entry = aEntry.Entry();
        entry.SetLocationL( KNullDesC );
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
            
            HBufC* defaultText = StringLoader::LoadLC( R_QTN_MEET_REQ_LOCATION_FIELD );
                        
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
// CESMRLocationField::ExpandedHeight
// ---------------------------------------------------------------------------
//
TInt CESMRLocationField::ExpandedHeight( ) const
    {
    FUNC_LOG;
    return iSize.iHeight;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRLocationField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    TBool ret( EFalse );
    
    TSize newSize = CalculateEditorSize( aSize );

    if ( aEdwin == iLocation && iSize != newSize )
            {
        iSize = newSize;

            if ( iObserver && aEdwin->IsFocused() )
            {
            iObserver->ControlSizeChanged (this );
            }

        if ( iFrameBgContext )
            {
            TRect visibleRect = CalculateVisibleRect( iLocation->Rect() );
            iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
            }
        
        ret = ETrue;
        }
    return ret;
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
        HBufC* text = iLocation->GetTextInHBufL();
        if ( text )
            {
            CleanupStack::PushL( text );
            TInt textLength( text->Length() );
            if ( iLocation->GetLimitLength() <= textLength )
            	{
            	NotifyEventAsyncL( EESMRCmdSizeExceeded );
            	}            
            CleanupStack::PopAndDestroy( text );
            }
        }	
	}
// ---------------------------------------------------------------------------
// CESMRLocationField::CESMRLocationField
// ---------------------------------------------------------------------------
//
CESMRLocationField::CESMRLocationField()
    : iSize( TSize( 0, 0 ) )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect( iLocation->Rect() );
    TRect inner(rect);
    TRect outer(rect);

    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL( 
            KAknsIIDQsnFrInput, 
            outer, 
            inner, 
            EFalse ) ;

    iFrameBgContext->SetParentContext( iBackground );
    iLocation->SetSkinBackgroundControlContextL( iFrameBgContext );
    }


// ---------------------------------------------------------------------------
// CESMRLocationField::PositionChanged()
// ---------------------------------------------------------------------------
//
void CESMRLocationField::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    
    if ( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iLocation->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::ConstructL()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldLocation );
    SetExpandable();

    iLocation = CESMREditor::NewL( this, 
                                   1,
                                   KMaxTextLength, //from ICAL spec.
                                   // do not change. other text fields do not
                                   // have this limitation
                                   CEikEdwin::EResizable |
                                   CEikEdwin::EAvkonEditor );

    iLocation->SetEdwinSizeObserver( this );
    iLocation->SetEdwinObserver( this );

    HBufC* buf = StringLoader::LoadLC( R_QTN_MEET_REQ_LOCATION_FIELD );
    iLocation->SetDefaultTextL( buf ); // ownership is transferred
    CleanupStack::Pop( buf );

    CESMRIconField::ConstructL( KAknsIIDQgnMeetReqIndiLocation, iLocation );
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::GetMinimumVisibleVerticalArea
// ---------------------------------------------------------------------------
//
void CESMRLocationField::GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
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
// CESMRCheckbox::SetWaypointIconL
// ---------------------------------------------------------------------------
//
void CESMRLocationField::SetWaypointIconL( TBool aEnabled )
    {
    FUNC_LOG;
    delete iIcon;
    iIcon = NULL;
    delete iWaypointIcon;
    iWaypointIcon = NULL;
    delete iWaypointIconMask;
    iWaypointIconMask = NULL;

    if( aEnabled )
        {
        TAknsItemID iconID;
        TInt fileIndex(-1);
        TInt fileMaskIndex(-1);
        iconID = KAknsIIDQgnPropBlidWaypoint;
        CESMRLayoutManager::SetBitmapFallback( iconID,
                                               fileIndex,
                                               fileMaskIndex );

        TFileName bitmapFilePath;
        ESMRHelper::LocateResourceFile(
                KESMRMifFile,
                KDC_APP_BITMAP_DIR,
                bitmapFilePath);

        AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                iconID,
                                iWaypointIcon,
                                iWaypointIconMask,
                                bitmapFilePath,
                                fileIndex,
                                fileMaskIndex
                                );

        // Even if creating mask failed, bitmap can be used (background is just not displayed correctly)
        if( iWaypointIcon )
            {
            AknIconUtils::SetSize( iWaypointIcon, KIconSize, EAspectRatioPreserved );
            iIcon = new (ELeave) CEikImage;
            iIcon->SetPictureOwnedExternally(ETrue);
            iIcon->SetPicture( iWaypointIcon, iWaypointIconMask );

            LayoutWaypointIcon();
            
            // change text rect size to fit icon
            iBorder->SetSize( BorderSize() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRLocationField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = CESMRIconField::CountComponentControls ( );
    if ( iIcon )
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
    if ( aInd == 2 )
        {
        return iIcon;
        }
    return CESMRIconField::ComponentControl ( aInd );
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRLocationField::SizeChanged( )
    {
    FUNC_LOG;
    CESMRIconField::SizeChanged();
    
    PositionFieldElements(); 
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::PositionFieldElements
// ---------------------------------------------------------------------------
//
void CESMRLocationField::PositionFieldElements()
    {
    FUNC_LOG;
    // editor (editor is wrapped inside the 'iBorder' member)
    if ( iIcon )
        {
        LayoutWaypointIcon();
        
        if ( iFrameBgContext && iBorder )
            {
            TRect visibleRect = CalculateVisibleRect( iLocation->Rect() );
            iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
            }
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
                {
                SetWaypointIconL( EFalse );
                break;
                }
            case EESMRCmdEnableWaypointIcon:
                {
                SetWaypointIconL( ETrue );
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
void CESMRLocationField::ExecuteGenericCommandL( 
		TInt aCommand )
	{
    FUNC_LOG;
	if ( EESMRCmdSizeExceeded == aCommand )
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
		}
	}

// ---------------------------------------------------------------------------
// CESMRLocationField::CalculateEditorSize
// ---------------------------------------------------------------------------
//
TSize CESMRLocationField::CalculateEditorSize( const TSize& aSize )
    {
    FUNC_LOG;
    TSize size( aSize );
    size.iHeight -= KEditorDifference;

    if ( iLayout->CurrentFontZoom() == EAknUiZoomSmall ||
        iLayout->CurrentFontZoom() == EAknUiZoomVerySmall)
        {
        size.iHeight -= KEditorDifference;
        }
    
    return size;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::BorderSize
// ---------------------------------------------------------------------------
//
TSize CESMRLocationField::BorderSize() const
    {
    FUNC_LOG;
    TSize borderSize( CESMRIconField::BorderSize() );
    
    // If icon exists, we have to adjust the border width
    if ( iIcon )
        {
        borderSize.iWidth -= ( KIconSize.iWidth + 2 * KEditorDifference );
        }
    
    return borderSize;
    }

// ---------------------------------------------------------------------------
// CESMRLocationField::LayoutWaypointIcon
// ---------------------------------------------------------------------------
//
void CESMRLocationField::LayoutWaypointIcon()
    {
    FUNC_LOG;
    if ( iIcon )
        {
        // Layout the icon
        TRect fieldRect( Rect() );
        TPoint iconPos( fieldRect.iBr.iX - KIconSize.iWidth,
                        fieldRect.iTl.iY + iLayout->IconTopMargin() );
        
        if ( CESMRLayoutManager::IsMirrored() )
            {
            iconPos.iX = fieldRect.iTl.iX;
            }
        
        iIcon->SetPosition( iconPos );
        iIcon->SetSize( KIconSize );
        }
    }

// EOF
