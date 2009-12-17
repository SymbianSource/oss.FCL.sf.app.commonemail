/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition for mrui list queries
*
*/

#ifndef CESMRRECURRENCE_H
#define CESMRRECURRENCE_H

#include <e32base.h>
#include <barsread.h> // resource reader
//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 * Helper class for recurrence texts
 */
NONSHARABLE_CLASS( CESMRRecurrence ) : public CBase
    {
public:
    /**
     * Constructor.
     */
    IMPORT_C CESMRRecurrence();
    
    /**
     * Destructor.
     */
    IMPORT_C ~CESMRRecurrence();

    /**
     * Constructs the recurrence text from resource.
     *
     * @param aEnv - Control environment to be used
     * @param aReader - resource reader for reading the correct text
     */
    IMPORT_C void ConstructFromResourceL( 
					CCoeEnv* aEnv, TResourceReader& aReader );

    /**
     * Get the type of recurrence
     *
     * @return type of recurrence
     */
    IMPORT_C TESMRRecurrenceValue RecurrenceValue();
    
    /**
     * Get the descriptor text for the type of recurrence
     *
     * @return descriptor text for the type of recurrence
     */
    IMPORT_C HBufC& RecurrenceText();
    
private:
    //Own: ID of the wanted recurrence text
    TESMRRecurrenceValue iId;
    //Own: Descriptor for the actual text
    HBufC* iTxt;
    };
#endif
