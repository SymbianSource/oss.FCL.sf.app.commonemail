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
* Description: This file defines class CESMRFieldCommandEvent.
*
*/


#ifndef CESMRFIELDCOMMANDEVENT_H
#define CESMRFIELDCOMMANDEVENT_H

#include <e32base.h>
#include "mesmrfieldevent.h"

/**
 *  Event class for sending command events from field to its observers.
 *
 *  @code
 *   TESMRFieldCommandEvent event( <command_id> );  
 *   CESMRField::NotifyEvent( event );
 *  @endcode
 *
 *  @lib esmrfieldbuildercommon.lib
 */
NONSHARABLE_CLASS( CESMRFieldCommandEvent ) : public CBase,
                                              public MESMRFieldEvent
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aCommand the command id.
     * @return command event.
     */
    IMPORT_C static CESMRFieldCommandEvent* NewL( MESMRFieldEventNotifier* aSource,
                                                  TInt aCommand );
    
    /**
     * Two-phased constructor.
     *
     * @param aCommand the command id.
     * @return command event.
     */
    IMPORT_C static CESMRFieldCommandEvent* NewLC( MESMRFieldEventNotifier* aSource,
                                                   TInt aCommand );
    
    /**
     * Destructor
     */
    virtual ~CESMRFieldCommandEvent();

// from base class MESMRFieldEvent

    /**
     * From MESMRFieldEvent.
     * Gets event type.
     *
     * @return MESMRFieldEvent::EESMRFieldCommandEvent
     */
    MESMRFieldEvent::TEventType Type() const;
    
    /**
     * From MESMRFieldEvent.
     * Gets the event source.
     * 
     * @return pointer to the sender or NULL.
     */
    MESMRFieldEventNotifier* Source() const;
    
    /**
     *  Gets the parameter count.
     *  @return 1.
     */
    TInt ParamCount() const;
        
    /**
     * Gets the event parameter.
     * Index 0 returns the command id as TInt.
     * 
     * @param aIndex parameter index.
     * @return the parameter.
     */
    TAny* Param( TInt aIndex ) const;

private:
    
    CESMRFieldCommandEvent( MESMRFieldEventNotifier* aSource );
    
    void ConstructL( TInt aCommand );
    
private: // data

    /**
     * Source.
     * Not owned.
     */
    MESMRFieldEventNotifier* iSource;
        
    /**
     * Command id.
     * Own.
     */
    TInt* iCommand;

    };

#endif // CESMRFIELDCOMMANDEVENT_H
