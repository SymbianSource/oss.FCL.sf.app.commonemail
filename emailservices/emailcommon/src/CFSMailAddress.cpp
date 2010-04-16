/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email address object
*
*/


//<cmail>
#include <nmcommonheaders.h>
#include "emailtrace.h"
#include "CFSMailAddress.h"
//</cmail>

//<qmail>
#include "nmaddress.h"
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailAddress::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailAddress::NewLC()
{
    FUNC_LOG;
    CFSMailAddress* adr = new (ELeave) CFSMailAddress();
    CleanupStack::PushL(adr);
    adr->ConstructL();
    return adr;
} 

// -----------------------------------------------------------------------------
// CFSMailAddress::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailAddress::NewL()
    {
    FUNC_LOG;
    CFSMailAddress* adr =  CFSMailAddress::NewLC();
    CleanupStack::Pop(adr);
    return adr;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailAddress::NewL( const NmAddress& aNmAddress )
{
    FUNC_LOG;
    CFSMailAddress* adr = new (ELeave) CFSMailAddress();
    CleanupStack::PushL(adr);
    adr->ConstructL(aNmAddress);
    CleanupStack::Pop(adr);
    return adr;
}

// </qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::ConstructL
// -----------------------------------------------------------------------------
void CFSMailAddress::ConstructL()
{
    FUNC_LOG;
    // Construction of shared data object
    iNmPrivateAddress = new NmAddressPrivate();
}
// <qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::ConstructL
// --------------------------------_--------------------------------------------
void CFSMailAddress::ConstructL( const NmAddress& aNmAddress )
{
    FUNC_LOG;
    // shared data object
    iNmPrivateAddress = aNmAddress.d;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailAddress::CFSMailAddress
// -----------------------------------------------------------------------------
CFSMailAddress::CFSMailAddress()
{
    FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CFSMailAddress::~CFSMailAddress
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress::~CFSMailAddress()
{
    FUNC_LOG;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::GetEmailAddress
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailAddress::GetEmailAddress() const
{
    FUNC_LOG;
    iEmailAddressPtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateAddress->mAddress.utf16()),
        iNmPrivateAddress->mAddress.length());
    return iEmailAddressPtr;
}

// -----------------------------------------------------------------------------
// CFSMailAddress::SetEmailAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailAddress::SetEmailAddress( const TDesC& aAddress )
{
    FUNC_LOG;
    QString qtEmailAddress = QString::fromUtf16(aAddress.Ptr(), aAddress.Length());
    iNmPrivateAddress->mAddress = qtEmailAddress;
}

// -----------------------------------------------------------------------------
// CFSMailAddress::GetDisplayName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailAddress::GetDisplayName() const
{
    FUNC_LOG;
    iDisplayNamePtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateAddress->mDisplayName.utf16()),
        iNmPrivateAddress->mDisplayName.length());
    return iDisplayNamePtr;
}

// -----------------------------------------------------------------------------
// CFSMailAddress::SetDisplayName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailAddress::SetDisplayName( const TDesC& aDisplayName )
{
    FUNC_LOG;
    QString qtDisplayName = QString::fromUtf16(aDisplayName.Ptr(), aDisplayName.Length());
    iNmPrivateAddress->mDisplayName = qtDisplayName;
}
  
// -----------------------------------------------------------------------------
// CFSMailAddress::GetNmAddress
// -----------------------------------------------------------------------------
EXPORT_C NmAddress CFSMailAddress::GetNmAddress()
{
    FUNC_LOG;
    NmAddress nmAddress(iNmPrivateAddress);
    return nmAddress;
}
// </qmail>


