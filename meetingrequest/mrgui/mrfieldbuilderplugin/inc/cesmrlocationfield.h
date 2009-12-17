/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR location field for CESMRListComponent
 *
*/


#ifndef CESMRLOCATIONFIELD_H
#define CESMRLOCATIONFIELD_H

#include <e32base.h>
#include <eikedwob.h>//MEikEdwinSizeObserver
#include "cesmriconfield.h"

//Forward declarations
class CESMREditor;
class CAknsFrameBackgroundControlContext;
class MAknsControlContext;
class CAknsFrameBackgroundControlContext;
class CESMRFeatureSettings;
class CFbsBitmap;

/**
 * This class is a custom field control that shows the location of calendar events
 */
NONSHARABLE_CLASS( CESMRLocationField ): public CESMRIconField, 
                                         public MEikEdwinSizeObserver,
                                         public MEikEdwinObserver
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

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TInt ExpandedHeight() const;
    void GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower);
    void ListObserverSet();
    void ExecuteGenericCommandL( TInt aCommand );
    TSize BorderSize() const;
    
public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aSize );

public: // From MEikEdwinObserver
	void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);
    
public: // From CCoeControl
    void ActivateL();
    void PositionChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    
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
    void PositionFieldElements();
    void LayoutWaypointIcon();
    
    // From MESMRFieldEventObserver
    void HandleFieldEventL( const MESMRFieldEvent& aEvent );
    TSize CalculateEditorSize( const TSize& aSize );
    
private:
    // Not owned. Location editor.
    CESMREditor* iLocation;
    // Field size.
    TSize iSize;
    // Background control context. not owned
    MAknsControlContext* iBackground;
    // Actual background for the editor. owned
    CAknsFrameBackgroundControlContext* iFrameBgContext;
    // Feature settings. own.
    CESMRFeatureSettings* iFeatures;
    // waypointicon bitmap for coordinates. Own
    CFbsBitmap* iWaypointIcon;
    // waypointicon bitmap for coordinates. Own
    CFbsBitmap* iWaypointIconMask;
    // waypointicon icon for coordinates. Own
    CEikImage* iIcon;
    // Location text. Own.
    HBufC* iLocationText;
    };

#endif  // CESMRLOCATIONFIELD_H

