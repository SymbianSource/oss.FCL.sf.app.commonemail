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
* Description:  ESMR alarm implementation
 *
*/


#ifndef CESMRALARM_H
#define CESMRALARM_H

// INCLUDES
#include <barsread.h>

// FORWARD DECLARATIONS
class CCoeEnv;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CESMRAlarm ): public CBase
    {
public:
    /**
     * Constructor
     */
    IMPORT_C CESMRAlarm();
    
    /**
     * Destructor
     */
    IMPORT_C ~CESMRAlarm();

public:
    /**
     * Loads alarm valus from resources
     */
    IMPORT_C void ConstructFromResourceL( TResourceReader& aReader );
    
    /**
     * Loads iText from resources
     */
    IMPORT_C void LoadTextL( CCoeEnv* aCoeEnv );
    
    /**
     * Returns relative alarm time in minutes
     * 
     * @return relative alarm time
     */
    IMPORT_C TTimeIntervalMinutes RelativeTimeInMinutes();
    
    /**
     * Returns alarm text
     * 
     * @return alarm text
     */
    IMPORT_C HBufC* Text();
    
    /**
     * Returns alarm id
     * 
     * @return alarm id
     */
    IMPORT_C TInt Id();
    
    /**
     * Returns relative alarm time
     * 
     * @returns relative alarm time
     */
    IMPORT_C TInt Relative();

private:
    /**
     * Alarm id
     */
    TInt iId;
    
    /**
     * Alarm value
     */
    TInt iValue;
    
    /**
     * Relative alarm value
     */
    TInt iRelative;
    
    /**
     * Alarm text id
     */
    TInt iTxtLink;
    
    /**
     * Own: Alarm text
     */
    HBufC* iText;
    };

#endif  // CESMRALARM_H

