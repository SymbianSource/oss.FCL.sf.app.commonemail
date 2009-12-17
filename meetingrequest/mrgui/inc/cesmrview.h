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
* Description:  ESMR container for the title label and list component
*
*/

#ifndef CESMRVIEW_H
#define CESMRVIEW_H

#include <coecntrl.h>
#include <eikmenup.h>

#include "cesmrlistcomponent.h"
#include "mesmrtitlepaneobserver.h"
#include "mesmrfontsizeobserver.h"

class CESMRTitlePane;
class MESMRCalEntry;
class MESMRFieldStorage;
class MAgnEntryUiCallback;
class CESMRLayoutManager;
class CAknsBasicBackgroundControlContext;

/**
 *  Container for the meeting request title and list control.
 * This class is deleted by framework
 */
NONSHARABLE_CLASS( CESMRView ):
    public CCoeControl,
    public MCoeControlBackground,
    public MESMRFontSizeObserver,
    public MESMRTitlePaneObserver
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aStorage - interface to storage that has all the fileds
     * @param aEntry - interface to entry information.
     * @param aRect - rect for this view
     * @return Pointer to created and initialized esmr view object.
     */
     static CESMRView* NewL( 
    		 MESMRFieldStorage* aStorage, 
    		 MESMRCalEntry& aEntry, 
    		 const TRect& aRect );

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
    * Checks if the command can be processed
    * be handled by CESMRView.
    *
    * @param aCommand
    * @return ETrue if command can be processed
    */
    TBool CanProcessEditorCommandL( TInt aCommand );

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
    TBool IsComponentVisible( TESMREntryFieldId aField ) const;

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
	
public: // From CAknDialog
    void DynInitMenuPaneL( /*TInt aResourceId,*/ 
    		CEikMenuPane* aMenuPane );	

public: // MESMRTitlePaneObserver
    void UpdateTitlePaneTextL( const TDesC& aText );
    void UpdateTitlePanePriorityIconL( TUint aPriority );

private: // implementation
    void ConstructL( const TRect& aRect );
    CESMRView( MESMRFieldStorage* aStorage, MESMRCalEntry& aEntry );

private:
    void LaunchHelpL( const TDesC& aContext );

public: // From CCoeControl
    void SetContainerWindowL(const CCoeControl& aContainer);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TSize MinimumSize();
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    void HandleResourceChange( TInt aType );
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    // <cmail>
    void FocusChanged(TDrawNow aDrawNow); 
    // </cmail>

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

private:
    /// Own: Priority icon
    CESMRTitlePane* iTitle;
    /// Own: Meeting Requst list component
    CESMRListComponent* iList;
    /// Ref: List item storage
    MESMRFieldStorage* iStorage;
    /// Ref: Current MR entry
    MESMRCalEntry& iEntry;
    /// Own: Pointer to layout manager
    CESMRLayoutManager* iLayout;
    /// Own: To enable skin as background for the iTitle   
    CAknsBasicBackgroundControlContext* iBgContext;
    };

#endif //CESMRVIEW_H
