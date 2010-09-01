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


#ifndef C_FSTEXTVIEWERSELECTSMANAGER_H
#define C_FSTEXTVIEWERSELECTSMANAGER_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>

#include "fstextviewerselect.h"

class CAlfVisual;
class CFsTextViewerVisualizer;
class CAlfTexture;

class CFsTextViewerSelectsManager : public CBase
    {
    public:
        static CFsTextViewerSelectsManager* NewL( 
             CFsTextViewerVisualizer* aOwner );
        ~CFsTextViewerSelectsManager();

    public:
        void AppendSelectVisL( TFsRangedVisual* aVisual, TInt aId, 
             CFsTextViewerSelect::TFsTextViewerSelectType aType );
        CFsTextViewerVisualizer* Visualizer();
        TBool MoveNextL( TBool& aChanged );
        TBool MovePrevL( TBool& aChanged );
        void RemoveVisual( TFsRangedVisual* aVisual );
        void ClickedL();
        TBool IsHotspotHighlighted();
        TInt GetCurrentSelectId();
        // <cmail>
        TInt GetCurrentSelectedIndex();
        // </cmail>
        void DimAllL();
        void RefreshSelectionL();
        void SelectL( TInt aId );
        void SelectByCharL( TInt aIndex );
        void SelectByIndexL( TInt aIndex );
        TSize GetDisplaySize();
        TBool IsCurrentHotspotVisible();
        TInt GetCurrentHotspotLine();
        void HideActionButtonFromCurrentHighlightedHotspot();
        void ShowActionButtonForCurrentHighlightedHotspotL();
        static TInt Compare( 
             const CFsTextViewerSelect& aFirst, 
             const CFsTextViewerSelect& aSecond );

    public:
        CAlfTexture* iHighlightLine;
        TReal32 iHighlightLineOpacity;
        CAlfTexture* iActionButton;
        TReal32 iActionButtonOpacity;
        TReal32 iActionButtonMargin;
        TReal32 iActionButtonWidth;
        TReal32 iActionButtonHeight;
        CAlfControl* iControl;

    private:
        void ConstructL();
        CFsTextViewerSelectsManager( CFsTextViewerVisualizer* aOwner );
    
    private:
        RPointerArray<CFsTextViewerSelect> iSelects;
        TInt iCurrent;
        CFsTextViewerVisualizer* iOwner;
        TSize iDisplaySize;
    };

#endif //C_FSTEXTVIEWERSELECTSMANAGER_H
