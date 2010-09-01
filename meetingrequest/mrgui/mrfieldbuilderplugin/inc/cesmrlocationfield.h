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
* Description:  ESMR location field.
 *
*/


#ifndef CESMRLOCATIONFIELD_H
#define CESMRLOCATIONFIELD_H

#include <e32base.h>
#include <eikedwob.h>   // MEikEdwinSizeObserver
#include <eikmobs.h>    // MEikMenuObserver

#include "cesmrfield.h"

//Forward declarations
class CESMREditor;
class CESMRFeatureSettings;
class CFbsBitmap;
class CMRImage;
class CAknsBasicBackgroundControlContext;
class CMRStylusPopupMenu;
class CMRLabel;
class CMRButton;

/**
 * This class is a custom field control that shows the location of calendar events
 */
NONSHARABLE_CLASS( CESMRLocationField ): public CESMRField, 
                                         public MEikEdwinSizeObserver,
                                         public MEikEdwinObserver,
                                         public MEikMenuObserver
    {
    public:
        /**
         * Creates new CESMRLocationField object. Ownership
         * is transferred to caller.
         * @return Pointer to created object,
         */
        static CESMRLocationField* NewL();
    
        /**
         * C++ Destructor.
         */
        ~CESMRLocationField();
    
    protected: // From CESMRField
        TSize MinimumSize();
        void InitializeL();
        void InternalizeL( MESMRCalEntry& aEntry );
        void ExternalizeL( MESMRCalEntry& aEntry );
        void SetOutlineFocusL( TBool aFocus );
        void GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower);
        void ListObserverSet();
        TBool ExecuteGenericCommandL( TInt aCommand );
        TBool HandleSingletapEventL( const TPoint& aPosition );
        void HandleLongtapEventL( const TPoint& aPosition );
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    protected: // From MEikEdwinSizeObserver
        TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aSize );
    
    protected: // From MEikEdwinObserver
        void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);

    protected: // From MEikMenuObserver
        void ProcessCommandL( TInt aCommandId );
        void SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/){}

    protected: // From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aInd ) const;
        void SizeChanged();
        void SetContainerWindowL( const CCoeControl& aContainer );
        TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent,
                                           TEventCode aType );
        TBool HandleRawPointerEventL( const TPointerEvent &aPointerEvent );

    private:
        /**
         * Constructor.
         */
        CESMRLocationField();
    
        /**
         * Second phase constructor.
         */
        void ConstructL();
    
        CESMRFeatureSettings& FeaturesL();
        void SetWaypointIconL( TBool aEnabled );

        // From MESMRFieldEventObserver
        void HandleFieldEventL( const MESMRFieldEvent& aEvent );

    private:
        // Own: Title of this field
        CMRLabel* iTitle;
        // Not owned. Location editor.
        CESMREditor* iLocation;
        // Own: Edwin size
        TSize iSize;
        // Feature settings. own.
        CESMRFeatureSettings* iFeatures;
        // Location text. Own.
        HBufC* iLocationText;
        // Own:Field left button
        CMRButton* iFieldButton;
        // Own: Waypoint icon
        CMRImage* iWaypointIcon;
        // Field editor line count
        TInt iLineCount;
        // Own: Background for the editor when it is focused
        CAknsBasicBackgroundControlContext* iBgControlContext;
        // Field rect for comparison purposes
        TRect iFieldRect;
        /// Own: Stylus popup menu
        CMRStylusPopupMenu* iMenu; TBool iMenuIsShown;
    };

#endif  // CESMRLOCATIONFIELD_H

