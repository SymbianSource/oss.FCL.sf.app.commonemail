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
* Description:  ESMR Description field for CESMRListComponent
 *
*/


#ifndef CESMRDESCRIPTIONFIELD_H
#define CESMRDESCRIPTIONFIELD_H

#include <eikedwob.h>
#include <frmtlay.h>
#include "cesmrfield.h"

class CESMREditor;
class CCalEntry;
class CAknsFrameBackgroundControlContext;
class CESMRRichTextLink;
class CESMRFeatureSettings;

/**
 * This class is a custom field control that shows the description of calendar events
 */
NONSHARABLE_CLASS( CESMRDescriptionField ): public CESMRField, 
											public MEikEdwinSizeObserver,
											public MEikEdwinObserver
    {
public:
    /**
     * Creates new CESMRDescriptionField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRDescriptionField* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRDescriptionField();

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TInt ExpandedHeight() const;
    TInt GetVerticalFocusPosition();
    void GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower);
    void ListObserverSet();
    void ExecuteGenericCommandL( TInt aCommand );
public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void ActivateL();
    void PositionChanged();

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aSize );

public: // From MEikEdwinObserver
	void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);
    
private:
    /**
     * Constructor.
     */
    CESMRDescriptionField();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Converts raw URL text to a link in description editor field
     *  
     * @param aEntry Calendar entry from which the description text is received
     */ 
    void StoreLinkL( MESMRCalEntry& aEntry );

    /**
     * Adds link to the beginning of text from editor field
     * 
     * @param abuf initial description text buffer before addition
     * 
     * @return The description text buffer with link added
     */ 
    HBufC* AddLinkToTextL( const TDesC& aBuf );
    
    /**
     * Gets feature settings
     */
    CESMRFeatureSettings& FeaturesL();
        
private:
    /**
     * Not owned. Description editor object.
     */
    CESMREditor* iDescription;

    /**
     * Field size.
     */
    TSize iSize;

    /**
     * Background control context
     */
    MAknsControlContext* iBackground;

    /**
     * Actual background for the editor
     */
    CAknsFrameBackgroundControlContext* iFrameBgContext;
    
    /**
     * Own: Location Url link
     */ 
    CESMRRichTextLink* iLocationLink;
    
    /**
     * Own: Feature settings.
     */
    CESMRFeatureSettings* iFeatures;
    };

#endif  // CESMRDESCRIPTIONFIELD_H

