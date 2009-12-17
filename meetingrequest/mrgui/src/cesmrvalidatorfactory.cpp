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
* Description:  ESMR ui base class
*
*/


#include "emailtrace.h"
#include "cesmrvalidatorfactory.h"
#include "cesmrmeetingtimevalidator.h"
#include "cesmrtodotimevalidator.h"
#include "cesmrmemotimevalidator.h"
#include "cesmrannivtimevalidator.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRValidatorFactory::CreateValidatorL
// ---------------------------------------------------------------------------
//
MESMRFieldValidator* CESMRValidatorFactory::CreateValidatorL(
        TESMRCalendarEventType aType )
    {
    FUNC_LOG;
    MESMRFieldValidator* validator=  NULL;
    switch ( aType )
        {
        case EESMREventTypeAppt: // activated by meeting and meeting request
        	{
            validator = CESMRMeetingTimeValidator::NewL ( );
            break;
        	}
        case EESMREventTypeETodo: // activated bt To-do
        	{
            validator = CESMRTodoTimeValidator::NewL ( );
            break;
        	}
        case EESMREventTypeEEvent: // activated by Memo- Day note
        	{
            validator = CESMRMemoTimeValidator::NewL ( );
            break;
        	}
        case EESMREventTypeEAnniv: // Activated by Aniverssary
        	{
            validator = CESMRAnnivTimeValidator::NewL ( );
            break;
        	}
        case EESMREventTypeEReminder: // Is meeting
        default:
        	{
        	}
        }
    return validator;
    }
// EOF

