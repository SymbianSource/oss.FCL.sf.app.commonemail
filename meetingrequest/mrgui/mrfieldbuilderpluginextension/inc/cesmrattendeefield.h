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
* Description:  ESMR titled field implementation
 *
*/


#ifndef CESMRATTENDEEFIELD_H
#define CESMRATTENDEEFIELD_H

#include <caluser.h>
#include <eikedwob.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "cesmrfield.h"
#include "cesmrcontacthandler.h"
#include "mesmrcontacthandlerobserver.h"
#include "cesmrncsaifeditor.h"

class CEikLabel;
class CESMRNcsPopupListBox;
class CEikButtonGroupContainer;
class MESMRMeetingRequestEntry;
class CAknsFrameBackgroundControlContext;

NONSHARABLE_CLASS( CESMRAttendeeField ):
public CESMRField,
public MEikEdwinSizeObserver,
public MESMRNcsAddressPopupList,
public MESMRContactHandlerObserver,
public MCoeControlObserver
    {
public:
    /**
     * Two phase constructor.
     * @param aRole attendee role
     */
    static CESMRAttendeeField* NewL( CCalAttendee::TCalRole aRole );

    /**
     * Destructor.
     */
    ~CESMRAttendeeField();

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    TInt ExpandedHeight() const;
    void GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower);
    void ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void PositionChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void SetContainerWindowL( const CCoeControl& aControl );
    void ActivateL();

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aDesirableEdwinSize);

public: // MESMRNcsAddressPopupList
    void UpdatePopupContactListL( const TDesC& aMatchString, TBool iListAll );

public: // from MFSEmailUiContactHandlerObserver
    void OperationCompleteL( TContactHandlerCmd aCmd, const RPointerArray<CESMRClsItem>* aContacts );
    void OperationErrorL( TContactHandlerCmd aCmd, TInt aError );

public: // from MCoeControlObserver
    void HandleControlEventL( CCoeControl *aControl, TCoeEvent aEventType );

private: // Implementation
    /**
     * Constructor.
     * @param aRole attendee role
     */
    CESMRAttendeeField( CCalAttendee::TCalRole aRole );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Removes all attendees.
     * @param aEntry calendar entry
     */
    void ClearAttendeeListL( CCalEntry& aEntry );

    /**
     * Adds attendees to calendar entry.
     * @param aEntry calendar entry
     * @param aPhoneOwnerRole Phone owner's role
     */
    void ParseAttendeesL(
            MESMRMeetingRequestEntry& aMREntry );

    /**
     * Calculates rect for predictive participant popup.
     * @return rect for popup
     */
    TRect ResolvePopupRectL();

    /**
     * launches search popup dialog for remote contact search.
     */
    void DoPopupSelectL();

    /**
     * close contact popup list
     */
    void ClosePopupContactListL();

    /**
     * Launch remote lookup
     * @param aExitReason optional error handling
     * @param aSearchText descriptor containing the search string
     */
    CESMRNcsEmailAddressObject* ExecuteRemoteSearchL(
        CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason& aExitReason,
        const TDesC& aSearchText );

    /**
     * Set default text for attendee field
     */
    void AppendDefaultTextL();

    /**
     * Clear editor
     */
    void ClearDefaultTextL();

    /**
     * show contact pop up list under attendee field.
     * @param aShow hide or show popuplist
     */
    void ShowPopupCbaL( TBool aShow );

    void UpdateSendOptionL();

private: //data
    /**
     * Own. Field title label.
     */
    CEikLabel* iTitle;

    /**
     * Temporary expanded size when editor line count grows.
     */
    TSize iExpandedSize;

    /**
     * Field title size.
     */
    TSize iTitleSize;

    /**
     * Attendee role.
     */
    CCalAttendee::TCalRole iRole;

    /**
     * Not owned. Attendee editor.
     */
    CESMRNcsAifEditor* iEditor;

    /**
     * Own. Predictive attendee popup.
     */
    CESMRNcsPopupListBox* iAacListBox;

    /**
     * Own.
     */
    CESMRContactHandler* iContactHandler;

    /**
     * Own. for default text of this field.
     */
    HBufC* iDefaultText;

    /**
     * Enable / Disable contact popup list
     */
    TBool iAacListBoxEnabled;
    
    /**
     * Not owned. CBA.
     */
    CEikButtonGroupContainer* iButtonGroupContainer;
    
    // Not owned. Background control context
    MAknsControlContext* iBackground;
    
    /**
     * Own. for record entry's phone owner
     */
    HBufC* iPhoneOwnerAddr;
    
    // Owned. Actual background for the editor
    CAknsFrameBackgroundControlContext* iFrameBgContext;
    };

#endif  // CESMRATTENDEEFIELD_H
