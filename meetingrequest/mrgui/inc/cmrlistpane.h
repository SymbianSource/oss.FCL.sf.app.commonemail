/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for GUI List pane component
*
*/

#ifndef CMRLISTPANE_H
#define CMRLISTPANE_H

// INCLUDES
#include <coecntrl.h>
#include "mesmrlistobserver.h"

// FORWARD DECLARATIONS
class MESMRFieldStorage;
class TAknDoubleSpanScrollBarModel;
class MFsTextViewerObserver;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRListPane ) : public CCoeControl,
                                   public MESMRListObserver
    {
public:
    /**
     * Static constructor.
     * 
     * @return New instance of this class
     */
    static CMRListPane* NewL( const CCoeControl& aParent,
                              MESMRFieldStorage& aFactory,
                              TAknDoubleSpanScrollBarModel& aScrollModel );
    /**
     * Destructor
     */
    ~CMRListPane();
    
public: // Interface
    
    /**
     * Return focused list item.
     * @return Focused list item or NULL if no focused item
     */
    CESMRField* FocusedItem() const;
    
    /**
     * Scrolls the specific field to be fully visible
     *
     * @param aInd Index of the field that needs to be fully visible
     */
    void ScrollItemVisible( TInt aInd );
    
    /**
     * Scrolls the view specified amount of pixels
     *
     * @param aAmount Amount of pixels to move the view
     */
    void ScrollView( TInt aAmount );

    /**
     * Updates focus position.
     */
    void UpdateFocusPosition();
  
    /**
     * @return center point of the visible part of the pane
     */
    TPoint GetViewCenterPosition() const;
                   
public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                 TEventCode aType );
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
public: // From MESMRListObserver
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
    
private: // Implementation
    CMRListPane( MESMRFieldStorage& aFactory,
                 TAknDoubleSpanScrollBarModel& aScrollModel );
    void ConstructL( const CCoeControl& aParent );
    TKeyResponse MoveFocusUpL();
    TKeyResponse MoveFocusDownL();
    void LayoutField( CESMRField& aField,
                      const TPoint& aTl );
    void MoveFields( TInt aIndex,
                     TPoint& aTl );
    void DoSetFocusL( TInt aFocus );
    void ScrollItemsUp( TInt aPx );
    void ScrollItemsDown( TInt aPx );
    void UpdateScrollBar();        
    /**
     * Moves focus to pointed field
     * @param aNextFieldIndex a new index
     * @return ETrue if move succeeded
     */
    TBool MoveFocusL( TInt aNextFieldIndex );
         
    
private: // Data
    /// Ref: Storage for list items.
    MESMRFieldStorage& iFactory;
    /// Ref: scrollbar model
    TAknDoubleSpanScrollBarModel& iScrollModel;
    /// Own: Current focus index
    TInt iFocus;
    /// Own: List top visible field index
    TInt iTopVisibleIndex;
    };
    
#endif // CMRLISTPANE_H
// End of file
