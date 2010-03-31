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
* Description:  Definition for ES MR Entry field
*
*/


#ifndef TESMRENTRYFIELD_H
#define TESMRENTRYFIELD_H

//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 *  TESMREntryField defines ES MR entry field.
 *
 *  Entry field information is read from resource. ESMR_FIELD
 *  resource defines ES MR entry field in resources.
 *
 * @lib esmrservices.lib
 */
class TESMREntryField
    {
public: // data
    /**
    * Field id
    */
    TESMREntryFieldId iFieldId;

    /**
    * Field type
    */
    TESMRFieldType  iFieldViewMode;

    /**
    * Field mode
    */
    TESMRFieldMode  iFieldMode;
    };

#endif // TESMRENTRYFIELD_H
