/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CFscContactActionMenuList.
*
*/
//<cmail> Layout changes


// INCUDES
#include "emailtrace.h"
#include <e32std.h>
#include <e32base.h>
#include <eikclbd.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <gulicon.h>
#include <coeaui.h>
#include <coefontprovider.h>
#include <gulfont.h>
#include <aknbiditextutils.h> 
#include <aknlayoutfont.h> 
#include <aknlayout2scalabledatadef.h>
#include <aknlayout2scalabledef.h>
#include <layoutmetadata.cdl.h>
#include <cdlengine.h>
#include <coemain.h>
#include <aknslistboxbackgroundcontrolcontext.h> 

#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>

#include "cfsccontactactionmenulist.h"
#include "mfsccontactactionmenulistobserver.h"
#include "mfsccontactactionmenumodel.h"
#include "cfsccontactactionmenuitem.h"
#include "cfsccontactactionmenulistbox.h"
#include "fsccontactactionmenuuidefines.h"

//_LIT( KListItemTextFormat, "%d\t%S" );
_LIT( KListItemTextFormat, "\t%d\t\t%S" );

const TInt KPeriodicDelay = 0; // immediately
const TInt KPeriodicInterval = 20*1000; // 20ms

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuList* CFscContactActionMenuList::NewL(
    MFscContactActionMenuListObserver* aObserver, 
    MFscContactActionMenuModel* aModel,
    TBool aAiMode, 
    MFsActionMenuPositionGiver* aPositionGiver,
    TBool aOpenedFromMR )
    {
    FUNC_LOG;

    CFscContactActionMenuList* self = new (ELeave) CFscContactActionMenuList( 
            aObserver, aModel, aAiMode, aPositionGiver, aOpenedFromMR );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::~CFscContactActionMenuList
// ---------------------------------------------------------------------------
//
CFscContactActionMenuList::~CFscContactActionMenuList()
    {
    FUNC_LOG;
    Hide( EFalse );
    delete iPeriodic;
    ClearControls();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CFscContactActionMenuList::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;

    TKeyResponse ret = ( EKeyWasConsumed );
    
    if ( iObserver->IsOperationCompleted() )
        {
        ret = EKeyWasNotConsumed;
        TInt closeKey = AknLayoutUtils::LayoutMirrored() ? 
            EStdKeyRightArrow : EStdKeyLeftArrow;
        if ( aType == EEventKey && aKeyEvent.iScanCode == closeKey )
            {
            iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuEventCloseKey );
            ret = EKeyWasConsumed;
            }
        else if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
            {
            ret = EKeyWasConsumed;
            // Handle application exit
            iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuEventExitKey );
            }
        else if ( aType == EEventKey && aKeyEvent.iCode == EKeyYes )
        	{
        	// Send key is not active in the action menu
        	ret = EKeyWasConsumed;
        	}
        else if ( !iAiMode ||
                  aKeyEvent.iScanCode == EStdKeyRightArrow || 
                  aKeyEvent.iScanCode == EStdKeyUpArrow || 
                  aKeyEvent.iScanCode == EStdKeyDownArrow ||  
                  aKeyEvent.iScanCode == EStdKeyEnter ||
                  aKeyEvent.iScanCode == EStdKeyDevice0 || //RSK 
                  aKeyEvent.iScanCode == EStdKeyDevice1 || //LSK
                  aKeyEvent.iScanCode == EStdKeyDevice3  ) //
            {
            ret = iListBox->OfferKeyEventL( aKeyEvent, aType );
            }
        else if ( aType == EEventKey )
            {
            // iAiMode == ETrue && some special key pressed
            // Inform observer and remove menu from stack
            iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuEventExitKey );
            }    
        }
    
    return ret;     
    }

// ---------------------------------------------------------------------------
// <cmail> Touch
// CFscContactActionMenuList::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    iListHasConsumedPointed = EFalse;
    iListBox->HandlePointerEventL(aPointerEvent);
    if (!iListHasConsumedPointed && aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
        Hide(ETrue);
        }
    }

// </cmail>
// ---------------------------------------------------------------------------
// CFscContactActionMenuList::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        //This code was commented out because of ELLO-7PDHT2
        //but perhaps error related to switching layout may be fixed, so
        //code is left here for future use
        if ( iOpenedFromMR )
            {
            iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuLayoutChanged );
            }
        else
            {
            LayoutPopupWindow( ETrue );
            }
        }
    else if ( aType == KAknsMessageSkinChange ||
              aType == KEikMessageColorSchemeChange )
        {
        UpdateColors();
        DrawDeferred();
        }
           
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::CountComponentControls() const
    {
    FUNC_LOG;
    TInt result = 1;
    return result;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CFscContactActionMenuList::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    CCoeControl* control = NULL;

    if ( aIndex == 0 && iListBox != NULL )
        {
        control = iListBox;
        } // No need for else block  

    return control;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuList::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::HandleListBoxEventL(
    CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    FUNC_LOG;
    if( aListBox == iListBox )
        {
// <cmail> Touch
    // Handle pointer events
       TBool executeAction = EFalse;
       if ( aEventType == EEventItemDraggingActioned )
           {
           iHasBeenDragged = ETrue;
           }
       else if ( aEventType == EEventItemClicked )
           {
           if (!iHasBeenDragged)
               {
               executeAction = ETrue;
               }
           else
               {
               iHasBeenDragged = EFalse;
               }
           }
       else if ( aEventType == EEventEnterKeyPressed)
           {
           executeAction = ETrue;
           iHasBeenDragged = EFalse;
           }
       iListHasConsumedPointed = ETrue;
       if ( executeAction )
           {
           iObserver->HandleListEvent( 
                           MFscContactActionMenuListObserver::EFscMenuEventItemClicked 
                           );
           }
        
        /*
        if ( aEventType == EEventItemClicked || 
            aEventType == EEventEnterKeyPressed )
            {
            iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuEventItemClicked 
                );
            }*/
// </cmail>
        }
    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::ShowL
// ---------------------------------------------------------------------------
//
// <cmail> Touch
void CFscContactActionMenuList::ShowL( 
    TFscContactActionMenuPosition aPosition,
    TInt aIndex )
    {
    FUNC_LOG;
      
    iPosition = aPosition;
// </cmail>    

    //UpdateListContentL();
    //UpdateWindowL();
    UpdateColors();
    
    DrawableWindow()->SetOrdinalPosition( KZero, KZero );
    SetupMenuOffset();
    iState = ESlidingIn;
    iMenuOffset.SetWithSpeed( KZero, KMenuSlideSpeed );
    if ( iMenuOffset.Interpolating() && !iPeriodic->IsActive() )
        {
        TCallBack callback( UpdateWindowPosition, this );
        iPeriodic->Start( KPeriodicDelay, KPeriodicInterval, callback );
        }
    
    iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityDialog );
    MakeVisible( ETrue );
    SetFocus( ETrue, EDrawNow ); 
    iListBox->SetCurrentItemIndex( aIndex );
        
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::Hide
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::Hide( TBool aSlide )
    {
    FUNC_LOG;
    
    if ( aSlide && ( iState == EIn || iState == ESlidingIn ) )
        {
        // Start sliding out when menu is fully or partialy shown
        iState = ESlidingOut;
        const TInt windowWidth( Rect().Width() );
        TInt newPos = 
            AknLayoutUtils::LayoutMirrored() ? 
                -windowWidth : windowWidth;
        iMenuOffset.SetWithSpeed( newPos, KMenuSlideSpeed );
        if ( iMenuOffset.Interpolating() && !iPeriodic->IsActive() )
            {
            TCallBack callback( UpdateWindowPosition, this );
            iPeriodic->Start( KPeriodicDelay, KPeriodicInterval, callback );
            }
        }
    else
        {
        // Hide menu now
        SetFocus( EFalse );
        MakeVisible( EFalse );
        iEikonEnv->EikAppUi()->RemoveFromStack( this );
        
        // If periodic running, stop it
        iPeriodic->Cancel();  
        
        if ( iState != EOut )
            {
            iState = EOut;
            // Inform observer         
            iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuEventMenuHidden
                );  
            }
        }
    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::SelectedItemIndex
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::SelectedItemIndex()
    {
    FUNC_LOG;
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::ItemCount
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::ItemCount()
    {
    FUNC_LOG;
    return iListBox->Model()->ItemTextArray()->MdcaCount();
    }   

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::CFscContactActionMenuList
// ---------------------------------------------------------------------------
//
// <cmail>
CFscContactActionMenuList::CFscContactActionMenuList(
    MFscContactActionMenuListObserver* aObserver, 
    MFscContactActionMenuModel* aModel,
    TBool aAiMode,
    MFsActionMenuPositionGiver* aPositionGiver,
    TBool aOpenedFromMR )
    : iState( EOut ), iObserver( aObserver ), iModel( aModel ),
      iAiMode( aAiMode ), iPositionGiver(aPositionGiver),
      iOpenedFromMR( aOpenedFromMR )
    {
// </cmail>
    FUNC_LOG;
    ASSERT( aObserver != NULL && aModel != NULL);
    
    iListItemId = KAknsIIDQgnFsGrafContactsContent;
    iListBottomId = KAknsIIDQsnBgColumnA;  
    iHighlightId = KAknsIIDQgnFsList;
    iHighlightCenterId = KAknsIIDQgnFsListCenter;   
    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::ConstructL()
    {
    FUNC_LOG;
    
    iPeriodic = CPeriodic::NewL( CActive::EPriorityUserInput );
    
    CreateWindowL();

    SetNonFocusing();
    SetBlank();    // This control need not draw
#ifndef FF_CMAIL_INTEGRATION
    EnableWindowTransparency(); //Needed to draw background correctly Not supported in 5.0
#endif
    
    // <cmail> Touch
    Window().SetPointerCapture(RWindowBase::TCaptureFlagEnabled | RWindowBase::TCaptureFlagAllGroups);
    // </cmail>
    
    //InitializeLayoutL();    
    ConstructControlsL();    
    
    // Finalize construct
    MakeVisible(ETrue);
    ActivateL();

    }   

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::LayoutList
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::LayoutList()
    {
    FUNC_LOG;
    TRAPD( error, LayoutListL() );
    return error;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::LayoutListL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::LayoutListL()
    {
    FUNC_LOG;

    const TRect menuPaneRect( Rect() );

    // Menu list pane
    TAknLayoutRect menuListPane;
    menuListPane.LayoutRect( menuPaneRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_pane( 
                    Max( 0, iModel->ItemCount() - 1 ) ) );
    const TRect menuPaneListRect( menuListPane.Rect() );

    // Menu gene pane
    TAknLayoutRect menuGenePane;
    menuGenePane.LayoutRect( menuPaneListRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane( 0 ) );
    const TRect menuGenePaneRect( menuGenePane.Rect() );
           
    // Menu gene pane g1
    TAknLayoutRect menuGenePaneG1;
    menuGenePaneG1.LayoutRect( menuGenePaneRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane_g1( 0 ) );
    const TRect menuGenePaneG1Rect( menuGenePaneG1.Rect() );

    // Menu gene pane t1
    TAknLayoutText menuGenePaneT1;
    menuGenePaneT1.LayoutText( menuGenePaneRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane_t1( 0 ) );
    const TRect menuGenePaneT1Rect( menuGenePaneT1.TextRect() );
    iFont = menuGenePaneT1.Font();

    iListBox->SetRect( menuPaneListRect );
   
    // Setup listbox columns
    CColumnListBoxData& columnData( *iListBox->ItemDrawer()->ColumnData() );
    columnData.SetColumnWidthPixelL( ELeftMarginColumn, 
            LeftMargin( menuGenePaneRect, menuGenePaneG1Rect ) );        
    columnData.SetColumnWidthPixelL( EIconColumn, menuGenePaneG1Rect.Width() );
    columnData.SetColumnWidthPixelL( EMidleMarginColumn, 
            MiddleMargin( menuGenePaneG1Rect, menuGenePaneT1Rect ) );
    columnData.SetColumnWidthPixelL( ETextColumn, menuGenePaneT1Rect.Width() );    
    columnData.SetGraphicsColumnL( EIconColumn, ETrue );
    columnData.SetColumnFontL( ETextColumn, iFont );         
    columnData.SetColumnBaselinePosL( ETextColumn,
            iFont->HeightInPixels() + 
            (menuGenePaneRect.Height() - iFont->HeightInPixels() - iFont->FontMaxDescent()) / 2 );
        
    AknLayoutUtils::LayoutMirrored() ? 
        columnData.SetColumnAlignmentL( ETextColumn, CGraphicsContext::ERight ) : 
        columnData.SetColumnAlignmentL( ETextColumn, CGraphicsContext::ELeft );

    // Setup item height
    columnData.SetItemCellSize( menuGenePaneRect.Size() );    
    iListBox->SetItemHeightL( menuGenePaneRect.Height() );    
    iListBox->SetBackgroundRect( menuPaneRect );  

    UpdateListContentL( menuGenePaneG1Rect.Size() );
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::LeftMargin
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::LeftMargin( const TRect& aMenuPane, 
        const TRect& aG1 ) const
    {
    FUNC_LOG;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        return Abs( aMenuPane.iBr.iX - aG1.iBr.iX );
        }
    return Abs( aG1.iTl.iX - aMenuPane.iTl.iX );
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::MiddleMargin
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::MiddleMargin( const TRect& aG1, 
        const TRect& aT1 ) const
    {
    FUNC_LOG;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        return Abs( aG1.iTl.iX - aT1.iBr.iX );
        }
    return Abs( aG1.iBr.iX - aT1.iTl.iX );
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::RightMargin
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::RightMargin( const TRect& aMenuPane, 
        const TRect& aT1 ) const
    {
    FUNC_LOG;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        return Abs( aT1.iTl.iX - aMenuPane.iTl.iX );
        }
    return Abs( aMenuPane.iBr.iX - aT1.iBr.iX );    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::ConstructControlsL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::ConstructControlsL()
    {
    FUNC_LOG;
        
    // Create list control
    iListBox = new ( ELeave ) CFscContactActionMenuListBox();
    iListBox->ConstructL( this );
    iListBox->HandleItemAdditionL();
    iListBox->SetFocus( ETrue );
    iListBox->SetContainerWindowL( *this );
    iListBox->SetListBoxObserver( this );           
    // Set icon array
    CArrayPtrFlat<CGulIcon>* iconArray = 
        new (ELeave) CArrayPtrFlat<CGulIcon>( KGranularity );
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
    LayoutPopupWindow();
    }   

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::ClearControls
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::ClearControls()
    {
    FUNC_LOG;
    
    if ( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }

    }  

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::UpdateListL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::UpdateListContentL( const TSize& aIconSize )
    {
    FUNC_LOG;

    CDesCArray* desArray =
        static_cast<CDesCArray*>( iListBox->Model()->ItemTextArray() );
    desArray->Reset();

    CArrayPtr<CGulIcon>* iconArray = 
        iListBox->ItemDrawer()->ColumnData()->IconArray();
    iconArray->ResetAndDestroy(); // Remove old icons  
    
    TInt itemCount = iModel->VisibleItemCount();
    for ( TInt i = 0; i < itemCount; i++ )
        {
        CFscContactActionMenuItem& item = iModel->VisibleItemL( i );
        
        const CGulIcon* origIcon = item.Icon();
        ASSERT( origIcon != NULL );
        
        CGulIcon* icon = CGulIcon::NewL( 
            origIcon->Bitmap(), origIcon->Mask() );
        CleanupStack::PushL( icon );
        icon->SetBitmapsOwnedExternally( ETrue );
            
        AknIconUtils::SetSize( icon->Bitmap(), aIconSize );
        AknIconUtils::SetSize( icon->Mask(), aIconSize );
        iconArray->AppendL(icon);
        CleanupStack::Pop( icon );  
        
        const TDesC& text = item.MenuText();
        HBufC* tempDes = HBufC::NewLC( 
                text.Length() + KListItemTextFormat().Length() );
        tempDes->Des().Format( KListItemTextFormat, i, &text );
        desArray->AppendL( *tempDes );
        CleanupStack::PopAndDestroy( tempDes );         
        }
                
    iListBox->HandleItemAdditionL();
    iListBox->UpdateScrollBarsL();

    }
   
// ---------------------------------------------------------------------------
// CFscContactActionMenuList::SetupMenuOffset
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::SetupMenuOffset()
    {
    FUNC_LOG;

    switch( iState )
        {
        case EOut:
        case ESlidingOut:
            {
            const TInt windowWidth( Rect().Width() );
            // Menu is out of the screen
            TInt newPos = 
            AknLayoutUtils::LayoutMirrored() ? 
                -windowWidth : windowWidth;
            iMenuOffset.Set( newPos );    
            }
            break;
        case EIn:
        case ESlidingIn:
            {
            // Menu is visible
            iMenuOffset.Set( 0 );
            }
            break;    
        default:
            {
            //Nothing to do
            break;
            }  
        }
    
    }
    
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuList::UpdateColors
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuList::UpdateColors()
    {
    FUNC_LOG;
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance(); 
    TRgb color;    
        
    // Highlight text color    
    TInt error = AknsUtils::GetCachedColor( 
        skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 );    
    if ( !error )
        {
        iListBox->ItemDrawer()->SetHighlightedTextColor( color );
        }
    else
        {
        error = AknsUtils::GetCachedColor( 
            skin, color, KAknsIIDFsHighlightColors, EAknsCIFsHighlightColorsCG1 );
        if ( !error )
            {
            iListBox->ItemDrawer()->SetHighlightedTextColor( color );
            }
        }
        
    // Text color
    error = AknsUtils::GetCachedColor( 
        skin, color, KAknsIIDFsTextColors, EAknsCIQsnTextColorsCG19 ); 
    if ( !error )
        { 
        iListBox->ItemDrawer()->SetTextColor( color );
        }
    else
        {
        // if colors are not found from the skins, set the default values
        error = AknsUtils::GetCachedColor( 
            skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
        if ( !error )
            {
            iListBox->ItemDrawer()->SetTextColor( color );
            }
        }
               
    }    
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuList::MaxListTextWidth
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuList::MaxListTextWidth()
    {
    FUNC_LOG;
    TPtrC itemText;
    TInt maxLength( 0 );
    for ( TInt i = 0; i < iModel->VisibleItemCount(); i++ )
        {
        TRAPD( error, itemText.Set( iModel->VisibleItemL( i ).MenuText() ) );
        if ( !error )
            {
            TInt offset( itemText.LocateReverse( TChar('\t') ) );        
            if ( offset != KErrNotFound && ++offset < itemText.Length() )
                {
                itemText.Set( itemText.Mid( offset ) );            
                }        
            const TInt length( AknBidiTextUtils::MeasureTextBoundsWidth(
                *iFont, itemText, CFont::TMeasureTextInput::EFVisualOrder ) );
            maxLength = length > maxLength ? length : maxLength;
            }
        }
    return maxLength;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::UpdateWindowPosition
// ---------------------------------------------------------------------------
//   
TInt CFscContactActionMenuList::UpdateWindowPosition( TAny *aPtr )
    {
    FUNC_LOG;
    CFscContactActionMenuList* list = 
        static_cast<CFscContactActionMenuList*>( aPtr );
    
    TRect newRect( list->iOrigMenuRect ); 
    newRect.Move( list->iMenuOffset.Now(), 0 ); 
    list->SetPosition( newRect.iTl );

    if ( !list->iMenuOffset.Interpolating() )
        {
        // Stop timer
        list->iPeriodic->Cancel();
        if ( list->iState == ESlidingOut )
            { 
            // Hide window totally
            list->iState = EOut;
            list->SetFocus( EFalse );
            list->MakeVisible( EFalse );
            static_cast< CEikonEnv* >
                ( list->iCoeEnv )->EikAppUi()->RemoveFromStack( list );
                
            // Inform observer        
            list->iObserver->HandleListEvent( 
                MFscContactActionMenuListObserver::EFscMenuEventMenuHidden 
                );        
            }
        else if ( list->iState == ESlidingIn )
            {
            list->iState = EIn;
            }
        }  
              
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::LayoutPopupWindow
// ---------------------------------------------------------------------------
//   
void CFscContactActionMenuList::LayoutPopupWindow( const TBool aLayoutChange )
    {
    FUNC_LOG;
    // Application window
    TRect applicationRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow,
        applicationRect );

    // Main 2 pane. This is the main pane that is used as parent layout
    // for the popup menup pane in the layout data, but using this would
    // make popup to overlap with the cba area.
    TAknLayoutRect main2Pane;
    main2Pane.LayoutRect( applicationRect, AknLayoutScalable_Apps::main2_pane(
        Layout_Meta_Data::IsLandscapeOrientation() ? 2 : 3 ) );
    TRect main2PaneRect = main2Pane.Rect();

    // Main pane
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );

    // Menu pane
    TAknLayoutRect menuPane;
    menuPane.LayoutRect( mainPaneRect,
        AknLayoutScalable_Apps::popup_sp_fs_action_menu_pane(
            iModel->ItemCount() + 1 ) );

    iOrigMenuRect = menuPane.Rect();
    AdjustByContent( iOrigMenuRect );
    if ( iPositionGiver )
        {
        TPoint customPos = iPositionGiver->ActionMenuPosition();
        iOrigMenuRect.Move( 0, -iOrigMenuRect.iTl.iY + mainPaneRect.iTl.iY
            + customPos.iY );
        }

    // Adjust menu pane position to keep it fully visible. The main2pane is
    // used here to enable menu pane to overlap with status pane area.
    if ( iOrigMenuRect.iTl.iY < main2PaneRect.iTl.iY )
        {
        iOrigMenuRect.Move( 0, main2PaneRect.iTl.iY - iOrigMenuRect.iTl.iY );
        }
    else if ( iOrigMenuRect.iBr.iY > main2PaneRect.iBr.iY )
        {
        iOrigMenuRect.Move( 0, main2PaneRect.iBr.iY - iOrigMenuRect.iBr.iY );
        }

    SetRect( iOrigMenuRect );

    if ( !aLayoutChange )
        {
        const TInt width( iOrigMenuRect.Width() );
        TPoint tl( iOrigMenuRect.iTl );
        tl.iX += AknLayoutUtils::LayoutMirrored() ? -width : width;
        SetPosition( tl );
        }
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::AdjustByContent
// ---------------------------------------------------------------------------
//   
void CFscContactActionMenuList::AdjustByContent( TRect& aRect )
    {
    FUNC_LOG;
    // Menu list pane
    TAknLayoutRect menuListPane;
    menuListPane.LayoutRect( aRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_pane( 
                    Max( 0, iModel->ItemCount() - 1 ) ) );
    const TRect menuPaneListRect( menuListPane.Rect() );

    // Menu gene pane
    TAknLayoutRect menuGenePane;
    menuGenePane.LayoutRect( menuPaneListRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane( 0 ) );
    const TRect menuGenePaneRect( menuGenePane.Rect() );
           
    // Menu gene pane g1
    TAknLayoutRect menuGenePaneG1;
    menuGenePaneG1.LayoutRect( menuGenePaneRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane_g1( 0 ) );
    const TRect menuGenePaneG1Rect( menuGenePaneG1.Rect() );
    
    // Menu gene pane t1
    TAknLayoutText menuGenePaneT1;
    menuGenePaneT1.LayoutText( menuGenePaneRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane_t1( 0 ) );
    const TRect menuGenePaneT1Rect( menuGenePaneT1.TextRect() );
    iFont = menuGenePaneT1.Font();

    const TInt textColumnWidth( menuGenePaneT1Rect.Width() );    
    const TInt maxTextWidth( MaxListTextWidth() + 
            RightMargin( menuGenePaneRect, menuGenePaneT1Rect ) );

    if ( maxTextWidth < textColumnWidth )
        {
        const TInt widthAdjustment( textColumnWidth - maxTextWidth );
        if ( !AknLayoutUtils::LayoutMirrored() )
            {
            aRect.Move( widthAdjustment, 0 );
            }
        aRect.Resize( -widthAdjustment, 0 );
        }
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuList::SizeChanged
// ---------------------------------------------------------------------------
//   
void CFscContactActionMenuList::SizeChanged()
    {
    FUNC_LOG;
    LayoutList();
    }

//</cmail>

