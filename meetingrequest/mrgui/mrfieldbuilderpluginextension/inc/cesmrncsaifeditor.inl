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
* Description:  Inline methods implementation
*
*/


// ---------------------------------------------------------------------------
// CESMRNcsAifEditor::SetPopupList
// ---------------------------------------------------------------------------
//
inline void CESMRNcsAifEditor::SetPopupList( MESMRNcsAddressPopupList* aPopupList )
    {
    iAddressPopupList = aPopupList;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::DisplayLength
// ---------------------------------------------------------------------------
//
inline TInt CESMRNcsAifEntry::DisplayLength() const
    {
    return iDisplayString->Length();
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::DisplayString
// ---------------------------------------------------------------------------
//
inline const TDesC& CESMRNcsAifEntry::DisplayString() const
    {
    return *iDisplayString;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::SetPos
// ---------------------------------------------------------------------------
//
inline TInt CESMRNcsAifEntry::SetPos( TInt aPos )
    {
    iCursorPos = aPos;
    iAnchorPos = aPos + DisplayLength();
    return iAnchorPos;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::Includes
// ---------------------------------------------------------------------------
//
inline TBool CESMRNcsAifEntry::Includes( TInt aPos ) const
    {
    return ( aPos >= LowerPos() && aPos < HigherPos() );
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::Start
// ---------------------------------------------------------------------------
//
inline TInt CESMRNcsAifEntry::Start() const
    {
    return LowerPos();
    }
	
// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::End
// ---------------------------------------------------------------------------
//
inline TInt CESMRNcsAifEntry::End() const
    {
    return HigherPos();
    }
	
// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::Address
// ---------------------------------------------------------------------------
//
inline const CESMRNcsEmailAddressObject& CESMRNcsAifEntry::Address() const
    {
    return *iAddress;
    }
	
// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::IsSameDN
// ---------------------------------------------------------------------------
//
inline TBool CESMRNcsAifEntry::IsSameDN( const CESMRNcsAifEntry& entry ) const
    {
    return iDisplayString->Compare(entry.DisplayString()) == 0;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::IsDup
// ---------------------------------------------------------------------------
//
inline TBool CESMRNcsAifEntry::IsDup() const 
    {
    return iIsDup;
    }

// ---------------------------------------------------------------------------
// CESMRNcsAifEntry::SetDup
// ---------------------------------------------------------------------------
//
inline void CESMRNcsAifEntry::SetDup( TBool aDup ) 
    {
    iIsDup = aDup;
    SetDisplayStringL();
    }
