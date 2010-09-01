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
* Description:  Definition for GUI extension interface
*
*/


#ifndef CESMRFIELDBUILDERINTERFACE_H
#define CESMRFIELDBUILDERINTERFACE_H

#include <ecom/ecom.h>

#include "esmrinternaluid.h"
#include "mesmrfieldvalidator.h"
#include "tesmrentryfield.h"
#include "mesmrresponseobserver.h"

class CESMRField;

/**
 *  MESMRFieldBuilder is the interface of the field builder composite.
 *  All field builder implementations will return only this
 */
class MESMRFieldBuilder
    {
public:
    /**
     * Creates editor field.
     *
     * @param aValidator - time validator
     * @param aField - field to be created
     */
    virtual CESMRField* CreateEditorFieldL(
            MESMRFieldValidator* aValidator,
            TESMREntryField aField ) = 0;

    /**
     * Creates viewer field.
     *
     * @param aResponseObserver - observer to be called when e.g.
     *                            answer is ready to be sent.
     * @param aField - field to be created
     * @param aResponseReady - NOT IN USE
     */
    virtual CESMRField* CreateViewerFieldL(
            MESMRResponseObserver* aResponseObserver,
            TESMREntryField aField,
            TBool aResponseReady ) = 0;
    };

class MESMRBuilderExtension
    {
public: // Destruction
    ~MESMRBuilderExtension() {}

public:
    /**
     * Put additional flags/checks here if needed
     */
    enum TMRCFSMailBoxCapability
        {
        EMRCFSAttendeeStatus = 0,
        EMRCFSResponseDontSend,
        EMRCFSRemoveFromCalendar,
        EMRCFSSupportsAttachmentsInMR
        };

    /**
     * Check if a cfsmailboxflag is set
     */
    virtual TBool CFSMailBoxCapabilityL( TMRCFSMailBoxCapability aCapa ) = 0;

    /**
     * Check if a cfsmailboxflag is set
     */
    virtual TBool CFSMailBoxCapabilityL(
            const TDesC& aEmailAddress,
            TMRCFSMailBoxCapability aCapa ) = 0;
    
    /**
     * Checks, whenter meeting request can be originated or not.
     * @return ETrue, if MR can be originated, EFalse otherwise.
     */
    virtual TBool MRCanBeOriginateedL() = 0;
    };

/**
 *  MESMRFieldBuilder is the interface of the field builder composite.
 *  All field builder implementations will return only this
 */
class CESMRFieldBuilderInterface : public CBase
    {
public:
    /**
     * Create specific implementation
     */
    inline static CESMRFieldBuilderInterface* CreatePluginL(
            TUid aImplementationUid );

    /**
     * Destructor
     */
    inline virtual ~CESMRFieldBuilderInterface();

    /**
     * return field builder to caller, implemented in all plugins
     */
    virtual MESMRFieldBuilder* FieldBuilderL() = 0;

    /**
     * extends the ecom interface
     */
    virtual TAny* ExtensionL( TUid aExtensionUid ) = 0;

private:
    // The ECom destructor key identifier
    TUid iDestructorIdKey;
    };

// -----------------------------------------------------------------------------
// CESMRFieldBuilderInterface::CreatePluginL
// -----------------------------------------------------------------------------
//
inline CESMRFieldBuilderInterface* CESMRFieldBuilderInterface::CreatePluginL(
        TUid aImplementationUid )
    {
    TAny* implementation =
        REComSession::CreateImplementationL( aImplementationUid,
                                             _FOFF( CESMRFieldBuilderInterface,
                                                    iDestructorIdKey ) );
    return reinterpret_cast<CESMRFieldBuilderInterface*>(implementation);
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderInterface::~CESMRFieldBuilderInterface
// -----------------------------------------------------------------------------
//
inline CESMRFieldBuilderInterface::~CESMRFieldBuilderInterface()
    {
    REComSession::DestroyedImplementation( iDestructorIdKey );
    }


#endif // CESMRFIELDBUILDERINTERFACE_H
