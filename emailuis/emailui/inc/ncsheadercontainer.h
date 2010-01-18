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
* Description:  Container class for email header
*
*/



#ifndef __NCSEMAILHEADERUI_H__
#define __NCSEMAILHEADERUI_H__

#include <aknlongtapdetector.h>

#include "cpbkxremotecontactlookupenv.h"
#include "cpbkxremotecontactlookupserviceuicontext.h"

#include "FreestyleEmailUi.hrh"
#include "ncsaddressinputfield.h"
#include "ncsfieldsizeobserver.h"
#include "ncsconstants.h"


class CFSMailBox;
class CNcsAttachmentField;
class CNcsEmailAddressObject;
class CNcsPopupListBox;
class CNcsSubjectField;

/**
*  CNcsHeaderContainer
*/
class CNcsHeaderContainer : public CCoeControl, public MNcsAddressPopupList
    {

public:

    /** Initialisation flags. */
    enum TInitFlags
        {
        ECcFieldVisible  = 0x01,
        EBccFieldVisible = 0x02
        };

    /**
     * Two-phased constructor.
     * Create a CNcsHeaderContainer object, which will draw itself to aRect
     * @param aRect The rectangle this view will be drawn to.
     * @param aMailBox reference to current mailbox item
     * @param aFlags Initialisation flags.
     * @return a pointer to the created instance of CNcsHeaderContainer.
     */
    static CNcsHeaderContainer* NewL( CCoeControl& aParent,
        CFSMailBox& aMailBox, TInt aFlags = NULL );

    /**
     * ~CNcsHeaderContainer
     * Destructor
     */
    virtual ~CNcsHeaderContainer();

private: // constructor/destructor

    /**
    * CNcsHeaderContainer
    * C++ constructor.
    * @param aParent Parent control.
    * @param aMailBox reference to current mailbox item
    */
	CNcsHeaderContainer( CCoeControl& aParent, CFSMailBox& aMailBox );
	
	/**
	* ConstructL
	* 2nd phase constructor.
	*/
	void ConstructL( TInt aFlags );

public: // function members
	
	/**
	* GetToFieldAddressesL
	* Get addresses in TO-field.
	* @return Array of addresses.
	*/
	const RPointerArray<CNcsEmailAddressObject>& GetToFieldAddressesL( 
        TBool aParseNow = ETrue );

	/**
	* GetCcFieldAddressesL
	* Get addresses in CC-field.
	* @return Array of addresses.
	*/
	const RPointerArray<CNcsEmailAddressObject>& GetCcFieldAddressesL( 
        TBool aParseNow = ETrue );

	/**
	* GetBccFieldAddressesL
	* Get addresses in BCC-field.
	* @return Array of addresses.
	*/
	const RPointerArray<CNcsEmailAddressObject>& GetBccFieldAddressesL( 
        TBool aParseNow = ETrue );

	/**
	* GetSubjectLC
	* Get text in SUBJECT-field.
	* @return Subject text.
	*/
	HBufC* GetSubjectLC() const;

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
	* GetToFieldSelectionLength
	* Get selected text length in TO-field.
	* @return Text length.
	*/
	TInt GetToFieldSelectionLength() const;

	/**
	* GetCcFieldSelectionLength
	* Get selected text length in CC-field.
	* @return Text length.
	*/
	TInt GetCcFieldSelectionLength() const;

	/**
	* GetBccFieldSelectionLength
	* Get selected text length in BCC-field.
	* @return Text length.
	*/
	TInt GetBccFieldSelectionLength() const;

	/**
	* GetLookupTextLC
	* Get lookup text in focused AIF.
	* @return Lookup text.
	*/
	HBufC* GetLookupTextLC() const;
	
	/**
	* SetToFieldAddressesL
	* Set addresses in TO-field.
	* @param aAddress Address array.
	*/
	void SetToFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress );

	/**
	* SetCcFieldAddressesL
	* Set addresses in CC-field.
	* @param aAddress Address array.
	*/
	void SetCcFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress );

	void SetBccFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress );

	void AppendToFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress );
	
	void AppendCcFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress );
	
	void AppendBccFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress );
	
	void SetSubjectL( const TDesC& aSubject );
	
	void SetMenuBar( CEikButtonGroupContainer* aMenuBar );
	
    void SetAttachmentLabelTextsLD( CDesCArray* aAttachmentNames, 
                                    CDesCArray* aAttachmentSizes );
    TInt FocusedAttachmentLabelIndex();
    void ShowAttachmentLabelL();
	void HideAttachmentLabel();

	void SetCcFieldVisibleL( 
        TBool aVisible, 
        TDrawNow aDrawNow = EDrawNow, 
        TBool aFocus = ETrue );

	void SetBccFieldVisibleL( 
        TBool aVisible, 
        TDrawNow aDrawNow = EDrawNow, 
        TBool aFocus = ETrue );
	
	void FixSemicolonInAddressFieldsL();

	TBool IsBccFieldVisible() const;

	TBool IsCcFieldVisible() const;

	TBool IsFocusAttachments() const;

	TBool IsFocusTo() const;

	TBool IsFocusCc() const;

	TBool IsFocusBcc() const;

	void IncludeAddressL();

	void IncludeAddressL( const CNcsEmailAddressObject& eml );

	// Update the field positions arround the anchor
    void UpdateFieldPosition( CCoeControl* aAnchor );

	// Check if the AIF menu should be displayed
	TBool NeedsAifMenu() const;

	// Get the total number of scrollable lines to update the scroll bar
	TInt ScrollableLines() const;

	// Get the total line count of all the controls in this container.
	TInt LineCount() const;
	
	// Get the current cursor position relative to the top of the container.
	TInt CursorPosition() const;

    // Get the cursor line number.
    TInt CursorLineNumber() const;

	void HandleControlArrayEventL(
		CCoeControlArray::TEvent aEvent,
		const CCoeControlArray *aArray,
		CCoeControl *aControl,
		TInt aControlId );

	//Set Middlesoftkey
	void SetMskL();
	
	void HandlePointerEventL( const TPointerEvent& aPointerEvent );
	
	void OpenPhonebookL();
	
	void HandleLongTap( const TPoint& aPenEventLocation, 
						 const TPoint& aPenEventScreenLocation );

    TBool NeedsLongTapL( const TPoint& aPenEventLocation );
	
	// Process a key event
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

	// Delete the selected address from the current AIF
	void DeleteSelectionL();
	
	TInt GetNumChars() const;

	void SelectAllToFieldTextL();

	void SelectAllCcFieldTextL();

	void SelectAllBccFieldTextL();

	void SelectAllSubjectFieldTextL();
	
	void FocusToField();

	void FocusAttachmentField();

    void SetPriority( TMsgPriority aPriority );

    void SetFollowUp( TBool aFollowUp );
    
    TBool AreAddressFieldsEmpty() const;
    
    void DoPopupSelectL();
    
    TBool IsPopupActive() const;

	void ClosePopupContactListL();
	
	void ShowPopupMenuBarL( TBool aShow );

	TBool IsToFieldEmpty() const;

	TBool IsCcFieldEmpty() const;

	TBool IsBccFieldEmpty() const;

    TBool IsSubjectFieldEmpty() const;

    void HandleDynamicVariantSwitchL();

    // sets up iSwitchChangeMskOff falg 
    void SwitchChangeMskOff(TBool aTag);
    
    virtual TInt LayoutLineCount() const;

    TBool IsAddressInputField( const CCoeControl* aControl ) const;

    TBool IsRemoteSearchInprogress() const;
private: // Function members

	void FocusChanged(TDrawNow aDrawNow);

	void UpdatePopupContactListL( const TDesC& aMatchString, TBool aListAll );

	CCoeControl* FindFocused() const;

	void Draw( const TRect& aRect ) const;

	void DrawAttachmentFocusNow();

	TKeyResponse ChangeFocusL( const TKeyEvent& aKeyEvent );

	void SizeChanged();

	void PositionChanged();

	TInt GetTotalHeight() const;

	void ChangePositions();

	TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    TRect CalculatePopupRect();

    void HandleAttachmentsOpenCommand();
    
    /**
    * Execute Remote Lookup Search.
    *
    * @param aExitReason RCL exit reason.
    * @param aSearchText Search text.
    * @return Selected email address object.
    */
    CNcsEmailAddressObject* ExecuteRemoteSearchL(
        CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason& 
            aExitReason,
        const TDesC& aSearchText );

    void ChangeMskCommandL( TInt aLabelResourceId );

    void CommitFieldL( CCoeControl* aField );

private:  //From MAknLongTapDetectorCallBack
    
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
            const TPoint& aPenEventScreenLocation );
    
private: // Data members

    /*
    * Parent window
    * Not Own
    */
    CCoeControl& iParent;

    MNcsFieldSizeObserver& iFieldSizeObserver;
    
    CAknLongTapDetector*      iLongTapDetector;
	
    CEikButtonGroupContainer* iMenuBar;
    
    CNcsAddressInputField* iToField;

    CNcsAddressInputField* iCcField;

    CNcsAddressInputField* iBccField;

    CNcsSubjectField* iSubjectField;

    CNcsAttachmentField* iAttachmentField;

	// The attachments count
    TInt iAttachmentCount;

	// Address popup data members
	CNcsPopupListBox* iAacListBox;

	CFSMailBox& iMailBox;
	
    //flag which disables changes of MSK label if any popup dialog is open
    TBool iSwitchChangeMskOff;

    CAknsBasicBackgroundControlContext* iBgContext;
    
    TBool iLongTapEventConsumed;
    TBool iRALInProgress;
    };


#endif
