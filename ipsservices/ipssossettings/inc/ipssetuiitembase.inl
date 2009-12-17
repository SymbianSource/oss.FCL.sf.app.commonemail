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
* Description:  Implementation of base item.
*
*/


// ----------------------------------------------------------------------------
// CIpsSetUiItem::Type()
// ----------------------------------------------------------------------------
//
inline TIpsSetUiSettingsType CIpsSetUiItem::Type() const
    {
    return iItemType;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::Value()
// ----------------------------------------------------------------------------
//
inline TInt64 CIpsSetUiItem::Value() const
    {
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::Value()
// ----------------------------------------------------------------------------
//
inline void CIpsSetUiItem::SetValue( const TInt64 /* aValue */ )
    {
    // Do nothing
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::Text()
// ----------------------------------------------------------------------------
//
inline const TDesC& CIpsSetUiItem::Text() const
    {
    return KNullDesC;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::SetText()
// ----------------------------------------------------------------------------
//
inline void CIpsSetUiItem::SetText(
    const TDesC& /* aText */ )
    {
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::HasLinkArray()
// ----------------------------------------------------------------------------
//
inline TBool CIpsSetUiItem::HasLinkArray() const
    {
    return EFalse;
    }

// End of file
