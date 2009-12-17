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
* Description:  Implementation of an action menu component.
*
*/


#include "emailtrace.h"
#include "fsactionmenu.h"
#include "fstreevisualizerbase.h"
#include "fslayoutmanager.h"
#include "fsactionmenucontrol.h"

//SYSTEM INCLUDES
#include <AknUtils.h>
#include <eikbtgpc.h>
#include <eikenv.h>
#include <eikspane.h> 
//TOOLKIT INCLUDES
// <cmail>
#include <alf/alfenv.h>
#include <alf/alfdecklayout.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <alf/alfborderbrush.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfbrusharray.h>
// </cmail>
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsActionMenu* CFsActionMenu::NewL( CAlfDisplay& aDisplay, TInt aCbaResource )
    {
    FUNC_LOG;
    CFsActionMenu* self = new( ELeave ) CFsActionMenu( aDisplay );
    CleanupStack::PushL( self );
    self->ConstructL( aCbaResource );
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsActionMenu::~CFsActionMenu() 
    {
    FUNC_LOG;
    delete iWait;
    delete iButtonGroupContainer;
    }


// ---------------------------------------------------------------------------
// Exetutes action menu at predefined vertical position.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsActionMenuResult CFsActionMenu::ExecuteL( TFsVActionMenuPosition aPos )
    {
    FUNC_LOG;

    iResult = EFsMenuDismissed;
        
    TInt count = Count();
    if ( count > 0  )
        {
        iVPos = aPos;
        iIsCustVPos = EFalse;
        
        FadeBehindActionMenuL( ETrue ); // <cmail>
        DoExecuteL();
        FadeBehindActionMenuL( EFalse ); // <cmail>
        HideCbaButtonsL();
        }
               
    return iResult;
    }


// ---------------------------------------------------------------------------
// Exetutes action menu at a custom vertical position.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsActionMenuResult CFsActionMenu::ExecuteL(  const TInt aYPos  )
    {
    FUNC_LOG;
    iResult = EFsMenuDismissed;
    
    TInt count = Count();
    if ( count > 0  )
        {
        iCustVPos = aYPos;
        iIsCustVPos = ETrue;
        
        FadeBehindActionMenuL( ETrue ); // <cmail>
        DoExecuteL();
        FadeBehindActionMenuL( EFalse ); // <cmail>
        HideCbaButtonsL();
        }
    
    return iResult;
    }


// ---------------------------------------------------------------------------
// The function returns size of Action Menu with the curent number of item.
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CFsActionMenu::ActionMenuSize ( )
    {
    FUNC_LOG;
    // <cmail>
    TRAP_IGNORE( SetAMSizeFromLayoutMgrL( ) );
    // </cmail>
    return iSize;
    }


// ---------------------------------------------------------------------------
// Sets visiblity of the border
// ---------------------------------------------------------------------------
//     
EXPORT_C void CFsActionMenu::SetBorderL( const TBool aVisible, CAlfTexture* aBorderTexture )
    {
    FUNC_LOG;
    CFsTreeList::SetBorderL( aVisible, aBorderTexture );
        
    const TInt KZero(0);
    
    if ( aVisible )
        {
        iAmXPadd = KZero;
        iAmYPadd = KZero;
        }
    else
        {
        iAmXPadd = KListBorderPadding;
        iAmYPadd = KListBorderPadding;
        }
    }


// ---------------------------------------------------------------------------
// The function returns a button group container.
// ---------------------------------------------------------------------------
//
EXPORT_C CEikButtonGroupContainer* CFsActionMenu::ButtonGroupContainer()
    {
    FUNC_LOG;
    return iButtonGroupContainer;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFsActionMenu::RootLayoutUpdatedL()
    {
    FUNC_LOG;
    TInt count = Count();
    if ( count > 0  )
        {
        iRootLayout->ClearFlag(EAlfVisualFlagLayoutUpdateNotification);
        UpdateActionMenuWindowL( );
        iRootLayout->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
        }    
    }


// ---------------------------------------------------------------------------
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::ProcessCommandL(TInt aCommandId)
    {
    FUNC_LOG;
    switch ( aCommandId )
        {
        case EAknSoftkeyCancel:
        case EAknSoftkeyExit:
        case EAknSoftkeyClose:
        case EAknSoftkeyNo:
        case EAknSoftkeyBack:
            {
            iResult = EFsMenuDismissed;
            DoDismissL();
            break;
            }
        case EAknSoftkeySelect:        
        case EAknSoftkeyYes:
        case EAknSoftkeyOk:
        case EAknSoftkeyDone:
            {
            iResult = EFsMenuItemSelected;
            DoDismissL();
            break;
            }
        default:
            {
            // Nothing to do
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// Shows action menu's cba.
// ---------------------------------------------------------------------------
//
// <cmail> Touch
void CFsActionMenu::TreeListEventL( const TFsTreeListEvent aEvent, const TFsTreeItemId /*aId*/ )
    {
    FUNC_LOG;
// </cmail>
    switch ( aEvent )
        {
        case MFsTreeListObserver::EFsTreeItemSelected:
            {
            iResult = EFsMenuItemSelected;
            DoDismissL(); // <cmail>
            break;
            }
        case MFsTreeListObserver::EFsTreeItemWithMenuSelected:
            {
            iResult = EFsMenuItemSelected;
            DoDismissL(); // <cmail>
            break;
            }
        case MFsTreeListObserver::EFsTreeListKeyRightArrow:
            {//right arrow click acts like "Cancel" in AH (mirrored) layouts
            if ( CFsLayoutManager::IsMirrored() )
                {
                iResult = EFsMenuDismissed;
                DoDismissL(); // <cmail>
                }
            break;
            }
        case MFsTreeListObserver::EFsTreeListKeyLeftArrow:
            {//left arrow click acts like "Cancel" in western layouts
            if ( !CFsLayoutManager::IsMirrored() )
                {
            	iResult = EFsMenuDismissed;
				DoDismissL(); // <cmail>
                }
            break;
            }
        case MFsTreeListObserver::EFsTreeListHidden:
            {//list fully hidden -> remove from the roster;
            iDisplay->Roster().Hide( *iCtrlGroup );
            break;
            }
        case MFsTreeListObserver::EFsTreeListShown:
            {//list fully shown
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// Shows action menu's cba.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::ShowCbaButtonsL()
    {
    FUNC_LOG;
    iButtonGroupContainer->MakeVisible(ETrue);
    }


// ---------------------------------------------------------------------------
// Hides action menu's cba.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::HideCbaButtonsL()
    {
    FUNC_LOG;
    iButtonGroupContainer->MakeVisible(EFalse);
    }
    

// ---------------------------------------------------------------------------
// Executes action menu. 
// ---------------------------------------------------------------------------
//
void CFsActionMenu::DoExecuteL()
    {
    FUNC_LOG;
    iRootLayout->ClearFlag(EAlfVisualFlagLayoutUpdateNotification);
    TAlfTimedValue opacity;
    opacity.SetValueNow(0.0);
    iRootLayout->SetOpacity(opacity);
       
    UpdateActionMenuWindowL( );
    SetFirstItemAsFocusedL( );
    ShowCbaButtonsL( );
    //////////////////////////////////////////
    iDisplay->Roster().ShowL( *iCtrlGroup );
    
    TBool fadeIn(EFalse), slideIn(EFalse);
        
    fadeIn = IsFadeIn();
    slideIn = IsSlideIn();
    
    this->ShowListL( fadeIn, slideIn );

    //show root layout
    opacity.SetValueNow(1.0);
    iRootLayout->SetOpacity(opacity);
    
    iRootLayout->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
    
    StartWait();
    
    }


// ---------------------------------------------------------------------------
// Dismiss action menu. 
// ---------------------------------------------------------------------------
//
void CFsActionMenu::DoDismissL()
    {
    FUNC_LOG;
    TBool fadeOut(EFalse), slideOut(EFalse);
    
    fadeOut = IsFadeOut();
    slideOut = IsSlideOut();
    this->HideListL( fadeOut, slideOut );
        
    //after the list is fully hidden we get a notification in TreeListEvent and
    //action menu is removed from roster
    
    StopWait();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFsActionMenu::StartWait()
    {
    FUNC_LOG;
    iWait->Start();
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFsActionMenu::StopWait()
    {
    FUNC_LOG;
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }


// ---------------------------------------------------------------------------
// The function sets size and position of Action Menu.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::UpdateActionMenuWindowL( )
    {
    FUNC_LOG;
    //AM's size
    SetAMSizeFromLayoutMgrL( );
    //AM's position
    CalculatePosition( );
    
    iVisualizer->SetPadding( TAlfBoxMetric(iAmXPadd, iAmXPadd, iAmYPadd, iAmYPadd) );        
    
    iRootLayout->SetSize( iSize, 0 );
    iRootLayout->SetPos( iPos, 0 );
    iRootLayout->UpdateChildrenLayout( 0 );
    }

// ---------------------------------------------------------------------------
// The function retrieves Action Menu's size from Layout Manager.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::SetAMSizeFromLayoutMgrL( )
    {
    FUNC_LOG;
    TInt count(0), variety(0);
    
    count = Count();
    if ( count <= 0 )
        {
        iSize.iWidth = 0;
        iSize.iHeight = 0;
        }
    else
        {
        variety = count + 1;//0-6 variety for 1-7 row AM
        if ( variety < 2 )
            {
            variety = 2; //lowest variety to avoid crashes
            }
        
        if ( count > 7 )
            {
            variety = 8; //highest variety
            SetScrollbarVisibilityL(EFsScrollbarShowAlways);
            }
        else
            {
            SetScrollbarVisibilityL(EFsScrollbarHideAlways);
            }
        
        TRect parentRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, parentRect );

        TRect clientRect;
        CFsLayoutManager::LayoutMetricsRect( parentRect, CFsLayoutManager::EFsLmMainPane, clientRect );
        
        CFsLayoutManager::TFsLayoutMetrics actionMenuMetrics(CFsLayoutManager::EFsLmPopupSpFsActionMenuPane);
        
        TRect actionMenuPopupRect;
        CFsLayoutManager::LayoutMetricsRect(
                        clientRect, //parent
                        actionMenuMetrics, //queried item
                        actionMenuPopupRect, // result
                        variety ); 
        
        iSize.iWidth = actionMenuPopupRect.Width();
        iSize.iHeight = actionMenuPopupRect.Height();
        iSize.iHeight += 1;
        }

    }


// ---------------------------------------------------------------------------
// The function calculates AM's position based on it's size and predefined vertical
// position (top,center,bottom) or custom vertical position set by the user.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::CalculatePosition( )
    {
    FUNC_LOG;
    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, clientRect );    
     
    //horizontal position
    TRect amRect;
    if ( CFsLayoutManager::IsMirrored() )
        {
        amRect.iTl.iX = 0;
        }
    else
        {
        amRect.iTl.iX = clientRect.iBr.iX - iSize.iWidth;        
        }
    
    amRect.iBr.iX = amRect.iTl.iX + iSize.iWidth;
    
    //vertical position
    if ( iIsCustVPos )
        {
        amRect.iTl.iY = iCustVPos;
        amRect.iBr.iY = amRect.iTl.iY + iSize.iHeight;
        }
    else
        {
        switch ( iVPos )
            {
            case EFsVPosTop:
                amRect.iTl.iY = 0;
                amRect.iBr.iY = amRect.iTl.iY + iSize.iHeight;
                break;
            case EFsVPosCenter:           
                amRect.iTl.iY = (clientRect.Height() - iSize.iHeight) / 2;
                amRect.iBr.iY = amRect.iTl.iY + iSize.iHeight;
                break;
            case EFsVPosBottom:
            default:
                amRect.iBr.iY = clientRect.Height();
                amRect.iTl.iY = amRect.iBr.iY - iSize.iHeight;
                break;
            }
        }
    
    iPos = amRect.iTl;
    }


// ---------------------------------------------------------------------------
// The function returns whether the slide in effect is set.
// ---------------------------------------------------------------------------
//
TBool CFsActionMenu::IsSlideIn()
    {
    FUNC_LOG;
    TBool slideIn(EFalse);
    if ( iVisualizer->SlideInDirection() != MFsTreeVisualizer::ESlideNone && 
         iVisualizer->SlideInDuration() > 0 )
        {
        slideIn = ETrue;
        }
    else
        {
        slideIn = EFalse;
        }
    return slideIn;
    }


// ---------------------------------------------------------------------------
// The function returns whether the fade in effect is set.
// ---------------------------------------------------------------------------
//
TBool CFsActionMenu::IsFadeIn()
    {
    FUNC_LOG;
    TBool fadeIn(EFalse);
    if ( iVisualizer->FadeInEffectTime() > 0 )
        {
        fadeIn = ETrue;
        }
    else
        {
        fadeIn = EFalse;
        }
    return fadeIn;
    }


// ---------------------------------------------------------------------------
// The function returns whether the slide out effect is set.
// ---------------------------------------------------------------------------
//
TBool CFsActionMenu::IsSlideOut()
    {
    FUNC_LOG;
    TBool slideOut(EFalse);
    if ( iVisualizer->SlideOutDirection() != MFsTreeVisualizer::ESlideNone && 
         iVisualizer->SlideOutDuration() > 0 )
        {
        slideOut = ETrue;
        }
    else
        {
        slideOut = EFalse;
        }
    return slideOut;
    }


// ---------------------------------------------------------------------------
// The function returns whether the fade out effect is set.
// ---------------------------------------------------------------------------
//
TBool CFsActionMenu::IsFadeOut()
    {
    FUNC_LOG;
    TBool fadeOut(EFalse);
    if ( iVisualizer->FadeOutEffectTime() > 0 )
        {
        fadeOut = ETrue;
        }
    else
        {
        fadeOut = EFalse;
        }
    return fadeOut;
    }


// ---------------------------------------------------------------------------
// The function sets first list item as focused.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::SetFirstItemAsFocusedL()
    {
    FUNC_LOG;
    const TInt KZero( 0 );
    
    if ( !IsEmpty( KFsTreeRootID ) )
        {
        TFsTreeItemId firstItemId( KFsTreeNoneID );
        
        firstItemId = Child( KFsTreeRootID, KZero );
        if ( firstItemId != KFsTreeNoneID )
            {
            iVisualizer->SetFocusedItemL( firstItemId );
            }
        }
    }


// ---------------------------------------------------------------------------
//  The function fades screen behind popup.
// ---------------------------------------------------------------------------
//
void CFsActionMenu::FadeBehindActionMenuL( TBool aFaded ) // <cmail>
    {
    FUNC_LOG;
    TBool faded (aFaded);
    
    CEikStatusPane* statusPane =  CEikonEnv::Static()->AppUiFactory()->StatusPane();
   
    if ( statusPane )
        {
        CCoeControl* control(NULL);
        TUid uid;
        
        uid.iUid = EEikStatusPaneUidSignal;     
        CEikStatusPaneBase::TPaneCapabilities subPane = statusPane->PaneCapabilities(uid);
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        
        
        uid.iUid = EEikStatusPaneUidBattery;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        

        uid.iUid = EEikStatusPaneUidContext;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        
        
        uid.iUid = EEikStatusPaneUidTitle;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        

        uid.iUid = EEikStatusPaneUidNavi;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        

        uid.iUid = EEikStatusPaneUidIndic;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        

        uid.iUid = EEikStatusPaneUidMessage;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
            

        uid.iUid = EEikStatusPaneUidClock;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        
        
        uid.iUid = EEikStatusPaneUidDigitalClock;
        subPane = statusPane->PaneCapabilities( uid );
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        
        
        uid.iUid = EEikStatusPaneUidEmpty;
        subPane = statusPane->PaneCapabilities( uid );    
        if ( subPane.IsPresent() && subPane.IsAppOwned() )
            {
            control = statusPane->ControlL( uid );
            control->DrawableWindow()->SetFaded( faded, RWindowTreeNode::EFadeIncludeChildren );
            }
        }
    
    

    TAlfTimedValue opacity;
    CAlfGradientBrush* brush(NULL);    
    if ( faded )
        {
        if (iFadeLayout->Brushes())
            {
            if ( iFadeLayout->Brushes()->Count() <= 0 )
                {
                TRgb color(240,240,240);
                brush = CAlfGradientBrush::NewL( iControl->Env() );
                brush->SetColor( color );
                brush->SetLayer(EAlfBrushLayerBackground);
                opacity.SetTarget( 0.55 , 0);
                brush->SetOpacity( opacity );            
                iFadeLayout->Brushes()->AppendL(brush,EAlfHasOwnership);
                }
            else
                {
                opacity.SetTarget( 0.55 , 0);
                brush = reinterpret_cast<CAlfGradientBrush*>(&iFadeLayout->Brushes()->At(0));
                brush->SetOpacity( opacity );
                }
            }        
        }
    else
        {
        if (iFadeLayout->Brushes())
            {
            opacity.SetTarget( 0.0 , 0);
            brush = reinterpret_cast<CAlfGradientBrush*>(&iFadeLayout->Brushes()->At(0));
            brush->SetOpacity( opacity );
            }
        }    
    }


// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsActionMenu::CFsActionMenu( CAlfDisplay& aDisplay )
: CFsTreeList( aDisplay.Env(), *iVisualizer ), //setting NULL visualizer, visualizer created in ConstructL
  iDisplay( &aDisplay ),
  iVPos( EFsVPosTop ),
  iCustVPos(0),
  iIsCustVPos( EFalse ), 
  KListBorderPadding(5),
  iAmXPadd(5),
  iAmYPadd(5)
    {
    FUNC_LOG;
    
    }    


// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsActionMenu::ConstructL( TInt aCbaResource )
    {
    FUNC_LOG;
    iWait = new ( ELeave ) CActiveSchedulerWait();
    //create action list's control group
    //check if the group exists - take first free slot
    iCtrlGroup = &iDisplay->Env().NewControlGroupL( KFsGroupActionMenu );
    
    iControl = new (ELeave) CFsActionMenuControl(this);
    // <cmail> Removed cleanup stack usage of mthe meber variables. </cmail>
    iControl->ConstructL( iDisplay->Env());
    
    iFadeLayout = CAlfDeckLayout::AddNewL (*iControl);
    iFadeLayout->EnableBrushesL();
    
    iRootLayout = CAlfDeckLayout::AddNewL (*iControl);
    // <cmail> Removed cleanup stack usage of mthe meber variables. </cmail>

    iRootLayout->SetFlag(EAlfVisualFlagManualLayout);
    iRootLayout->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
    
    
    //set initial pos/size - proper values set by ExecuteL
    iRootLayout->ClearFlag(EAlfVisualFlagLayoutUpdateNotification);
    iRootLayout->SetPos(TAlfRealPoint(30,15));
    iRootLayout->SetSize(TSize ( 150, 100 ) );
    iRootLayout->UpdateChildrenLayout();
    iRootLayout->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
    iControl->SetRootVisual( iRootLayout );

    ///////////////////////////////////////
    //DEBUG BORDER
    /*
    iRootLayout->EnableBrushesL( ETrue );
    CAlfBorderBrush* border = CAlfBorderBrush::NewLC(aEnv,1, 1, 0, 0);
    border->SetColor(KRgbRed);
    iRootLayout->Brushes()->AppendL(border, EAlfHasOwnership);
    CleanupStack::Pop(border);
    */
    ///////////////////////////////////////

    
    //create list's visualizer
    iVisualizer = CFsTreeVisualizerBase::NewL( iControl, *iRootLayout );
    // <cmail> Removed cleanup stack usage of mthe meber variables. </cmail>
    //construct the list's base class
    CFsTreeList::SetTreeVisualizer( *iVisualizer );
    CFsTreeList::ConstructL( iDisplay->Env() );
    //use custom padding - turn off list's padding
    iVisualizer->EnableListPanePadding( EFalse );
    //set some list's properties
    CFsTreeList::SetMarkTypeL( CFsTreeList::EFsTreeListNonMarkable );
    CFsTreeList::SetLoopingType( EFsTreeListLoopingDisabled );
    CFsTreeList::SetScrollTime( KFsActionMenuScrollTime, KFsActionMenuAccelerationRate );
    //Observe the list.
    CFsTreeList::AddObserverL(*this);
    
    iCtrlGroup->AppendL( iControl );
    iCtrlGroup->AppendL( this->TreeControl() );    
    
    iRootLayout->UpdateChildrenLayout();
    
    //hide the list
    this->HideListL( EFalse, EFalse );       
    iDisplay->Roster().Hide( *iCtrlGroup );
    
    //construct Cba buttons
    iButtonGroupContainer = CEikButtonGroupContainer::NewL( 
            CEikButtonGroupContainer::ECba,
            CEikButtonGroupContainer::EHorizontal,
            this, aCbaResource );
    //hide cba buttons
    iButtonGroupContainer->MakeVisible(EFalse);
    // <cmail> Removed cleanup stack usage of mthe meber variables. </cmail>
    }




//End Of File

