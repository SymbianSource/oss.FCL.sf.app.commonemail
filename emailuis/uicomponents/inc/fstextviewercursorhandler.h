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
* Description: This file defines class CFsTextViewerCursorHandler.
*
*/


#ifndef C_FSTEXTVIEWERCURSORHANDLER_H
#define C_FSTEXTVIEWERCURSORHANDLER_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>

class CAlfVisual;
class CFsTextViewerVisualizerData;

class CFsTextViewerCursorHandler : public CBase
    {
    public:
        CFsTextViewerCursorHandler* NewL();
        ~CFsTextViewerCursorHandler;

    public:
        MoveLeft();
        MoveRight();
        Show();
        Hide();

    private:
        void ConstructL();
        CFsTextViewerCursorHandler();

    private:
        TInt iCurrentVisual;
        TInt iVisualOffset;
        CAlfVisual* iCursor;
        CFsTextViewerVisualizerData* iVisStore;
    }

#endif //C_FSTEXTVIEWERCURSORHANDLER_H
