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
* Description: This file defines class CNcsAifEntry.
*
*/



#ifndef CNCSAIFEDITOR_H
#define CNCSAIFEDITOR_H


#include "ncseditor.h"


class CNcsEmailAddressObject;
class MNcsAddressPopupList;
class CNcsAifEntry;


/**
*  CNcsAifEditor class.
*/
class CNcsAifEditor: public CNcsEditor, public MEikEdwinObserver
    {
public: // constructors/destructor
    
    CNcsAifEditor( MNcsFieldSizeObserver* aSizeObserver, const TDesC& aCaptionText );
    	
    void ConstructL( const CCoeControl* aParent,
                     TInt aNumberOfLines,
                     TInt aTextLimit );    

    virtual ~CNcsAifEditor();
    
public: // from CoeControl
    
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

public: // from MEikEdwinObserver
    
    void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);

public: // new functions
    
    virtual TInt LineCount() const;
        
    virtual TInt CursorLineNumber() const;
        
    void UpdateAddressListAllL();

    void CopyToStoreL(
        CStreamStore& aStore, 
        CStreamDictionary& aDict );
        
    void CheckAddressWhenFocusLostL();

    void ParseNewAddressL();
    
    TKeyResponse HandleContactDeletionL(const TKeyEvent& aKeyEvent, TEventCode aType);
    
    const CNcsEmailAddressObject* EmailAddressObjectBySelection() const;

    /**
    * Get the text to match in either the local address book
    * or the remote directory server
    */
    HBufC* GetLookupTextLC() const;

    void AddAddressL(
        const TDesC& aDN, 
        const TDesC& aEmail,
        TBool aDisplayFull = EFalse,
        TBool aUpdateEditorText = ETrue );
		
    void AddAddressL( const CNcsEmailAddressObject& aAddress, TBool aUpdateEditorText = ETrue );

    void SetAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddress );

    void AppendAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddress );
		
    const RPointerArray<CNcsEmailAddressObject>& GetAddressesL();
		
    void RecalculateEntryPositions();

    inline void SetPopupList(MNcsAddressPopupList* aPopupList);
    
private: // new functions

    enum TEntryDirection
        {
        EDirectionNone,
        EDirectionRight,
        EDirectionLeft
        };

    void UpdateAddressAutoCompletionL();
    void UpdateAddressAutoCompletionL( const TCursorSelection& aSelection );

    void DoCharChangeL();

    TChar CharAtPos( TInt aPos ) const;

    void AddAddressL( CNcsAifEntry* aEntry, TBool aUpdateEditorText );

    TKeyResponse SetEditorSelectionL( const TKeyEvent& aKeyEvent, TEventCode aType );

    TInt CalculateAddressListLength(
        RPointerArray<CNcsAifEntry>& aEntries,
        TBool aDisplayList = ETrue ) const;

    /**
    * Creates formatted address list of given entries.
    *
    * @param aEntries Entries from which list is constructed.
    * @param aDisplayList If ETrue, constructs list of display names
    *                     of the addresses. If EFalse, constructs list
    *                     of email addresses.
    */
    HBufC* GetFormattedAddressListLC( 
        RPointerArray<CNcsAifEntry>& aEntries,
        TBool aDisplayList = ETrue ) const;

    HBufC* GetFormattedAddressListL(
        RPointerArray<CNcsAifEntry>& aEntries,
        TBool aDisplayList = ETrue ) const;

    void RepositionEntriesL( const CNcsAifEntry* aEntry );

    void CheckAndRemoveInvalidEntriesL();
    
    CNcsAifEntry* GetEntryAt( 
        TInt aPos, 
        TEntryDirection aDirection = EDirectionNone ) const;
    
    /** Browse backwards from given position and return the first found entry */
    CNcsAifEntry* GetPreviousEntryFrom( TInt aPos ) const;
    
    HBufC* GetNonEntryTextLC() const;

    TKeyResponse CopyEntriesToClipboardL(
        const TKeyEvent& aKeyEvent, 
        TEventCode aType );

    void FindSelectedEntriesL( RPointerArray<CNcsAifEntry>& aEntries );

	// <cmail> fixed CS high cat. finding
    void UpdateDuplicateEntryMarkingsL();

    void HandleTextUpdateDeferred();
    static TInt DoHandleTextUpdate( TAny* aSelf );
    
    void HandleTextUpdateL();
    /** 
     * Handle addition of complete address. May result in complete redraw of the field.
     * @return  ETrue   if terminated recipient entry was found and added to array. 
     */
    TBool HandleTextUpdateL( const TCursorSelection& aSelection );
    void HandleNavigationEventL();
    TCursorSelection NonEntryTextAtPos( TUint aPosition ) const;
    TCursorSelection NonEntryTextBeforePos( TUint aPosition ) const;
    TBool IsSentinel( TChar aCharacter ) const;
    TBool IsWhitespace( TChar aCharacter ) const;
    TBool IsNavigationKey( const TKeyEvent& aKeyEvent ) const;
    TBool IsCharacterKey( const TKeyEvent& aKeyEvent ) const;
    void GetMatchingEntryCountsL( 
            const CNcsAifEntry* aEntry,
            TInt& aNrOfMatchesInText,
            TInt& aNrOfMatchesInEntryArray );
    void CompleteEntryL();

private: // data

    RPointerArray<CNcsAifEntry> iArray;

    RPointerArray<CNcsEmailAddressObject> iAddressArray;
		
    MNcsAddressPopupList* iAddressPopupList;
	
    CAsyncCallBack* iAsyncCallBack;

    TInt iLastTimeCursorPos;
    
    // Flag indicating whether add the leftover text to the end of the field
    TBool iAddLeftover;
    TCursorSelection  iTextSelection;
    TBool             iPartialRemove;
	};


///////////////////////////////////////////////////////////////////////
// CNcsAifEntry
// This class represents a full entry in the AIF field.
// It is derived from TCursorSelection which helps in atomizing the
// items when the cursor moves over them.
// NOTES:
//		iAnchorPosition is non-inclusive.  This position represents
//		the anchor of a selection and is not included in the selection
//		count.  So the selection would be iCursorPosition to iAnchorPosition
//		- 1.  The SetSelection function will highlight anything left of the
//		anchor up to and including the cursor.
//		The idea is that if you were to use shift-cursor to select text,
//		The anchor would be the cursor you started to hold down the shift.
//		Since this location is not included in the selection, but rather
//		the first position left or right of this poistion, the anchor is
//		never included in the selection.
class CNcsAifEntry : public CBase, public TCursorSelection
    {
public:

    static CNcsAifEntry* CNcsAifEntry::NewL( const CNcsEmailAddressObject& aAddr );
	
	static CNcsAifEntry* CNcsAifEntry::NewL(
        const TDesC& aDn, 
        const TDesC& aEml,
        TBool aDisplayFull );
		
	CNcsAifEntry::~CNcsAifEntry();
	
private:

	CNcsAifEntry();
		
	void ConstructL( const TDesC& aDn, const TDesC& aEml, TBool aDisplayFull );
		
	void ConstructL( const CNcsEmailAddressObject& aAddress );
		
	void ConstructL() ;
		
	void SetDisplayStringL(); 
		
public:

	inline TInt DisplayLength() const;
		
	inline const TDesC& DisplayString() const;
		
	inline TInt SetPos(TInt aPos);
	
	inline TBool Includes(TInt aPos) const;
	
	inline TInt Start() const;
		
	inline TInt End() const;
		
	inline const CNcsEmailAddressObject& Address() const;
		
	inline TBool IsDup() const; 
	
	void SetDupL( TBool aDup = ETrue );
		
    TBool IsSameDN(const CNcsAifEntry& aEntry) const;
		
private:

	CNcsEmailAddressObject* iAddress;
	HBufC* iDisplayString;
	
    TBool iIsDup;
    };

#include "ncsaifeditor.inl"

#endif // CNCSSUBJECTFIELD_H


// End of File
