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
* Description:  Whole line highlight class
*
*/


#ifndef C_FSTEXTVIEWERSELECTLINE_H
#define C_FSTEXTVIEWERSELECTLINE_H

//#include <fsconfig.h> // <cmail>
#include "fstextviewerselect.h"

class CAlfLayout;
class CAlfImageVisual;

class CFsTextViewerSelectLine : public CFsTextViewerSelect
    {
    public:
        static CFsTextViewerSelectLine* NewL( 
                CFsTextViewerSelectsManager* aManager );
        void SelectL();
        void DeselectL();
        void SelectedL();
        void HideActionButton();
        void ShowActionButtonL();
        
    public:
        ~CFsTextViewerSelectLine();
        
    protected:
        CFsTextViewerSelectLine( CFsTextViewerSelectsManager* aManager );

    private:
        CAlfLayout* iLayout;
        CAlfImageVisual* iActionButton;
        
        RArray<CAlfImageVisual*> iHighlightLines;
        TBool iIsShown;
        TBool iIsActionButton;
    };

#endif //C_FSTEXTVIEWERSELECTLINE_H
