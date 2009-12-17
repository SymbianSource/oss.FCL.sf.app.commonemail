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
* Description:  ESMR field storage definition
*
*/

#ifndef CESMRFIELDSTORAGE_H
#define CESMRFIELDSTORAGE_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "mesmrfieldstorage.h"
#include "cesmrfieldbuilderinterface.h"

class CESMRField;
class MESMRCalEntry;
class MESMRFieldEventObserver;
class CESMRFieldEventQueue;

/**
 * CESMRFieldStorage is a base class for different storage classes.
 * Storage class owns the fields needed to draw UI. This class also has methods
 * to add new fields into field array.
 *
 * fields for the ui are created are created in a separate ecom plugin 
 * loaded here
 */
NONSHARABLE_CLASS( CESMRFieldStorage ): public CBase, 
										public MESMRFieldStorage
    {
public:
    // Destructor
    virtual ~CESMRFieldStorage();

public:
    /**
     * Adds new field into field array.
     *
     * @param aField - field to be added
     */
    void AddFieldL( CESMRField* aField );

    /**
     * Adds new field into field array.
     *
     * @param aField - field to be added
     * @param aVisible - is this field visible by default ot not.
     */
    void AddFieldL( CESMRField* aField, TBool aVisible );

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

public: // From MESMRFieldStorage
    TInt Count() const;
    CESMRField* Field( TInt aInd ) const;
    CESMRField* FieldById( TESMREntryFieldId aId ) const;
    virtual void InternalizeL( MESMRCalEntry& aEntry );
    virtual void ExternalizeL( MESMRCalEntry& aEntry );
    virtual TInt Validate( TESMREntryFieldId& aId, TBool aForceValidate );

protected:
    // C++ constructor
    CESMRFieldStorage(
            MESMRFieldEventObserver& aEventObserver );

    void BaseConstructL();
    
    CESMRFieldEventQueue& EventQueueL();
    
private:
    /**
     * loads fieldbuilder ecom plugin
     * delayed until the first time the plugin is needed
     */
    void LoadPluginL();

    MESMRFieldBuilder* FieldBuilderL();    

private:
    /// Own: Array that owns all the fields
    RPointerArray< CESMRField > iArray;
    /// Own: ecom field builder interface
    CESMRFieldBuilderInterface* iPlugin;
    /// Ref: composite class of field builder interface
    MESMRFieldBuilder* iFieldBuilder;
    /// Ref: Event observer
    MESMRFieldEventObserver& iEventObserver;
    /// Own: Event Queue
    CESMRFieldEventQueue* iEventQueue;
    };

#endif // CESMRFIELDSTORAGE_H

// EOF

