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
* Description:  Definition of the class CMsgConverter
*
*/


#ifndef __CFSMSGCONVERTER_INTERFACE_H__
#define __CFSMSGCONVERTER_INTERFACE_H__

// INCLUDES
#include <e32base.h>
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMsgActive;


/**
 * Abstract class provding interface to AS Library that will convert the
 * RFC 822 message to MTM message.
 * Library will be loaded as a polymorfic dll.
 */
class CMsgConverter : public CBase
    {
public:
    virtual CMsgActive* GetMsgActiveObjectL( const TMsvId aParentId, const TDesC& aFileName, 
  								const TDesC8& aServerId,CMsvServerEntry& aServerEntry,
  								TBool aNestedEmailAttachment = EFalse, 
  								TMsvId aOriginalAttachmentId = NULL 
  							    )=0;
    virtual void StartL( TRequestStatus* aStatus )=0;
private:
};
		
#endif //__CFSMSGCONVERTER_INTERFACE_H__

// End of File
