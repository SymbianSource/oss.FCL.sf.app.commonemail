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
* Description:  ESMR subject field implementation
 *
*/


#ifndef CESMRSUBJECTFIELD_H
#define CESMRSUBJECTFIELD_H

#include <e32cmn.h>
#include <eikedwob.h>
#include <aknscontrolcontext.h>

#include "cesmrfield.h"
#include "nmrbitmapmanager.h"

class CESMREditor;
class MESMRTitlePaneObserver;
class CMRImage;
class CAknsBasicBackgroundControlContext;

/**
 * This class is a custom field control that shows the subject of calendar events
 */
NONSHARABLE_CLASS( CESMRSubjectField ) :
        public CESMRField,
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
    
public: // Interface
    void SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver );
    
protected: // From CESMRField
    TSize MinimumSize();
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );    
    void GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower);
    void ListObserverSet();
    TBool ExecuteGenericCommandL( TInt aCommand );

protected: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aSize );

protected: // From MEikEdwinObserver
    void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);

protected: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, 
                                       TEventCode aType );

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
    void ConstructL( 
            TESMREntryFieldId aId, 
            TInt aTextId, 
            NMRBitmapManager::TMRBitmapId aIconID );
    
    void SetPriorityIconL( TUint aPriority );
    
private:
    // Not owned. Text editor.
    CESMREditor* iSubject;
    // Own: Field icon
    CMRImage* iFieldIcon;
    // Own: Priority icon
    CMRImage* iPriorityIcon;
    // Own: Edwin size
    TSize iSize;
    // Not owned
    MESMRTitlePaneObserver* iTitlePaneObserver;
    // Field editor line count
    TInt iLineCount;
    // Own: Background for the editor when it is focused
    CAknsBasicBackgroundControlContext* iBgControlContext;
    // Stores current priority value 
    TUint iCurrentPriority;
    };

#endif  // CESMRSUBJECTFIELD_H

