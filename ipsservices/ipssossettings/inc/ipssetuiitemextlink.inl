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
* Description:  Implementation of extended link item.
*
*/


// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::Value()
// ----------------------------------------------------------------------------
//
inline TInt64 CIpsSetUiItemLinkExt::Value() const
    {
    return iUserValue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::Value()
// ----------------------------------------------------------------------------
//
inline void CIpsSetUiItemLinkExt::SetValue( const TInt64 aValue )
    {
    iUserValue = aValue;
    }

// End of file
