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
* Description:  Meeting request description field
 *
*/

#ifndef CESMRVIEWERDESCRIPTIONFIELD_H
#define CESMRVIEWERDESCRIPTIONFIELD_H

#include <e32cmn.h>
#include <eikedwob.h>
#include "cesmrfield.h"
#include "cesmrrichtextviewer.h"

class CESMRLocationPlugin;
class CESMRFeatureSettings;
class CESMRUrlParserPlugin;

/**
 * Description field is a custom control
 * that shows the description of a calendar event.
 */
NONSHARABLE_CLASS( CESMRViewerDescriptionField ): public CESMRField,
                                                  public MEikEdwinSizeObserver,
                                                  public MESMRRichTextObserver
    {

public:
    /**
     * Constructor of the description field.
     *
     * @return New description field object.
     */
    static CESMRViewerDescriptionField* NewL();

    // Destructor
    ~CESMRViewerDescriptionField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void GetCursorLineVerticalPos( TInt& aUpper, TInt& aLower );
    void ListObserverSet();
    TBool ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );
    void HandleLongtapEventL( const TPoint& aPosition );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType );
    void SetContainerWindowL( const CCoeControl& aContainer );
    TSize MinimumSize();
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

public:
    // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType,
            TSize aSize );

protected:
    TBool HandleRichTextLinkSelection( const CESMRRichTextLink* aLink );

private: // Implementation
    CESMRViewerDescriptionField();
    void ConstructL();
    void AddShowOnMapLinkL( MESMRCalEntry& aEntry );
    void SetShowOnMapLinkMiddleSoftKeyL();
    CESMRFeatureSettings& FeaturesL();
    CESMRLocationPlugin& LocationPluginL();
    void ShowLocationOnMapL( const CESMRRichTextLink& aLink );
    void StoreGeoValueL( CCalEntry& aCalEntry,
                         const TDesC& aLocationUrl );
    CESMRUrlParserPlugin& UrlParserL();

private:

    /**
     * Field size.
     */
    TSize iSize;

    /**
     * Rich text viewer.
     * Own.
     */
    CESMRRichTextViewer* iRichTextViewer;

    /**
     * Location plugin.
     * Own.
     */
    CESMRLocationPlugin* iLocationPlugin;

    /**
     * Feature settings.
     * Own.
     */
    CESMRFeatureSettings* iFeatures;

    /**
     * Location URL parser.
     * Own.
     */
    CESMRUrlParserPlugin* iUrlParser;
    };

#endif /* CESMRVIEWERDESCRIPTIONFIELD_H */
