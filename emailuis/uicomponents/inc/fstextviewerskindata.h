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
* Description: This file defines class CFsTextViewerSkinData.
*
*/


#ifndef C_FSTEXTVIEWERSKINDATA_H
#define C_FSTEXTVIEWERSKINDATA_H

#include <e32base.h>

class CFsTextViewerSkinData : public CBase
    {
    public:
        static CFsTextViewerSkinData* NewL();
        ~CFsTextViewerSkinData();

    public:
        TRgb iCursorColor;
        TRgb iMarkedTextBg;
        TRgb iMarkedTextColor;
        TRgb iHotspotSelectedColor;
        TRgb iHotspotSelectedBg;
        TRgb iHotspotDimmedColor;
        TRgb iHotspotDimmedBg;

    private:
        CFsTextViewerSkinData();
        void ConstructL();
    };

#endif //C_FSTEXTVIEWERSKINDATA_H
