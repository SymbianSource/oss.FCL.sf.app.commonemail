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
* Description:  common base mailbox object
*
*/

#include <AknUtils.h>           // AknTextUtils
    
#include "emailtrace.h"
#include "cfsmailboxbase.h"

_LIT( KCharsToReplace, "\r\n\t\x2028\x2029" );

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailBoxBase::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBoxBase* CFSMailBoxBase::NewLC(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
  CFSMailBoxBase* api = new (ELeave) CFSMailBoxBase();
  CleanupStack:: PushL(api);
  api->ConstructL(aMailBoxId);
  return api;
} 

// -----------------------------------------------------------------------------
// CFSMailBoxBase::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBoxBase* CFSMailBoxBase::NewL(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
  CFSMailBoxBase* api =  CFSMailBoxBase::NewLC(aMailBoxId);
  CleanupStack:: Pop(api);
  return api;
}
// -----------------------------------------------------------------------------
// CFSMailBoxBase::CFSMailBoxBase
// -----------------------------------------------------------------------------
CFSMailBoxBase::CFSMailBoxBase() : CExtendableEmail()
{
    FUNC_LOG;
	
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::~CFSMailBoxBase
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBoxBase::~CFSMailBoxBase()
{
    FUNC_LOG;
    if( iMailBoxName )
        {
  		delete iMailBoxName;
        }
	iMailBoxName = NULL;

    if( iBrId )
        {
  		delete iBrId;
        }
	iBrId = NULL;

    if( iOwnMailAddress )
        {
  		delete iOwnMailAddress;
        }
	iOwnMailAddress = NULL;	

    if( iMRInfoProcessor )
        {
        delete iMRInfoProcessor;
        }
    iMRInfoProcessor = NULL;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::ConstructL
// -----------------------------------------------------------------------------
void CFSMailBoxBase::ConstructL(const TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
	iMailBoxId = aMailBoxId;	
	
	// <cmail> code moved here and modified to fix OOM test
    // prepare null empty descriptor
    iMailBoxName = HBufC::NewL(1);
    iMailBoxName->Des().Copy(KNullDesC());

    iBrId = HBufC::NewL(1);
    iBrId->Des().Copy(KNullDesC());
	// </cmail> 	
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailBoxBase::GetId() const
{
    FUNC_LOG;
	return iMailBoxId;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailBoxBase::GetName() const
{
    FUNC_LOG;
	return *iMailBoxName;		
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetName( const TDesC& aMailBoxName )
{
    FUNC_LOG;
	// init mailbox name
	HBufC* name = HBufC::New(aMailBoxName.Length());
	
	// store new mailbox name
	if(name)
	{
        name->Des().Copy(aMailBoxName);
        TPtr textPtr = name->Des();
        AknTextUtils::ReplaceCharacters( textPtr, KCharsToReplace, ' ' );
	
		delete iMailBoxName;
		iMailBoxName = name;
	}
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetStatus
// -----------------------------------------------------------------------------
EXPORT_C TFSMailBoxStatus CFSMailBoxBase::GetStatus( ) const
{
    FUNC_LOG;
	return EFSMailBoxOffline;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetStatus
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetStatus( const TFSMailBoxStatus /*aStatus*/ )
{
    FUNC_LOG;
	
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetRCLInfo
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::GetRCLInfo(TUid& aProtocolUid, TUint& aAccountUid)
{
    FUNC_LOG;
	aProtocolUid = iProtocolUid;
	aAccountUid = iAccountUid;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetRCLInfo
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetRCLInfo(const TUid aProtocolUid, const TUint aAccountUid)
{
    FUNC_LOG;
	iProtocolUid = aProtocolUid;
	iAccountUid = aAccountUid;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetSettingsUid
// -----------------------------------------------------------------------------
EXPORT_C const TUid CFSMailBoxBase::GetSettingsUid()
	{
    FUNC_LOG;
	return iSettingsUid;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetSettingsUid
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetSettingsUid(const TUid aUid)
	{
    FUNC_LOG;
		iSettingsUid = aUid;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::MRInfoProcessorL
// -----------------------------------------------------------------------------
EXPORT_C MMRInfoProcessor& CFSMailBoxBase::MRInfoProcessorL()
	{
    FUNC_LOG;
	return *iMRInfoProcessor;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::IsMRInfoProcessorSet
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailBoxBase::IsMRInfoProcessorSet()
	{
    FUNC_LOG;
	if(iMRInfoProcessor)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetMRInfoProcessorL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetMRInfoProcessorL(MMRInfoProcessor* aMRInfoProcessor)
	{
    FUNC_LOG;

	if(iMRInfoProcessor)
	{
		delete iMRInfoProcessor;
	}
	iMRInfoProcessor = aMRInfoProcessor;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::OwnMailAddress
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress& CFSMailBoxBase::OwnMailAddress( )
{
    FUNC_LOG;
	return *iOwnMailAddress;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetOwnMailAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetOwnMailAddressL( CFSMailAddress* aOwnMailAddress)
{
    FUNC_LOG;
	if(iOwnMailAddress)
		{
		delete iOwnMailAddress;
		}
	iOwnMailAddress = aOwnMailAddress;

	// set also branding id based on domain name
    TPtrC ptr = OwnMailAddress().GetEmailAddress();
    TInt index = ptr.Locate('@') + 1;
    if(index > 0 && ptr.Length() > 0)
    {
        if(iBrId)
        {
            delete iBrId;
            iBrId = NULL;
        }
        ptr.Set( ptr.Right( ptr.Length() - index ) );        
        iBrId = HBufC::New(ptr.Length());
        iBrId->Des().Copy(ptr);
    }
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::BrandingId
// -----------------------------------------------------------------------------
TDesC& CFSMailBoxBase::BrandingId( )
{
    FUNC_LOG;
    return *iBrId;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::MailBoxId
// -----------------------------------------------------------------------------
void CFSMailBoxBase::SetMailBoxId( TFSMailMsgId aMailBoxId )
{
    FUNC_LOG;
    iMailBoxId = aMailBoxId;
}


