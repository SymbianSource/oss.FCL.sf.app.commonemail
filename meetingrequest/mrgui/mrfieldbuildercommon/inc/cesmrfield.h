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
* Description:  Parent class for specific fields owned by fieldcontainer
*
*/
#ifndef CESMRFIELD_H
#define CESMRFIELD_H

#include <e32base.h>
#include <fbs.h>
//<cmail>
#include "esmrdef.h"
#include "esmrcommands.h"
//</cmail>

#include "mesmrcalentry.h"
#include "mesmrtitlepaneobserver.h"
#include "mesmrfieldeventobserver.h"
#include "mesmrfieldeventnotifier.h"

#include <coecntrl.h>
#include <esmrgui.mbg>

class MESMRListObserver;
class MESMRCalEntry;
class CESMRBorderLayer;
class CMRBackground;
class MTouchFeedback;
class MESMRFieldValidator;
class CEikMenuPane;
class MTouchFeedback;

// Enumeration for border type
enum TESMRFieldFocusType
    {
    EESMRNoFocus = 0,
    EESMRBorderFocus,
    EESMRHighlightFocus
    };

/**
 *  CESMRField defines a list item.
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
    		CCoeControl* aControl );

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

    /**
     * Set identifier for preceding item index.
     *
     * @param PreItemIndex Index for pre item in the list
     */
    IMPORT_C void SetPreItemIndex( TInt aPreItemIndex );

    /**
     * Set identifier for current item index.
     *
     * @param aCurrentItemIndex Index for current item in the list
     */
    IMPORT_C void SetCurrentItemIndex( TInt aCurrentItemIndex );

public:
    /**
     * Layout this field relative to given TRect. Implementation MUST
     * update the height of this field to given TRect so the calling
     * list component knows where to place the next field.
     *
     * @param aRect Contains the rect for this field. The height must be updated.
     * @deprecated Use InitializeL() and ExpandableHeight() instead.
     */
    IMPORT_C void DoLayout( TRect& aRect );

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
     * Identifier for this field.
     *
     * @return TESMRField
     */
    IMPORT_C virtual TESMREntryFieldId FieldId() const;

    /**
     * Identifier for preceding item index.
     *
     * @return The pre item index in the listpane
     */
    IMPORT_C TInt PreItemIndex() const;

    /**
     * Identifier for preceding item index.
     *
     * @return The current item index in the listpane
     */
    IMPORT_C TInt CurrentItemIndex() const;

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
      * Get current cursor line virtical position, it's relative to current field
      * If no internal cursor in this field, default return whole field rect size.
      * @param aUpper, the upper vertical coordinate
      * @param aLower, the lower vertical coordinate
      */
     IMPORT_C virtual void GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower);

     /**
      * Trigger for notifying when the list observer is set
      */
     IMPORT_C virtual void ListObserverSet();

     /**
      * Executes generic command if needed by the field.
      *
      * @param aCommand, commandId to handle specific command
      * @return ETrue if the command is used, EFalse otherwise
      */
     IMPORT_C virtual TBool ExecuteGenericCommandL( TInt aCommand );

     /**
      * Notifies long tap event to field.
      * Calls virtual CESMRField::HandleLongtapEventL for
      * field specific event handling.
      *
      * @param aPosition position of the long tap event.
      * @see HandleLongtapEventL
      */
     IMPORT_C void LongtapDetectedL( const TPoint& aPosition );


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

     /**
      * Set middle softkey command and name
      * @param aCommandId, command to execute on MSK press
      * @param aResourceId, resource string to display on MSK
      */
     IMPORT_C void ChangeMiddleSoftKeyL( TInt aCommandId, TInt aResourceId);

     /**
      * Sets validator for field.
      *
      * @param aValidator validator to use.
      */
     IMPORT_C virtual void SetValidatorL( MESMRFieldValidator* aValidator );

     /**
      * Tests if field control has been activated.
      */
     IMPORT_C TBool IsFieldActivated() const;

     /**
      * Provides possibility for field to dynamically
      * intialize context menu.
      *
      * @param aResourceId Context menu resource ID
      * @param aMenuPane Pointer to menu pane
      */
     IMPORT_C virtual void DynInitMenuPaneL(
             TInt aResourceId,
             CEikMenuPane* aMenuPane );

     /**
      * Setter for field view mode. Enumeration TESMRFieldType
      * lists possible view modes.
      * @param aViewMode View mode of the field
      */
     IMPORT_C void SetFieldViewMode( TESMRFieldType aViewMode );

     /**
      * Getter for the field view mode.
      * @return Field view mode
      */
     IMPORT_C TESMRFieldType FieldViewMode() const;

     /**
      * Updates field extension control and takes ownership
      * @param aControl Pointer to extension control.
      */
     IMPORT_C void UpdateExtControlL(
                  CCoeControl* aControl );

     /**
      * Set field as locked field(can't be deleted or edited)
      */
     IMPORT_C void virtual LockL();

     /**
      * Get lock status of the field
      * @return ETrue shows field locked EFalse shows field not locked
      */
     IMPORT_C TBool IsLocked();

     /**
      * Finds out if field supports longtap functionality or not
      * @param aPointerEvent, where the event actually occured
      * @return ETrue if field supports longtap functionality
      */
     IMPORT_C virtual TBool SupportsLongTapFunctionalityL(
    		 const TPointerEvent &aPointerEvent );

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

    /**
     * Long tap event handler for field. Subclasses should execute their
     * specific long tap action when this event handler is called.
     *
     * @param aPosition position of long tap event
     */
    IMPORT_C virtual void HandleLongtapEventL( const TPoint& aPosition );

     /**
      * Single tap event handler for field. Subclasses should execute their
      * specific single tap action when this event handler is called.
      * @param aPosition position of single tap event
      * @return ETrue if field handles single tap event.
      */
    IMPORT_C virtual TBool HandleSingletapEventL( const TPoint& aPosition );
     /**
      * Pointer event handler for field. This is being called if the received
      * pointer event has not been handled by single tap or long tap handler.
      * @param aPointerEvent the pointer event
      * @return ETrue if pointer event was consumed
      */
    IMPORT_C virtual TBool HandleRawPointerEventL( const TPointerEvent& aPointerEvent );

public: // From base class CCoeControl
    IMPORT_C void SetContainerWindowL(const CCoeControl& aContainer);
    IMPORT_C virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );
    IMPORT_C virtual void SizeChanged();
    IMPORT_C virtual TInt CountComponentControls() const;
    IMPORT_C virtual CCoeControl* ComponentControl( TInt aInd ) const;
    IMPORT_C virtual TSize MinimumSize();

protected: // From base class CCoeControl

    /**
     *  Implements pointer event handling for fields.
     *  Subclasses must not override this but implement the field specific
     *  actions using the template methods.
     *
     *  @param aPointerEvent the pointer event to handle
     *  @see HandleSingletapEventL
     *  @see HandleRawPointerEventL
     */
    IMPORT_C void HandlePointerEventL( const TPointerEvent& aPointerEvent );

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

    /**
     * Initiates tactile feedback when called
     */
    IMPORT_C void HandleTactileFeedbackL();


private:
    void AquireTactileFeedback();

protected: // data
    /// Own: control which is surrounded by border
    CCoeControl* iExtControl;
    /// Ref: Observer for notifying list component
    MESMRListObserver* iObserver;
    /// Ref: Id for this field.
    TESMREntryFieldId iFieldId;
    /// Ref: Id for preceding item index.
    TInt iPreItemIndex;
    /// Ref: Id for current item index.
    TInt iCurrentItemIndex;
    /// Field event queue.
    MESMRFieldEventQueue* iEventQueue;
    /// Custom MSK flag
    TBool iCustomMsk;
    /// Redraw flag
    TBool iDisableRedraw;
    /// Field default value for MSK visibility
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
    /// Field view mode. Required for disabling a field
    TESMRFieldType iFieldViewMode;
    /// Ref: Reference to tactile feedback
    MTouchFeedback* iTactileFeedback;
    /// Ref: Validator
    MESMRFieldValidator* iValidator;
    /// Own: lock status
    TBool iLocked;
    };

#endif
