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
* Description: This file defines class CFsTextViewerSelectHotspot.
*
*/


#ifndef C_FSTEXTVIEWERSELECTHOTSPOT_H
#define C_FSTEXTVIEWERSELECTHOTSPOT_H

#include "fstextviewerselect.h"

class CFsTextViewerSelectHotspot : public CFsTextViewerSelect
    {
    public:
        static CFsTextViewerSelectHotspot* NewL( 
                CFsTextViewerSelectsManager* aManager );
        void SelectL();
        void DeselectL();
        void SelectedL();

    protected:
        CFsTextViewerSelectHotspot( CFsTextViewerSelectsManager* aManager );
    };

#endif //C_FSTEXTVIEWERSELECTHOTSPOT_H
