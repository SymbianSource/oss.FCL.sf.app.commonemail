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
#include "emailtrace.h"
#include "cfsmailaddress.h"
//</cmail>

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

// -----------------------------------------------------------------------------
// CFSMailAddress::ConstructL
// -----------------------------------------------------------------------------
void CFSMailAddress::ConstructL()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::CFSMailAddress
// -----------------------------------------------------------------------------
CFSMailAddress::CFSMailAddress()
    {
    FUNC_LOG;
    iEmailAddress = HBufC::New(1);
    iEmailAddress->Des().Copy(KNullDesC());

    iDisplayName = HBufC::New(1);
    iDisplayName->Des().Copy(KNullDesC());
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::~CFSMailAddress
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress::~CFSMailAddress()
    {
    FUNC_LOG;
    if(iEmailAddress)
        {
        delete iEmailAddress;
        }
    iEmailAddress = NULL;
        
    if (iDisplayName)
        {
        delete iDisplayName;
        }
    iDisplayName = NULL;
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::GetEmailAddress
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailAddress::GetEmailAddress() const
    {
    FUNC_LOG;
    return *iEmailAddress;
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::SetEmailAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailAddress::SetEmailAddress(const TDesC& aAddress)
    {
    FUNC_LOG;
    // init mailbox name
    HBufC* address = HBufC::New(aAddress.Length());
    
    // store new mailbox name
    if(address)
        {
        delete iEmailAddress;
        iEmailAddress = address;
        iEmailAddress->Des().Copy(aAddress);
        }

    }

// -----------------------------------------------------------------------------
// CFSMailAddress::GetDisplayName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailAddress::GetDisplayName() const
    {
    FUNC_LOG;
    return *iDisplayName;
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::SetDisplayName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailAddress::SetDisplayName(const TDesC& aDisplayName)
    {
    FUNC_LOG;
    // init mailbox name
    HBufC* name = HBufC::New(aDisplayName.Length());
    
    // store new mailbox name
    if(name)
        {
        delete iDisplayName;
        iDisplayName = name;
        iDisplayName->Des().Copy(aDisplayName);
        }
    }

