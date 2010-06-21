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
* Description: This file implements classes CNcsAifEntry, CNcsAifEditor. 
*
*/



#include "emailtrace.h"
#include <AknsDrawUtils.h>
#include <s32mem.h>
#include <txtrich.h>
#include <baclipb.h>
#include <PtiDefs.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>

#include "ncsaifeditor.h"
#include "ncsconstants.h"
#include "ncsaddressinputfield.h"
#include "ncsutility.h"
#include "FreestyleEmailUiUtilities.h"
#include "ncsemailaddressobject.h"
#include "FreestyleEmailUiLayoutData.h"
#include "FSDelayedLoader.h"
#include "FSEmail.pan"

const TChar KCharAddressDelimeterSemiColon = ';';
const TChar KCharAddressDelimeterComma = ',';
const TChar KCharSpace = ' ';
const TChar KCharAt = '@';

// ---------------------------------------------------------------------------
// CNcsAifEntry::NewL
// ---------------------------------------------------------------------------
//
CNcsAifEntry* CNcsAifEntry::NewL( const CNcsEmailAddressObject& aAddr )
    {
    FUNC_LOG;
    CNcsAifEntry* self = new (ELeave) CNcsAifEntry;
    CleanupStack::PushL(self);
    self->ConstructL( aAddr );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::NewL
// ---------------------------------------------------------------------------
//
CNcsAifEntry* CNcsAifEntry::NewL(
    const TDesC& aDn, 
    const TDesC& aEml,
    TBool aDisplayFull )
    {
    FUNC_LOG;
    CNcsAifEntry* self = new ( ELeave ) CNcsAifEntry;
    CleanupStack::PushL( self );
    self->ConstructL( aDn, aEml, aDisplayFull );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::~CNcsAifEntry
// ---------------------------------------------------------------------------
//
CNcsAifEntry::~CNcsAifEntry()
    {
    FUNC_LOG;
    delete iAddress;
    delete iDisplayString;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::CNcsAifEntry
// ---------------------------------------------------------------------------
//
CNcsAifEntry::CNcsAifEntry() 
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CNcsAifEntry::ConstructL( const TDesC& aDn, const TDesC& aEml, TBool aDisplayFull )
    {
    FUNC_LOG;
    iAddress = CNcsEmailAddressObject::NewL( aDn, aEml );
    iAddress->SetDisplayFull( aDisplayFull );
    ConstructL();
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CNcsAifEntry::ConstructL( const CNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    iAddress = CNcsEmailAddressObject::NewL( aAddress );
    ConstructL();
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CNcsAifEntry::ConstructL() 
    {
    FUNC_LOG;
    SetDisplayStringL();
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::SetDisplayStringL
// ---------------------------------------------------------------------------
//
void CNcsAifEntry::SetDisplayStringL() 
    {
    FUNC_LOG;
    delete iDisplayString;
    iDisplayString = NULL;

    const TDesC& dname = iAddress->DisplayName();
    const TDesC& email = iAddress->EmailAddress();
    
    TInt dnameLength = dname.Length();
    TInt emailLength = email.Length();
    
    TBool displayFull = iAddress->DisplayFull() || iIsDup;
    
    TInt length;
    // Show only display name OR email address if showing both is not required
    // or if display name doesn't contain anything but the email address
    // or if the display name is empty
    if ( !displayFull ||
         dname == email ||
         !dnameLength )
        {
        length = dnameLength > 0 ? dnameLength : emailLength;
        length += KEmailAddressSeparator().Length(); // ';'
        
        iDisplayString = HBufC::NewL( length );
        TPtr ptr = iDisplayString->Des();
        
        ptr.Append( dname.Length() > 0 ? dname : email );
        ptr.Append( KEmailAddressSeparator );
        }
    
    // Otherwise, show both display name and email addresss
    else 
        {
        // Display, Name;
        length = dnameLength + emailLength + 
            KSpace().Length() + 
            KEmailAddressDecorationHead().Length() +
            KEmailAddressDecorationTail().Length() +
            KEmailAddressSeparator().Length();
        
        iDisplayString = HBufC::NewL( length );
        TPtr ptr = iDisplayString->Des();
        
        ptr.Append( dname );
        ptr.Append( KSpace );
        ptr.Append( KEmailAddressDecorationHead );
        ptr.Append( email );
        ptr.Append( KEmailAddressDecorationTail );
        ptr.Append( KEmailAddressSeparator );
        }
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::SetDupL
// ---------------------------------------------------------------------------
//
void CNcsAifEntry::SetDupL( TBool aDup ) 
    {
    FUNC_LOG;
    if ( iIsDup != aDup )
        {
        iIsDup = aDup;
        // Display string needs to be recreated unless there's no
        // meaningful display name
        if ( iAddress->DisplayName().Length() &&
             iAddress->DisplayName() != iAddress->EmailAddress() )
            {
            SetDisplayStringL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::IsSameDN
// ---------------------------------------------------------------------------
//
TBool CNcsAifEntry::IsSameDN( const CNcsAifEntry& entry ) const
    {
    FUNC_LOG;
    const TDesC& ownDn = Address().DisplayName();
    const TDesC& otherDn = entry.Address().DisplayName();
    return ownDn.Compare( otherDn ) == 0;
    }

// ---------------------------------------------------------------------------
// constructor/destructor
// ---------------------------------------------------------------------------
//
CNcsAifEditor::CNcsAifEditor(
    MNcsFieldSizeObserver* aSizeObserver, const TDesC& aCaptionText ) 
    : CNcsEditor( aSizeObserver, ETrue, ENcsEditorAddress, aCaptionText ), iAddressPopupList( NULL ),
    iAddLeftover( ETrue )
    {
    FUNC_LOG;
	SetEdwinObserver( this );
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::ConstructL( const CCoeControl* aParent,
        TInt aNumberOfLines,
        TInt aTextLimit )
    {
    FUNC_LOG;
    CNcsEditor::ConstructL( aParent, aNumberOfLines, aTextLimit );
    iAsyncCallBack = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CNcsAifEditor::~CNcsAifEditor()
    {
    FUNC_LOG;
	iArray.ResetAndDestroy();
	iAddressArray.Reset();
	if ( iAsyncCallBack )
	    {
	    iAsyncCallBack->Cancel();
	    delete iAsyncCallBack;
	    }
    }


// -----------------------------------------------------------------------------
// CNcsAifEditor::CursorLineNumber() const
// -----------------------------------------------------------------------------
//
TInt CNcsAifEditor::CursorLineNumber() const
    {
    FUNC_LOG;
    
    TInt ret = iLayout->GetLineNumber( CursorPos() );
    ret++;
    return ret;    
    }
    
// -----------------------------------------------------------------------------
// CNcsAifEditor::LineCount() const
// -----------------------------------------------------------------------------
//    
TInt CNcsAifEditor::LineCount() const
    {
    FUNC_LOG;
    TInt lineCount = iLayout->GetLineNumber( TextLength() );
    lineCount++;
    return lineCount;
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsAifEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret = EKeyWasNotConsumed;

    // check if we are copying
    if ( ret == EKeyWasNotConsumed )
        {
        ret = CopyEntriesToClipboardL( aKeyEvent, aType );
        }

    // Check if we need to delete a contact
    // This is done before select since they key off of the same key code.
    if ( ret == EKeyWasNotConsumed )
        {
        ret = HandleContactDeletionL( aKeyEvent, aType );
        }

    // Check if we need to highlight a contact
    if ( ret == EKeyWasNotConsumed )
        {
        ret = SetEditorSelectionL( aKeyEvent, aType );
        }
    
    //when press a key down, record the coursor position
    if ( aType == EEventKeyDown )
    	{
    	iLastTimeCursorPos = CursorPos();
    	}

    if ( ret == EKeyWasNotConsumed )
        {
        // enter completes the address entry
        if( aType == EEventKey && (aKeyEvent.iCode == EKeyEnter || 
        	aKeyEvent.iScanCode == EStdKeyEnter) )
        	{
        	// make sure there is really some text inputted 
            TInt cursorPos( CursorPos() );
        	
            TCursorSelection selection = NonEntryTextAtPos( cursorPos );
            
            TInt length( selection.Length() );
            
            HBufC* text = HBufC::NewLC( length );
            TPtr ptr = text->Des();
            Text()->Extract( ptr, selection.LowerPos(), length );
            ptr.Trim();
            
            // complete the entry by adding a semicolon, 
            // address will be added in HandleTextUpdateL
            if( ptr.Length() > 0 )
            	{            
				Text()->InsertL( cursorPos, KCharAddressDelimeterSemiColon );
				HandleTextChangedL();
				SetCursorPosL( cursorPos + 1, EFalse );
            	}
            
            CleanupStack::PopAndDestroy( text );            
        	}
        iTextSelection = Selection();        
        ret = CNcsEditor::OfferKeyEventL( aKeyEvent, aType );
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::HandleEdwinEventL()
// This function gets called if a character is entered through the FEP.
// Otherwise the character entry is added through OfferKeyEvent
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::HandleEdwinEventL( CEikEdwin* /*aEdwin*/,
    TEdwinEvent aEventType )
    {
    FUNC_LOG;
    if ( aEventType == MEikEdwinObserver::EEventTextUpdate )
        {
        // Remove any invalid entries. This is needed when entries have been marked
        // and have got replaced with some key event handled by FEP.
        CheckAndRemoveInvalidEntriesL();
        
        // Make a deferred call to HandleTextUpdateL() because it may result in
        // changing the text field contents, and doing so directly within HandleEdwinEventL
        // causes problems for the FEP in some special cases.
        HandleTextUpdateDeferred();
        }
    else if ( aEventType == MEikEdwinObserver::EEventNavigation )
        {
        iTextSelection = Selection();
        HandleNavigationEventL();
        }
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::SetEditorSelectionL()
// -----------------------------------------------------------------------------
//   
TKeyResponse CNcsAifEditor::SetEditorSelectionL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;
    CNcsAifEntry* entry = NULL;
    TCursorSelection selection = Selection();
    
    // Moving to a new line is a special case.
    // We need to offer the key to the editor control first so it can
    // move the cursor for us.  Then we check if it's in an entry.
    if ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        CompleteEntryL();
		
        response = CNcsEditor::OfferKeyEventL( aKeyEvent,aType );
        if ( response == EKeyWasConsumed ) 
            {
            // We're moving to a new line.
            entry = GetEntryAt( CursorPos() );
            if ( entry )
                {
                SetSelectionL( entry->iCursorPos, entry->iAnchorPos );
                }
            }
        }
    // Check if the cursor is in any of the addresses
    else if( aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyBackspace ) 
        {
        // We're moving left, but haven't yet.
        entry = GetEntryAt( CursorPos(), EDirectionLeft );
        if ( entry )
            {
            if ( selection.Length() && aKeyEvent.iCode == EKeyLeftArrow)
                {
                // Adds or removes the entry from the current selection.
                SetSelectionL( entry->LowerPos(), selection.iAnchorPos );
                response = EKeyWasConsumed;
                }
            else if ( !selection.Length() )
                {
                SetSelectionL( entry->LowerPos(), entry->HigherPos() );
                response = EKeyWasConsumed;
                }
            }
        }
    else if( aKeyEvent.iCode == EKeyRightArrow || aKeyEvent.iCode == EKeyDelete )
        {
        // We're moving right, but haven't yet.
        entry = GetEntryAt( CursorPos(), EDirectionRight );
        if ( entry )
            {
            if ( selection.Length() && aKeyEvent.iCode == EKeyRightArrow  )
                {
                // Adds or removes the entry form the current selection.
                SetSelectionL( entry->HigherPos(), selection.iAnchorPos );
                response = EKeyWasConsumed;
                }
            else if ( !selection.Length() )
                {
                SetSelectionL( entry->HigherPos(), entry->LowerPos() );
                response = EKeyWasConsumed;
                }
            }
        }
    // to fix problems with updating CBA when hash key is pressed and hold
    else if ( aKeyEvent.iScanCode == EStdKeyHash ) 
        {
        iAddressPopupList->ClosePopupContactListL();
        }

    // Close the address popup if we handled the event
    if ( response == EKeyWasConsumed )
        {
        iAddressPopupList->ClosePopupContactListL();
        }
    
    return response;
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::HandleContactDeletionL()
// ---------------------------------------------------------------------------
//
TKeyResponse CNcsAifEditor::HandleContactDeletionL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response  = EKeyWasNotConsumed;
    if ( SelectionLength() && aType == EEventKey 
        && IsCharacterKey( aKeyEvent ) )
        {
        // Delete highlighted entries.
        TCursorSelection selection = Selection();
        TBool entryDeleted = EFalse;
        for ( TInt ii = iArray.Count() - 1; ii >= 0; --ii )
            {
            if ( iArray[ii]->LowerPos() >= selection.LowerPos() &&
                iArray[ii]->HigherPos() <= selection.HigherPos() )
                {
                delete iArray[ii];
                iArray.Remove( ii );
                entryDeleted = ETrue;
                }
            }

        if ( entryDeleted )
            {
            // Check that duplicate entries are correctly marked.
            UpdateDuplicateEntryMarkingsL();
    
            // Set the cursor after the entry before the ones we just deleted
            CNcsAifEntry* entry = NULL;
            for ( TInt ii = iArray.Count() - 1; ii >= 0; --ii )
                {
                if ( iArray[ii]->HigherPos() <= selection.LowerPos() )
                    {
                    entry = iArray[ii];
                    break;
                    }
                }
            
            ClearSelectionL();
            
            RepositionEntriesL( entry );

            // The key event is set consumed only on delete and backpace
            // events, other events need to be forwarded to the editor.
            if ( aKeyEvent.iCode == EKeyDelete || 
                 aKeyEvent.iCode == EKeyBackspace )
                {
                response = EKeyWasConsumed;
                }
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::DoCharChangeL
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::DoCharChangeL()
	{
    FUNC_LOG;
	RecalculateEntryPositions();

	TChar previousChar = CharAtPos( CursorPos() - 1 );
	TBool sentinel = ( previousChar == KCharAddressDelimeterSemiColon || 
	    previousChar == KCharAddressDelimeterComma );
	if ( sentinel )
        {
        // if comma was pressed we replace it with semicolon
		if ( previousChar == KCharAddressDelimeterComma )
            {
			CPlainText* text = Text();
			text->DeleteL( CursorPos() - 1, 1 );
			text->InsertL( CursorPos() - 1, KCharAddressDelimeterSemiColon );
            }
		ParseNewAddressL();
        }
	UpdateAddressAutoCompletionL();
	}

// ---------------------------------------------------------------------------
// CNcsAddressInputField::CharAtPos
// ---------------------------------------------------------------------------
//
TChar CNcsAifEditor::CharAtPos( TInt aPos ) const
    {
    FUNC_LOG;
	if ( aPos >= 0 && aPos < TextLength() )
        {
		TBuf<1> buf;
		Text()->Extract( buf, aPos, 1 );
		return buf[0];
        }
	else
        {
		return 0;
        }
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::SetAddressesL()
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::SetAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
    iArray.Reset();
    AppendAddressesL( aAddresses );
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::AppendAddressesL()
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::AppendAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
    // First, add all the addresses without updating the editor text contents 
    for ( TInt i=0 ; i<aAddresses.Count() ; i++ )
        {
        AddAddressL( *aAddresses[i], EFalse );
        }
    // Update editor text content after all the items have been added
    RepositionEntriesL( NULL );
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::GetAddressesL()
// -----------------------------------------------------------------------------
//   
const RPointerArray<CNcsEmailAddressObject>& CNcsAifEditor::GetAddressesL()
    {
	// Clear the existing array since it may be out of sync
	iAddressArray.Reset();

	for ( TInt i=0 ; i<iArray.Count() ; i++ ) 
        {
		iAddressArray.AppendL(&iArray[i]->Address());
        }

	return iAddressArray;
    }
    
// -----------------------------------------------------------------------------
// CNcsAifEditor::GetEntryAt()
// -----------------------------------------------------------------------------
//   
CNcsAifEntry* CNcsAifEditor::GetEntryAt( 
    TInt aPos, 
    TEntryDirection aDirection ) const
    {
    FUNC_LOG;
    const TChar KSpace( ' ' );

	for( TInt i = 0; i < iArray.Count(); i++ )
        {
        CNcsAifEntry* entry = iArray[i];
        if ( aDirection == EDirectionNone )
            {
            // no direction, check if cursor is on entry
            if ( entry->Includes( aPos ) )
                {
                return entry;
                }
            }
        else if ( aDirection == EDirectionRight )
            {
            // direction to the righ. check if cursor is on entry or
            // entry is immediately to the right of the cursor
            if ( entry->Includes( aPos ) )
                {
                return entry;
                }

            if ( entry->Start() >= aPos && entry->Start() - aPos <= 1 &&
                CharAtPos( aPos ) == KSpace )
                {
                return entry;
                }
            }
        else if ( aDirection == EDirectionLeft )
            {
            // direction to the left. decrease cursor by one and check if it
            // is on entry or if entry is immediately to the left of the cursor
            if ( entry->Includes( aPos - 1 ) )
                {
                return entry;
                }
            
            if ( aPos >= entry->End() && aPos - entry->End() <= 1 &&
                CharAtPos( aPos - 1 ) == KSpace )
                {
                return entry;
                }
            }
        }
	return NULL;
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::GetPreviousEntryFrom()
// -----------------------------------------------------------------------------
//
CNcsAifEntry* CNcsAifEditor::GetPreviousEntryFrom( TInt aPos ) const
    {
    FUNC_LOG;
    CNcsAifEntry* entry = NULL;
    
    for( TInt i = 0 ; i < iArray.Count() ; i++ )
        {
        if ( iArray[i]->End() < aPos )
            {
            entry = iArray[i];
            }
        else
            {
            break;
            }
        }
    
    return entry;
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::CheckAddressWhenFocusLostL()
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::CheckAddressWhenFocusLostL()
    {
    FUNC_LOG;
	ParseNewAddressL();
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::ParseNewAddressL()
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::ParseNewAddressL()
	{
    FUNC_LOG;
	HBufC* text = GetNonEntryTextLC();
	__ASSERT_ALWAYS( text, Panic(EFSEmailUiNullPointerException) );

	if ( text->Length() )
		{
        // if changing focus leftover text is parsed to email
        // object - we don't need to add it anymore
        iAddLeftover = EFalse;
        // check if there is a name for the email address
        HBufC* name = CFsDelayedLoader::InstanceL()->GetContactHandlerL()->GetLastSearchNameL( *text );
	if ( name )
		{
            CleanupStack::PushL( name );
		AddAddressL( *name, *text, ETrue );
		CleanupStack::PopAndDestroy( name );
		}
	else
		{
		AddAddressL( KNullDesC, *text );
		}
	    }
	
	CleanupStack::PopAndDestroy(text);
	}

// -----------------------------------------------------------------------------
// CNcsAifEditor::GetNonEntryTextL()
// This will extract any text that was entered that is not
// part of any existing entries
// -----------------------------------------------------------------------------
//
HBufC* CNcsAifEditor::GetNonEntryTextLC() const
    {
    FUNC_LOG;
    
    // "non-entry text" starts after last "entry"
    TInt start( 0 );
    if ( iArray.Count() > 0 )
        {
        start = iArray[iArray.Count() - 1]->End();
        }
    TInt length( TextLength() - start );

    // Allocate space and extract it
    HBufC* text = HBufC::NewLC( length );
    TPtr ptr = text->Des();
    Text()->Extract( ptr, start, length );
    
    // Wipe out possible delimiter
    TInt pos = ptr.Locate( KCharAddressDelimeterSemiColon );
    if ( pos != KErrNotFound )
        {
        ptr.Delete( pos, 1 );
        }
    
    // Remove unnecessary whitespaces
    ptr.Trim();
    
    INFO_1("non-entry text == %S", text);
    return text;
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::CopyEntriesToClipBoardL
// ---------------------------------------------------------------------------
//
TKeyResponse CNcsAifEditor::CopyEntriesToClipboardL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret = EKeyWasNotConsumed;
    // check that we are copying
    TBool copyKeyEvent = ( aType == EEventKey && aKeyEvent.iCode == 3 &&
                           aKeyEvent.iModifiers & EModifierCtrl &&
                           aKeyEvent.iScanCode == EPtiKeyQwertyC );
    TBool cutKeyEvent = ( aType == EEventKey && aKeyEvent.iCode == 24 &&
                          aKeyEvent.iModifiers & EModifierCtrl &&
                          aKeyEvent.iScanCode == EPtiKeyQwertyX );
    if ( copyKeyEvent || cutKeyEvent ) 
        {
        RPointerArray<CNcsAifEntry> entries;
        CleanupClosePushL( entries );
        FindSelectedEntriesL( entries );
        if ( entries.Count() > 0 )
            {
            CancelFepTransaction();
            HBufC* formattedText = GetFormattedAddressListLC( entries, EFalse );
            TFsEmailUiUtility::CopyToClipboardL( *formattedText );
            CleanupStack::PopAndDestroy( formattedText );
            
            if ( !cutKeyEvent )
                { // cutting needs more handling
                ret = EKeyWasConsumed;
                }
            }
        CleanupStack::PopAndDestroy( &entries );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::FindSelectedEntriesL( )
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::FindSelectedEntriesL( RPointerArray<CNcsAifEntry>& aEntries )
    {
    FUNC_LOG;
    TCursorSelection selection = Selection();
    TInt count = iArray.Count();
    for ( TInt i = 0; i < iArray.Count(); i++ )
        {
        CNcsAifEntry* entry = iArray[i];
        if ( entry->Start() >= selection.LowerPos() &&
             entry->End() <= selection.HigherPos() )
            {
            aEntries.AppendL( entry );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::EmailAddressIndexNameBySelection( )
// -----------------------------------------------------------------------------
//
const CNcsEmailAddressObject* CNcsAifEditor::EmailAddressObjectBySelection() const
    {
    FUNC_LOG;
	// Find the contact the cursor is in
	const CNcsAifEntry* aEntry = GetEntryAt(CursorPos());
	ASSERT(aEntry != NULL);
	return &aEntry->Address();
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::AddAddressL()
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::AddAddressL( const CNcsEmailAddressObject& aAddress, TBool aUpdateEditorText /*= ETrue*/ )
    {
    FUNC_LOG;
    CNcsAifEntry* entry = CNcsAifEntry::NewL( aAddress );
    CleanupStack::PushL( entry );
    AddAddressL( entry, aUpdateEditorText );
    CleanupStack::Pop( entry );
    }

void CNcsAifEditor::AddAddressL( 
    const TDesC& aDisplayName, 
    const TDesC& aEmail,
    TBool aDisplayFull /*= EFalse*/,
    TBool aUpdateEditorText /*= ETrue*/ )
    {
    FUNC_LOG;
    CNcsAifEntry* entry = CNcsAifEntry::NewL( aDisplayName, aEmail, aDisplayFull );
    CleanupStack::PushL( entry );
	AddAddressL( entry, aUpdateEditorText );
	CleanupStack::Pop( entry );
    }

void CNcsAifEditor::AddAddressL( CNcsAifEntry* aNewEntry, TBool aUpdateEditorText )
    {
    FUNC_LOG;
	TInt idx;
	
	// Check for duplicate display names
	for ( idx=0 ; idx<iArray.Count() ; idx++ ) 
        {
		if ( iArray[idx]->IsSameDN(*aNewEntry) ) 
            {
			iArray[idx]->SetDupL();
			aNewEntry->SetDupL();
            }
        }

	// Find the location where we need to insert the address.
	// Browse from back to forth to make last index as default index. 
	// This ensures items remain in correct order when populating field from
	// existing message.
	TInt cursorPos = CursorPos();
    // if we are at the end of editor the address was
    // added from MRU list or separator was typed in
    if ( cursorPos == Text()->DocumentLength() )
        {
        iAddLeftover = EFalse;
        }
	
	for ( idx = iArray.Count() ; idx > 0 ; idx-- ) 
        {
		if ( cursorPos >= iArray[idx-1]->End() ) break;
        }
	if ( idx == iArray.Count() ) 
        {
		// Tack the address onto the end of the array
		iArray.AppendL( aNewEntry );
        }
	else
        {
		iArray.InsertL( aNewEntry, idx );
        }
	
	if ( aUpdateEditorText )
	    {
	    // Trap because we must not leave after we have taken the ownership of aNewEntry.
	    // Otherwise douple deletion might happen.
	    TRAP_IGNORE( RepositionEntriesL( aNewEntry ) );
	    }
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::RecalculateEntryPositions()
// The text has changed, so recalculate the positions of the items.
// ---------------------------------------------------------------------------
//	
void CNcsAifEditor::RecalculateEntryPositions()
	{
    FUNC_LOG;
	// We only need to worry about items right of the cursor
	TInt pos = CursorPos();
	TInt error = KErrNone;
	
	// Find the first item to the right of the cursor
	TInt idx = 0;
	for ( idx = 0; idx < iArray.Count(); idx++ )
		{
		if ( ( iArray[idx]->Includes( iLastTimeCursorPos ) ) 
				 || ( iArray[idx]->Start() >= iLastTimeCursorPos ) )   
			{
			break;
			}
		}	
	
   	// If no entry was to the right of the cursor position
	// then the new text was added at the end of the text.
	// Don't do anything
	if ( idx == iArray.Count() )
		{
		return;
		}
	
	// Find the location of the first entry to the right
	// of the cursor using a display string match
	pos = Min( iArray[idx]->Start(), pos );
	TRAP( error, pos = FindTextL( &iArray[idx]->DisplayString(), pos,
	    CEikEdwin::EFindCaseSensitive | CEikEdwin::ENoBusyMessage ) );
	ASSERT( KErrNone == error && KErrNotFound != pos );

	// Now reposition all entries to the right
	for ( ; idx<iArray.Count(); idx++ ) 
		{
		pos = iArray[idx]->SetPos( pos );
		pos++; // for whitespace
		}
	}

// ---------------------------------------------------------------------------
// CNcsAifEditor::RepositionEntriesL()
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::RepositionEntriesL( const CNcsAifEntry* aPosEntry )
	{
    FUNC_LOG;
	TInt pos = 0;
	CNcsAifEntry* entry;
	for ( TInt i=0 ; i<iArray.Count() ; i++ ) 
		{
		entry = iArray[i];
		pos = entry->SetPos( pos );
		pos++; // for whitespace
		}

  // Reset the text
	HBufC* text = NULL;
	text = GetFormattedAddressListLC( iArray );
	// fix for dissapearing text PWAN-82DNEJ	
	SetCursorPosL( 0, EFalse ); //In case the cursor pos is invalid
	
    if ( iAddLeftover )
        {
        TInt lengthBefore = Text()->DocumentLength();
        HBufC* textBefore = HBufC::NewLC( lengthBefore );
        TPtr ptrBefore = textBefore->Des();
        Text()->Extract( ptrBefore, 0, lengthBefore );
        ptrBefore.Trim();
        // find text after last semicolon
        TInt colon = ptrBefore.LocateReverseF( 
                KCharAddressDelimeterSemiColon ) + 1;
        TPtrC leftover = ptrBefore.Mid( colon );
        HBufC* newText = HBufC::NewLC( text->Length() + leftover.Length() );
        TPtr newTextPtr = newText->Des();
        // add all email addresses
        newTextPtr.Append( text->Des() );
        // add the text that was after last email object
        newTextPtr.Append( leftover );
    
        SetTextL( newText );
        CleanupStack::PopAndDestroy( newText );
        CleanupStack::PopAndDestroy( textBefore );
        }
    else
        {
        SetTextL( text );
        }
    CleanupStack::PopAndDestroy( text );
    HandleTextChangedL();
    
    // Set the cursor at the end of the given entry 
    if ( !aPosEntry )
    	{
	    SetCursorPosL( 0, EFalse );
    	}
    else
    	{
    	SetCursorPosL( aPosEntry->End(), EFalse );
    	}
	}

// ---------------------------------------------------------------------------
// CNcsAifEditor::CheckAndRemoveInvalidEntriesL()
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::CheckAndRemoveInvalidEntriesL()
    {
    FUNC_LOG;
    TInt currentCursorPos( CursorPos() );
    const TInt KNoEntryRemoved = -1;
    TInt removedEntryIndex( KNoEntryRemoved );
    
    for ( TInt i = iArray.Count() - 1 ; i >= 0 ; --i )
        {
        TInt matchesInText;
        TInt matchesInArray;
        TInt arrayItemLowPos( iArray[i]->LowerPos() );
        TInt arrayItemHighPos( iArray[i]->HigherPos());

        GetMatchingEntryCountsL( iArray[i], matchesInText, matchesInArray );

        // Entry is removed if:
        // a) there's no matches for it in the text, or
        // b) there're less matches for it in the text than in array (i.e.,
        //    a duplicate ("foo(at)foo.org; foo(at)foo.org") has just been removed)
        // In b) case the correct duplicate is the one that is in current
        // cursor position (or one off due to possible whitespace).
        if ( 0 == matchesInText ||
             ( matchesInText < matchesInArray ) &&
               ( currentCursorPos >= arrayItemLowPos && 
                 currentCursorPos <= arrayItemHighPos )) 
            {
            delete iArray[i];
            iArray.Remove(i);
            removedEntryIndex = i;
            if ( iTextSelection.iAnchorPos != iTextSelection.iCursorPos &&
                 iTextSelection.HigherPos() < arrayItemHighPos )
                {
                iPartialRemove = ETrue;
                }
            }
        }
    
    if ( KNoEntryRemoved != removedEntryIndex )
        {
        // at least one entry has been removed => udpates duplicate markings
        UpdateDuplicateEntryMarkingsL();
        }
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::GetLookupTextLC()
// ---------------------------------------------------------------------------
//
HBufC* CNcsAifEditor::GetLookupTextLC() const
    {
    FUNC_LOG;
	HBufC* text = GetTextInHBufL();
	
	if (text == NULL) return NULL;

	CleanupStack::PushL( text );
	TPtr ptr( text->Des() );
	TInt location = ptr.LocateReverse( KCharAddressDelimeterSemiColon );
	if( location != KErrNotFound )
        {
		ptr = ptr.RightTPtr( ptr.Length() - location -1 );
		ptr.TrimLeft();
        }
	return text;
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::GetFormattedAddressListLC()
// ---------------------------------------------------------------------------
//
HBufC* CNcsAifEditor::GetFormattedAddressListLC(
    RPointerArray<CNcsAifEntry>& aEntries,
    TBool aDisplayList ) const
    {
    FUNC_LOG;
	TInt length = CalculateAddressListLength( aEntries, aDisplayList );
	if ( length <= 0 )
        {
		return HBufC::NewLC(0);
        }
    
	HBufC* buf = HBufC::NewLC( length );
	TPtr ptr = buf->Des();
    
	TInt count = aEntries.Count();
	for ( TInt i = 0; i < count; i++ )
        {
        if ( aDisplayList )
            {
            ptr.Append( aEntries[i]->DisplayString() );
            }
        else
            {
            ptr.Append( aEntries[i]->Address().EmailAddress() );
            ptr.Append( KEmailAddressSeparator );
            }
        
		// append whitespace, if not in the last entry
        if ( i < count - 1 )
            {
            ptr.Append( KLineFeed );
            }
        }
		
	return buf;
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::GetFormattedAddressListL()
// ---------------------------------------------------------------------------
//
HBufC* CNcsAifEditor::GetFormattedAddressListL(
    RPointerArray<CNcsAifEntry>& aEntries,
    TBool aDisplayList ) const
	{
    FUNC_LOG;
    HBufC* buf = GetFormattedAddressListLC( aEntries, aDisplayList );
    CleanupStack::Pop( buf );
    return buf;
	}

// ---------------------------------------------------------------------------
// CNcsAifEditor::CalculateAddressListLength()
// ---------------------------------------------------------------------------
//
TInt CNcsAifEditor::CalculateAddressListLength(
    RPointerArray<CNcsAifEntry>& aEntries,
    TBool aDisplayList ) const
    {
    FUNC_LOG;
	TInt length = 0;
	TInt count = aEntries.Count();
	for ( TInt i = 0; i < count; i++ )
        {
		CNcsAifEntry* entry = aEntries[ i ];
		if ( !entry ) continue;
        if ( aDisplayList )
            {
            length += entry->Length();
            }
        else
            {
            // +1 is for semicolon
            length += entry->Address().EmailAddress().Length() + 1;
            }
        }
	
	// add one white space after that so the format is
	// aamiumaubb.com; ccmiumaudd.com; eemiumauff.com
	if ( count > 1 )
        {
		// ( count - 1 ) we do need white space after the last address
		length += count - 1 ;
        }
    
	if ( aEntries.Count() > 0 )
        {
	    length += 2;
        }		
		
	return length;
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::UpdateAddressAutoCompletionL()
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::UpdateAddressAutoCompletionL()
    {
    FUNC_LOG;
	HBufC* text = GetNonEntryTextLC();
	__ASSERT_ALWAYS( text, Panic(EFSEmailUiNullPointerException) );

		iAddressPopupList->UpdatePopupContactListL( *text, EFalse );
		CleanupStack::PopAndDestroy( text );
        }

// ---------------------------------------------------------------------------
// CNcsAifEditor::UpdateAddressAutoCompletionL()
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::UpdateAddressAutoCompletionL(
    const TCursorSelection& aSelection )
    {
    FUNC_LOG;
    TInt length = aSelection.Length();
    HBufC* text = HBufC::NewLC( length );
    TPtr ptr = text->Des();
    Text()->Extract( ptr, aSelection.LowerPos(), length );
    ptr.Trim();
    if ( text->Length() )
        {
        iAddressPopupList->UpdatePopupContactListL( *text, EFalse );
        }
    else
        {
        iAddressPopupList->ClosePopupContactListL();
        }
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::UpdateAddressListAllL()
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::UpdateAddressListAllL()
    {
    FUNC_LOG;
	iAddressPopupList->UpdatePopupContactListL( KNullDesC, ETrue );
    }


// ---------------------------------------------------------------------------
// Updates the duplicate markings in the entry array.
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::UpdateDuplicateEntryMarkingsL()
    {
    FUNC_LOG;
    const TInt entryCount = iArray.Count();
    for ( TInt ii = entryCount - 1; ii >= 0; --ii )
        {
        if ( ii > 0 )
            {
            TBool duplicateFound = EFalse;
            for ( TInt jj = ii - 1; jj >= 0; --jj )
                {
                if ( iArray[ii]->IsSameDN( *iArray[jj] ) )
                    {
                    duplicateFound = ETrue;
                    iArray[jj]->SetDupL( ETrue );
                    }
                }
            iArray[ii]->SetDupL( duplicateFound );
            }
        }
    }

// ---------------------------------------------------------------------------
// Makes a deferred call to HandleTextUpdateL
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::HandleTextUpdateDeferred()
    {
    FUNC_LOG;
    if ( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        iAsyncCallBack->Set( TCallBack( DoHandleTextUpdate, this ) );
        iAsyncCallBack->CallBack();
        }
    }

// ---------------------------------------------------------------------------
// Static wrapper function for HandleTextUpdateL() 
// ---------------------------------------------------------------------------
//
TInt CNcsAifEditor::DoHandleTextUpdate( TAny* aSelf )
    {
    FUNC_LOG;
    CNcsAifEditor* self = static_cast<CNcsAifEditor*>( aSelf );
    TRAPD( err, self->HandleTextUpdateL() );
    return err;
    }

// ---------------------------------------------------------------------------
// Handles text update.
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::HandleTextUpdateL()
    {
    FUNC_LOG;
    RecalculateEntryPositions();
    TCursorSelection textSelection = NonEntryTextAtPos( CursorPos() );
    TBool newEntryCreated = EFalse;
    if ( textSelection.Length() )
        {
        // Check non-entry text for complete entries.
        newEntryCreated = HandleTextUpdateL( textSelection );
        }
    
    if ( newEntryCreated )
        {
        iAddressPopupList->ClosePopupContactListL();
        
        // add line feed after new entry
        TInt cursorPos( CursorPos() );
        // related to PWAN-82DNEJ cursorPos shouldn't be 0 here
        if (cursorPos == 0)
          {
          cursorPos = TextLength();
          }
          
        if ( !iPartialRemove )
            {
            Text()->InsertL( cursorPos, TChar(CEditableText::ELineBreak) );
            }
        HandleTextChangedL();
        SetCursorPosL( cursorPos + 1, EFalse );
        iSizeObserver->UpdateFieldSizeL( ETrue );
        iPartialRemove = EFalse;
        }
    else
        {
        UpdateAddressAutoCompletionL( textSelection );
        }
    }

// ---------------------------------------------------------------------------
// CNcsAifEditor::HandleTextUpdateL()
// ---------------------------------------------------------------------------
//
TBool CNcsAifEditor::HandleTextUpdateL( const TCursorSelection& aSelection )
    {
    FUNC_LOG;
    iAddLeftover = ETrue;
    TInt firstCharacter = aSelection.LowerPos();
    TInt lastCharacter = aSelection.HigherPos();
    
    // get the inputted text
    TInt length = lastCharacter - firstCharacter;
    
    HBufC* text = HBufC::NewLC( length );
    TPtr ptr = text->Des();
    Text()->Extract( ptr, firstCharacter, length );
    ptr.Trim();
    
    TBool entriesFound( EFalse );
    
    // start looking for entries separated with semicolon
    TInt start( 0 );
    TInt end( ptr.Length() );
    TInt lastSentinel = KErrNotFound;
    
    for ( TInt ii = 0; ii < end; ++ii )
        {
        TChar character = ptr[ii];
        TBool addAddress = EFalse;
        
        if ( IsSentinel( character ) )
            {
            if ( character == KCharSpace )
                {
                if ( ptr.Mid( start, ii-start ).Locate( KCharAt ) 
                        != KErrNotFound )
                    {
                    ptr[ii] = KCharAddressDelimeterSemiColon;
                    lastSentinel = ii;
                    addAddress = ETrue;
                    }
                }
            else if ( character == KCharAddressDelimeterComma )
                {
                // Replace comma with semicolon
                ptr[ii] = KCharAddressDelimeterSemiColon;
                lastSentinel = ii;
                addAddress = ETrue;
                }
            else if ( character == KCharAddressDelimeterSemiColon )
                {
                lastSentinel = ii;
                addAddress = ETrue;
                }

            // Create new entry.
            if ( addAddress && start < end )
                {
                // only if longer than 0, if not we'll get 
                // "empty" email address
                if ( ii-start )
                    {
                    AddAddressL( KNullDesC(), ptr.Mid(start, ii-start) );
                    start = Min( ii + 1, end );
                    entriesFound = ETrue;
                    }
                addAddress = EFalse;
                }
            }
        }
    
    // add email that wasn't ended with semicolon
    if ( lastSentinel != KErrNotFound )
        {
        if ( lastSentinel < end && start < end )
            {
            AddAddressL( KNullDesC(), ptr.Mid(start, end-start) );
            }
        }
    
    CleanupStack::PopAndDestroy( text );
        
    return entriesFound;
    }

// ---------------------------------------------------------------------------
// Handles navigation event.
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::HandleNavigationEventL()
    {
    FUNC_LOG;
    // Close the contact popup when cursor is moved withing the field to make it less distracting. 
    // It's reopened when user types something.
    iAddressPopupList->ClosePopupContactListL();
    }

// ---------------------------------------------------------------------------
// Gets the range of non-entry text at the given position.
// ---------------------------------------------------------------------------
//
TCursorSelection CNcsAifEditor::NonEntryTextAtPos( TUint aPosition ) const
    {
    FUNC_LOG;
    TCursorSelection text( TextLength(), 0 );
    for ( TInt ii = iArray.Count() - 1; ii >= 0; --ii )
        {
        if ( iArray[ii]->Includes( aPosition - 1) )
            {
            // Given position is included in existing entry
            text.SetSelection( 0, 0 );
            break;
            }
        else if ( iArray[ii]->LowerPos() >= aPosition )
            {
            // Found entry after the given position
            text.iCursorPos = iArray[ii]->LowerPos();
            }
        else if ( iArray[ii]->HigherPos() < aPosition )
            {
            // Found first entry before given position
            text.iAnchorPos = iArray[ii]->HigherPos();
            break;
            }
        }

    // get the selected text to remove whitespace
    TInt length( text.Length() );    
    
    HBufC* selectedText = NULL;
    TRAPD( err, selectedText = HBufC::NewL( length ) );
    
    if( err == KErrNone )
    	{
		TPtr ptr = selectedText->Des();
		Text()->Extract( ptr, text.LowerPos(), length );
		
		// trim from end
		TInt index( length - 1 );
		
		while( index >= 0 && IsWhitespace( ptr[index--] ) )
			{
			text.iCursorPos--;
			}
		
		// trim from begin
		index = 0;
		
		while( index < length && IsWhitespace( ptr[index++] ) )
			{
			text.iAnchorPos++;
			}

		delete selectedText;
		selectedText = NULL;
    	}    
    	
    return text;
    }

// ---------------------------------------------------------------------------
// Gets the range of text immediatelly before given position that does not
// belong to any entry. 
// ---------------------------------------------------------------------------
//
TCursorSelection CNcsAifEditor::NonEntryTextBeforePos( TUint aPosition ) const
    {
    FUNC_LOG;
    TCursorSelection text( aPosition, 0 );
    for ( TInt ii = iArray.Count() - 1; ii >= 0; --ii )
        {
        if ( iArray[ii]->Includes( aPosition - 1 ) )
            {
            // Given position is included in existing entry
            text.SetSelection( 0, 0 );
            break;
            }
        else if ( iArray[ii]->HigherPos() < aPosition )
            {
            // Found first existing entry before given position
            text.SetSelection( aPosition, iArray[ii]->HigherPos() );
            break;
            }
        }
    return text;
    }

// ---------------------------------------------------------------------------
// Checks whether given character is considered as sentinel.
// ---------------------------------------------------------------------------
//
TBool CNcsAifEditor::IsSentinel( TChar aCharacter ) const
    {
    FUNC_LOG;
    return ( aCharacter == KCharAddressDelimeterSemiColon || 
        aCharacter == KCharAddressDelimeterComma || aCharacter == KCharSpace );
    }

// ---------------------------------------------------------------------------
// Checks whether given character is considered as whitespace.
// ---------------------------------------------------------------------------
//
TBool CNcsAifEditor::IsWhitespace( TChar aCharacter ) const
    {
    FUNC_LOG;
    return ( aCharacter == KCharSpace || 
    		 aCharacter == TChar(CEditableText::ELineBreak) || 
    		 aCharacter == TChar(CEditableText::EParagraphDelimiter) );
    }

// ---------------------------------------------------------------------------
// Checks whether given event is considered as navigation event.
// ---------------------------------------------------------------------------
//
TBool CNcsAifEditor::IsNavigationKey( const TKeyEvent& aKeyEvent ) const
    {
    FUNC_LOG;
    return ( aKeyEvent.iCode == EKeyLeftArrow ||
             aKeyEvent.iCode == EKeyRightArrow ||
             aKeyEvent.iCode == EKeyUpArrow ||
             aKeyEvent.iCode == EKeyDownArrow ||
             aKeyEvent.iScanCode == EStdKeyLeftArrow ||
             aKeyEvent.iScanCode == EStdKeyRightArrow ||
             aKeyEvent.iScanCode == EStdKeyUpArrow ||
             aKeyEvent.iScanCode == EStdKeyDownArrow );
    }

// ---------------------------------------------------------------------------
// Checks whether given event is one which generates visible character
// ---------------------------------------------------------------------------
//
TBool CNcsAifEditor::IsCharacterKey( const TKeyEvent& aKeyEvent ) const
    {
    FUNC_LOG;
    TUint ctrlModifiers = EModifierLeftCtrl | EModifierRightCtrl | EModifierCtrl;
    TBool ctrlEvent = aKeyEvent.iModifiers & ctrlModifiers;
    TBool isAppKey = ( aKeyEvent.iScanCode >= EStdKeyApplication0) && (aKeyEvent.iScanCode <= EStdKeyKeyboardExtend);
    return ( !ctrlEvent && !IsNavigationKey(aKeyEvent) && !isAppKey  );
    }

// ---------------------------------------------------------------------------
// Gets the count of substrings (in current text field) matching the aEntry's
// DisplayName (DN) and the count of matching (same DN) items in iArray. 
// ---------------------------------------------------------------------------
//
void CNcsAifEditor::GetMatchingEntryCountsL(  const CNcsAifEntry* aEntry,
                                              TInt& aNrOfMatchesInText,
                                              TInt& aNrOfMatchesInEntryArray )
    {
    aNrOfMatchesInText = 0;
    aNrOfMatchesInEntryArray = 0;
    TInt pos( 0 );
    const TInt end_pos( TextLength() );

    // First a checking loop for finding the number of matching substrings
    // (i.e., substrings that match the entry's displaystring) in current text
    while ( pos < end_pos )
        {
        pos = FindTextL( &aEntry->DisplayString(), pos, 
                         CEikEdwin::EFindCaseSensitive | 
                         CEikEdwin::ENoBusyMessage );

        // No more matches for entry found => checking finished for this one 
        if ( pos < 0 )
            {
            pos = end_pos; // ends the loop
            }
        // Match found => update counter
        else
            {
            ++aNrOfMatchesInText;
            // Move to next word
            TInt len;
            TInt startPos;
            GetWordInfo( pos, startPos, len );
            pos = startPos+len;
            }
        }

    // Secondly check the number of entries in entry array that match
    // the given entry's displayname.
    for ( TInt i = iArray.Count()-1; i >= 0; --i )
        {
        if ( !aEntry->DisplayString().Compare( iArray[i]->DisplayString() ) )
            {
            ++aNrOfMatchesInEntryArray;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsAifEditor::HandlePointerEventL()
// Handles pointer events
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        CTextLayout* textLayout = TextLayout();
        TInt cursorPos = CursorPos();
        TPoint touchPoint( aPointerEvent.iPosition );

        //adjust touch point to mach editor coordinates
        touchPoint.iX -= Position().iX;
        
        TInt pointerLineNbr = textLayout->GetLineNumber( textLayout->XyPosToDocPosL( touchPoint ));
        TInt cursorLineNbr = textLayout->GetLineNumber( cursorPos );
        
        
        if ( pointerLineNbr != cursorLineNbr )
            {
            CompleteEntryL();
            
            // We're moving to a new line.
            CNcsAifEntry* entry = NULL;
            entry = GetEntryAt( CursorPos() );
            if ( entry )
                {
                SetSelectionL( entry->iCursorPos, entry->iAnchorPos );
                }
            }    
        }
            
    CEikEdwin::HandlePointerEventL( aPointerEvent );
    }


// -----------------------------------------------------------------------------
// CNcsAifEditor::CompleteEntryL()
// Adds semicolol to the of the entry
// -----------------------------------------------------------------------------
//
void CNcsAifEditor::CompleteEntryL()
    {
    // make sure there is really some text inputted 
    TInt cursorPos( CursorPos() );

    TCursorSelection selection = NonEntryTextAtPos( cursorPos );

    TInt length( selection.Length() );

    HBufC* text = HBufC::NewLC( length );
    TPtr ptr = text->Des();

    if( selection.LowerPos() >= 0 )
        {
        Text()->Extract( ptr, selection.LowerPos(), length );
        ptr.Trim();
        
        // complete the entry
        if( ptr.Length() > 0 )
            {
            Text()->InsertL( selection.HigherPos(), KCharAddressDelimeterSemiColon );
            HandleTextChangedL();
            HandleTextUpdateL( TCursorSelection(selection.LowerPos(), selection.HigherPos() + 1) );
            }
        }

    CleanupStack::PopAndDestroy( text );
    }
// End of File

