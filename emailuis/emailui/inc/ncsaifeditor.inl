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
* Description:  Inline methods implementation
*
*/


// ---------------------------------------------------------------------------
// CNcsAifEditor::SetPopupList
// ---------------------------------------------------------------------------
//
inline void CNcsAifEditor::SetPopupList( MNcsAddressPopupList* aPopupList )
    {
    iAddressPopupList = aPopupList;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::DisplayLength
// ---------------------------------------------------------------------------
//
inline TInt CNcsAifEntry::DisplayLength() const
    {
    return iDisplayString->Length();
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::DisplayString
// ---------------------------------------------------------------------------
//
inline const TDesC& CNcsAifEntry::DisplayString() const
    {
    return *iDisplayString;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::SetPos
// ---------------------------------------------------------------------------
//
inline TInt CNcsAifEntry::SetPos( TInt aPos )
    {
    iCursorPos = aPos;
    iAnchorPos = aPos + DisplayLength();
    return iAnchorPos;
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::Includes
// ---------------------------------------------------------------------------
//
inline TBool CNcsAifEntry::Includes( TInt aPos ) const
    {
    return ( aPos >= LowerPos() && aPos < HigherPos() );
    }

// ---------------------------------------------------------------------------
// CNcsAifEntry::Start
// ---------------------------------------------------------------------------
//
inline TInt CNcsAifEntry::Start() const
    {
    return LowerPos();
    }
	
// ---------------------------------------------------------------------------
// CNcsAifEntry::End
// ---------------------------------------------------------------------------
//
inline TInt CNcsAifEntry::End() const
    {
    return HigherPos();
    }
	
// ---------------------------------------------------------------------------
// CNcsAifEntry::Address
// ---------------------------------------------------------------------------
//
inline const CNcsEmailAddressObject& CNcsAifEntry::Address() const
    {
    return *iAddress;
    }
	
// ---------------------------------------------------------------------------
// CNcsAifEntry::IsDup
// ---------------------------------------------------------------------------
//
inline TBool CNcsAifEntry::IsDup() const 
    {
    return iIsDup;
    }
