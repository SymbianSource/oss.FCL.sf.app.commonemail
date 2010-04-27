/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for compose view
*
*/


#ifndef CNCSCOMPOSEVIEWCONTAINER_H
#define CNCSCOMPOSEVIEWCONTAINER_H


#include <AknDef.h>
#include <AknsControlContext.h>
#include <eikedwob.h>
#include <aknphysicsobserveriface.h> // MAknPhysicsObserver

#include "ncsfieldsizeobserver.h"
#include "ncsconstants.h"
#include "FreestyleEmailUiContactHandlerObserver.h"
#include "FreestyleEmailUi.hrh"
#include <aknlongtapdetector.h>

class CAknPhysics;
class CNcsEditor;
class CNcsComposeView;
class CNcsHeaderContainer;
class CAknLongTapDetector;
class CAknDoubleSpanScrollBar;
class CFSMailBox;
class CAknsLayeredBackgroundControlContext;
class CFsAutoSaver;
class CNcsEmailAddressObject;

enum TAppendAddresses 
	{
	EAppendTo,
	EAppendCc,
	EAppendBcc
	};


/**
*  CNcsComposeViewContainer container control class.
*/
class CNcsComposeViewContainer: public CCoeControl, public MNcsFieldSizeObserver,
                                public MEikScrollBarObserver,
                                public MFSEmailUiContactHandlerObserver,
                                public MAknLongTapDetectorCallBack,
                                public MEikEdwinObserver,
                                public MEikEdwinSizeObserver,
                                public MAknPhysicsObserver
    {
    
public:
 
    /** Initialisation flags. */
    enum TInitFlags
        {
        ECcFieldVisible  = 0x1,
        EBccFieldVisible = 0x2
        };

    /**
    * Two-phased constructor.
    * Create a CNcsComposeViewContainer object, that draws itself to aRect
    * @param aView The parent view of this container.
    * @param aRect The rectangle this view will be drawn to.
    * @param aMailBox reference to current mailbox item.
    * @param aFlags Initilisation flags defined in @c TInitFlag enumeration.
    * @return a pointer to the created instance of CNcsComposeViewContainer.
    */
    static CNcsComposeViewContainer* NewL( CNcsComposeView& aView, 
        const TRect& aRect, CFSMailBox& aMailBox, CFsAutoSaver& aAutoSaver,
        TInt aFlags = NULL );

    /**
    * Destructor.
    */
    virtual ~CNcsComposeViewContainer();

public: // new functions

    /**
    * UpdateScrollBar
    * Updates scroll bar data.
    */
    void UpdateScrollBar();

    /**
    * GetToFieldAddressesL
    * Get addresses in TO-field.
    * @return Array of address objects.
    */
    const RPointerArray<CNcsEmailAddressObject>& 
    GetToFieldAddressesL( TBool aParseNow=ETrue );

    /**
    * GetCcFieldAddressesL
    * Get addresses in CC-field.
    * @return Array of address objects.
    */
    const RPointerArray<CNcsEmailAddressObject>& 
    GetCcFieldAddressesL( TBool aParseNow=ETrue );

    /**
    * GetBccFieldAddressesL
    * Get addresses in BCC-field.
    * @return Array of address objects.
    */
    const RPointerArray<CNcsEmailAddressObject>& 
    GetBccFieldAddressesL( TBool aParseNow=ETrue );

    /**
    * GetSubjectLC
    * Get text in subject field.
    * @return Descriptor containing subject field text.
    */
    HBufC* GetSubjectLC() const;

    /**
    * GetToFieldSelectionLength
    * Get the length of selected text in TO-field.
    * @return Selection length.
    */
    TInt GetToFieldSelectionLength() const;

    /**
    * GetCcFieldSelectionLength
    * Get the length of selected text in CC-field.
    * @return Selection length.
    */
    TInt GetCcFieldSelectionLength() const;

    /**
    * GetBccFieldSelectionLength
    * Get the length of selected text in BCC-field.
    * @return Selection length.
    */
    TInt GetBccFieldSelectionLength() const;

    /**
    * GetToFieldLength
    * Get TO-field text length.
    * @return Text length.
    */
    TInt GetToFieldLength() const;

    /**
    * GetCcFieldLength
    * Get CC-field text length.
    * @return Text length.
    */
    TInt GetCcFieldLength() const;

    /**
    * GetBccFieldLength
    * Get BCC-field text length.
    * @return Text length.
    */
    TInt GetBccFieldLength() const;
        
    /**
    * GetSubjectFieldLength
    * Get SUBJECT-field text length.
    * @return Text length.
    */
    TInt GetSubjectFieldLength() const;

    /**
    * GetMessageFieldLength
    * Get MESSAGE-field text length.
    * @return Text length.
    */
    TInt GetMessageFieldLength() const;

    /**
    * GetAttachmentCount
    * Get number of attachments.
    * @return Attachment count.
    */
    TInt GetAttachmentCount() const;

    /**
    * HasRemoteAttachments
    * Tells if the message has one or more remote attachments
    * @return ETrue if the message has at least one remote attachment.
    */
    TBool HasRemoteAttachments() const;

    /**
    * GetLookupTextLC
    * Get the text to match for fast addressing or remote lookup.
    * @return Match text.
    */
    HBufC* GetLookupTextLC() const;

    /**
    * SetToFieldAddressesL
    * Set TO-field addresses.
    * @param aAddress Array of address objects.
    */
    void SetToFieldAddressesL( 
            RPointerArray<CNcsEmailAddressObject>& aAddress );

    /**
    * SetCcFieldAddressesL
    * Set CC-field addresses.
    * @param aAddress Array of address objects.
    */
    void SetCcFieldAddressesL( 
            RPointerArray<CNcsEmailAddressObject>& aAddress );

    /**
    * SetBccFieldAddressesL
    * Set BCC-field addresses.
    * @param aAddress Array of address objects.
    */
    void SetBccFieldAddressesL( 
            RPointerArray<CNcsEmailAddressObject>& aAddress );

    /**
    * AppendToFieldAddressesL
    * Append TO-field addresses.
    * @param aAddress Array of address objects.
    */
    void AppendToFieldAddressesL( 
            RPointerArray<CNcsEmailAddressObject>& aAddress );

    /**
    * AppendCcFieldAddressesL
    * Append CC-field addresses.
    * @param aAddress Array of address objects.
    */
    void AppendCcFieldAddressesL( 
            RPointerArray<CNcsEmailAddressObject>& aAddress );

    /**
    * AppendBccFieldAddressesL
    * Append BCC-field addresses.
    * @param aAddress Array of address objects.
    */
    void AppendBccFieldAddressesL( 
            RPointerArray<CNcsEmailAddressObject>& aAddress );

    /**
    * SetSubjectL
    * Set SUBJECT-field text.
    * @param aSubject Text.
    */
    void SetSubjectL( const TDesC& aSubject );

    /**
    * SetCcFieldVisibleL
    * Set the visibility of CC-field.
    * @param aVisible True if the field is to be visible.
    * @param aFocus True if the field is to be focused.
    */
    void SetCcFieldVisibleL( TBool aVisible, TBool aFocus=ETrue );

    /**
    * SetBccFieldVisibleL
    * Set the visibility of BCC-field.
    * @param aVisible True if the field is to be visible.
    * @param aFocus True if the field is to be focused.
    */
    void SetBccFieldVisibleL( TBool aVisible, TBool aFocus=ETrue );

    /**
    * SetMenuBar
    * Set the menu bar.
    * @param aMenuBar The menu bar.
    */
    void SetMenuBar( CEikButtonGroupContainer* aMenuBar );

    /**
     * SetAttachmentLabelTextsL
     * Set attachment label informations from arrays of file name and
     * size informations. Some decorations may be added. 
     * The file name will be truncated autoamtically if needed.
     * The method takes ownership of the passed arrays.
     * @param  aAttachmentNames Descriptor array containing file names.
     * @param  aAttachmentSizes Descriptor array containing file sizes.
     */
    void SetAttachmentLabelTextsLD( CDesCArray* aAttachmentNames, 
                                    CDesCArray* aAttachmentSizes );

    /**
     * FocusedAttachmentLabelIndex
     * Returns the index of the attachment label that is currently focused
     * @return Index of the focused attachment label.
     */
    TInt FocusedAttachmentLabelIndex();
    
    /**
     * FixSemicolonL
     * Fixes possibly missing semicolon in message header address fields.
     */
    void FixSemicolonL();
    
    void HideAttachmentLabel();
 
    /**
    * IsCcFieldVisible
    * Test if CC-field is visible.
    * @return True if visible.
    */
    TBool IsCcFieldVisible() const;

    /**
    * IsBccFieldVisible
    * Test if BCC-field is visible.
    * @return True if visible.
    */
    TBool IsBccFieldVisible() const;

    /**
    * IsFocusTo
    * Test if TO-field is focused.
    * @return True if focused.
    */
    TBool IsFocusTo() const;

    /**
    * IsFocusCc
    * Test if CC-field is focused.
    * @return True if focused.
    */
    TBool IsFocusCc() const;

    /**
    * IsFocusBcc
    * Test if BCC-field is focused.
    * @return True if focused.
    */
    TBool IsFocusBcc() const;

    /**
    * IsFocusAttachments
    * Test if ATTACHMENTS-field is focused.
    * @return True if focused.
    */
    TBool IsFocusAttachments() const;
        
    /**
    * IncludeAddressL
    */
    void IncludeAddressL();

    /**
    * IncludeAddressL
    */
    void IncludeAddressL(const CNcsEmailAddressObject& eml);
        
    /**
    * ClosePopupContactListL
    * Closes the popup listbox.
    */
    void ClosePopupContactListL();

    /**
    * DeleteSelectionL
    * Deletes selected text in focused AIF
    */
    void DeleteSelectionL();

    /**
     * Set the contents of the message body.
     * @param aMessage Contents of MESSAGE-field.
     * @parem aReadOnlyQuote Contents of read-only quote field.
     */
    void SetBodyContentL( const TDesC& aMessage, const TDesC& aReadOnlyQuote );

    /**
     * Get the contents of message body. The message body consists of
     * MESSAGE-field followed by optional read-only quote field.
     * @return Message body.
     */
    HBufC* GetBodyContentLC();

    /**
     * BodyText
     * Gets modifiable reference to the text in body text field.
     */
    CRichText& BodyText();

    /**
    * SetFocusToToField
    * Focuses the TO-field.
    */
    void SetFocusToToField();

    /**
    * SetFocusToAttachmentField
    * Focuses the ATTACHEMENT-field.
    */
    void SetFocusToAttachmentField();
        
    /**
    * SetFocusToMessageFieldL
    * Focuses the MESSAGE-field.
    */
    void SetFocusToMessageFieldL();

    /**
    * AddQuickTextL
    * Inserts text in current cursor position.
    * @param aText Text to insert.
    */
    void AddQuickTextL( const TDesC& aText );
        
    /**
    * SelectAllToFieldTextL
    * Selects all TO-field text.
    */
    void SelectAllToFieldTextL();

    /**
    * SelectAllCcFieldTextL
    * Selects all CC-field text.
    */
    void SelectAllCcFieldTextL();

    /**
    * SelectAllBccFieldTextL
    * Selects all BCC-field text.
    */
    void SelectAllBccFieldTextL();

    /**
    * SelectAllSubjectFieldTextL
    * Selects all SUBJECT-field text.
    */
    void SelectAllSubjectFieldTextL();
			
    /**
    * IsSubjectFieldEmpty
    * Tells if SUBJECT-field is empty (has default text or is empty).
    * @return True if user has not put anything to the field.
    */
    TBool IsSubjectFieldEmpty();
		
    /**
    * AppendAddressesL
    * Displays contacts dialog and appends selected addresses in 
    * focused AIF.
    *
    * @return ETrue if addresses we tried to append, EFalse otherwise. 
    */
    TBool AppendAddressesL();
		
    /**
    * HandleAttachmentsOpenCommand
    * 
    */
    void HandleAttachmentsOpenCommandL();
		
    /**
    * LaunchStylusPopupMenu
    * 
    */
    void LaunchStylusPopupMenu( const TPoint& aPenEventScreenLocation );
    
    /**
    * AreAddressFieldsEmpty
    * 
    */
    TBool AreAddressFieldsEmpty();
		
    void DoPopupSelectL();
		
    void HandleLayoutChangeL();
    
    void HandleSkinChangeL();
    
    TInt ContentTotalHeight();
    /**
    * CommitL
    * Commits changes to message
    */
    void CommitL( TFieldToCommit aFieldToCommit = EAllFields );

    /**
     * SwitchChangeMskOff
     * Sets up iSwitchChangeMskOff falg, which disables changes of MSK label
     * if any popup dialog is open
     */
    void SwitchChangeMskOff(TBool aTag);

    /**
     * Scroll
     * @param aTargetPos Scrolls display to given position
     */
    void Scroll( TInt aTargetPos, TBool aDrawNow = ETrue );

public: // from CoeControl

    /**
    * Draw
    * Draw this CNcsComposeViewContainer to the screen.
    * @param aRect the rectangle of this view that needs updating
    */
    void Draw( const TRect& aRect ) const;
    
    void SetMskL();
    
    /**
    * Handle pointer event
    * 
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
	
    /**
    * OfferKeyEventL
    * Key event handler.
    * @param aKeyEvent The key event.
    * @param aType The type of key event.
    * @return Indicates whether the key event was used by this control.
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
    /**
    * SizeChanged
    * Responds to changes to the size and position of the contents of this control.
    */
    void SizeChanged();
        
    /**
    * Responds to a change in focus.
    * @param aDrawNow Contains the value that was passed to SetFocus.
    */
    void FocusChanged(TDrawNow aDrawNow);
        
    /**
    * MopSupplyObject
    * Retrieves an object of the same type as that encapsulated in aId.
    * @param aId An encapsulated object type ID.
    */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    
	/**
    * Informs if Remotesearch is in progress
    */
    TBool IsRemoteSearchInprogress() const;
    
public: // from MEikScrollBarObserver
    
    /**
    * HandleScrollEventL
    */
    void HandleScrollEventL( 
            CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );

public: // from MFSEmailUiContactHandlerObserver

    void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CFSEmailUiClsItem>& aContacts );
    void OperationErrorL( TContactHandlerCmd aCmd, TInt aError );

public: // from base class MEikEdwinObserver

    void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );

public: // from base class MEikEdwinSizeObserver
    
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, 
            TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize );
    
public:  // from MNcsFieldSizeObserver

    TBool UpdateFieldSizeL( TBool aDoScroll );
    void UpdateFieldPosition( CCoeControl* aAnchor );

private:  //From MAknLongTapDetectorCallBack
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
            const TPoint& aPenEventScreenLocation );
    
private:
    
    /**
    * Constructor
    * @param aView The parent view
    */
    CNcsComposeViewContainer( CNcsComposeView& aView, CFsAutoSaver& aAutoSaver,
        CFSMailBox& aMailBox );

    /**
    * ConstructL
    * 2nd phase constructor
    * @param aRect Rectangle where container is drawn to.
    */
    void ConstructL( const TRect& aRect, TInt aFlags );
        
    /**
    * ChangeFocusL
    * Changes focus according to key event and focused control.
    * @param aKeyEvent The key event.
    * @return Indicates whether the key event was used by this control.
    */
    TKeyResponse ChangeFocusL( const TKeyEvent& aKeyEvent );
        
    /**
    * UpdateScreenPositionL
    * @param aKeyCode Needed to recognize presses of the enter key
    *                 in moving of display.
    */
    void UpdateScreenPositionL( const TUint& aKeyCode = EKeyNull );

    /**
     * CalculateSeparatorLineSecondaryColor
     * @return Separator line secondary color
     */
    TRgb CalculateSeparatorLineSecondaryColor();
    
    void UpdatePhysicsL();

private: // from MAknPhysicsObserver

   /**
    * @see MAknPhysicsObserver::ViewPositionChanged
    */
   virtual void ViewPositionChanged( const TPoint& aNewPosition, 
           TBool aDrawNow, TUint aFlags );

   /**
    * @see MAknPhysicsObserver::PhysicEmulationEnded
    */
   virtual void PhysicEmulationEnded();

   /**
    * @see MAknPhysicsObserver::ViewPosition
    */
   virtual TPoint ViewPosition() const;

private:

   void DoUpdateSubjectL();

private: // data
    
    /**
    * header container
    */
    CNcsHeaderContainer* iHeader;
    
    /**
    * message body field
    */
    CNcsEditor* iMessageField;
        
    /**
    * field for read-only quote used with some protocols on reply/forward
    */
    CNcsEditor* iReadOnlyQuoteField;
        
    /**
    * currently focused control
    */
    CCoeControl* iFocused;
    
    /**
     * Long tap detector.
     * Owned.
     */
    CAknLongTapDetector* iLongTapDetector;
    
    /**
    *
    */
    TPoint iContentBasePoint;
        
    /**
    *
    */
    CAknDoubleSpanScrollBar* iScrollBar;
        
    /**
    *
    */
    TAknDoubleSpanScrollBarModel iScrollBarModel;
        
    /**
    *
    */
    TRect iApplicationRect;
        
    /**
    *
    */
    HBufC* iLengthReference;
        
    /**
    * owner view
    */
    CNcsComposeView& iView;
        
    TInt iAmountMesFieldMovedUp;
        
    CFsAutoSaver& iAutoSaver;

    CAknsBasicBackgroundControlContext* iBgContext;

    /** Reference to current mailbox */
    CFSMailBox& iMailBox;
		
    TAppendAddresses iAppendAddresses;

    TInt iSeparatorLineYPos;
    
    // panning related
    CAknPhysics* iPhysics;
    TPoint iPreviousPosition;
    TPoint iOriginalPosition;
    TBool iIsDragging;
    TBool iIsFlicking;
    TTime iStartTime;
    
    // for body editor row line drawing
    TInt iPrevDesiredHeigth;
    TInt iMessageEditorMinHeigth;

    // not owned contact handler pointer
    CFSEmailUiContactHandler* iContactHandler;

    // Descriptor for read-only quote text. Own.
    HBufC* iReadOnlyQuote;

    TInt iTotalComposerHeight; 
    TInt iVisibleAreaHeight;
    TInt iTotalMoveY;
    TInt iSeparatorHeight;
    };


#endif
