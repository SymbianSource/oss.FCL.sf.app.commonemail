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
* Description:  ESMR Tracking view container for the title label and list component
*
*/

#ifndef CESMRTRACKINGVIEW_H_
#define CESMRTRACKINGVIEW_H_

#include <coecntrl.h>
#include <aknlists.h> 
#include <eiklbo.h>

#include "mesmrtitlepaneobserver.h"
#include "mesmrfontsizeobserver.h"
#include "tnavipaneclearer.h"

#include "resmrstatic.h"
#include "cesmrcontactmenuhandler.h"
#include "nmrbitmapmanager.h"

class CESMRTitlePane;
class MESMRCalEntry;
class MESMRFieldStorage;
class MAgnEntryUiCallback;
class CAknsBasicBackgroundControlContext;
class CESMRTrackingViewDialog;

/**
 *  Container for the meeting request title and list control.
 * This class is deleted by framework
 */
NONSHARABLE_CLASS( CESMRTrackingView ):
    public CCoeControl,
    public MCoeControlBackground,
    public MESMRFontSizeObserver,
    public MEikListBoxObserver
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aEntry - interface to entry information.
     * @param aRect - rect for this view
     * @param aReqAttendee - the count of required attendees
     * @param aOptAttendee - the count of optional attendees
     * @return Pointer to created and initialized esmr view object.
     */
     static CESMRTrackingView* NewL( 
             MESMRCalEntry& aEntry, 
             const TRect& aRect,
             const TInt &aReqAttendee,
             const TInt &aOptAttendee);

    /**
     * Destructor.
     */
    ~CESMRTrackingView();

public:

    /**
    * Externalize the list component items. Data in list items
    * is stored to iEntry member variable.
    *
    * @param TBool ETrue if force validation should be used
    */
    void ExternalizeL( TBool aForceValidation = EFalse );

    /**
    * Internalize the list component items. Data from iEntry is updated
    * to all fields.
    */
    void InternalizeL();

    /**
    * Process commands. Dialog forwards the commands that must
    * be handled by CESMRTrackingView.
    *
    * @param aCommand
    */
    void ProcessEditorCommandL( TInt aCommand );

    /**
    * Title can be changed by calling this method
    *
    * @param aTitle New title for the view
    * @param aDefault
    */
    void SetTitleL( const TDesC& aTitle );

    /**
     * Launches editor help
     */
    void LaunchEditorHelpL();

    /**
     * Launches viewer help
     */
    void LaunchViewerHelpL();

    void SetParentDialog(CCoeControl* aParentDialog);
    
private: // implementation
    void ConstructL( const TRect& aRect );
    CESMRTrackingView( MESMRCalEntry& aEntry, 
            const TInt &aReqAttendee,
            const TInt &aOptAttendee);

private:
    // Define the type of icons for status type
    enum TStatusIconType
        {
        /** For accept */
        EAcceptIcon,
        /** For reject */
        ERejectIcon,
        /** For tentative */
        ETentativeIcon,
        /** For none status */
        ENoneIcon,
        /** For account total type number */
        ETotalIconNumber
        };
    void LaunchHelpL( const TDesC& aContext );
    void CreateTrackingListL();
    void InitTrackingListL();
    void InitStatusIconArrayL();
    CGulIcon* InitStatusIconL(NMRBitmapManager::TMRBitmapId aStatus);

public: // From CCoeControl
    void SetContainerWindowL(const CCoeControl& aContainer);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TSize MinimumSize();
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    void HandleResourceChange( TInt aType );
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

private: // From CCoeControl
    void Draw( const TRect& aRect ) const;

protected: // FromMESMRFontSizeObserver
    void FontSizeSettingsChanged();

public: // From MCoeControlBackground
    void Draw(CWindowGc& aGc, 
              const CCoeControl& aControl, 
              const TRect& aRect) const;
    void GetTextDrawer(
            CCoeTextDrawerBase*& aText, 
            const CCoeControl* aControl) const;

public: // From MEikListBoxObserver
    virtual void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);    
    
private:
    /// Own: Priority icon
    CESMRTitlePane* iTitle;
    /// Own: attendee list
    /// Own: The list for attendee names and tracking status
    CAknSingleGraphicStyleListBox* iList;
    /// Ref: List item storage
//    MESMRFieldStorage* iStorage;
    /// Ref: Current MR entry
    MESMRCalEntry& iEntry;
    /// Own: To enable skin as background for the iTitle   
    CAknsBasicBackgroundControlContext* iBgContext;
    /// Own: This clears the navipane for us.
    TNavipaneClearer iClearer;
    /// Required attendee count
    TInt iReqAttendeeCount;
    /// Optional attendee count
    TInt iOptAttendeeCount;
    /// Own: list text array
    MDesCArray* iItemArray;
    /**
     * Static TLS stored variables.
     */
    RESMRStatic iESMRStatic;
    /**
     * Ref: Reference to menu handler
     */
    CESMRContactMenuHandler* iMenuHandler;
    /// Ref: Tracking View Dialog
    CESMRTrackingViewDialog* iParentDialog;
    /// Own: attendees' contact info
    TDesC* iAddress;
    /// Own: Focused list item index
    TInt iFocusedItemIndex;
    /// Own: Index of "Optional" item
    TInt iIndexOfOptLabel;
    };

#endif /* CESMRTRACKINGVIEW_H_ */
