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
* Description:  Mail icon off/on handling. Uses parts from previous Bridge
*                 implementation.
*
*/


#include "emailtrace.h"
#include <e32property.h>

#include "fsmailiconhandler.h"
#include "fsnotificationhandlertimer.h"

// Define startup Max retry counter for trying to set icon property
static const TInt KMaxIconRetry = 5;

// Timeout value for retrying to set the icon property again
static const TInt KIconRetryTimeout = 10*1000*1000;


CFSMailIconHandler* CFSMailIconHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
	CFSMailIconHandler* self =
	    new (ELeave) CFSMailIconHandler( aOwner );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

CFSMailIconHandler::CFSMailIconHandler(
    MFSNotificationHandlerMgr& aOwner ) :
    CFSNotificationHandlerBase( aOwner ),
    iState( EStarting ),
    iRetryCount( 0 )
    {
    FUNC_LOG;
    }

void CFSMailIconHandler::ConstructL()
    {
    FUNC_LOG;
    CFSNotificationHandlerBase::ConstructL();
    
    iTimer = CFSNotificationHandlerTimer::NewL( *this );
    
    // Initialize
    TimerCallBackL( KErrNone );
    }

CFSMailIconHandler::~CFSMailIconHandler()
    {
    FUNC_LOG;
    SetObserving( EFalse );
    delete iTimer;
    iTimer = NULL;
    }

// ---------------------------------------------------------------------------
// At startup this function will try up to KMaxIconRetry times
// to set the inital Icon property. If the property still fails to be set, we will
// assume that the Icon is not supported by the hardware, and we won't continue.
// ---------------------------------------------------------------------------
//
void CFSMailIconHandler::TimerCallBackL( TInt aError )
    {
    FUNC_LOG;
    // Active object request complete handler
	if ( aError == KErrNone )
	    {

        switch ( iState )
            {
            case EStarting:
                {
                TInt ret( KErrNone );
                ret = TestIcon();

    			if ( KErrNone != ret )
        			{
    		 		// Setting Icon Failed
    		 		iRetryCount++;
    		 		if ( iRetryCount > KMaxIconRetry )
        		 		{
    			 		// Give up, assume Email Icon is Not supported
    			 		// Message store will not be observed for changes
    			 		ERROR_1( ret, "CFSMailIconHandler startup failed %d", ret );
    		 			iState = EFailed;
        		 		}
    		 		else
        		 		{
    		 		    // Start a timer to wait 10 seconds
    					iTimer->After( KIconRetryTimeout );
        		 		}

        		    }
    			else 
        			{
        			SetObserving( ETrue );
    				iState = EStarted;
        			}

                break;
                }
            case EStarted:
                {
    			// Unexpected Icon started state
                break;
                }
            default:
                {
                break;
                }
            }
    	}
        else 
    	{
        // Error
    	}
    }

// ---------------------------------------------------------------------------
// HandleEvent is implemented also here because we need to check the
// Home Screen status.
// ---------------------------------------------------------------------------
//
void CFSMailIconHandler::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* aParam3 )
    {
    FUNC_LOG;
    // Event is checked also in CFSNotificationHandlerBase::HandleEventL.
    // Implementation could be improved so that there would be no need
    // for doing it twice. Now it must be done here as we don't want
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

TInt CFSMailIconHandler::SetProperty( TUid aCategory, TUint aKey, TInt aValue )
    {
    FUNC_LOG;
	RProperty property;
	TInt ret = property.Attach( aCategory, aKey );

	if ( KErrNone == ret )
	    {
   		ret = property.Set( aValue );
	    }
    ERROR_2( ret, "Error %d in setting property %d", ret, aKey )

	property.Close();

    return ret;
    }
    
TInt CFSMailIconHandler::TestIcon()
    {
    FUNC_LOG;
    RProperty property;
	TInt ret = property.Attach( KPSUidCoreApplicationUIs, 
	    KCoreAppUIsNewEmailStatus );

    // Intention here is just to test the icon by first readin it and then
    // writing the same value back. Notice that it might be possible that 
    // something has turned the icon off in between the reading and writing.
    // If thisproves to be a problem then change this.
	if ( KErrNone == ret )
	    {	    
	    TInt currentValue( 0 );
	    ret = property.Get( currentValue );
	    
	    
	    if ( KErrNone == ret )
	        {
   		    ret = property.Set( currentValue );
	        }
	    }
	property.Close();
    return ret;
    }

void CFSMailIconHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    /*
     * Mailindicator handling was removed fomr 9.2
     */
    /*SetProperty( KPSUidCoreApplicationUIs, 
        KCoreAppUIsNewEmailStatus, ECoreAppUIsNewEmail );*/
    }

void CFSMailIconHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    /*
     * Mailindicator handling was removed fomr 9.2
     */
    /*SetProperty( KPSUidCoreApplicationUIs, 
        KCoreAppUIsNewEmailStatus, ECoreAppUIsNoNewEmail );*/
    }

