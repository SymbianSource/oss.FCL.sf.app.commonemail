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
* Description:  A visualier class for the MultiLine item.
*
*/


//////SYSTEM INCLUDES

//////TOOLKIT INCLUDES
#include "emailtrace.h"
#include <alf/alfenv.h>
#include <alf/alflayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alfcontrol.h>
#include <alf/alfimage.h>

//////PROJECT INCLUDES
#include "fsmultilineitemvisualizer.h"
#include "fsmultilineitemdata.h"
#include "fslayoutmanager.h"


//CONSTANTS
const TInt KZero = 0;
const TInt KOne = 1;
const TInt KTwo = 2;
const TInt KThree = 3;
const TInt KFour = 4;
const TInt KFive = 5;
const TInt KSix = 6;
const TInt KSeven = 7;
const TInt KEight = 8;

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsMultiLineItemVisualizer* CFsMultiLineItemVisualizer::NewL( CAlfControl& aOwnerControl )
    {
    FUNC_LOG;
    CFsMultiLineItemVisualizer* self = new( ELeave ) CFsMultiLineItemVisualizer( aOwnerControl );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CFsMultiLineItemVisualizer::~CFsMultiLineItemVisualizer()
    {
    FUNC_LOG;
    
    }


//item visualizer's interface

// ---------------------------------------------------------------------------
// The function sets how many lines the multiline item has when it's not extended.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::SetNumberOfLinesWhenNotExtended( TInt aNumberOfLines )
    {
    FUNC_LOG;
    if ( 1 <= aNumberOfLines && aNumberOfLines <= 4 && aNumberOfLines <= iNumLinesExtended )
        {
        iNumLinesNotExtended = aNumberOfLines;
        }
    }


// ---------------------------------------------------------------------------
// The function returns how many lines the item has when it's not extended.
// ---------------------------------------------------------------------------
//
TInt CFsMultiLineItemVisualizer::NumberOfLinesWhenNotExtended( ) const
    {
    FUNC_LOG;
    return iNumLinesNotExtended;
    }


// ---------------------------------------------------------------------------
// The function sets how many lines the multiline item has when it's extended.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::SetNumberOfLinesWhenExtended( TInt aNumberOfLines )
    {
    FUNC_LOG;
    if ( aNumberOfLines >= 1 && aNumberOfLines <= 4 && aNumberOfLines >= iNumLinesNotExtended )
        {
        iNumLinesExtended = aNumberOfLines;
        }
    }


// ---------------------------------------------------------------------------
// The function returns how many lines the item has when it's extended.
// ---------------------------------------------------------------------------
//
TInt CFsMultiLineItemVisualizer::NumberOfLinesWhenExtended( ) const
    {
    FUNC_LOG;
    return iNumLinesExtended;
    }


// ---------------------------------------------------------------------------
// The function sets state of the item's checkbox.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::SetCheckboxState( TBool aChecked )
    {
    FUNC_LOG;
    iCheckState = aChecked;
    }


// ---------------------------------------------------------------------------
// The function returns state of the item's checkbox. 
// ---------------------------------------------------------------------------
//
TBool CFsMultiLineItemVisualizer::CheckboxState () const
    {
    FUNC_LOG;
    return iCheckState;
    }


// ---------------------------------------------------------------------------
// The function sets whether place holders should be kept when icons in the 
// first column are not present. 
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::SetKeepEmptyIconsPlaceHolders( TBool aKeep )
    {
    FUNC_LOG;
    iKeepPlaceHolders = aKeep;
    }

// ---------------------------------------------------------------------------
// The function returnes whether place holders are kept in case icons are
// not set.
// ---------------------------------------------------------------------------
//
TBool CFsMultiLineItemVisualizer::IsKeepEmptyIconsPlaceHolders( ) const
    {
    FUNC_LOG;
    return iKeepPlaceHolders;
    }


// from base class MFsTreeItemVisualizer

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Returns the type of the item's visualizer. 
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsMultiLineItemVisualizer::Type() const
    {
    FUNC_LOG;
    return EFsMultiLineItemVisualizer;
    }


// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Returns the size of an item in a normal (not extended) state.
// ---------------------------------------------------------------------------
//
TSize CFsMultiLineItemVisualizer::Size() const
    {
    FUNC_LOG;
    if ( IsAlwaysExtended() )
        {
        return ExtendedSize();
        }
    else
        {
        if ( iFlags & KFsTreeListItemManagedLayout )
            {
            return SizeWithNumberOfLines( iNumLinesNotExtended );
            }
        else
            {
            return iSize;
            }
        }
    }


// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Returns the size of an item in expanded state.
// ---------------------------------------------------------------------------
//    
TSize CFsMultiLineItemVisualizer::ExtendedSize() const
    {
    FUNC_LOG;
    if ( iFlags & KFsTreeListItemManagedLayout )
        {
        return SizeWithNumberOfLines( iNumLinesExtended );
        }
    else
        {
        return iExtendedSize;
        }    
    }


// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// A call to this function means that the item is requested to draw itself
// within specified parent layout. 
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::ShowL( CAlfLayout& aParentLayout, const TUint aTimeout )
    {
    FUNC_LOG;
   
    if (!iLayout)
        {
        iParentLayout = &aParentLayout;
        iLayout = CAlfAnchorLayout::AddNewL(iOwnerControl,iParentLayout);
        iLayout->EnableBrushesL();
        iLayout->SetClipping(ETrue);
    ///////////////////////////first line///////////////////////////
        //text
        if (!iFirstLineTextVis)
            {
            iFirstLineTextVis = CAlfTextVisual::AddNewL(iOwnerControl, iLayout );
            }
        //icons
        if (!iFirstLineIconVis) //optional priority icon in first line
            {
            iFirstLineIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iFirstLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }
        
        if (!iCheckBoxIconVis)
            {
            iCheckBoxIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iCheckBoxIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }
        
        if (!iMarkIconVis)
            {
            iMarkIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iMarkIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }
        
        if(!iMenuIconVis)
            {
            iMenuIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iMenuIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }

    ///////////////////////////second line///////////////////////////
        //text
        if (!iSecondLineTextVis)
            {
            iSecondLineTextVis = CAlfTextVisual::AddNewL(iOwnerControl, iLayout );
            }
        //icon
        if (!iSecondLineIconVis)
            {
            iSecondLineIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iSecondLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }

    ///////////////////////////third line///////////////////////////
        //text
        if (!iThirdLineTextVis)
            {
            iThirdLineTextVis = CAlfTextVisual::AddNewL(iOwnerControl, iLayout );
            }
        //icon
        if (!iThirdLineIconVis)
            {
            iThirdLineIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iThirdLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }
        
    ///////////////////////////fourth line///////////////////////////
        //text
        if (!iFourthLineTextVis)
            {
            iFourthLineTextVis = CAlfTextVisual::AddNewL(iOwnerControl, iLayout );
            }
        //icon
        if (!iFourthLineIconVis)
            {
            iFourthLineIconVis = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iFourthLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            }
  
        }
    else
        {
        //visuals are already created but not attached to the list layout
        if (!iLayout->Layout()) 
            {
            aParentLayout.Append(iLayout, aTimeout);
            iParentLayout = &aParentLayout;
            }
        }
    }


// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// A call to this function means that the item is requested to update its
// visual content. 
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::UpdateL( const MFsTreeItemData& aData,
                                        TBool aFocused,
                                        const TUint aLevel,
                                        CAlfTexture*& aMarkOnIcon,
                                        CAlfTexture*& /*aMarkOffIcon*/,
                                        CAlfTexture*& aMenuIcon,
                                        const TUint /*aTimeout*/,
                                        TBool aUpdateData)
    {
    FUNC_LOG;
    
    if (!iLayout)
        {
        return;
        }
        
    //text color
    TRgb textColor;
    if ( aFocused )
        {
        if (!iIsCustomTextFocusedColor)
            {//custom color not set, use color from skin - load it in case theme has changed
            iFocusedTextColor = FocusedStateTextSkinColor();
            }
        textColor = iFocusedTextColor;
        iFlags |= KFsTreeListItemFocused;
        }
    else
        {
        if (!iIsCustomTextNormalColor)
            {//custom color not set, use color from skin - load it in case theme has changed
            iNormalTextColor = NormalStateTextSkinColor( );
            }
        textColor = iNormalTextColor;
        iFlags &= ~KFsTreeListItemFocused;
        }
    
    //text aignment in visuals    
    TAlfAlignHorizontal currTextAlignment( EAlfAlignHLeft );            
    currTextAlignment = CurrentTextAlignmentL( iTextAlign, NULL, NULL ); //Western/AH layout
    
    //text's shadow
    TBool enableShadow(EFalse);
    enableShadow = iFlags & KFsTreeListItemTextShadow; 
    
    TInt styleId = ModifiedStyleIdL( );
    
    //set data to the proper visuals
    if ( aData.Type() == KFsMultiLineItemDataType )
        {
        const CFsMultiLineItemData* data = static_cast<const CFsMultiLineItemData*>(&aData);
        
        //set icons and texts to their visuals
        
///////////////////////////first line///////////////////////////////////////////
        //text
        if (iFirstLineTextVis && aUpdateData)
            {
            iFirstLineTextVis->Env().CancelCommands( iFirstLineTextVis );
            
            iFirstLineTextVis->EnableShadow( enableShadow );
            iFirstLineTextVis->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
            iFirstLineTextVis->SetAlign( currTextAlignment, EAlfAlignVCenter );
            iFirstLineTextVis->SetTextStyle( styleId );
            iFirstLineTextVis->SetColor( textColor );
            
            iFirstLineTextVis->SetTextL( data->LineText( 0 ));
            }
        
        //icon
        if (iCheckBoxIconVis)
            {
            if ( CheckboxState() )
                {
                if ( data->CheckBoxCheckedIcon() )
                    {
                    iCheckBoxIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
                    iCheckBoxIconVis->SetImage( TAlfImage( *data->CheckBoxCheckedIcon() ) );
                    }
                else
                    {
                    //zero visual's opacity
                    }
                }
            else 
                {
                if ( data->CheckBoxUncheckedIcon() )
                    {
                    iCheckBoxIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
                    iCheckBoxIconVis->SetImage( TAlfImage( *data->CheckBoxUncheckedIcon() ) );
                    }
                else
                    {
                    //zero visual's opacity
                    }
                }
            }
        //icon            
        if ( iFirstLineIconVis && data->LineIcon( 0 ) )
            {
            iFirstLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            iFirstLineIconVis->SetImage( TAlfImage( *data->LineIcon( 0 ) ) );                
            }
        //mark icon
        if ( iMarkIconVis && IsMarked() && aMarkOnIcon )
            {
            iMarkIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            iMarkIconVis->SetImage( *aMarkOnIcon );
            }
        //menu icon
        if ( iMenuIconVis && (iFlags & KFsTreeListItemHasMenu) && aMenuIcon )
            {
            iMenuIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            iMenuIconVis->SetImage( *aMenuIcon );
            }
///////////////////////////second line//////////////////////////////////////////
        //text
        if ( iSecondLineTextVis && aUpdateData)
            {
            iSecondLineTextVis->Env().CancelCommands( iSecondLineTextVis );
                            
            iSecondLineTextVis->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
            iSecondLineTextVis->SetAlign( currTextAlignment, EAlfAlignVCenter );
            iSecondLineTextVis->EnableShadow( enableShadow );
            iSecondLineTextVis->SetTextStyle( styleId );
            iSecondLineTextVis->SetColor( textColor );
            
            iSecondLineTextVis->SetTextL( data->LineText( 1 ));
            }
        
        //icon            
        if ( iSecondLineIconVis && data->LineIcon( 1 ) )
            {
            iSecondLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            iSecondLineIconVis->SetImage( TAlfImage( *data->LineIcon( 1 ) ) );                
            }
            
///////////////////////////third line///////////////////////////////////////////
        //text
        if ( iThirdLineTextVis && aUpdateData )
            {
            iThirdLineTextVis->Env().CancelCommands( iThirdLineTextVis );
                            
            iThirdLineTextVis->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
            iThirdLineTextVis->SetAlign( currTextAlignment, EAlfAlignVCenter );
            iThirdLineTextVis->EnableShadow( enableShadow );
            iThirdLineTextVis->SetTextStyle( styleId );
            iThirdLineTextVis->SetColor( textColor );
            
            iThirdLineTextVis->SetTextL( data->LineText( 2 ) );
            }
        
        //icon
        if ( iThirdLineIconVis && data->LineIcon( 2 ) )
            {
            iThirdLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            iThirdLineIconVis->SetImage( TAlfImage( *data->LineIcon( 2 ) ) );
            }
            
///////////////////////////fourth line//////////////////////////////////////////
        //text
        if ( iFourthLineTextVis && aUpdateData)
            {
            iFourthLineTextVis->Env().CancelCommands( iFourthLineTextVis );
                            
            iFourthLineTextVis->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
            iFourthLineTextVis->SetAlign( currTextAlignment, EAlfAlignVCenter );
            iFourthLineTextVis->EnableShadow( enableShadow );
            iFourthLineTextVis->SetTextStyle( styleId );
            iFourthLineTextVis->SetColor( textColor );
            
            iFourthLineTextVis->SetTextL( data->LineText( 3 ) );
            }
        
        //icon
        if ( iFourthLineIconVis && data->LineIcon( 3 ) )
            {
            iFourthLineIconVis->SetScaleMode( CAlfImageVisual::EScaleFit );
            iFourthLineIconVis->SetImage( TAlfImage( *data->LineIcon( 3 ) ) );
            }
        
        const TInt indent( iTextIndentation * ( aLevel - 1 ) );
        UpdateLayout(data, indent);
        iLayout->UpdateChildrenLayout();
        }    
    }


// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// A call to this function means that the item goes out of the visible 
// items scope. The visualizer should destroy all its visuals to save 
// memory. 
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::Hide( const TInt aTimeout )
    {
    FUNC_LOG;
    if ( iLayout )
        {
        iParentLayout->Remove(iLayout, aTimeout);
        iLayout->RemoveAndDestroyAllD();
        iLayout = NULL;
        iParentLayout = NULL;
///////////////////////////first line///////////////////////////
        iFirstLineTextVis = NULL;
        iFirstLineIconVis = NULL;
        iCheckBoxIconVis = NULL;
        iMarkIconVis = NULL;
        iMenuIconVis = NULL;
///////////////////////////second line///////////////////////////
        iSecondLineTextVis = NULL;
        iSecondLineIconVis = NULL;
///////////////////////////third line///////////////////////////
        iThirdLineTextVis = NULL;
        iThirdLineIconVis = NULL;
///////////////////////////fourth line///////////////////////////
        iFourthLineTextVis = NULL;
        iFourthLineIconVis = NULL;
        }
    }


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CFsMultiLineItemVisualizer::CFsMultiLineItemVisualizer( CAlfControl& aOwnerControl )
: CFsTreeItemVisualizerBase( aOwnerControl ),
  iCheckState(EFalse),
  iKeepPlaceHolders(EFalse),
  iNumLinesNotExtended(KOne),
  iNumLinesExtended(KFour)
    {
    FUNC_LOG;
    
    }


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::ConstructL( )
    {
    FUNC_LOG;
    CFsTreeItemVisualizerBase::ConstructL( ); //set text colors
    }


// ---------------------------------------------------------------------------
// Updates anchor for visuals according to the current item's setup.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::UpdateLayout(const CFsMultiLineItemData* aData, const TInt aIndentation )
    {
    FUNC_LOG;

///////////////////////////first line///////////////////////////
    //icon
    TAlfTimedPoint tpFirstLineCheckBoxIconTl(0.0,0.0);
    TAlfTimedPoint tpFirstLineCheckBoxIconBr(0.0,0.0);
    //priority icon
    TAlfTimedPoint tpFirstLineIconTl(0.0,0.0);
    TAlfTimedPoint tpFirstLineIconBr(0.0,0.0);
    //text
    TAlfTimedPoint tpFirstLineTextTl(0.0,0.0);
    TAlfTimedPoint tpFirstLineTextBr(0.0,0.0);
    //mark icon
    TAlfTimedPoint tpFirstLineMarkIconTl(0.0,0.0);
    TAlfTimedPoint tpFirstLineMarkIconBr(0.0,0.0);
    //menu icon
    TAlfTimedPoint tpFirstMenuIconTl(0.0,0.0);
    TAlfTimedPoint tpFirstMenuIconBr(0.0,0.0);
    
    //LM metrics
    CFsLayoutManager::TFsLayoutMetrics firstLineIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG1),
                                       firstLinePrioIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG2),
                                       firstLineTextMetrics(CFsLayoutManager::EFsLmListSingleDycRowTextPane),
                                       firstLineMenuIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG3),
                                       firstLineMarkIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG4);
    TInt firstLineIconVariety(KZero),
         firstLinePrioIconVariety(KZero),
         firstLineTextVariety(KZero),
         firstLineMenuIconVariety(KZero),
         firstLineMarkIconVariety(KZero); 
    
///////////////////////////second line///////////////////////////
    //icon
    TAlfTimedPoint tpSecondLineIconTl(0.0,0.0);
    TAlfTimedPoint tpSecondLineIconBr(0.0,0.0);
    //text
    TAlfTimedPoint tpSecondLineTextTl(0.0,0.0);
    TAlfTimedPoint tpSecondLineTextBr(0.0,0.0);
    //LM metrics
    CFsLayoutManager::TFsLayoutMetrics secondLineIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG1),
                                       secondLineTextMetrics(CFsLayoutManager::EFsLmListSingleDycRowTextPane);
    TInt secondLineTextVariety(KZero);
///////////////////////////third line///////////////////////////
    //icon
    TAlfTimedPoint tpThirdLineIconTl(0.0,0.0);
    TAlfTimedPoint tpThirdLineIconBr(0.0,0.0);
    //text
    TAlfTimedPoint tpThirdLineTextTl(0.0,0.0);
    TAlfTimedPoint tpThirdLineTextBr(0.0,0.0);
    //LM metrics
    CFsLayoutManager::TFsLayoutMetrics thirdLineIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG1),
                                       thirdLineTextMetrics(CFsLayoutManager::EFsLmListSingleDycRowTextPane);
    TInt thirdLineTextVariety(KZero);
///////////////////////////fourth line///////////////////////////    
    //icon
    TAlfTimedPoint tpFourthLineIconTl(0.0,0.0);
    TAlfTimedPoint tpFourthLineIconBr(0.0,0.0);
    //text
    TAlfTimedPoint tpFourthLineTextTl(0.0,0.0);
    TAlfTimedPoint tpFourthLineTextBr(0.0,0.0);
    CFsLayoutManager::TFsLayoutMetrics fourthLineIconMetrics(CFsLayoutManager::EFsLmListSingleDycRowPaneG1),
                                       fourthLineTextMetrics(CFsLayoutManager::EFsLmListSingleDycRowTextPane);
    TInt fourthLineTextVariety(KZero);
/////////////////////////////////////////////////////////////////    
        
    
    TBool itemFocused(EFalse);
    itemFocused = iFlags & KFsTreeListItemFocused;
    
    TAlfTimedValue opacity;
    
    TInt linesVisible(1);
    
    if ( (itemFocused || IsAlwaysExtended()) && IsExtendable() ) // the item has focus or is always extended
        {
        linesVisible = NumberOfLinesWhenExtended();
        }
    else
        {
        linesVisible = NumberOfLinesWhenNotExtended();
        }

    
    //0. set opacities for visuals
    ///////////////////////////first line///////////////////////////
    
    opacity.SetValueNow( 1.0 );
     
    if ( iFirstLineTextVis )
        {
        iFirstLineTextVis->SetOpacity(opacity);
        }
        
    //set opacities to visuals in lines 2-4
        
    ///////////////////////////second line///////////////////////////
    if ( linesVisible >= 2 ) // line visible?
        {
        opacity.SetValueNow( 1.0 );
        }
    else 
        {
        opacity.SetValueNow( 0.0 );
        }
     
    if ( iSecondLineIconVis )
        {
        iSecondLineIconVis->SetOpacity(opacity);
        }
    if ( iSecondLineTextVis )
        {
        iSecondLineTextVis->SetOpacity(opacity);
        }
    ///////////////////////////third line///////////////////////////
    if ( linesVisible >= 3 ) // line visible?
        {
        opacity.SetValueNow( 1.0 );
        }
    else 
        {
        opacity.SetValueNow( 0.0 );
        }
        
    if ( iThirdLineIconVis )
        {
        iThirdLineIconVis->SetOpacity(opacity);
        }
    if ( iThirdLineTextVis )
        {
        iThirdLineTextVis->SetOpacity(opacity);
        }
    ///////////////////////////fourth line///////////////////////////    
    if ( linesVisible == 4 ) // line visible?
        {
        opacity.SetValueNow( 1.0 );
        }
    else 
        {
        opacity.SetValueNow( 0.0 );
        }
    if ( iFourthLineIconVis )
        {
        iFourthLineIconVis->SetOpacity(opacity);
        }
    if ( iFourthLineTextVis )
        {
        iFourthLineTextVis->SetOpacity(opacity);
        }
    
    //1. set anchor values for visuals
///////////////////////////first line///////////////////////////
    //no icons, only text
    firstLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
    firstLineTextVariety = KEight;
        
    TBool isCheckboxPres(EFalse),isMarkPres(EFalse);
    
    if ( CheckboxState() && aData->CheckBoxCheckedIcon() ||
         !CheckboxState() && aData->CheckBoxUncheckedIcon()   )
        {
        isCheckboxPres = ETrue;
        firstLineIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
        firstLineIconVariety = KZero;
        
        firstLineTextVariety = KFive;
        
        opacity.SetValueNow( 1.0 );
        iCheckBoxIconVis->SetOpacity(opacity);
        }
    else
        {
        isCheckboxPres = EFalse;
        //zero image visual's opacity
        opacity.SetValueNow( 0.0 );
        iCheckBoxIconVis->SetOpacity(opacity);
        }
    
    if ( aData->LineIcon( KZero ) )
        {
        if ( isCheckboxPres )
            {
            firstLinePrioIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG2;
            firstLinePrioIconVariety = KZero;
            firstLineTextVariety = KTwo;
            }
        else
            {
            firstLinePrioIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
            firstLinePrioIconVariety = KZero;
            firstLineTextVariety = KFive;
            }
        opacity.SetValueNow( 1.0 );
        iFirstLineIconVis->SetOpacity(opacity);
        }
    else
        {
        if ( !isCheckboxPres && IsKeepEmptyIconsPlaceHolders() )
            {
            firstLineTextVariety = KFive;
            }
        
        //zero image visual's opacity
        opacity.SetValueNow( 0.0 );
        iFirstLineIconVis->SetOpacity(opacity);
        }
    
    
    if ( IsMarked() )
        {
        isMarkPres = ETrue;
        firstLineMarkIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
        firstLineMarkIconVariety = KZero;
        
        if ( firstLineTextVariety == KTwo )
            {
            firstLineTextVariety = KOne;
            }
        else if ( firstLineTextVariety == KFive )
            {
            firstLineTextVariety = KFour;
            }
        else //KEight
            {
            firstLineTextVariety = KSeven;
            }
        
        opacity.SetValueNow( 1.0 );
        iMarkIconVis->SetOpacity(opacity);
        }
    else
        {
        isMarkPres = EFalse;
        //zero image visual's opacity
        opacity.SetValueNow( 0.0 );
        iMarkIconVis->SetOpacity(opacity);
        }
    
    if ( itemFocused && ( iFlags & KFsTreeListItemHasMenu ) )
        {
        if ( isMarkPres )
            {
            firstLineMenuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
            firstLineMenuIconVariety = KZero;
            if ( firstLineTextVariety == KOne )
                {
                firstLineTextVariety = KZero;
                }
            else if ( firstLineTextVariety ==  KFour )
                {
                firstLineTextVariety = KThree;
                }
            else //KSeven
                {
                firstLineTextVariety = KSix;
                }
            }
        else
            {
            firstLineMenuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
            firstLineMenuIconVariety = KZero;
            
            if ( firstLineTextVariety == KTwo )
                {
                firstLineTextVariety = KOne;
                }
            else if ( firstLineTextVariety == KFive )
                {
                firstLineTextVariety = KFour;
                }
            else //KEight
                {
                firstLineTextVariety = KSeven;
                }
            }
        opacity.SetValueNow( 1.0 );
        iMenuIconVis->SetOpacity(opacity);
        }
    else
        {
        //zero image visual's opacity
        opacity.SetValueNow( 0.0 );
        iMenuIconVis->SetOpacity(opacity);
        }    
    
///////////////////////////second line///////////////////////////    
    if ( aData->LineIcon( KOne ) )
        {
        secondLineIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
        secondLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
        
        secondLineTextVariety = KFive;
        }
    else
        {
        if ( IsKeepEmptyIconsPlaceHolders() )
            {
            secondLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
            secondLineTextVariety = KFive;
            }
        else
            {
            secondLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
            secondLineTextVariety = KEight;
            }
        
        opacity.SetValueNow( 0.0 );
        iSecondLineIconVis->SetOpacity(opacity);
        }
    
///////////////////////////third line///////////////////////////
    if ( aData->LineIcon( KTwo ) )
        {
        thirdLineIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
        thirdLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
        thirdLineTextVariety = KFive;
        }
    else
        {
        if ( IsKeepEmptyIconsPlaceHolders() )
            {
            thirdLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
            thirdLineTextVariety = KFive;
            }
        else
            {
            thirdLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
            thirdLineTextVariety = KEight;
            }
        
        opacity.SetValueNow( 0.0 );
        iThirdLineIconVis->SetOpacity(opacity);
        }

///////////////////////////fourth line///////////////////////////
    if ( aData->LineIcon( KThree ) )
        {
        fourthLineIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
        fourthLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
        fourthLineTextVariety = KFive;
        }
    else
        {
        if ( IsKeepEmptyIconsPlaceHolders() )
            {
            fourthLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
            fourthLineTextVariety = KFive;
            }
        else
            {
            fourthLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPane;
            fourthLineTextVariety = KEight;
            }
        
        opacity.SetValueNow( 0.0 );
        iFourthLineIconVis->SetOpacity(opacity);
        }    

//3. get correct metrics based on text's size range    
    //not needed anymore
    //layout manager is queried for medium values - LM returnes rects based on phone's global text settings 
//4. get item size from Layout Manager - this will be used as parent in icons'/texts' size queries to LM
    
    //use full 4-line item size    
    TRect rect;    
    TInt rowVariety (KThree);//four lines
    TRect itemRect;

    itemRect = SizeWithNumberOfLines( KThree );
    
    //modify item size with list's indentation
    if ( CFsLayoutManager::IsMirrored() )
        {
        itemRect.iBr = itemRect.iBr - TPoint(aIndentation, 0);
        }
    else
        {
        itemRect.iTl = itemRect.iTl + TPoint(aIndentation, 0);
        }
    
//. Set visuals' anchors
    
    TInt iconVisualIndex(KErrNotFound), textVisualIndex(KErrNotFound);
///////////////////////////first line///////////////////////////
    TRect rowRect;
    TInt rowNumber(KZero);
    rowVariety = KThree;
    rowNumber = KZero;

    CFsLayoutManager::LayoutMetricsRect( itemRect, CFsLayoutManager::EFsLmListSingleDycRowPane, rowRect, rowVariety, rowNumber );
    
    
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            firstLineIconMetrics,
            rect,
            firstLineIconVariety );
    
    tpFirstLineCheckBoxIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFirstLineCheckBoxIconBr.SetTarget(TAlfRealPoint(rect.iBr));
        
    iconVisualIndex = iLayout->FindVisual( iCheckBoxIconVis );
    if ( iconVisualIndex != KErrNotFound )
        {
        tpFirstLineCheckBoxIconTl.SetStyle(EAlfTimedValueStyleLinear);        
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpFirstLineCheckBoxIconTl );
        
        tpFirstLineCheckBoxIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpFirstLineCheckBoxIconBr ); 
        }
    //priority icon 
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            firstLinePrioIconMetrics,
            rect,
            firstLinePrioIconVariety );
    
    tpFirstLineIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFirstLineIconBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    iconVisualIndex = iLayout->FindVisual( iFirstLineIconVis );
    if ( iconVisualIndex != KErrNotFound )
        {
        tpFirstLineIconTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft,
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpFirstLineIconTl );
        
        tpFirstLineIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight,
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpFirstLineIconBr );
        }
    
    //text
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            firstLineTextMetrics,
            rect,
            firstLineTextVariety );
    
    tpFirstLineTextTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFirstLineTextBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    textVisualIndex = iLayout->FindVisual( iFirstLineTextVis );
    if ( textVisualIndex != KErrNotFound )
        {
        tpFirstLineTextTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpFirstLineTextTl );
        
        tpFirstLineTextBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpFirstLineTextBr ); 
        }
    
    //mark icon
    TInt markVisualIndex(KErrNotFound), menuVisualIndex(KErrNotFound);
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            firstLineMarkIconMetrics,
            rect,
            firstLineMarkIconVariety );
    
    tpFirstLineMarkIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFirstLineMarkIconBr.SetTarget(TAlfRealPoint(rect.iBr));  
    
    markVisualIndex = iLayout->FindVisual( iMarkIconVis );
    if ( markVisualIndex != KErrNotFound )
        {
        tpFirstLineMarkIconTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                        markVisualIndex,
                        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                        EAlfAnchorMetricAbsolute, 
                        EAlfAnchorMetricAbsolute,
                        tpFirstLineMarkIconTl );
                
        tpFirstLineMarkIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                        markVisualIndex,
                        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                        EAlfAnchorMetricAbsolute, 
                        EAlfAnchorMetricAbsolute,
                        tpFirstLineMarkIconBr );
        }
    //menu icon
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            firstLineMenuIconMetrics,
            rect,
            firstLineMenuIconVariety );
    
    tpFirstMenuIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFirstMenuIconBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    menuVisualIndex = iLayout->FindVisual( iMenuIconVis );
    if ( menuVisualIndex != KErrNotFound )
        {
        tpFirstMenuIconTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                        menuVisualIndex,
                        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                        EAlfAnchorMetricAbsolute, 
                        EAlfAnchorMetricAbsolute,
                        tpFirstMenuIconTl );
        
        tpFirstMenuIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                        menuVisualIndex,
                        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                        EAlfAnchorMetricAbsolute, 
                        EAlfAnchorMetricAbsolute,
                        tpFirstMenuIconBr );
        }
    
    CFsLayoutManager::TFsText textInfo;
///////////////////////////second line///////////////////////////
    rowNumber = KOne;

    CFsLayoutManager::LayoutMetricsRect( itemRect, CFsLayoutManager::EFsLmListSingleDycRowPane, rowRect, rowVariety, rowNumber );
        
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            secondLineIconMetrics,
            rect,
            KZero );
    tpSecondLineIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpSecondLineIconBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    iconVisualIndex = iLayout->FindVisual( iSecondLineIconVis );
    if ( iconVisualIndex != KErrNotFound )
        {
        tpSecondLineIconTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpSecondLineIconTl );
        
        tpSecondLineIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpSecondLineIconBr ); 
        }
    
    CFsLayoutManager::LayoutMetricsRect(
                rowRect,
                secondLineTextMetrics,
                rect,
                secondLineTextVariety );
    
    tpSecondLineTextTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpSecondLineTextBr.SetTarget(TAlfRealPoint(rect.iBr));

    textVisualIndex = iLayout->FindVisual( iSecondLineTextVis );
    if ( textVisualIndex != KErrNotFound )
        {
        tpSecondLineTextTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft,
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpSecondLineTextTl );
        
        tpSecondLineTextBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpSecondLineTextBr );
        }
///////////////////////////third line///////////////////////////
    rowNumber = KTwo;

    CFsLayoutManager::LayoutMetricsRect( itemRect, CFsLayoutManager::EFsLmListSingleDycRowPane, rowRect, rowVariety, rowNumber );
    
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            thirdLineIconMetrics,
            rect );
    tpThirdLineIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpThirdLineIconBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    iconVisualIndex = iLayout->FindVisual( iThirdLineIconVis );
    if ( iconVisualIndex != KErrNotFound )
        {
        tpThirdLineIconTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpThirdLineIconTl );
        
        tpThirdLineIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpThirdLineIconBr ); 
        }
    
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            thirdLineTextMetrics,
            rect,
            thirdLineTextVariety );

    tpThirdLineTextTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpThirdLineTextBr.SetTarget(TAlfRealPoint(rect.iBr));
        
    textVisualIndex = iLayout->FindVisual( iThirdLineTextVis );
    if ( textVisualIndex != KErrNotFound )
        {
        tpThirdLineTextTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft,
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpThirdLineTextTl );
        
        tpThirdLineTextBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpThirdLineTextBr );
        }    
    
///////////////////////////fourth line///////////////////////////
    rowNumber = KThree;

    CFsLayoutManager::LayoutMetricsRect( itemRect, CFsLayoutManager::EFsLmListSingleDycRowPane, rowRect, rowVariety, rowNumber );
    
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            fourthLineIconMetrics,
            rect );
    tpFourthLineIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFourthLineIconBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    iconVisualIndex = iLayout->FindVisual( iFourthLineIconVis );
    if ( iconVisualIndex != KErrNotFound )
        {
        tpFourthLineIconTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpFourthLineIconTl );
        
        tpFourthLineIconBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight, 
                iconVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, 
                EAlfAnchorMetricAbsolute,
                tpFourthLineIconBr ); 
        }
    
    CFsLayoutManager::LayoutMetricsRect(
            rowRect,
            fourthLineTextMetrics,
            rect,
            fourthLineTextVariety );

    tpFourthLineTextTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpFourthLineTextBr.SetTarget(TAlfRealPoint(rect.iBr));
    
    textVisualIndex = iLayout->FindVisual( iFourthLineTextVis );
    if ( textVisualIndex != KErrNotFound )
        {
        tpFourthLineTextTl.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorTopLeft,
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpFourthLineTextTl );
        
        tpFourthLineTextBr.SetStyle(EAlfTimedValueStyleLinear);
        iLayout->SetAnchor( EAlfAnchorBottomRight,
                textVisualIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpFourthLineTextBr );
        }
    }


// ---------------------------------------------------------------------------
// Returns item's size in state with given number of lines. 
// ---------------------------------------------------------------------------
//
TSize CFsMultiLineItemVisualizer::SizeWithNumberOfLines( const TInt aNumberLines ) const
    {
    FUNC_LOG;
    TSize size;
    if ( aNumberLines < KOne || aNumberLines > KFour )
        {
        size.iWidth = 0;
        size.iHeight = 0;
        }
    else
        {
        /* family
        Screen                            ->EFsLmScreen
        application_window                ->EFsLmApplicationWindow
        main_pane                         ->EFsLmMainPane
        main_sp_fs_listscroll_pane_te     ->EFsLmMainSpFsListscrollPaneTe
        main_sp_fs_list_pane              ->EFsLmMainSpFsListPane
        list_single_fs_dyc_pane           ->EFsLmListSingleFsDycPane
        list_single_dyc_row_pane          ->EFsLmListSingleDycRowPane
        */
        TRect rect, rect2;
        CFsLayoutManager::LayoutMetricsRect( rect, CFsLayoutManager::EFsLmScreen, rect2 );
        CFsLayoutManager::LayoutMetricsRect( rect2, CFsLayoutManager::EFsLmApplicationWindow, rect );
        CFsLayoutManager::LayoutMetricsRect( rect, CFsLayoutManager::EFsLmMainPane, rect2 );
        CFsLayoutManager::LayoutMetricsRect( rect2, CFsLayoutManager::EFsLmMainSpFsListscrollPaneTe, rect );
        CFsLayoutManager::LayoutMetricsRect( rect, CFsLayoutManager::EFsLmMainSpFsListPane, rect2 );
        
        TInt rowVariety (KThree);//four lines
        TRect itemRect;
        
        switch ( aNumberLines )
            {
            case KOne:
                {
                rowVariety = KZero;
                break;
                }
            case KTwo:
               {
               rowVariety = KOne; 
               break;
               }
           case KThree:
               {
               rowVariety = KTwo; 
               break;
               }
           case KFour:
           default:
               {
               rowVariety = KThree;
               break;
               }
            }
        
        CFsLayoutManager::LayoutMetricsRect( rect2, CFsLayoutManager::EFsLmListSingleFsDycPane, itemRect, rowVariety );
        
        size = itemRect.Size();
        }
    
    return size;
    }

// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  This method sets flag disable wrapping the text item.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemVisualizer::OffWrapping()
	{
	FUNC_LOG;
	}

//End of file

