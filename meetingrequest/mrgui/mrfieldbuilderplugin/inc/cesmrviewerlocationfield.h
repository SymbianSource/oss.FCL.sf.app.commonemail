/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Meeting request location field
 *
*/


#ifndef CESMRVIEWERLOCATIONFIELD_H_
#define CESMRVIEWERLOCATIONFIELD_H_

// INCLUDES
#include <e32base.h>
#include <eikedwob.h>
#include <eikmobs.h>
#include <coecobs.h>

#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CESMRRichTextViewer;
class CAknButton;
class CESMRFeatureSettings;

/**
 * Shows the location of the meeting/event
 */
NONSHARABLE_CLASS( CESMRViewerLocationField ): public CESMRField,
                                                      public MEikEdwinSizeObserver,
                                                      public MCoeControlObserver
    {
    public:
        /**
         * Creates new CESMRViewerLocationField object. Ownership
         * is transferred to caller.
         * @return Pointer to created object,
         */
        static CESMRViewerLocationField* NewL( );


        /**
         * C++ Destructor.
         */
        ~CESMRViewerLocationField( );

    protected:// From CESMRField
        void InternalizeL( MESMRCalEntry& aEntry );
        TSize MinimumSize();
        void InitializeL();
        void ListObserverSet();
        TBool ExecuteGenericCommandL( TInt aCommand );
        void SetOutlineFocusL( TBool aFocus );
        void HandleLongtapEventL( const TPoint& aPosition );
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        void LockL();
        TBool SupportsLongTapFunctionalityL(
            		 const TPointerEvent &aPointerEvent );

    protected:
		TBool HandleSingletapEventL( const TPoint& aPosition );

    protected: // From CCoeControl
        TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType );
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aInd ) const;
        void SizeChanged();
        void GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower);

    protected: // From MEikEdwinSizeObserver
        TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType,
                TSize aSize );

    protected: // From MCoeControlObserver
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    private: // Implementation
        CESMRViewerLocationField( );
        void ConstructL( );
        void SetWaypointIconL( TBool aEnabled );
        TBool HandleTapEventL( const TPoint& aPosition );

    private: // data
        // Own: Edwin size
        TSize iSize;
        /// Own: Field button
        CAknButton* iFieldButton;
        /// Ref: Field text label
        CESMRRichTextViewer* iRichTextViewer;
        /// Own: Waypoint icon
        CMRImage* iWaypointIcon;
        /// Own: Lock icon
        CMRImage* iLockIcon;
        /// Own: RichTextViewer line count.
        TInt iLineCount;
        /// Own: Middle softkey command id
        TInt iMskCommandId;
        /// Own: Feature settings
        CESMRFeatureSettings* iFeatures;
    };

#endif /*CESMRVIEWERLOCATIONFIELD_H_*/
