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
* Description:  Manager for selectable elements (hotspots and expand areas)
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfvisual.h>
// </cmail>
#include "fstextviewerselectsmanager.h"
#include "fstextviewerselecthotspot.h"
#include "fstextviewerselectembed.h"
#include "fstextviewerselectline.h"
#include "fstextviewervisualizer.h"
#include "fstextviewervisualizersettings.h"
#include "fstextviewervisualizerdata.h"
#include "fstextparser.h"


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerSelectsManager* CFsTextViewerSelectsManager::NewL( 
        CFsTextViewerVisualizer* aOwner )
    {
    FUNC_LOG;
    CFsTextViewerSelectsManager* self = 
        new (ELeave) CFsTextViewerSelectsManager( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerSelectsManager
// ---------------------------------------------------------------------------
CFsTextViewerSelectsManager::~CFsTextViewerSelectsManager()
    {
    FUNC_LOG;
    iOwner = NULL;
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        delete iSelects[i];
        }
    iSelects.Close();
    }

// ---------------------------------------------------------------------------
// AppendSelectVisL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::AppendSelectVisL( TFsRangedVisual* aVisual, 
        TInt aId,
        CFsTextViewerSelect::TFsTextViewerSelectType aType )
    {
    FUNC_LOG;
    TBool newNeeded = ETrue;
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        if ( iSelects[i]->GetId() == aId 
                && iSelects[i]->GetType() == aType )
            {
            iSelects[i]->AppendVisualL( aVisual );
            newNeeded = EFalse;
            }
        }

    if ( newNeeded )
        {
        CFsTextViewerSelect* select = NULL;
        switch ( aType )
            {
            case CFsTextViewerSelect::EFsHotspotType:
                {
                select = CFsTextViewerSelectHotspot::NewL( this );
                break;
                }
            case CFsTextViewerSelect::EFsEmbedType:
                {
                select = CFsTextViewerSelectEmbed::NewL( this );
                break;
                }
            case CFsTextViewerSelect::EFsLineType:
                {
                select = CFsTextViewerSelectLine::NewL( this );
                break;
                }
            default:
                {
                break;
                }

            }
            select->SetId( aId );
            select->AppendVisualL( aVisual );
            iSelects.AppendL( select );
        }
    else
        {
        }

    TLinearOrder<CFsTextViewerSelect> order( Compare );
    iSelects.Sort( order );
    }

// ---------------------------------------------------------------------------
// Visualizer
// ---------------------------------------------------------------------------
CFsTextViewerVisualizer* CFsTextViewerSelectsManager::Visualizer()
    {
    FUNC_LOG;
    return iOwner;
    }

// ---------------------------------------------------------------------------
// MoveNext
// ---------------------------------------------------------------------------
TBool CFsTextViewerSelectsManager::MoveNextL( TBool& aChanged )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    
    if ( iCurrent >= 0 && iCurrent < iSelects.Count() )
        {
        iSelects[iCurrent]->DeselectL();
        }
    
    if ( iCurrent < 0 && iSelects.Count() > 0 )
        {
        iCurrent = 0;
        if ( iSelects[0]->IsVisible() )
			{
            retVal = ETrue;
            }
        }
    else if ( iSelects.Count() > 0 ) 
        {
        for ( TInt i = iCurrent + 1; i < iSelects.Count(); ++i )
            {
            if ( iSelects[i]->IsVisible() && !retVal )
                {
                iCurrent = i;
                retVal = ETrue;
                }
            }
        }
        
    if ( !retVal && iCurrent >= 0 && iCurrent < iSelects.Count() && !iSelects[iCurrent]->IsVisible() )
        {
        for ( TInt i = 0; i < iCurrent; ++i )
            {
            if ( iSelects[i]->IsVisible() && !retVal )
                {
                iCurrent = i;
                retVal = ETrue;
                }
            }
        }
        
    if ( iCurrent >= 0 && iCurrent < iSelects.Count() && iSelects[iCurrent]->IsVisible() )
        {
        iSelects[iCurrent]->SelectL();
        }
        
    aChanged = retVal;
    if ( iCurrent >= 0 
            && iCurrent < iSelects.Count() 
            && iSelects[iCurrent]->GetStartLine() >= iOwner->Navigator()->GetFirstVisible()->iLineNumber + 4 
            && aChanged )
        {
        retVal = EFalse;
        }
    else
        {
        
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// MovePrev
// ---------------------------------------------------------------------------
TBool CFsTextViewerSelectsManager::MovePrevL( TBool& aChanged )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    
    if ( iCurrent >= 0 && iCurrent < iSelects.Count() )
        {
        iSelects[iCurrent]->DeselectL();
        }
    
    if ( iCurrent < 0 && iSelects.Count() > 0 )
        {
        iCurrent = 0;
        if ( iSelects[0]->IsVisible() )
        	{
            retVal = ETrue;
            }
        }
    else if ( iSelects.Count() > 0 )
        {
        for ( TInt i = iCurrent - 1; i >= 0; --i )
            {
            if ( iSelects[i]->IsVisible() && !retVal )
                {
                iCurrent = i;
                retVal = ETrue;
                }
            }
        }
    
    if ( !retVal && iCurrent >= 0 && iCurrent < iSelects.Count() && !iSelects[iCurrent]->IsVisible() )
        {
        for ( TInt i = iSelects.Count() - 1; i > iCurrent; --i )
            {
            if ( iSelects[i]->IsVisible() && !retVal )
                {
                iCurrent = i;
                retVal = ETrue;
                }
            }
        }
    
    if ( iCurrent >= 0 && iCurrent < iSelects.Count() && iSelects[iCurrent]->IsVisible() )
        {
        iSelects[iCurrent]->SelectL();
        }
    
    if ( iCurrent >= 0 && iCurrent < iSelects.Count() 
            && iSelects[iCurrent]->GetStartLine() <= iOwner->Navigator()->GetLastVisible()->iLineNumber - 4 && aChanged )
        {
        retVal = EFalse;
        }
    else
        {
        
        }
    
    aChanged = retVal;
    return retVal;
    }

// ---------------------------------------------------------------------------
// RemoveVisual
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::RemoveVisual( TFsRangedVisual* aVisual )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        iSelects[i]->RemoveVisual( aVisual );
        }
    }

// ---------------------------------------------------------------------------
// Clicked
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::ClickedL()
    {
    FUNC_LOG;
    if ( iCurrent >= 0 && iCurrent < iSelects.Count() )
        {
        iSelects[iCurrent]->SelectedL();
        }
    }

// ---------------------------------------------------------------------------
// IsHotspotHighlighted
// ---------------------------------------------------------------------------
TBool CFsTextViewerSelectsManager::IsHotspotHighlighted()
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iCurrent >= 0 && 
            iSelects.Count() > 0 
            && ( iSelects[iCurrent]->GetType() 
                == CFsTextViewerSelect::EFsHotspotType 
            || iSelects[iCurrent]->GetType() 
                == CFsTextViewerSelect::EFsLineType ) )
        {
        retVal = ETrue;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// GetCurrentSelectId
// ---------------------------------------------------------------------------
TInt CFsTextViewerSelectsManager::GetCurrentSelectId()
    {
    FUNC_LOG;
    TInt retVal = -1;

    if ( iCurrent >= 0 && 
            iSelects.Count() > 0 
            && ( iSelects[iCurrent]->GetType() 
                == CFsTextViewerSelect::EFsHotspotType
            || iSelects[iCurrent]->GetType() 
                == CFsTextViewerSelect::EFsLineType ) )
        {
        retVal = iSelects[iCurrent]->GetId();
        }

    return retVal;
    }

// <cmail>
TInt CFsTextViewerSelectsManager::GetCurrentSelectedIndex()
    {
    return iCurrent;
    }
// </cmail>

// ---------------------------------------------------------------------------
// DimAll
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::DimAllL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        if ( i != iCurrent 
           && iSelects[i]->GetType() != CFsTextViewerSelect::EFsLineType )
            {
            if ( iSelects[i]->IsVisible() )
                {
                iSelects[i]->DeselectL();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// RefreshSelection
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::RefreshSelectionL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        if ( i != iCurrent 
           && iSelects[i]->GetType() != CFsTextViewerSelect::EFsLineType )
            {
            iSelects[i]->DeselectL();
            }
        }
    if ( iCurrent >= 0 )
        {
        iSelects[iCurrent]->SelectL();
        }
    }


// ---------------------------------------------------------------------------
// SelectL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::SelectL( TInt aId )
    {
    FUNC_LOG;
    CFsTextViewerSelect* hotspotFound = NULL;
    TInt arrayIndex = 0;
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        if ( iSelects[i]->GetId() == aId )
            {
            hotspotFound = iSelects[i];
            arrayIndex = i;
            }
        }
    if ( hotspotFound )
        {
        if ( iCurrent >= 0 )
            {
            iSelects[iCurrent]->DeselectL();
            }       
        iCurrent = arrayIndex;
        if ( iSelects[iCurrent]->IsVisible() )
            {
            iSelects[iCurrent]->SelectL();
            }
        }
    }

// ---------------------------------------------------------------------------
// SelectByCharL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::SelectByCharL( TInt aIndex )
    {
    FUNC_LOG;
    CFsTextViewerSelect* hotspotFound = NULL;
    TInt arrayIndex = 0;
    
    for ( TInt i = 0; i < iSelects.Count(); ++i )
        {
        if ( iOwner->Parser()->GetStartIndexOfHotSpotAreaL( iSelects[i]->GetId(), ETrue ) <= aIndex && 
                iOwner->Parser()->GetEndIndexOfHotSpotAreaL( iSelects[i]->GetId(), ETrue ) >= aIndex )
            {
            hotspotFound = iSelects[i];
            arrayIndex = i;
            }
        }
    
    if ( hotspotFound )
        {
        if ( iCurrent >= 0 )
            {
            iSelects[iCurrent]->DeselectL();
            }       
        iCurrent = arrayIndex;
        if ( iSelects[iCurrent]->IsVisible() )
            {
            iSelects[iCurrent]->SelectL();
            }
        }
    }

// ---------------------------------------------------------------------------
// SelectByIndexL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::SelectByIndexL( TInt aIndex )
    {
    FUNC_LOG;
    if ( aIndex >= 0 && aIndex < iSelects.Count() )
        {
        if ( iCurrent >= 0 && iCurrent < iSelects.Count() )
            {
            if ( iSelects[iCurrent]->IsVisible() )
                {
                iSelects[iCurrent]->DeselectL();
                }
            }
        iCurrent = aIndex;
        if ( iSelects[iCurrent]->IsVisible() )
            {
            iSelects[iCurrent]->SelectL();
            }
        }
    }

// ---------------------------------------------------------------------------
// GetDisplaySize
// ---------------------------------------------------------------------------
TSize CFsTextViewerSelectsManager::GetDisplaySize()
    {
    FUNC_LOG;
    return iDisplaySize;
    }

// ---------------------------------------------------------------------------
// IsCurrentHotspotVisible
// ---------------------------------------------------------------------------
TBool CFsTextViewerSelectsManager::IsCurrentHotspotVisible()
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iSelects.Count() > 0 && iCurrent >=0 )
        {
        retVal = iSelects[iCurrent]->IsVisible();
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// GetCurrentHotspotLine
// ---------------------------------------------------------------------------
TInt CFsTextViewerSelectsManager::GetCurrentHotspotLine()
    {
    FUNC_LOG;
    TInt retVal = -1;
    if ( iSelects.Count() > 0 && iCurrent >=0 )
        {
        retVal = iSelects[iCurrent]->GetStartLine();
        }
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// HideActionButtonFromCurrentHighlightedHotspot
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager
::HideActionButtonFromCurrentHighlightedHotspot()
    {
    FUNC_LOG;
    if ( iCurrent >= 0 && 
         iSelects.Count() > 0 &&
         iSelects[iCurrent]->GetType() == CFsTextViewerSelect::EFsLineType)
        {        
        static_cast<CFsTextViewerSelectLine*>(iSelects[iCurrent])
            ->HideActionButton();
        }
    }

// ---------------------------------------------------------------------------
// ShowActionButtonForCurrentHighlightedHotspotL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager
::ShowActionButtonForCurrentHighlightedHotspotL()
    {
    FUNC_LOG;
    if ( iCurrent >= 0 && 
         iSelects.Count() > 0 &&
         iSelects[iCurrent]->GetType() == CFsTextViewerSelect::EFsLineType)
        {        
        static_cast<CFsTextViewerSelectLine*>
            (iSelects[iCurrent])->ShowActionButtonL();
        }    
    }

// ---------------------------------------------------------------------------
// Compare
// ---------------------------------------------------------------------------
TInt CFsTextViewerSelectsManager::Compare( 
        const CFsTextViewerSelect& aFirst, 
        const CFsTextViewerSelect& aSecond )
    {
    FUNC_LOG;
//    if ( aFirst.iRect.iTl.iY > aSecond.iRect.iTl.iY )
//        {
//        return 1;
//        }
//    else if ( aFirst.iRect.iTl.iY == aSecond.iRect.iTl.iY )
//        {
//        if ( aFirst.iRect.iTl.iX > aSecond.iRect.iTl.iX )
//            {
//            return 1;
//            }
//        else if ( aFirst.iRect.iTl.iX == aSecond.iRect.iTl.iX )
//            {
//            return 0;
//            }
//        else 
//            {
//            return -1;
//            }
//        }
    if ( aFirst.GetId() > aSecond.GetId() )
            {
            return 1;
            }
    else if ( aFirst.GetId() == aSecond.GetId() )
            {
            return 0;
            }
        else 
            {
            return -1;
            }
        }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerSelectsManager::ConstructL()
    {
    FUNC_LOG;
    iCurrent = -1;
    iHighlightLine = iOwner->iSettings->iHighlightLine;
    iHighlightLineOpacity = iOwner->iSettings->iHighlightLineOpacity;
    iActionButton = iOwner->iSettings->iActionButton;
    iActionButtonOpacity = iOwner->iSettings->iActionButtonOpacity;
    iActionButtonMargin = iOwner->iSettings->iActionButtonMargin;
    iActionButtonWidth = iOwner->iSettings->iActionButtonWidth;
    iActionButtonHeight = iOwner->iSettings->iActionButtonHeight;
    iDisplaySize = iOwner->iDisplaySize;
    iControl = iOwner->Control();
    }

// ---------------------------------------------------------------------------
// CFsTextViewerSelectsManager
// ---------------------------------------------------------------------------
CFsTextViewerSelectsManager::CFsTextViewerSelectsManager( 
        CFsTextViewerVisualizer* aOwner )
    {
    FUNC_LOG;
    iOwner = aOwner;
    }

