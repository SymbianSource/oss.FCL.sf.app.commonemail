/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MBUtils ECOM definition
*
*/
#ifndef C_MRENTRYPROCESSOR_H
#define C_MRENTRYPROCESSOR_H

#include "mmrentryprocessor.h"
#include "esmrdef.h"
#include <e32base.h>

class MESMRCalDbMgr;

class CMREntryProcessor : 
        public CBase,
        public MESMREntryProcessor
    {
public:
    static CMREntryProcessor* NewL(
            TESMRCalendarEventType aEventType,
            MESMRCalDbMgr& aCalDb );
    
    ~CMREntryProcessor();
    
private:
    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
     */
    TUid iDtor_ID_Key;    
    };

#include "cmrentryprocessor.inl"

#endif // C_MRENTRYPROCESSOR_H

// EOF
