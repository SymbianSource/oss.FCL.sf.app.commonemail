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
* Description:  Expand area class
*
*/


#ifndef C_FSTEXTVIEWERSELECTEMBED_H
#define C_FSTEXTVIEWERSELECTEMBED_H

//#include <fsconfig.h>

#include "fstextviewerselect.h"

class CAlfLayout;

class CFsTextViewerSelectEmbed : public CFsTextViewerSelect
    {
    public:
        static CFsTextViewerSelectEmbed* NewL( 
                CFsTextViewerSelectsManager* aManager );
        void SelectL();
        void DeselectL();
        void SelectedL();
    protected:
        CFsTextViewerSelectEmbed( CFsTextViewerSelectsManager* aManager );

    protected:
        CAlfLayout* iLayout;
        TBool iIsExpanded;
        TSize iLayoutSize;
    };

#endif //C_FSTEXTVIEWERSELECTEMBED_H

