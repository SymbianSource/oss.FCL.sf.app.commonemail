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
* Description:  Generic field event declaration
*
*/


#ifndef CESMRGENERICFIELDEVENT_H
#define CESMRGENERICFIELDEVENT_H

#include <e32base.h>

#include "mesmrfieldevent.h"

class CESMRFieldEventValue;
class CESMRField;

/**
 *  Generic field event.
 *  Field event implementation with type information and any parameter count.
 *
 *  @code
 *   MESMRFieldEvent::TEventType type = MESMRFieldEvent::EESMRFieldChangeEvent;
 *   CESMRGenericFieldEvent* event = CESMRGenericFieldEvent::NewLC( type );
 *   event->AddParamL( param );
 *   CESMRField::NotifyEventL( *event );
 *   CleanupStack::PopAndDestroy( event );
 *  @endcode
 *
 *  @lib esmrfieldbuildercommon.lib
 */
NONSHARABLE_CLASS( CESMRGenericFieldEvent ) : public CBase,
                                              public MESMRFieldEvent
    {

public:

    /**
     * Two-phased constructor.
     * @param aType the event type.
     */
    IMPORT_C static CESMRGenericFieldEvent* NewL(
            MESMRFieldEventNotifier* aSource,
            MESMRFieldEvent::TEventType aType );
    
    /**
     * Two-phased constructor.
     * @param aType the event type.
     */
    IMPORT_C static CESMRGenericFieldEvent* NewLC(
            MESMRFieldEventNotifier* aSource,
            MESMRFieldEvent::TEventType aType );

    /**
     * Destructor.
     */
    virtual ~CESMRGenericFieldEvent();

    /**
     * Adds parameter to event.
     * Ownership is transferred.
     *
     * @param aValue parameter
     * @param aEncapsulate. If ETrue, aValue stored as CESMRFieldEventValue. 
     */
    IMPORT_C void AddParamL( CESMRFieldEventValue* aValue,
                             TBool aEncapsulate = EFalse );

// from base class MESMRFieldEvent

    /**
     * From MESMRFieldEvent.
     * Gets event type
     */
    MESMRFieldEvent::TEventType Type() const;
    
    /**
     * From MESMRFieldEvent.
     * Gets the event source.
     * 
     * @return pointer to the sender.
     */
    MESMRFieldEventNotifier* Source() const;
    /**
     * From MESMRFieldEvent.
     * Gets the event parameter count.
     * 
     * @return the parameter count.
     */
    TInt ParamCount() const;
    
    /**
     * From MESMRFieldEvent.
     * Gets the event parameter.
     * 
     * @param aIndex parameter index.
     * @return the parameter.
     */
    TAny* Param( TInt aIndex ) const;

private:

    CESMRGenericFieldEvent( MESMRFieldEventNotifier* aSource,
                            MESMRFieldEvent::TEventType aType );

private: // data

    /**
     * Event type
     */
    MESMRFieldEvent::TEventType iType;

    /**
     * Event source.
     * Not owned.
     */
    MESMRFieldEventNotifier* iSource;
    
    /**
     * Array of parameters
     */
    RPointerArray< MESMRFieldEventValue > iParams;
    
    };

NONSHARABLE_CLASS( CESMRFieldEventValue ) : public CBase,
                                            public MESMRFieldEventValue
    {
    
public:
    
    /**
     * Two-phased constructor.
     * @param aType the value type.
     * @param aValue value
     */
    IMPORT_C static CESMRFieldEventValue* NewL(
            MESMRFieldEventValue::TValueType aType,
            TAny* aValue );
    /**
     * Two-phased constructor.
     * @param aType the value type.
     * @param aValue value
     */
    IMPORT_C static CESMRFieldEventValue* NewLC(
            MESMRFieldEventValue::TValueType aType,
            TAny* aValue );
    
    /**
     * Destructor.
     */
    virtual ~CESMRFieldEventValue();
    
    /**
     * Gets the value type
     */
    MESMRFieldEventValue::TValueType Type() const;
    
    /**
     * Gets the value.
     */
    TAny* Value() const;
    
    /**
     * Gets the value as TInt.
     */
    TInt IntValue() const;
    
    /**
     * Gets the value as TDesC16&.
     */
    const TDesC& StringValue() const;
    
    /**
     * Gets the value as TDesC8&.
     */
    const TDesC8& String8Value() const;
    
    /**
     * Gets the value as CESMRField&.
     */
    const CESMRField& FieldValue() const;
    
    /**
     * Gets the value as CBase*.
     */
    const CBase* CBaseValue() const;
    
    /**
     * Gets the value as TTime.
     */
    TTime TimeValue() const;
    
private:
    
    CESMRFieldEventValue( MESMRFieldEventValue::TValueType aType);
    
    void ConstructL( TAny* aValue );
    
private: // data
    
    MESMRFieldEventValue::TValueType iType;
    
    TAny* iValue;
    };

#endif // CESMRGENERICFIELDEVENT_H
