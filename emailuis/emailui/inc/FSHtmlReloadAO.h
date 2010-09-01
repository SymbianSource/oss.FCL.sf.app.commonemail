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
* Description: This file defines class CFSHtmlReloadAO.
*
*/

#ifndef FSHtmlReloadAO_H
#define FSHtmlReloadAO_H

#include <e32base.h>

class CFsEmailUiHtmlViewerView;
class CFSHtmlReloadAO : public CActive
    {
public:
    static CFSHtmlReloadAO* NewL( CFsEmailUiHtmlViewerView& aView );
    ~CFSHtmlReloadAO();
    void ReloadPageAysnc();
private:
    CFSHtmlReloadAO( CFsEmailUiHtmlViewerView& aView );

    void ConstructL();
    void RunL();
    void DoCancel();

private:
    CFsEmailUiHtmlViewerView&       iView;
    };

#endif /*FSHtmlReloadAO_H*/
