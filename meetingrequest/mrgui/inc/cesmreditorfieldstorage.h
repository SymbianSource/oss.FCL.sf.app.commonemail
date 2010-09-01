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
* Description:  ESMR Field storage for editor dialog
 *
*/

#ifndef CESMREDITORFIELDSTORAGE_H
#define CESMREDITORFIELDSTORAGE_H

#include "cesmrfieldstorage.h"
//<cmail>
#include "esmrdef.h"
//</cmail>

class CESMRPolicy;
class MESMRFieldValidator;
class MESMRCalEntry;
class TESMREntryField;

NONSHARABLE_CLASS( CESMREditorFieldStorage ):
        public CESMRFieldStorage
    {

public:
    /**
     * Two phase constructor.
     *
     * @param aPolicy policy determines visible fields
     * @param aCommandObserver Reference to command observer
     */
    static CESMREditorFieldStorage* NewL(
            const CESMRPolicy& aPolicy,
            MESMRFieldEventObserver& aEventObserver );

    /**
     * Destructor.
     */
    ~CESMREditorFieldStorage();

public: // from CESMRFieldStorage
    void ExternalizeL( MESMRCalEntry& aEntry );
    void InternalizeL( MESMRCalEntry& aEntry );

    /**
     * If validating fails (return value is != KErrNone), the given
     * TESMREntryFieldId contains the prefrred focus item.
     *
     * @return KErrNone if validation succeeds, error code otherwise
     */
    TInt Validate( TESMREntryFieldId& aUpdatedFocus, TBool aForceValidate );
    
    /**
     * Adjusts this storage for new policy. Creates fields according to new
     * policy.
     * 
     * @param aNewPolicy the new policy
     * @param aEntry the entry to edit
     */
    void ChangePolicyL(
            const CESMRPolicy& aNewPolicy,
            MESMRCalEntry& aEntry );
    
private: // Implementation
    CESMREditorFieldStorage(
            MESMRFieldEventObserver& aEventObserver );
    void ConstructL( const CESMRPolicy& aPolicy );

private:
    /**
     * Own. Validator object.
     */
    MESMRFieldValidator* iValidator;
    };

#endif  // CESMREDITORFIELDSTORAGE_H

