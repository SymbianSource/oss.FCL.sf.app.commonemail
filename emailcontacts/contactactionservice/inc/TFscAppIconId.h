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
* Description:  Utility class for defined icon identification.
 *
*/


#ifndef TFSCAPPICONID_H
#define TFSCAPPICONID_H

#include <TPbk2IconId.h>
#include <Pbk2UID.h>
#include <Pbk2IconId.hrh>

/**
 * Utility class for defined icon identification.
 */
class TFscAppIconId : public TPbk2IconId
    {
public:
    // Constructor

    /**
     * Constructor.
     *
     * @param aPbkCoreIconId    The core Phonebook 2 icon id.
     */
    inline TFscAppIconId(TPbk2CoreIconId aPbkCoreIconId) :
        TPbk2IconId(TUid::Uid(KPbk2UID3), aPbkCoreIconId)
        {
        }
    };

#endif // TFSCAPPICONID_H
// End of File
