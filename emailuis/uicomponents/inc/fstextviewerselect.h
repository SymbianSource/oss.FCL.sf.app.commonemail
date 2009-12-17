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
* Description:  Base class for selectables (hotspots and expand areas)
*
*/


#ifndef C_FSTEXTVIEWERSELECT_H
#define C_FSTEXTVIEWERSELECT_H

#include <e32base.h>

class CFsTextViewerSelectsManager;
class TFsRangedVisual;

class CFsTextViewerSelect : public CBase
    {
    public:
        enum TFsTextViewerSelectType
            {
            EFsHotspotType,
            EFsLineType,
            EFsEmbedType
            }; 

    public:
        virtual ~CFsTextViewerSelect();
        void SetType( TFsTextViewerSelectType aType );
        TFsTextViewerSelectType GetType();
        void SetId( TInt aId );
        TInt GetId() const;
        void AppendVisualL( TFsRangedVisual* aVisual );
        virtual void SelectL() = 0;
        virtual void DeselectL() = 0;
        virtual void SelectedL() = 0;
        void RemoveVisual( TFsRangedVisual* aVisual );
        TInt GetStartLine();
        TInt GetStartHeight();
        TBool IsVisible();

    public:
        TRect iRect;
        TInt iEndChar;

    protected:
        void ConstructL();

    protected:
        TFsTextViewerSelectType iType;
        TInt iId;
        RPointerArray<TFsRangedVisual> iVisuals;
        CFsTextViewerSelectsManager* iManager;
    };

#endif //C_FSTEXTVIEWERSELECT_H
