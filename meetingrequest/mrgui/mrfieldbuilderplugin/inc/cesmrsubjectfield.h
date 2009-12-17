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
* Description:  ESMR subject field implementation
 *
*/


#ifndef CESMRSUBJECTFIELD_H
#define CESMRSUBJECTFIELD_H

#include <e32cmn.h>
#include <eikedwob.h>
#include <AknsControlContext.h>

#include "cesmriconfield.h"

class CESMREditor;
class MESMRTitlePaneObserver;
class CAknsFrameBackgroundControlContext;

/**
 * This class is a custom field control that shows the subject of calendar events
 */
NONSHARABLE_CLASS( CESMRSubjectField ) :
        public CESMRIconField,
        public MEikEdwinSizeObserver,
        public MEikEdwinObserver
    {
public:
    enum TFieldType
        {
        ETypeSubject = 0,
        ETypeOccasion
        };

    /**
     * Creates new CESMRSubjectField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRSubjectField* NewL( TFieldType aType = ETypeSubject );

    /**
     * Destructor.
     */
    virtual ~CESMRSubjectField();

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TInt ExpandedHeight() const;
    void GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower);
    void ListObserverSet();
    void ExecuteGenericCommandL( TInt aCommand );
public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aSize );

public: // From MEikEdwinObserver
    void HandleEdwinEventL( CEikEdwin *aEdwin, TEdwinEvent aEventType );

public: // From CCoeControl
    void ActivateL();
    void PositionChanged();
    
public: // Interface
    void SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver );

private:
    /**
     * Constructor.
     */
    CESMRSubjectField();

    /**
     * Second phase constructor.
     *
     * @param aId field id
     * @param aTextId default text id
     * @param aIconID icon id
     */
    void ConstructL( TESMREntryFieldId aId, TInt aTextId, TAknsItemID aIconID );

private:
    // Not owned. Text editor.
    CESMREditor* iSubject;

    // Field size.
    TSize iSize;

    // Default text id.
    TInt iTextId;

    // Not owned
    MESMRTitlePaneObserver* iTitlePaneObserver;

    // Background control context
    MAknsControlContext* iBackground;

    // Actual background for the editor
    CAknsFrameBackgroundControlContext* iFrameBgContext;
    };

#endif  // CESMRSUBJECTFIELD_H

