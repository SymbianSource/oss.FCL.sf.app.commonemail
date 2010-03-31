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
* Description:  Definition of the class MESMRFieldEvent
*
*/

#ifndef MESMRFIELDEVENT_H
#define MESMRFIELDEVENT_H


#include <e32std.h>

class MESMRFieldEventNotifier;
class CESMRField;

/**
 *  Interface for field events
 *
 *  @lib esmrfieldbuildercommon.lib
 */
class MESMRFieldEvent
    {

public:

    /**  Event types */
    enum TEventType
        {
        /**
         * Triggers command from field.
         * @param Command value. TInt.
         */
        EESMRFieldCommandEvent = 0,
        /**
         * Notifies that field value has changed
         * @param Field that has changed. TESMREntryFieldId as TInt.
         * @param New field value. MESMRFieldEventValue*.
         */
        EESMRFieldChangeEvent
        };

    
    /**
     * Gets the event type.
     *
     * @return Event type from TEventType enumeration.
     */
    virtual TEventType Type() const = 0;
    
    /**
     * Gets the event source.
     * 
     * @return pointer to the sender or NULL.
     */
    virtual MESMRFieldEventNotifier* Source() const = 0;
    
    /**
     * Gets the event parameter count.
     * 
     * @return the parameter count.
     */
    virtual TInt ParamCount() const = 0;
    
    /**
     * Gets the event parameter.
     * 
     * @param aIndex parameter index.
     * @return the parameter.
     */
    virtual TAny* Param( TInt aIndex ) const = 0;

    /**
     * Destructor.
     */
    virtual ~MESMRFieldEvent() {}
    };

/**
 *  Generic field event value interface.
 *  
 *  @lib esmrfieldbuildercommon.lib
 */
class MESMRFieldEventValue
    {
    
public:
    
    /**
     * Value type definition.
     */
    enum TValueType
        {
        /**
         * TInt
         */
        EESMRInteger = 0,
        /**
         * TDesC16*. Not owned.
         */
        EESMRStringRef,
        /**
         * TDesC16*. Owned.
         */
        EESMRString,
        /**
         * TDesC8*. Not owned.
         */
        EESMRString8Ref,
        /**
         * TDesC8*. Owned.
         */
        EESMRString8,
        /**
         * CESMRField*. Not owned.
         */
        EESMRFieldRef,
        /**
         * MESMRFieldEventValue*. Owned.
         */
        EESMRFieldEventValue,
        /**
         * CBase*. Not owned.
         */
        EESMRCBaseRef,
        /**
         * CBase*. Owned.
         */
        EESMRCBase,
        /**
         * TTime.
         */
        EESMRTTime
        };
    
    /**
     * Destructor.
     */
    virtual ~MESMRFieldEventValue() {}

    /**
     * Gets the value type
     */
    virtual TValueType Type() const = 0;
    
    /**
     * Gets the value.
     */
    virtual TAny* Value() const = 0;
    
    /**
     * Gets the value as TInt.
     */
    virtual TInt IntValue() const = 0;
    
    /**
     * Gets the value as TDesC16&.
     */
    virtual const TDesC& StringValue() const = 0;
    
    /**
     * Gets the value as TDesC8&.
     */
    virtual const TDesC8& String8Value() const = 0;
    
    /**
     * Gets the value as CESMRField&.
     */
    virtual const CESMRField& FieldValue() const = 0;
    
    /**
     * Gets the value as CBase*.
     */
    virtual const CBase* CBaseValue() const = 0;
    
    /**
     * Gets the value as TTime.
     */
    virtual TTime TimeValue() const = 0;

    };



#endif // MESMRFIELDEVENT_H
