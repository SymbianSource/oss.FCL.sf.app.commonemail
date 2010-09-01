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
* Description:  Observer for FsTextViewer class
*
*/


#ifndef M_FSTEXTVIEWEROBSERVER_H
#define M_FSTEXTVIEWEROBSERVER_H


NONSHARABLE_CLASS( MFsTextViewerObserver )
    {
public:
    /** events sent to observers */
    enum TFsTextViewerEvent
        {
        EFsTextViewerHotspotClicked = 0,
        EFsTextViewerHotspotChanged,
        EFsTextViewerHotspotLongTap,
        EFsTextViewerMarkStart,
        EFsTextViewerMarkChange,
        EFsTextViewerMarkEnd,
        EFsTextViewerScrollUp,
        EFsTextViewerScrollDown
        };
    
public:

    /**
    * C++ destructor
    */

    virtual ~MFsTextViewerObserver() {};
    

    /**
    * Invoked when event occurs.
    *
    * @param aEvent event type.
    */
    virtual void HandleTextViewerEventL( TFsTextViewerEvent aEvent ) = 0;
    };

#endif // M_FSTEXTVIEWEROBSERVER_H


