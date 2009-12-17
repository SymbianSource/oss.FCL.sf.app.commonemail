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
* Description:  ESMR Custom list component
*
*/

#ifndef CESMRLISTCOMPONENT_H
#define CESMRLISTCOMPONENT_H

#include <coecntrl.h>
#include <eiksbobs.h>
#include <eikscrlb.h>
#include <aknphysicsobserveriface.h> // MAknPhysicsObserver

#include "mesmrlistobserver.h"

class CESMRField;
class CESMRLayoutManager;
class MESMRFieldStorage;
class MESMRCalEntry;
class CMRListPane;
class CAknPhysics;

/**
 * CESMRListComponent is custom list for Meeting Request edtior and viewer.
 * List contains a List pane of CESMRField items that are stored in CESMRFieldStorage
 * class.
 *
 * @see CESMRField
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRListComponent ):
    public CCoeControl,
    public MESMRListObserver,
    public MEikScrollBarObserver,
    public MAknPhysicsObserver
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aParent Parent control for this list component
     * @param aStorage Storage contains all the list items.
     * @return Pointer to created and initialized esmr list component object.
     */
     static CESMRListComponent* NewL( 
                 const CCoeControl* aParent, 
                 MESMRFieldStorage* aStorage, 
                 CESMRLayoutManager* aLayout );

    /**
     * Destructor.
     */
    ~CESMRListComponent();

    /**
     * InternalizeL is used to obtain the data from current CCalEntry.
     * CCalEntry is wrapped inside MESMRCalEntry object. This call is
     * propagated to all list items in the list.
     *
     * @param aEntry Current entry
     * @see MESMRCalEntry
     */
    void InternalizeL( MESMRCalEntry& aEntry );

    /**
     * ExternalizeL is used to store the list item data into current
     * CCalEntry. CCalEntry is wrapped inside the MESMRCalEntry object.
     *
     * @param aEntry Current entry.
     * @param aForceValidation ETrue if force validation should be used
     * @see MESMRCalEntry
     */
    void ExternalizeL( MESMRCalEntry& aEntry, TBool aForceValidation = EFalse );

    /**
     * Return MESMRListObserver interface. Interface allows the caller
     * to add, remove field and update field size.
     */
     MESMRListObserver& ListObserver();

    /**
     * Return focused list item.
     * @return Focused list item or NULL if no focused item
     */
     CESMRField* FocusedItem() const;

    /**
     * Initializes all fields. Needed when theme changes
     */
    void InitializeL();

    /**
     * Changes every fields font
     */
    void HandleFontChange();

    void DisableSizeChange(TBool aDisable );

public: // From MEikScrollBarObserver
    void HandleScrollEventL( CEikScrollBar* aScrollBar,
                             TEikScrollEvent aEventType );

public: // from MAknPhysicsObserver

   /**
    * @see MAknPhysicsObserver::ViewPositionChanged
    */
   virtual void ViewPositionChanged( const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags );

   /**
    * @see MAknPhysicsObserver::PhysicEmulationEnded
    */
   virtual void PhysicEmulationEnded();

   /**
    * @see MAknPhysicsObserver::ViewPosition
    */
   virtual TPoint ViewPosition() const;


protected: // From MESMRListObserver
    void ControlSizeChanged( CESMRField* aCtrl );
    void InsertControl( TESMREntryFieldId aField );
    void RemoveControl( TESMREntryFieldId aField );
    TBool IsControlVisible( TESMREntryFieldId aField );
    void SetControlFocusedL( TESMREntryFieldId aField );
    // SCROLLING_MOD: These two methods are for CESMRRichTextViewer to 
    //be able to control view area
    void MoveListAreaDownL( TInt aAmount );
    void MoveListAreaUpL( TInt aAmount );
    TInt ListHeight();
    TBool IsFieldBottomVisible();

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                 TEventCode aType );
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void MakeVisible(TBool aVisible);


private:
    /**
     * Default c++ constuctor
     */
    CESMRListComponent( MESMRFieldStorage* aFactory,
                        CESMRLayoutManager* aLayout );
    /**
     * Second phase constructor
     */
    void ConstructL( const CCoeControl* aParent );

private: // Helper methods

    /**
     * Scrolls the specific field to be fully visible
     *
     * @param aInd Index of the field that needs to be fully visible
     * @param aSizeChanged ETrue if the field size has been changed
     */
    void ScrollItemVisible( TInt aInd/*, TBool aSizeChanged*/ );

    /**
     * Scrolls items in the list up.
     *
     * @param aPx Amount of pixels that items are moved up
     */
    void ScrollItemsUp( TInt aPx );

    /**
     * Scrolls items in the list down.
     *
     * @param aPx Amount of pixels that items are moved down
     */
    void ScrollItemsDown( TInt aPx );

    /**
     * Updates scroll bar
     */
    void UpdateScrollBar();

    /**
     * Sets physics engine parameters
     */
    void UpdatePhysicsL();
    
private:
    /// Own: scrollbar
    CAknDoubleSpanScrollBar* iScroll;
    /// Ref: scrollbar model
    TAknDoubleSpanScrollBarModel iScrollModel;
    /// Own: Storage for list items.
    MESMRFieldStorage* iFactory;
    /// Own: List item container
    CMRListPane* iContainer;
    /// Ref: Pointer to Layout data for list component
    CESMRLayoutManager* iLayout;
    // Own: Boolean for disabling SizeChanged handling
    TBool iDisableSizeChanged;
    // previous scroll bar thumb position
    TInt iPrevThumbPos;

    CAknPhysics* iPhysics;
    TBool iIsDragging;
    TBool iIsScrolling;
    TPoint iPreviousPosition;
    TPoint iOriginalPosition;
    TTime iStartTime;
    TInt iCurrentScrollSpan;
    };
#endif //CESMRLISTCOMPONENT_H
