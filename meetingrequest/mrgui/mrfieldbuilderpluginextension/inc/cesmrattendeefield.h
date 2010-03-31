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
* Description:  ESMR titled field implementation
 *
*/


#ifndef CESMRATTENDEEFIELD_H
#define CESMRATTENDEEFIELD_H

#include <e32base.h>
#include <fbs.h>
#include <caluser.h>
#include <eiklbo.h>

//<cmail>
#include "esmrdef.h"
//</cmail>
#include "cesmrfield.h"
#include "cesmrcontacthandler.h"
#include "mesmrcontacthandlerobserver.h"
#include "cesmrncsaifeditor.h"

class CESMRNcsPopupListBox;
class CEikButtonGroupContainer;
class MESMRMeetingRequestEntry;
class CAknsFrameBackgroundControlContext;
class CMRLabel;
class CAknsBasicBackgroundControlContext;
class CMRImage;
class CAknButton;
class CMRButton;

NONSHARABLE_CLASS( CESMRAttendeeField ): public CESMRField,
                                                MEikEdwinSizeObserver,
                                                MESMRNcsAddressPopupList, 
                                                MESMRContactHandlerObserver, 
                                                MCoeControlObserver,
                                                MEikListBoxObserver
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

protected: // MESMRNcsAddressPopupList
    void UpdatePopupContactListL( const TDesC& aMatchString, TBool iListAll );

protected: // from MFSEmailUiContactHandlerObserver
    void OperationCompleteL( TContactHandlerCmd aCmd, const RPointerArray<CESMRClsItem>* aContacts );
    void OperationErrorL( TContactHandlerCmd aCmd, TInt aError );

protected: // from MCoeControlObserver
    void HandleControlEventL( CCoeControl *aControl, TCoeEvent aEventType );

protected: // From CESMRField
    TSize MinimumSize();
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );
    TBool HandleSingletapEventL( const TPoint& aPosition );
    void HandleLongtapEventL( const TPoint& aPosition );

protected: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void SetContainerWindowL( const CCoeControl& aControl );
    
protected: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aDesirableEdwinSize);

protected: // from MEikListBoxObserver
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
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
    void GetCursorLineVerticalPos( TInt& aUpper, TInt& aLower );
    void UpdateAttendeesL( 
    		MESMRMeetingRequestEntry& aEntry );
    
private: //data

    // Editor size provided by edwin event.
    TSize iSize;
    // Field title size.
    TSize iTitleSize;
    // Attendee role.
    CCalAttendee::TCalRole iRole;
    // Own: Title of this field
    CMRLabel* iTitle;
    // Owned. Attendee editor.
    CESMRNcsAifEditor* iEditor;
    // Own. Predictive attendee popup.
    CESMRNcsPopupListBox* iAacListBox;
    // Own. Contact handler
    CESMRContactHandler* iContactHandler;
    // Own. for default text of this field.
    HBufC* iDefaultText;
    // Enable / Disable contact popup list
    TBool iAacListBoxEnabled;
    // Not owned. CBA.
    CEikButtonGroupContainer* iButtonGroupContainer;
    // Own. Background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    /// Own: Field icon
    CMRButton* iFieldButton;
    // Own: Listboxes previous highlighted index
    TInt iPreviousIndex;
    };

#endif  // CESMRATTENDEEFIELD_H
