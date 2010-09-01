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
* Description:  Class to handle mail led turning on. Uses parts from previous
*                Bridge implementation.
*
*/


#include "emailtrace.h"
#include <e32property.h>
#include "EmailLedValues.h"
#include "fsmailledhandler.h"
#include "fsnotificationhandlertimer.h"


// Timeout value for retrying to set the led property again
static const TInt KLedRetryTimeout = 15*1000*1000;


CFSMailLedHandler* CFSMailLedHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CFSMailLedHandler* self =
        new (ELeave) CFSMailLedHandler( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CFSMailLedHandler::CFSMailLedHandler(
    MFSNotificationHandlerMgr& aOwner ) :
    CFSNotificationHandlerBase( aOwner ),
    iState( EStarting ),
    iRetryCount( 0 )
    {
    FUNC_LOG;
    }

void CFSMailLedHandler::ConstructL()
    {
    FUNC_LOG;
    CFSNotificationHandlerBase::ConstructL();
    
    iTimer = CFSNotificationHandlerTimer::NewL( *this );
    
    // Now try and retrieve the property value
    TimerCallBackL( KErrNone );
    }

CFSMailLedHandler::~CFSMailLedHandler()
    {
    FUNC_LOG;
    delete iTimer;
    iTimer = NULL;
    }

// ---------------------------------------------------------------------------
// At startup this function will try up to KMaxLedRetry times
// to open the LED property. If the property still fails to open, we will
// assume that the LED is not supported by the hardware.
// ---------------------------------------------------------------------------
//
void CFSMailLedHandler::TimerCallBackL( TInt aError )
    {
    FUNC_LOG;
    // request complete handler
    if ( aError == KErrNone )
        {
        switch ( iState )
            {
            case EStarting:
                {
                // Check if LED property exists. If not then this handler
                // is not necessary.
                RProperty property;

                TInt ret = property.Attach( KPropertyUidEmailLedCategory,
                                            KEmailLEDOnRequest );
                if ( KErrNone != ret )
                    {
                    // Led Starting Attach Failed
                    property.Close();
                    
                    // timer not needed any more
                    delete iTimer;
                    iTimer = NULL;
                    
                    ERROR_1( ret, "Attaching prop KPropertyUidEmailLedCategory failed: %d", ret )
                    return;
                    }

                TInt val;
                ret = property.Get( val );

                if ( KErrNone != ret )
                    {
                    iRetryCount++;
                    if ( iRetryCount > KMaxLedRetry )
                        {
                        // Done Retrying, assume Led not supported
                        iState = ENotSupportLED;
                        }
                    else
                        {
                        //start a timer to wait 15 seconds
                        iTimer->After( KLedRetryTimeout );
                        }
                    }
                else 
                    {
                    // We are now ready to observe mail events
                    SetObserving( ETrue );
                    iState = ESupportLED;
                    }
                property.Close();
                break;
                }
            case ESupportLED:
            case ENotSupportLED:
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleEvent is implemented also here because we need to check the
// Home Screen status.
// ---------------------------------------------------------------------------
//
void CFSMailLedHandler::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* aParam3 )
    {
    FUNC_LOG;
    // Event is checked also in CFSNotificationHandlerBase::HandleEventL.
    // Implementation could be improved so that there would be no need
    // for doing it twice. Now it has must be done here as we don't want
    // to check HS if this would not be a new mail. Also
    // CFSNotificationHandlerBase::HandleEventL needs the check the event
    // because CFSNotificationHandlerBase::HandleEventL is used in some
    // cases alone. (without the kind of prechecking that this function
    // does)
    if ( aEvent == TFSEventNewMail )
        {
        CFSNotificationHandlerBase::HandleEventL( aEvent,
                                                  aMailbox,
                                                  aParam1,
                                                  aParam2,
                                                  aParam3 );        
        }
    }



void CFSMailLedHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    RProperty property;

    TInt ret = property.Attach( KPropertyUidEmailLedCategory, KEmailLEDOnRequest );

    if ( KErrNone != ret )
        {
        // Led Attach Failed
        property.Close();
        return;
        }

    ret = property.Set( KSetEmailLedOn );
    property.Close();
    ERROR_1( ret, "TurnLedOn failed: %d", ret )
    }

void CFSMailLedHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    return;
    }

// End of file

