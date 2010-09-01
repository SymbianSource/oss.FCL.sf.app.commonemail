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
* Description:  Helper class for TextViewer visualizer
*
*/


#ifndef C_FSTEXTVIEWERCOORDINATOR
#define C_FSTEXTVIEWERCOORDINATOR

#include <e32base.h>
#include <biditext.h>

class CFsTextViewerCoordinator : public CBase
    {
    public:
        static CFsTextViewerCoordinator* NewL ();
        ~CFsTextViewerCoordinator();

    public:
        void RegisterObstacleL( const TRect& aRect );
        TBool Contains( const TRect aRect );
        void SetSize( const TSize& aSize );
        TInt NextFreeCol() const;
        TBool NextFreeCol( TRect& aRect, TBidiText::TDirectionality aDirection ) const;
        TBool IsNewLine() const;
        void SetNewLine();
        void RemoveRect( TRect aRect );
        void RemoveFromOffset( TInt aOffset );
        void Reset();
        void SetLeftMargin( TInt aMargin );
        void SetRightMargin( TInt aMargin );
        
    private:
        CFsTextViewerCoordinator();
        void ConstructL();

    private:
        CArrayPtrSeg<TRect>* iObstacles;
        TSize iSize;
        TInt iLastObstacle;
        TInt iLeftMargin;
        TInt iRightMargin;
    };

#endif // C_FSTEXTVIEWERCOORDINATOR

