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
* Description:  MRUI field build plugin extension implementation
 *
*/


#ifndef __CESMRFIELDBUILDERPLUGINEXTENSION__
#define __CESMRFIELDBUILDERPLUGINEXTENSION__

#include <e32base.h>
#include "cesmrfieldbuilderinterface.h"
#include "resmrpluginextensionstatic.h"

/**
 *  This class implements most of the field builder composite.
 *  All Freestyle email related MRUI fields are built here
 */
class CESMRFieldBuilderPluginExtension : public CESMRFieldBuilderInterface,
                                         public MESMRFieldBuilder,
                                         public MESMRBuilderExtension
    {
public: // construction & destruction
    /**
     * Creates new CESMRFieldBuilderPluginExtension object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRFieldBuilderPluginExtension* NewL();
    
    /**
     * C++ Destructor.
     */
    virtual ~CESMRFieldBuilderPluginExtension();

protected: // from CESMRFieldBuilderInterface
    virtual MESMRFieldBuilder* FieldBuilderL();
    virtual TAny* ExtensionL( TUid aExtensionUid );

protected: // from MESMRFieldBuilder
    /**
     * Creates editor field.
     *
     * @param aValidator - time validator
     * @param aField - field to be created
     */
    CESMRField* CreateEditorFieldL(
            MESMRFieldValidator* aValidator,
            TESMREntryField aField );

    /**
     * Creates viewer field.
     *
     * @param aResponseObserver - observer to be called when e.g.
     *                            answer is ready to be sent.
     * @param aField - field to be created
     * @param aResponseReady - NOT IN USE
     */
    CESMRField* CreateViewerFieldL(
            MESMRResponseObserver* aResponseObserver,
            TESMREntryField aField,
            TBool aResponseReady );

protected: //from  MESMRBuilderExtension
    TBool CFSMailBoxCapabilityL( EMRCFSMailBoxCapability aCapa );
    TBool MRCanBeOriginateedL( TBool aForceResetDefaultMRMailbox );

private: // Implementation
    CESMRFieldBuilderPluginExtension();
    void ConstructL();

    /**
     * Access CFSmailbox TLS static
     */
    RESMRPluginExtensionStatic iESMRStatic;

    /**
     * iESMRstatic accessed
     */
    TBool iESMRStaticAccessed;
    };

#endif // CESMRFieldBuilderPluginExtension
