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
* Description:  Responsible for marking capabilities
*
*/


#ifndef C_FSTEXTVIEWERMARKFACILITY_H
#define C_FSTEXTVIEWERMARKFACILITY_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstextviewervisualizerdata.h"

class CFsTextViewerVisualizer;
class CAlfVisual;
class CAlfControl;
class CAlfBorderBrush;
class CFsTextViewerSkinData;

class CFsTextViewerMarkFacility : public CBase
    {
    public:
        static CFsTextViewerMarkFacility* NewL( 
                CAlfLayout* aLayout, 
                CAlfControl* aControl, 
                CFsTextViewerVisualizerData* aNavigator,
                CFsTextViewerSkinData* aSkinData );
        ~CFsTextViewerMarkFacility();
        void SetHeadOffsetL( TInt aHead );
        void SetTailOffsetL( TInt aTail );
        void FollowCursorL();
        void SetLayout( CAlfLayout* aLayout );
        void GetRange( TInt& aBegin, TInt& aEnd ) const;

    protected:
        CAlfVisual* CreateMarkVisualL( 
                CAlfVisual* aVisual, 
                TInt aBegin, TInt aEnd );
        void UpdateImageVisualL( 
                CAlfVisual* aVisual, 
                CAlfVisual* aBaseVisual );
        void UpdateTextVisualL( CAlfVisual* aVisual, 
                                CAlfVisual* aBaseVisual, 
                                TInt aBegin, TInt aEnd );

    private:
        void ConstructL();
        CFsTextViewerMarkFacility( 
                CAlfLayout* aLayout, 
                CAlfControl* aControl,
                CFsTextViewerVisualizerData* aNavigator,
                CFsTextViewerSkinData* aSkinData );

    private:
        TInt VisualLength( CAlfVisual* aVisual );
        TInt Find( CAlfVisual* aVisual );
        CAlfVisual* Find( TInt aOffset );
        TInt FindIndex( TInt aOffset );
        TInt LocalVisOffset( TInt aGlobalOffset );
        void DeleteUnused();
        TBool IsHeadVisual( TInt aGlobalOffset );

    private:
        CFsTextViewerVisualizerData* iNavigator;
        TInt iHead;
        TInt iTail;
        CAlfLayout* iLayout;
        CAlfControl* iControl;
        RArray<TFsRangedVisual> iVisuals;
        CAlfBorderBrush* iBorder;
        CFsTextViewerSkinData* iSkinData;

    };

#endif //C_FSTEXTVIEWERMARKFACILITY_H
