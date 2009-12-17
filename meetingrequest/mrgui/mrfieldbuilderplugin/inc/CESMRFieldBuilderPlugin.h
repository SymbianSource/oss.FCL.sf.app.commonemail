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
* Description:  MRUI field builder plugin definition
*
*/


#ifndef CESMRFIELDBUILDERPLUGIN_H
#define CESMRFIELDBUILDERPLUGIN_H

#include <e32base.h>
#include "cesmrfieldbuilderinterface.h"

/**
 *  This class implements most of the field builder composite.
 *  Everything but attendee field is built here
 */
class CESMRFieldBuilderPlugin : public CESMRFieldBuilderInterface,
                                public MESMRFieldBuilder
    {
public: // construction & destruction
    /**
     * Creates new CESMRFieldBuilderPlugin object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRFieldBuilderPlugin* NewL();

    /**
     * C++ Destructor.
     */
    virtual ~CESMRFieldBuilderPlugin();

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

private: // Implementation
    CESMRFieldBuilderPlugin();
    
    /**
     * loads fieldbuilder ecom plugin composite
     * delayed until the first time the extension is needed
     */
    void LoadExtensionL();

    MESMRFieldBuilder* FieldBuilderExtensionL();

private:
    //ecom field builder interface
    CESMRFieldBuilderInterface* iExtension;
    //composite class of field builder interface
    MESMRFieldBuilder* iFieldExtension;
    };

#endif // CESMRFIELDBUILDERPLUGIN_H

// EOF

