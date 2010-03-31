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
* Description: This file implements classes CESMRNcsAifEntry, CESMRNcsAifEditor.
*
*/

#include "emailtrace.h"
#include "cesmrncsaifeditor.h"

#include <aknsdrawutils.h>
#include <s32mem.h>
#include <txtrich.h>
#include <baclipb.h>
#include <ptidefs.h>
#include <stringloader.h>
#include <eikedwin.h>
#include <txtclipboard.h>

#include "cesmrncsemailaddressobject.h"
#include "cesmrcontacthandler.h"
#include "esmrfieldbuilderdef.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::NewL
// ---------------------------------------------------------------------------
//
CESMRNcsAifEntry* CESMRNcsAifEntry::NewL(
    const CESMRNcsEmailAddressObject& addr,
    TBool aDisplayFull )
    {
    FUNC_LOG;
    CESMRNcsAifEntry* self = new (ELeave) CESMRNcsAifEntry( aDisplayFull );
    CleanupStack::PushL(self);
    self->ConstructL( addr );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::NewL
// ---------------------------------------------------------------------------
//
CESMRNcsAifEntry* CESMRNcsAifEntry::NewL(
    const TDesC& aDn,
    const TDesC& aEml,
    TBool aDisplayFull )
    {
    FUNC_LOG;
    CESMRNcsAifEntry* self = new ( ELeave ) CESMRNcsAifEntry( aDisplayFull );
    CleanupStack::PushL(self);
    self->ConstructL( aDn, aEml);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::~CESMRNcsAifEntry
// ---------------------------------------------------------------------------
//
CESMRNcsAifEntry::~CESMRNcsAifEntry()
    {
    FUNC_LOG;
    delete iAddress;
    delete iDisplayString;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::CESMRNcsAifEntry
// ---------------------------------------------------------------------------
//
CESMRNcsAifEntry::CESMRNcsAifEntry( TBool aDisplayFull ) :
    iDisplayFull( aDisplayFull )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRNcsAifEntry::ConstructL( const TDesC& aDn, const TDesC& aEml )
    {
    FUNC_LOG;
    iAddress = CESMRNcsEmailAddressObject::NewL( aDn, aEml );
    ConstructL();
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRNcsAifEntry::ConstructL( const CESMRNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    iAddress = CESMRNcsEmailAddressObject::NewL( aAddress );
    ConstructL();
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRNcsAifEntry::ConstructL()
    {
    FUNC_LOG;
    SetDisplayStringL();
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::SetDisplayStringL
// ---------------------------------------------------------------------------
//
void CESMRNcsAifEntry::SetDisplayStringL()
    {
    FUNC_LOG;
    if ( iDisplayString )
        {
        delete iDisplayString;
        iDisplayString = NULL;
        }

    const TDesC& dname = iAddress->DisplayName();
    const TDesC& email = iAddress->EmailAddress();

    TInt dnameLength = dname.Length();
    TInt emailLength = email.Length();

    TInt length;
    if ( !iIsDup && !iDisplayFull )
        {
        length = dnameLength > 0 ? dnameLength : emailLength;
        length += 1; // ';'

        iDisplayString = HBufC::NewL( length );
        TPtr ptr = iDisplayString->Des();

        ptr.Append( dname.Length() > 0 ? dname : email );
        ptr.Append( KAddressDelimeterSemiColon );
        }
    else
        {
        // Display, Name <display.name(at)dn.com>;
        length = dnameLength + emailLength + 4;

        iDisplayString = HBufC::NewL( length );
        TPtr ptr = iDisplayString->Des();

        ptr.Append( dname );
        _LIT(KLeft, " <");
        ptr.Append( KLeft );
        ptr.Append( email );
        _LIT(KRight, ">");
        ptr.Append( KRight );
        ptr.Append( KAddressDelimeterSemiColon );
        }
    }

// ---------------------------------------------------------------------------
// constructor/destructor
// ---------------------------------------------------------------------------
//
CESMRNcsAifEditor::CESMRNcsAifEditor( CESMRContactHandler& aContactHandler, HBufC* aDefaultText ) :
    CESMRNcsEditor( aDefaultText ), iContactHandler( aContactHandler )
    {
    FUNC_LOG;
    SetEdwinObserver( this );
    }

// ---------------------------------------------------------------------------
// constructor/destructor
// ---------------------------------------------------------------------------
//
CESMRNcsAifEditor::~CESMRNcsAifEditor()
    {
    FUNC_LOG;
    iArray.ResetAndDestroy();
    iAddressArray.Reset();
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::CursorLineNumber() const
// -----------------------------------------------------------------------------
//
TInt CESMRNcsAifEditor::CursorLineNumber() const
    {
    FUNC_LOG;

    TInt ret = iLayout->GetLineNumber( CursorPos() );
    ret++;
    if( ret > KMaxAddressFieldLines )
        {
        ret = KMaxAddressFieldLines;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::LineCount() const
// -----------------------------------------------------------------------------
//
TInt CESMRNcsAifEditor::LineCount() const
    {
    FUNC_LOG;
    TInt lineCount = iLayout->GetLineNumber( TextLength() );
    lineCount++;
    if( lineCount > KMaxAddressFieldLines )
        {
        lineCount = KMaxAddressFieldLines;
        }
    return lineCount;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRNcsAifEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret = EKeyWasNotConsumed;

    // check if we are copying
    if ( ret == EKeyWasNotConsumed )
        {
        ret = CopyEntriesToClipboardL( aKeyEvent, aType );
        }

    // Check if we need to delete a contact
    // We must do this before select sincey they
    // key off of the same key code.
    if ( ret == EKeyWasNotConsumed )
        {
        ret = HandleContactDeletionL( aKeyEvent, aType );
        }

    // Check if we need to highlight a contact
    if ( ret == EKeyWasNotConsumed )
        {
        ret = SetEditorSelectionL( aKeyEvent, aType );
        }

    if ( ret == EKeyWasNotConsumed )
        {
        ret = CESMRNcsEditor::OfferKeyEventL( aKeyEvent, aType );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::HandleEdwinEventL()
// This function gets called if a character is entered through the FEP.
// Otherwise the character entry is added through OfferKeyEvent
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::HandleEdwinEventL( CEikEdwin* /*aEdwin*/, TEdwinEvent aEventType )
    {
    FUNC_LOG;
    if ( aEventType == MEikEdwinObserver::EEventTextUpdate )
        {
        HandleContactDeletionL();
        HandleTextUpdateL();
        if ( IsVisible() )
        	{
        	DrawDeferred();
        	}
        }
    else if ( aEventType == MEikEdwinObserver::EEventNavigation )
        {
        HandleNavigationEventL();
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::SetEditorSelectionL()
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRNcsAifEditor::SetEditorSelectionL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;
    CESMRNcsAifEntry* entry = NULL;
    TCursorSelection selection = Selection();

    // Moving to a new line is a special case.
    // We need to offer the key to the editor control first so it can
    // move the cursor for us.  Then we check if it's in an entry.
    if ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        response = CESMRNcsEditor::OfferKeyEventL( aKeyEvent,aType );
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

    // Close the address popup if we handled the event
    if ( response == EKeyWasConsumed )
        {
        iAddressPopupList->ClosePopupContactListL();
        }

    DrawDeferred();
    return response;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::HandleContactDeletionL()
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRNcsAifEditor::HandleContactDeletionL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response  = EKeyWasNotConsumed;
    if ( SelectionLength() && aType == EEventKey
        && !IsNavigationKey( aKeyEvent ) )
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
            UpdateDuplicateEntryMarkings();

            // Set the cursor after the entry before the ones we just deleted
            CESMRNcsAifEntry* entry = NULL;
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

// ---------------------------------------------------------
// CESMRNcsAifEditor::DoCharChangeL
// ---------------------------------------------------------
//
void CESMRNcsAifEditor::DoCharChangeL()
    {
    FUNC_LOG;
    RecalculateEntryPositions();

    TChar previousChar = CharAtPos( CursorPos() - 1 );
    TBool sentinel = ( previousChar == KSemiColon ||
        previousChar == KComma );
    if ( sentinel )
        {
        // if comma was pressed we replace it with semicolon
        if ( previousChar == KComma )
            {
            CPlainText* text = Text();
            text->DeleteL( CursorPos() - 1, 1 );
            text->InsertL( CursorPos() - 1, KSemiColon );
            }
        TBool check = ParseNewAddressL();
        }
    UpdateAddressAutoCompletionL();
    }

// ---------------------------------------------------------
// CESMRNcsAifEditor::CharAtPos
// ---------------------------------------------------------
//
TChar CESMRNcsAifEditor::CharAtPos( TInt aPos ) const
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
// CESMRNcsAifEditor::SetAddressesL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::SetAddressesL( const RPointerArray<CESMRNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
    iArray.ResetAndDestroy();
    AppendAddressesL( aAddresses );
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::AppendAddressesL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::AppendAddressesL( const RPointerArray<CESMRNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
    iArray.ReserveL( iArray.Count() + aAddresses.Count() );

    CESMRNcsAifEntry* entry = NULL;
    for (int i=0;i<aAddresses.Count();i++)
        {
        CESMRNcsEmailAddressObject* address = aAddresses[i];
        entry = CESMRNcsAifEntry::NewL( *address, address->DisplayFull() );

        int idx;
        // Check for duplicate display names
        for ( idx=0; idx<iArray.Count(); idx++ )
            {
            if ( iArray[idx]->IsSameDN( *entry ) )
                {
                iArray[idx]->SetDup();
                entry->SetDup();
                }
            }

        CleanupStack::PushL( entry );
        iArray.AppendL( entry );
        CleanupStack::Pop( entry );
        }

    RepositionEntriesL( entry );
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::GetAddressesL()
// -----------------------------------------------------------------------------
//
const RPointerArray<CESMRNcsEmailAddressObject>& CESMRNcsAifEditor::GetAddressesL()
    {
    // Clear the existing array since it may be out of sync
    iAddressArray.Reset();

    for (int i=0;i<iArray.Count();i++)
        {
        iAddressArray.AppendL(&iArray[i]->Address());
        }

    return iAddressArray;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::GetEntryAt()
// -----------------------------------------------------------------------------
//
CESMRNcsAifEntry* CESMRNcsAifEditor::GetEntryAt(
    TInt aPos,
    TEntryDirection aDirection ) const
    {
    FUNC_LOG;

    TChar KSpace( ' ' );

    for( TInt i = 0; i < iArray.Count(); i++ )
      {
      CESMRNcsAifEntry* entry = iArray[i];
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
// CESMRNcsAifEditor::CheckAddressWhenFocusLostL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::CheckAddressWhenFocusLostL()
    {
    FUNC_LOG;
    ParseNewAddressL();
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::ParseNewAddressL()
// -----------------------------------------------------------------------------
//
TBool CESMRNcsAifEditor::ParseNewAddressL()
    {
    FUNC_LOG;
    HBufC* text = GetNonEntryTextLC();

    if ( !text )
        {
        return false;
        }
    // check if there is a name for the email address

    // if name = NULL, it isn't in cleanupstack
    HBufC* name = iContactHandler.GetLastSearchNameLC( *text );
    if ( name )
        {
        AddAddressL( *name, *text, ETrue );
        CleanupStack::PopAndDestroy( name );
        }
    else
        {
        AddAddressL(KNullDesC(),*text);
        }

    CleanupStack::PopAndDestroy(text);

    return true;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::GetNonEntryTextL()
// This will extract any text that was entered that is not
// part of any existing entries
// -----------------------------------------------------------------------------
//
HBufC* CESMRNcsAifEditor::GetNonEntryTextLC() const
    {
    FUNC_LOG;
    // The user entered a new email address directly into the AIF
    TInt pos = CursorPos();

    // Check if the cursor is in an existing entry
    for (int i=0;i<iArray.Count(); i++)
        {
        if (iArray[i]->Includes(pos-1)) return NULL;
        }

    // Pull the text to the left of the cursor up to the
    // previous entry or the beginning of the doc
    int idx = -1;
    for (int i=0;i<iArray.Count();i++)
        {
        if (iArray[i]->End() < pos) idx = i;
        }

    // No entry before this text, pull to beginning of doc
    int len, start;
    if (idx == -1)
        {
        len = pos;
        start = 0;
        }
    else
        {
        len = pos - iArray[idx]->End();
        start = iArray[idx]->End();
        }

    // The user could have moved the cursor into the new text.
    // Pull the text to the right of the cursor up to the
    // next entry or the end of the doc
    for (idx=0;idx<iArray.Count();idx++)
        {
        if (iArray[idx]->Start() >= pos) break;
        }

    // No entry after this text, pull to end of doc
    if (iArray.Count() == 0 || idx == iArray.Count())
        {
        len += TextLength() - pos;
        }
    else
        {
        len += iArray[idx]->Start() - pos;
        }

    // Extract the address text
    HBufC* text = HBufC::NewLC(len);
    TPtr ptr = text->Des();
    Text()->Extract(ptr,start,len);

    // Wipe out any delimiters
    TChar semiColon(KSemiColon);
    pos = ptr.Locate(semiColon);
    if (pos != KErrNotFound)
        ptr.Delete(pos,1);

    // Trim the string and add it to the entries list
    ptr.Trim();

    if (ptr.Length() == 0)
        {
        CleanupStack::PopAndDestroy(text);
        return NULL;
        }

    return text;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::CopyEntriesToClipBoardL
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRNcsAifEditor::CopyEntriesToClipboardL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret = EKeyWasNotConsumed;

    const TInt KCopyKeyEventCode = 3;
    const TInt KCutKeyEventCode = 24;

    // check that we are copying
    TBool copyKeyEvent = ( aType == EEventKey && aKeyEvent.iCode == KCopyKeyEventCode &&
                           aKeyEvent.iModifiers & EModifierCtrl &&
                           aKeyEvent.iScanCode == EPtiKeyQwertyC );
    TBool cutKeyEvent = ( aType == EEventKey && aKeyEvent.iCode == KCutKeyEventCode &&
                          aKeyEvent.iModifiers & EModifierCtrl &&
                          aKeyEvent.iScanCode == EPtiKeyQwertyX );
    if ( copyKeyEvent || cutKeyEvent )
        {
        RPointerArray<CESMRNcsAifEntry> entries;
        CleanupClosePushL( entries );
        FindSelectedEntriesL( entries );
        if ( entries.Count() > 0 )
            {
            CancelFepTransaction();
            HBufC* formattedText = GetFormattedAddressListLC( entries, EFalse );
            CopyToClipboardL( *formattedText );
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
// CESMRNcsAifEditor::FindSelectedEntriesL( )
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::FindSelectedEntriesL( RPointerArray<CESMRNcsAifEntry>& aEntries )
    {
    FUNC_LOG;
    TCursorSelection selection = Selection();
    TInt count = iArray.Count();
    for ( TInt i = 0; i < iArray.Count(); i++ )
        {
        CESMRNcsAifEntry* entry = iArray[i];
        if ( entry->Start() >= selection.LowerPos() &&
             entry->End() <= selection.HigherPos() )
            {
            aEntries.AppendL( entry );
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::EmailAddressIndexNameBySelection( )
// -----------------------------------------------------------------------------
//
const CESMRNcsEmailAddressObject* CESMRNcsAifEditor::EmailAddressObjectBySelection() const

    {
    FUNC_LOG;
    // Find the contact the cursor is in
    const CESMRNcsAifEntry* aEntry = GetEntryAt(CursorPos());
    ASSERT(aEntry != NULL);
    return &aEntry->Address();
    }


// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::AddAddressL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::AddAddressL( const CESMRNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    AddAddressL( CESMRNcsAifEntry::NewL( aAddress, aAddress.DisplayFull() ) );
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::AddAddressL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::AddAddressL(
    const TDesC& aDisplayName,
    const TDesC& aEmail,
    TBool aDisplayFull )
    {
    FUNC_LOG;
    AddAddressL( CESMRNcsAifEntry::NewL( aDisplayName, aEmail, aDisplayFull ) );
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::AddAddressL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::AddAddressL( CESMRNcsAifEntry* aNewEntry )
    {
    FUNC_LOG;
    int idx;
    // Check for duplicate display names
    for (idx=0;idx<iArray.Count();idx++)
        {
        if (iArray[idx]->IsSameDN(*aNewEntry))
            {
            iArray[idx]->SetDup();
            aNewEntry->SetDup();
            }
        }

    // Find the location where we need to insert the address
    TInt pos = CursorPos();
    for (idx=0;idx<iArray.Count();idx++)
        {
        if (pos <= iArray[idx]->Start()) break;
        }
    if (idx == iArray.Count())
        {
        // Tack the address onto the end of the array
        iArray.Append(aNewEntry);
        }
    else
        {
        iArray.Insert(aNewEntry,idx);
        }
    RepositionEntriesL( aNewEntry );
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::RecalculateEntryPositions()
// The text has changed, so recalculate the positions of the items.
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::RecalculateEntryPositions()
    {
    FUNC_LOG;

    TInt pos(0);
    TInt idx(0);
    TInt error(0);
    for	(;idx<iArray.Count();idx++)
    	{
    	    TRAP( error, pos = FindTextL( &iArray[idx]->DisplayString(), pos,
    	        CEikEdwin::EFindCaseSensitive | CEikEdwin::ENoBusyMessage ) );
    	    ASSERT( KErrNone == error && KErrNotFound != pos );
    	    if(pos != iArray[idx]->Start())
    	    	{
    	    	break;
    	    	}
    	    pos += iArray[idx]->Length();
    	}

    if(idx != iArray.Count())
    	{
		for (; idx<iArray.Count(); idx++ )
			{
			pos = iArray[idx]->SetPos( pos );
			pos++; // for whitespace
			}
    	}
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::RepositionEntriesL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::RepositionEntriesL( const CESMRNcsAifEntry* aPosEntry )
    {
    FUNC_LOG;
    TInt pos = 0;
    CESMRNcsAifEntry* aEntry;
    for (int i=0;i<iArray.Count();i++)
        {
        aEntry = iArray[i];
        pos = aEntry->SetPos( pos );
        pos++; // for whitespace
        }

    // Reset the text
    SetCursorPosL( 0, EFalse ); //In case the cursor pos is invalid
    HBufC* text = NULL;
    text = GetFormattedAddressListL( iArray );
    CleanupStack::PushL( text );

    // text lenght plus one to ensure the formatting
    // used is full, not band formatting.
    SetUpperFullFormattingLength( text->Length() + 1 );

    // The stupid control will reset all atributes if the text is blank ( font color, etc.).
    if ( text->Length() == 0 )
        {
        CleanupStack::PopAndDestroy( text );
        text = KEmptySpace().AllocL();
        CleanupStack::PushL( text );
        SetTextL( text );
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
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::GetLookupTextLC()
// -----------------------------------------------------------------------------
//
HBufC* CESMRNcsAifEditor::GetLookupTextLC() const
    {
    FUNC_LOG;
    HBufC* text = GetTextInHBufL();

    if (text == NULL) return NULL;

    CleanupStack::PushL( text );
    TPtr ptr( text->Des() );
    ptr = ptr.LeftTPtr( CursorPos() );
    TChar semiColon(KSemiColon);
    TInt location = ptr.LocateReverse( semiColon );
    if( location != KErrNotFound )
        {
        ptr = ptr.RightTPtr( ptr.Length() - location -1 );
        ptr.TrimLeft();
        }
    return text;
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::GetFormattedAddressListLC()
// -----------------------------------------------------------------------------
//
HBufC* CESMRNcsAifEditor::GetFormattedAddressListLC(
    RPointerArray<CESMRNcsAifEntry>& aEntries,
    TBool aDisplayList ) const
    {
    FUNC_LOG;
    TInt length = CalculateAddressListLength( aEntries, aDisplayList );
    if( length <= 0 )
        {
        return HBufC::NewLC(0);
        }

    HBufC* buf = HBufC::NewLC( length );
    TPtr ptr = buf->Des();

    TInt count = aEntries.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if ( aDisplayList )
            {
            ptr.Append( aEntries[i]->DisplayString() );
            }
        else
            {
            ptr.Append( aEntries[i]->Address().EmailAddress() );
            ptr.Append( KAddressDelimeterSemiColon );
            }

        // append whitespace, if not in the last entry
        if ( i < count - 1 )
            {
            ptr.Append( KEmptySpace );
            }
        }

    return buf;
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::GetFormattedAddressListL()
// -----------------------------------------------------------------------------
//
HBufC* CESMRNcsAifEditor::GetFormattedAddressListL(
    RPointerArray<CESMRNcsAifEntry>& aEntries,
    TBool aDisplayList ) const
    {
    FUNC_LOG;
    HBufC* buf = GetFormattedAddressListLC( aEntries, aDisplayList );
    CleanupStack::Pop( buf );
    return buf;
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::CalculateAddressListLength()
// -----------------------------------------------------------------------------
//
TInt CESMRNcsAifEditor::CalculateAddressListLength(
    RPointerArray<CESMRNcsAifEntry>& aEntries,
    TBool aDisplayList ) const
    {
    FUNC_LOG;
    TInt length = 0;
    TInt count = aEntries.Count();
    for( TInt i = 0; i < count; i++ )
        {
        CESMRNcsAifEntry* aEntry = aEntries[ i ];
        if ( !aEntry  ) continue;
        if ( aDisplayList )
            {
            length += aEntry->Length();
            }
        else
            {
            // +1 is for semicolon
            length += aEntry->Address().EmailAddress().Length() + 1;
            }
        }

    // add one white space after that so the format is
    // aa(at)bb.com; cc(at)dd.com; ee(at)ff.com
    if( count > 1 )
        {
        // ( count - 1 ) we do need white space after the last address
        length += count - 1 ;
        }

    if( aEntries.Count() > 0 )
        {
        length += 2;
        }

    return length;
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::UpdateAddressAutoCompletionL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::UpdateAddressAutoCompletionL()
    {
    FUNC_LOG;
    HBufC* text = GetNonEntryTextLC();

    if( text )
        {
        iAddressPopupList->UpdatePopupContactListL( *text, EFalse );
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        iAddressPopupList->UpdatePopupContactListL( KNullDesC, EFalse );
        }
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::UpdateAddressAutoCompletionL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::UpdateAddressAutoCompletionL(
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

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::UpdateAddressListAllL()
// -----------------------------------------------------------------------------
//
void CESMRNcsAifEditor::UpdateAddressListAllL()
    {
    FUNC_LOG;
    iAddressPopupList->UpdatePopupContactListL( KNullDesC, ETrue );
    }

void CESMRNcsAifEditor::UpdateDuplicateEntryMarkings()
    {
    FUNC_LOG;
    const TInt entryCount = iArray.Count();
    for ( TInt ii = entryCount - 1; ii >= 0; --ii )
        {
        TBool duplicateFound = EFalse;
        for ( TInt jj = entryCount - 1; jj >= 0; --jj )
            {
            if ( ii != jj && iArray[ii]->IsSameDN( *iArray[jj] ) )
                {
                duplicateFound = ETrue;
                iArray[jj]->SetDup( ETrue );
                }
            }
        iArray[ii]->SetDup( duplicateFound );
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::CopyToClipboardL
// -----------------------------------------------------------------------------
void CESMRNcsAifEditor::CopyToClipboardL( const TDesC &aBuf )
    {
    FUNC_LOG;
    CClipboard* cb = CClipboard::NewForWritingLC( CCoeEnv::Static()->FsSession() );

    cb->StreamDictionary().At( KClipboardUidTypePlainText );

    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL( plainText );

    plainText->InsertL( 0 , aBuf );

    plainText->CopyToStoreL( cb->Store(), cb->StreamDictionary(), 0, plainText->DocumentLength() );

    CleanupStack::PopAndDestroy( plainText );
    cb->CommitL();
    CleanupStack::PopAndDestroy( cb );
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::HandleContactDeletionL
// -----------------------------------------------------------------------------
void CESMRNcsAifEditor::HandleContactDeletionL()
    {
    FUNC_LOG;
    const TInt count(iArray.Count());
    for (TInt i=count-1;i>=0;--i)
        {
        CESMRNcsAifEntry* entry = iArray[i];
        if ( FindTextL( &entry->DisplayString(), 0, 0 ) == KErrNotFound )
            {
            iArray.Remove(i);
            delete entry;
            }
        }
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::HandleTextUpdateL
// -----------------------------------------------------------------------------
void CESMRNcsAifEditor::HandleTextUpdateL()
    {
    FUNC_LOG;
    RecalculateEntryPositions();
    TCursorSelection textSelection = NonEntryTextAtPos( CursorPos() );
    if ( textSelection.Length() )
        {
        // Check non-entry text for complete entries.
        HandleTextUpdateL( textSelection );
        }
    UpdateAddressAutoCompletionL( textSelection );
    }
// -----------------------------------------------------------------------------
// CESMRNcsAifEditor::HandleTextUpdateL
// -----------------------------------------------------------------------------
void CESMRNcsAifEditor::HandleTextUpdateL( TCursorSelection& aSelection )
    {
    FUNC_LOG;
    TInt firstCharacter = aSelection.LowerPos();
    TInt lastCharacter = aSelection.HigherPos();
    for ( TInt ii = firstCharacter; ii <= lastCharacter; ++ii )
        {
        TChar character = CharAtPos( ii );
        if ( IsSentinel( character ) )
            {
            if ( character == KComma )
                {
                // Replace comma with semicolon
                CPlainText* text = Text();
                text->DeleteL( ii, 1 );
                text->InsertL( ii, KSemiColon );
                }

            // Create new entry.
            AddEntryL( firstCharacter, ii );
            TCursorSelection tmpSelection = NonEntryTextAtPos( CursorPos() );
            firstCharacter = tmpSelection.LowerPos();
            lastCharacter = tmpSelection.HigherPos();
            }
        }
    aSelection.SetSelection( lastCharacter, firstCharacter );

    }

// ---------------------------------------------------------------------------
// Handles navigation event.
// ---------------------------------------------------------------------------
//
void CESMRNcsAifEditor::HandleNavigationEventL()
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
TCursorSelection CESMRNcsAifEditor::NonEntryTextAtPos( TUint aPosition ) const
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
    return text;
    }

// ---------------------------------------------------------------------------
// Gets the range of text immediatelly before given position that does not
// belong to any entry.
// ---------------------------------------------------------------------------
//
TCursorSelection CESMRNcsAifEditor::NonEntryTextBeforePos( TUint aPosition ) const
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
TBool CESMRNcsAifEditor::IsSentinel( TChar aCharacter ) const
    {
    FUNC_LOG;
    return ( aCharacter == KSemiColon ||
        aCharacter == KComma );
    }

// ---------------------------------------------------------------------------
// Checks whether given event is considered as navigation event.
// ---------------------------------------------------------------------------
//
TBool CESMRNcsAifEditor::IsNavigationKey( const TKeyEvent& aKeyEvent ) const
    {
    FUNC_LOG;
    return ( aKeyEvent.iCode == EKeyLeftArrow ||
             aKeyEvent.iCode == EKeyRightArrow ||
             aKeyEvent.iCode == EKeyUpArrow ||
             aKeyEvent.iCode == EKeyDownArrow );
    }

// ---------------------------------------------------------------------------
// Creates new entry from the specified section of the the input field.
// ---------------------------------------------------------------------------
//
void CESMRNcsAifEditor::AddEntryL( TInt aStart, TInt aEnd )
    {
    FUNC_LOG;
    // Get the text for the entry (excluding the delimiter)
    TInt length = aEnd - aStart;
    HBufC* text = HBufC::NewLC( length );
    TPtr ptr = text->Des();
    Text()->Extract( ptr, aStart, length );
    ptr.Trim();
    if ( text->Length() )
        {
        AddAddressL( KNullDesC(), *text );
        }
    CleanupStack::PopAndDestroy( text );
    }
// End of File

