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
* Description:  ESMR container for the title label and list component
*
*/

#ifndef CESMRVIEW_H
#define CESMRVIEW_H

#include <coecntrl.h>
#include <eiksbobs.h>
#include <eikscrlb.h>

#include "cmrlistpane.h"
#include "mesmrtitlepaneobserver.h"
#include "mmrscrollbarobserver.h"
#include "tnavipaneclearer.h"
#include "esmrgui.hrh"

class CESMRTitlePane;
class MESMRFieldStorage;
class MAgnEntryUiCallback;
class CAknsBasicBackgroundControlContext;
class CESMRPolicy;
class CEikMenuPane;
class MMRFocusStrategy;
class MTouchFeedback;
class CMRNaviArrow;
class MESMRNaviArrowEventObserver;
class CMRToolbar;
class MESMRFieldEvent;
class CMRAttachmentIndicator;

/**
 *  Container for the meeting request title and list control.
 * This class is deleted by framework
 */
NONSHARABLE_CLASS( CESMRView ):
    public CCoeControl,
    public MCoeControlBackground,
    public MESMRTitlePaneObserver,
    public MEikScrollBarObserver,
    public MMRScrollBarObserver
    {
    public:
        /**
         * Two-phased constructor.
         *
         * @param aStorage - interface to storage that has all the fileds
         * @param aEntry - interface to entry information.
         * @param aRect - rect for this view
         * @param aFocusStrategy - interface to focus strategy
         * @param aToolbar - Reference to MR toolbar
         * @param aObserver - navi arrow observer
         * @return Pointer to created and initialized esmr view object.
         */
         static CESMRView* NewL(
                 MESMRFieldStorage* aStorage,
                 MESMRCalEntry& aEntry,
                 const TRect& aRect,
                 MMRFocusStrategy& aFocusStrategy,
                 CMRToolbar& aToolbar,
                 MESMRNaviArrowEventObserver* aObserver = NULL );

        /**
         * Destructor.
         */
        ~CESMRView();

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
        * be handled by CESMRView.
        *
        * @param aCommand
        */
        void ProcessEditorCommandL( TInt aCommand );

        /**
        * Query if a component with given id is currently visible
        *
        * @param aField Field Id which visibility we are interested in.
        * @return True if component is currently visible
        */
        TBool IsControlVisible( TESMREntryFieldId aField ) const;

        /**
        * Title can be changed by calling this method
        *
        * @param aTitle New title for the view
        * @param aDefault
        */
        void SetTitleL( const TDesC& aTitle, TBool aDefault = EFalse );

        /**
        * Focused field in the list can be changed by calling this method
        *
        * @param aField Field id of the field that needs to be focused
        */
        void SetControlFocusedL( TESMREntryFieldId aField );

        /**
         * Launches editor help
         */
        void LaunchEditorHelpL();

        /**
         * Launches viewer help
         */
        void LaunchViewerHelpL();

        /**
        * Query if a component with given id is currently focused
        *
        * @param aField Field Id which focus we are interested in.
        * @return True if component is currently focused
        */
        TBool IsComponentFocused( TESMREntryFieldId aField ) const;

        /**
         * Returns the the currently focused field id.
         */
        TESMREntryFieldId FocusedField() const;
        
        /**
         * Returns the the currently clicked (if exists) or focused field id.
         */
        TESMREntryFieldId ClickedOrFocusedField() const;
        
        /**
         * Set the navi arrows' visibility.
         * @param aLeftArrowVisible  Indicate if the left arrow visible
         * @param aRightArrowVisible Indicate if the right arrow visible
         */
        void SetNaviArrowStatus(TBool aLeftArrowVisible, TBool aRightArrowVisible);

        /**
         * Adjusts view for a new entry and a new policy
         */
        void AdjustViewL(
                MESMRCalEntry* aNewEntry,
                const CESMRPolicy& aNewPolicy );

        /**
         * Scroll the list to put the responeArea at the top and
         * set the focus on it
         */
        void InitialScrollL();

        /**
         * Provides possibility to initialize current menu pane.
         * @param aMenuStyle style of the provided menu pane
         * @param aResourceId Context menu resource ID
         * @param aMenuPane Pointer to menu pane
         */
        void DynInitMenuPaneL(
                TMRMenuStyle aMenuStyle,
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        
        /**
         * Processes event.
         * @param aEvent Reference to event to be processed
         */
        void ProcessEventL( const MESMRFieldEvent& aEvent );
        
        inline CMRListPane* ListPane() { return iListPane; }
    public: // From CCoeControl
        TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

    public:
    	/*
        * Set MRUI MR view mode for judge editor mode or viewer mode is using.
        * @param aMode MRUI MR view mode
        */    	
    	void SetViewMode(TESMRViewMode aMode);
    	TESMRViewMode GetViewMode();
    	
    private: // MESMRTitlePaneObserver
        void UpdateTitlePaneTextL( const TDesC& aText );

    private: // From MMRScrollBarObserver
        void ScrollBarPositionChanged( TInt aThumbPosition );

    private: // From MEikScrollBarObserver
        void HandleScrollEventL( CEikScrollBar* aScrollBar,
                                 TEikScrollEvent aEventType );
        
    private: // From MESMRToolbarVisibilityObserver
    	void MRToolbarVisibilityChanged( 
    	        TBool aVisible, 
    	        TRect aToolbarRect );
    	

    private: // From MCoeControlBackground
        void Draw(CWindowGc& aGc,
                  const CCoeControl& aControl,
                  const TRect& aRect) const;
        void GetTextDrawer(
                CCoeTextDrawerBase*& aText,
                const CCoeControl* aControl) const;

    private: // From CCoeControl 
        void SetContainerWindowL(const CCoeControl& aContainer);
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aInd ) const;
        void SizeChanged();
        TSize MinimumSize();
        void HandleResourceChange( TInt aType );
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        void HandlePointerEventL(
            const TPointerEvent &aPointerEvent );

    private: // From CCoeControl
        void Draw( const TRect& aRect ) const;

    private: // implementation
        void ConstructL( const TRect& aRect );
        CESMRView(MESMRFieldStorage* aStorage,
                  MESMRCalEntry& aEntry,
                  MMRFocusStrategy& aFocusStrategy,
                  MESMRNaviArrowEventObserver* aObserver,
                  CMRToolbar& aToolbar );
        void LaunchHelpL( const TDesC& aContext );
        void SetTitlePaneObserver();
        TRect CalculateAttachmentIndicatorLayout();
        TRect ContainerRect() const;
        
    private: // data
        
        /**
         * Defines the component is clicked
         */
        enum TComponentPointed
            {
            // No component is clicked
            EComponentNone,
            // Scroll bar is clicked
            EComponentScrollBar,
            // List component is clicked
            EComponentListPane
            };
        
        /// Own: UI list pane in event editor and viewer
        CMRListPane* iListPane;
        /// Ref: List item storage
        MESMRFieldStorage* iStorage;
        /// Own: scrollbar
        CAknDoubleSpanScrollBar* iScrollBar;
        /// Ref: scrollbar model
        TAknDoubleSpanScrollBarModel iScrollBarModel;
        /// Own: current scroll bar thumb position
        TInt iScrollBarThumbPosition;
        /// Ref: Current MR entry
        MESMRCalEntry* iEntry;
        /// Own: To enable skin as background for the iTitle
        CAknsBasicBackgroundControlContext* iBgContext;
        /// Own: This clears the navipane for us.
        TNavipaneClearer iClearer;
        /// Ref: Focus strategy reference
        MMRFocusStrategy& iFocusStrategy;
        /// Own: Left navigation arrow component control.
        CMRNaviArrow* iNaviArrowLeft;
        /// Own: Right navigation arrow component control.
        CMRNaviArrow* iNaviArrowRight;
        /// Ref: Navigation arrow event observer.
        MESMRNaviArrowEventObserver* iObserver;
        /// Own: judge which view mode is using
        TESMRViewMode iViewMode;
        /// Ref: Reference to viewer dialog owned mr toolbar 
        CMRToolbar& iToolbar;
        /// Own: Record the rect of stripe.
        TRect iStripeRect;
        /// Own: Attachment download indicator
        CMRAttachmentIndicator* iAttachmentIndicator;
        /// Own: Control array
        RArray<CCoeControl*> iControls;
    };

#endif //CESMRVIEW_H
