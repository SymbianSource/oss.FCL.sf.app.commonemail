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
* Description: This file defines classes MESMRNcsAddressPopupList, CESMRNcsAifEditor, CESMRNcsAifEntry
*
*/


#ifndef CESMRNCSAIFEDITOR_H
#define CESMRNCSAIFEDITOR_H

#include "cesmrncseditor.h"

class CESMRNcsEmailAddressObject;
class CESMRNcsAifEntry;
class CESMRContactHandler;

/**
 *  MESMRNcsAddressPopupList observer to the contact search pop up list box
 */
class MESMRNcsAddressPopupList
    {
public:
    /**
     * Handle and update serach strings events of the list box
     * @param aMatchString string to send to the search engines
     * @param iListAll update all or single entry
     */
    virtual void UpdatePopupContactListL( const TDesC& aMatchString, TBool iListAll ) = 0;
    
    /**
     * Handle closing of popup search box
     */
    virtual void ClosePopupContactListL() = 0;
    };

/**
 *  CNcsAifEditor handles the editor and arrays of the email address lists for attendee field.
 */
NONSHARABLE_CLASS( CESMRNcsAifEditor ) : public CESMRNcsEditor, public MEikEdwinObserver
    {
public: // constructors/destructor
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * @param aContactHandler reference to contact handler class
     * @param default text of the editor
     * @returns CESMRClsItem instance
     */
    CESMRNcsAifEditor( CESMRContactHandler& aContactHandler, HBufC* aDefaultText );

    /**
     * C++ Virtual Destructor.
     */
    virtual ~CESMRNcsAifEditor();

public: // new functions
    /**
     * Count number of lines of the editor
     * @return number of lines of the editor
     */
    virtual TInt LineCount() const;

    /**
     * Get number of the current line of the editor
     * @return current line number of the editor
     */
    virtual TInt CursorLineNumber() const;

public: // from CoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

public: // Interface    
    /**
     * Parse new address when focus is lost
     */
    void CheckAddressWhenFocusLostL();

    /**
     * Parse new address
     * @return ETrue if there was any text available
     */
    TBool ParseNewAddressL();

    /**
     * Handle the key event to delete a contact in the editor
     * @param aKeyEvent key event to be handled
     * @param aType TKeyEvent TEventCode type to be handled 
     */
    TKeyResponse HandleContactDeletionL(const TKeyEvent& aKeyEvent, TEventCode aType);

    const CESMRNcsEmailAddressObject* EmailAddressObjectBySelection() const;

    /**
     * Add address to the editor the editor
     * @param aDN Descriptor of the address
     * @param aEmail email descriptor (address) 
     * @param aDisplayFull choose to display the description fully 
     */
    void AddAddressL( const TDesC& aDN, const TDesC& aEmail, TBool aDisplayFull = EFalse );

    /**
     * Add address to the editor the editor
     * @param email address object to display in thye editor
     */
    void AddAddressL(const CESMRNcsEmailAddressObject& aAddress);

    /**
    * Get the text to match in either the local address book
    * or the remote directory server
    * @param descriptor of the current search text
    */
    HBufC* GetLookupTextLC() const;

    /**
     * Reset and fill the editor with CESMRNcsEmailAddressObjects
     * @param aAddress array of CESMRNcsEmailAddressObjects
     */
    void SetAddressesL( const RPointerArray<CESMRNcsEmailAddressObject>& aAddress );

    /**
     * Append the editor with CESMRNcsEmailAddressObjects
     * @param aAddress array of CESMRNcsEmailAddressObjects
     */
    void AppendAddressesL( const RPointerArray<CESMRNcsEmailAddressObject>& aAddress );

    /**
     * Get currently available array of  CESMRNcsEmailAddressObjects
     * @return currently available array of CESMRNcsEmailAddressObjects
     */
    const RPointerArray<CESMRNcsEmailAddressObject>& GetAddressesL();

    /**
     * Find out where address objects can be added
     */
    void RecalculateEntryPositions();

    /**
     * Update all editors address list arrays
     */
    void UpdateAddressListAllL();

    /**
     * Set array of the popup list
     * @param aPopupList array of the aPopupList
     */
    inline void SetPopupList(MESMRNcsAddressPopupList* aPopupList);
    
public: // From MEikEdwinObserver
    void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);

private: // Implementation
    enum TEntryDirection
        {
        EDirectionNone,
        EDirectionRight,
        EDirectionLeft
        };

    /**
    * Creates formatted address list of given entries.
    * and leave it on the cleanupstack
    *
    * @param aEntries Entries from which list is constructed.
    * @param aDisplayList If ETrue, constructs list of display names
    *                     of the addresses. If EFalse, constructs list
    *                     of email addresses.
    */
    HBufC* GetFormattedAddressListLC(
        RPointerArray<CESMRNcsAifEntry>& aEntries,
        TBool aDisplayList = ETrue ) const;

    /**
    * Creates formatted address list of given entries.
    *
    * @param aEntries Entries from which list is constructed.
    * @param aDisplayList If ETrue, constructs list of display names
    *                     of the addresses. If EFalse, constructs list
    *                     of email addresses.
    */
    HBufC* GetFormattedAddressListL(
        RPointerArray<CESMRNcsAifEntry>& aEntries,
        TBool aDisplayList = ETrue ) const;

    TInt CalculateAddressListLength(
        RPointerArray<CESMRNcsAifEntry>& aEntries,
        TBool aDisplayList = ETrue ) const;
    
    void RepositionEntriesL( const CESMRNcsAifEntry* aEntry );
    CESMRNcsAifEntry* GetEntryAt( TInt aPos, TEntryDirection aDirection = EDirectionNone ) const;
    HBufC* GetNonEntryTextLC() const;
    void UpdateAddressAutoCompletionL();
    void UpdateAddressAutoCompletionL(const TCursorSelection& aSelection );
    void UpdateDuplicateEntryMarkings();
    void DoCharChangeL();
    TChar CharAtPos( TInt aPos ) const;
    void AddAddressL( CESMRNcsAifEntry* aEntry );
    TKeyResponse SetEditorSelectionL( const TKeyEvent& aKeyEvent, TEventCode aType );
    TKeyResponse CopyEntriesToClipboardL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void FindSelectedEntriesL( RPointerArray<CESMRNcsAifEntry>& aEntries );
    void CopyToClipboardL( const TDesC &aBuf );
    void HandleContactDeletionL();
    void HandleTextUpdateL();
    void HandleTextUpdateL( TCursorSelection& aSelection );
    void HandleNavigationEventL();
    TCursorSelection NonEntryTextAtPos( TUint aPosition ) const;
    TCursorSelection NonEntryTextBeforePos( TUint aPosition ) const;
    TBool IsSentinel( TChar aCharacter ) const;
    TBool IsNavigationKey( const TKeyEvent& aKeyEvent ) const;
    void AddEntryL( TInt aFirstCharacter, TInt aLastCharacter );

private: // data
    RPointerArray<CESMRNcsAifEntry> iArray;
    RPointerArray<CESMRNcsEmailAddressObject> iAddressArray;
    MESMRNcsAddressPopupList* iAddressPopupList;
    CESMRContactHandler& iContactHandler;
    };

///////////////////////////////////////////////////////////////////////
// CESMRNcsAifEntry
// This class represents a full entry in the AIF field.
// It is derived from TCursorSelection which helps in atomizing the
// items when the cursor moves over them.
// NOTES:
//      iAnchorPosition is non-inclusive.  This position represents
//      the anchor of a selection and is not included in the selection
//      count.  So the selection would be iCursorPosition to iAnchorPosition
//      - 1.  The SetSelection function will highlight anything left of the
//      anchor up to and including the cursor.
//      The idea is that if you were to use shift-cursor to select text,
//      The anchor would be the cursor you started to hold down the shift.
//      Since this location is not included in the selection, but rather
//      the first position left or right of this poistion, the anchor is
//      never included in the selection.
class CESMRNcsAifEntry : public CBase, public TCursorSelection
    {
public:
    /**
     * Two-phased constructor.
     * Creates a new instance of class CESMRNcsAifEntry
     * @addr aContactHandler reference to contact handler class
     * @aDisplayFull default text of the editor
     * @returns Pointer to CESMRNcsAifEntry
     */
    static CESMRNcsAifEntry* NewL(
        const CESMRNcsEmailAddressObject& addr,
        TBool aDisplayFull );

    static CESMRNcsAifEntry* NewL(
        const TDesC& dn,
        const TDesC& eml,
        TBool aDisplayFull );

    /**
     * C++ Virtual Destructor.
     */
    ~CESMRNcsAifEntry();

public:
    /**
     * Count number of characters of the entry
     * @return number of characters of the entry
     */
    inline TInt DisplayLength() const;

    /**
     * Get the entry display string
     * @return descriptor containing the display string
     */
    inline const TDesC& DisplayString() const;

    /**
     * Setters and getters of the intry
     */
    inline TInt SetPos(int pos);
    inline TBool Includes(int pos) const;
    inline TInt Start() const;
    inline TInt End() const;

    /**
     * Get the CESMRNcsEmailAddressObject of this entry
     * @return the CESMRNcsEmailAddressObject of the entry
     */
    inline const CESMRNcsEmailAddressObject& Address() const;

    /**
     * Test if entry has the same description
     */
    inline TBool IsSameDN(const CESMRNcsAifEntry& entry) const;

    /**
     * Test for duplicates
     */
    inline TBool IsDup() const;

    /**
     * Set entry as a duplicate
     */
    inline void SetDup( TBool aDup = ETrue );
    
private: // Implementation
    CESMRNcsAifEntry( TBool aDisplayFull = EFalse );
    void ConstructL( const TDesC& dn, const TDesC& eml );
    void ConstructL( const CESMRNcsEmailAddressObject& aAddress );
    void ConstructL() ;
    void SetDisplayStringL();
    
private: // data
    CESMRNcsEmailAddressObject* iAddress; //own
    HBufC* iDisplayString; //own
    // flag indicating that both name and email address should be shown
    // there is many reasons why this is wanted, for example when
    // contact has multiple email addresses
    TBool iDisplayFull;
    TBool iIsDup;
    };

#include "cesmrncsaifeditor.inl"

#endif // CESMRNCSAIFEDITOR_H

// End of File
