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
* Description:  ESMR Field item for CESMRListComponent
*
*/
#ifndef CESMRFIELD_H
#define CESMRFIELD_H

//<cmail>
#include "esmrdef.h"
#include "esmrcommands.h"
//</cmail>

#include "cesmrlayoutmgr.h"
#include "mesmrcalentry.h"
#include "mesmrtitlepaneobserver.h"
#include "mesmrfieldeventobserver.h"
#include "mesmrfieldeventnotifier.h"

#include <coecntrl.h>
#include <esmrgui.mbg>

class MESMRListObserver;
class MESMRCalEntry;
class CESMRBorderLayer;
class CESMRLayoutManager;
class CMRBackground;

// Enumeration for border type
enum TESMRFieldFocusType
    {
    EESMRNoFocus = 0,
    EESMRBorderFocus,
    EESMRHighlightFocus
    };

/**
 *  CESMRField defines a list item for CESMRListComponent.
 *  This is the base class for all fields.
 *
 *  @lib esmrgui.lib
 */
class CESMRField : public CCoeControl,
                   public MESMRFieldEventObserver,
                   public MESMRFieldEventNotifier
    {
public:
    /**
     * Destructor.
     */
    IMPORT_C virtual ~CESMRField();

    /**
     * Construct field with the given component. Given component
     * will have outline focus when focused.
     *
     * @param aControl Field control, ownership transferred
     */
    IMPORT_C void ConstructL( 
    		CCoeControl* aControl, 
    		TESMRFieldFocusType aFocusType = EESMRBorderFocus ); 

    /**
     * Observer interface is used to notify the list component when
     * component is added, removed or needs resizing.
     *
     * @param aObserver List component observer
     */
    IMPORT_C virtual void SetListObserver( MESMRListObserver* aObserver );

    /**
     * Set identifier for this field.
     *
     * @param aFieldId Id for this field
     */
    IMPORT_C virtual void SetFieldId( TESMREntryFieldId aFieldId );

public:
    /**
     * Layout this field relative to given TRect. Implementation MUST
     * update the height of this field to given TRect so the calling
     * list component knows where to place the next field.
     *
     * @param aRect Contains the rect for this field. The height must be updated.
     * @param aMgr LayoutManager (LAF)
     * @deprecated Use InitializeL() and ExpandableHeight() instead.
     */
    IMPORT_C void DoLayout( TRect& aRect, CESMRLayoutManager& aMgr );

    /**
     * Method is called by list component to retrieve new height of the field.
     *
     * @see SetExpandable()
     * @return height after expansion
     */
    IMPORT_C virtual TInt ExpandedHeight() const;

    /**
     * Called in the constuct phase. Font and colors are typically set here
     */
    IMPORT_C virtual void InitializeL();

    /**
     * Called if the Font needs to be updated from CESMRLayoutManager
     *
     * @see InitializeL()
     */
    IMPORT_C virtual void FontChangedL();

    /**
     * Sets this field to be expandable. After this is called list
     * component asks the new height of the field every time its size changes.
     *
     * @see ExpandedHeight()
     */
    IMPORT_C virtual void SetExpandable();

    /**
     * Method is called to ask is this field expandable or not.
     *
     * @return boolean ETrue/EFalse
     */
    IMPORT_C virtual TBool IsExpandable() const;

    /** Sets the layoutmanager for the field. If overwritten, the derived class MUST
     *  call iBorder->SetLayoutManager() to enable intended behaviour.
     *  @param layoutmanager for the field
     */
    IMPORT_C virtual void SetLayoutManager( CESMRLayoutManager* aLayout );

    /**
     * Identifier for this field.
     *
     * @return TESMRField
     */
    IMPORT_C virtual TESMREntryFieldId FieldId() const;

    /**
     * Called by the list component. Implementations should update the
     * the field according to MESMRCalEntry argument. Default implementation
     * does nothing.
     *
     * @param aEntry Current calendar entry
     */
    IMPORT_C virtual void InternalizeL( MESMRCalEntry& aEntry );

    /**
     * Called by the list component. Implementations should update the
     * the given calendar entry with its data.
     *
     * @param aEntry Current calendar entry
     */
    IMPORT_C virtual void ExternalizeL( MESMRCalEntry& aEntry );

    /**
     * Set this fields focus.
     * @param aFocus
     */
    IMPORT_C virtual void SetOutlineFocusL( TBool aFocus );

    /**
     * Subclasses should overwrite this method when any validity
     * checks are needed.
     *
     * @return ETrue if data is valid, EFalse if not (focus not changed)
     */
    IMPORT_C virtual TBool OkToLoseFocusL( TESMREntryFieldId aNextItem );

     /**
     * Return minimun vertical area that should be shown when
     * scrolling the list.
     * @param aUpper, the upper vertical coordinate
     * @param aLower, the lower vertical coordinate
     */
     IMPORT_C virtual void GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower);

     /**
      * Trigger for notifying when the list observer is set
      */
     IMPORT_C virtual void ListObserverSet();

     /**
      * Calculates the shown rect in given rect within the list area
      */
     IMPORT_C virtual TRect CalculateVisibleRect( TRect aRect );

     /**
      * Executes generic command if needed by the field
      * @param aCommand, commandId to handle specific command
      */
     IMPORT_C virtual void ExecuteGenericCommandL( TInt aCommand );

     /**
      * Set titlepane observer to see title pane subject events
      * @param aObserver title pane observer to be set
      */
     IMPORT_C virtual void SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver );
     
     /**
      * From MESMRFieldEventNotifier
      * Sets field event queue.
      * @param aObserver observer to receive field events. 
      */
     IMPORT_C void SetEventQueueL( MESMRFieldEventQueue* aEventQueue );
     
     /**
      * Getter for retrieving outline focus state.
      * @return ETrue if field is focused EFalse otherwise.
      */
     IMPORT_C TBool HasOutlineFocus() const;
     
     /**
      * Getter for focus rect. CMRBackground uses this to get rectangle
      * it draws background focus to.
      * Returned rectangle should be relative to Field's rect.
      * @return focus rectangle.
      */
     IMPORT_C TRect GetFocusRect() const;
     
     /**
      * Setter for background focus area. Every field should set
      * this in their SizeChanged methods.
      * Given rectangle should be relative to Field's rect.
      * @param aFocusRect rectangle for focus.
      */
     IMPORT_C void SetFocusRect( const TRect& aFocusRect );
     
     /**
      * Getter for background focus type. CMRBackground uses this
      * method to determine how to draw focus.
      * @return focus type.
      */
     IMPORT_C TESMRFieldFocusType GetFocusType() const;
     
     /**
      * Setter for focus type. Every field should set this in e.g. their
      * ConstructL.
      * @param aFocusType focus type.
      */
     IMPORT_C void SetFocusType( TESMRFieldFocusType aFocusType );
     
     /**
      * Setter for field mode. Field is either in viewer or editor mode.
      * @param aModeOfField Mode of the field
      */
     IMPORT_C void SetFieldMode( TESMRFieldMode aMode );
     
     /**
      * Getter for the field mode.
      * @return Field mode
      */
     IMPORT_C TESMRFieldMode FieldMode() const;
     
protected:
    
    /**
     * Sends event to the event queue.
     * @param aEvent event to send.
     */
    IMPORT_C void NotifyEventL( const MESMRFieldEvent& aEvent );
    
    /**
     * Sends command event to the event queue.
     * @param aCommand command to send
     */
    IMPORT_C void NotifyEventL( TInt aCommand );
    
    /**
     * Sends event to the event queue asynchronously.
     * Ownership of the event is transferred to the event queue.
     * @param aEvent event to send.
     */
    IMPORT_C void NotifyEventAsyncL( MESMRFieldEvent* aEvent );

    /**
     * Sends command event to the event queue asynchronously.
     * Ownership of the event is transferred to the event queue.
     * @param aEvent event to send.
     */
    IMPORT_C void NotifyEventAsyncL( TInt aCommand );

    /**
     * Set middle softkey command and name
     * @param aCommandId, command to execute on MSK press
     * @param aResourceId, resource string to display on MSK
     */
    IMPORT_C void ChangeMiddleSoftKeyL( TInt aCommandId, TInt aResourceId);

    /**
     * Set middle softkey command
     * @param aResourceId, MSK CBA_BUTTON resource id 
     */
    IMPORT_C void ChangeMiddleSoftKeyL( TInt aResourceId );
         
    /**
     * Requests dialog to restore middle softkey to the default one.
     */
    IMPORT_C void RestoreMiddleSoftKeyL();

    /**
     * Sets middle softkey visible in field.
     * @param aVisible if ETrue sets middle softkey visible.
     */
    IMPORT_C void SetMiddleSoftKeyVisible( TBool aVisible );

public: // From CCoeControl
    IMPORT_C void SetContainerWindowL(const CCoeControl& aContainer);
    IMPORT_C virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );
    IMPORT_C virtual void SizeChanged();
    IMPORT_C virtual TInt CountComponentControls() const;
    IMPORT_C virtual CCoeControl* ComponentControl( TInt aInd ) const;
    IMPORT_C virtual TSize MinimumSize();     

protected:
    
    IMPORT_C CESMRField();
    
    // From MESMRFieldEventObserver
    /**
     * Handles an event from other field.
     * Default implementation delegates command events to
     * <c>ExecuteGenericCommandL()</c>
     * @param aEvent event to handle
     */
    IMPORT_C void HandleFieldEventL( const MESMRFieldEvent& aEvent );
    
    // From MESMRFieldEventNotifier
    /**
     * Returns the event observer interface
     */
    IMPORT_C MESMRFieldEventObserver* EventObserver() const;
    
protected:
    /// Own: Border control. Draws the outline border
    CESMRBorderLayer* iBorder;
    /// Ref: Observer for notifying list component
    MESMRListObserver* iObserver;
    /// Ref: Id for this field.
    TESMREntryFieldId iFieldId;
    /// Ref: Pointer to layoutmanager
    CESMRLayoutManager* iLayout;
    /// Own: Flag to inform is this field expandable or not.
    TBool iExpandable;
    /// Field event queue. 
    MESMRFieldEventQueue* iEventQueue;
    /// Custom MSK flag
    TBool iCustomMsk;
    /// Own:
    TBool iDisableRedraw;
    /// 
    TBool iDefaultMskVisible;
    /// Msk disable/enable flag
    TBool iMskVisible;
    /// Own: Background/focus layer
    CMRBackground* iBackground;
    /// Boolean whether field has outline focus
    TBool iOutlineFocus;
    /// Background focus rect
    TRect iFocusRect;
    /// Background focus type
    TESMRFieldFocusType iFocusType;    
    /// Field mode (editor or viewer)
    TESMRFieldMode iFieldMode;
    };

#endif
