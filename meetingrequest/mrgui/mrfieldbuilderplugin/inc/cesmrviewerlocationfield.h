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
* Description:  Meeting request location field
 *
*/


#ifndef CESMRVIEWERLOCATIONFIELD_H_
#define CESMRVIEWERLOCATIONFIELD_H_

#include <eikedwob.h>

#include "cesmrfield.h"

class CMRImage;
class CESMRRichTextViewer;
class CESMRFeatureSettings;

/**
 * Shows the location of the meeting/event
 */
NONSHARABLE_CLASS( CESMRViewerLocationField ): public CESMRField, 
                                                      MEikEdwinSizeObserver
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

public:// From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    TSize MinimumSize();
    void InitializeL();
    void ListObserverSet();
    void ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType,
            TSize aSize );

private: // Implementation
    CESMRViewerLocationField( );
    void ConstructL( );
    CESMRFeatureSettings& FeaturesL();
    void SetWaypointIconL( TBool aEnabled );
    TRect RichTextViewerRect();

private: // data 
       
    TSize iSize;
    // Owned: Field icon
    CMRImage* iFieldIcon;
    // Owned: Field text label
    CESMRRichTextViewer* iRichTextViewer;
    // Owned: Waypoint icon
    // Feature settings. Own
    CESMRFeatureSettings* iFeatures;
    // RichTextViewer line count. Own
    TInt iLineCount;
    // Middle softkey command id
    TInt iMskCommandId;
    };

#endif /*CESMRVIEWERLOCATIONFIELD_H_*/
