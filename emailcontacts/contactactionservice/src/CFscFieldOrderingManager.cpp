/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of Field ordering manager
 *
*/


// INCLUDES
#include "emailtrace.h"
#include "CFscFieldOrderingManager.h"

// From VirtualPhonebook
#include <MVPbkFieldType.h>
#include <barsread.h>

// LOCAL

#ifdef _DEBUG
namespace
    {

    enum TPanic
        {
        EPanic_PostCond_ItemIndex = 1
        };

    void Panic(TPanic aPanicCode)
        {
        _LIT( KPanicCat, "CFscFieldOrderingManager" );
        User::Panic(KPanicCat, aPanicCode);
        }
    }
#endif // _DEBUG

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::TFieldTypeOrder::TFieldTypeOrder
// --------------------------------------------------------------------------
//
CFscFieldOrderingManager::TFieldTypeOrder::TFieldTypeOrder(
        TResourceReader& aReader)
    {
    FUNC_LOG;
    iFieldTypeResId = aReader.ReadInt32();
    iOrderingItem = aReader.ReadInt8();
    iAddItemOrdering = aReader.ReadInt8();
    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::CFscFieldOrderingManager
// --------------------------------------------------------------------------
//
CFscFieldOrderingManager::CFscFieldOrderingManager()
    {
    FUNC_LOG;
    // Do nothing
    }

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::~CFscFieldOrderingManager
// --------------------------------------------------------------------------
//
CFscFieldOrderingManager::~CFscFieldOrderingManager()
    {
    FUNC_LOG;
    iOrderingItems.Close();
    }

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::NewL
// --------------------------------------------------------------------------
//
CFscFieldOrderingManager* CFscFieldOrderingManager::NewL(
        TResourceReader& aReader)
    {
    FUNC_LOG;
    CFscFieldOrderingManager* self = new (ELeave) CFscFieldOrderingManager();
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::ConstructL
// --------------------------------------------------------------------------
//
void CFscFieldOrderingManager::ConstructL(TResourceReader& aReader)
    {
    FUNC_LOG;
    const TInt itemCount = aReader.ReadInt16();
    for (TInt i = 0; i < itemCount; ++i)
        {
        iOrderingItems.AppendL(TFieldTypeOrder(aReader) );
        }
    }

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::OrderingItem
// --------------------------------------------------------------------------
//
TInt CFscFieldOrderingManager::OrderingItem(const MVPbkFieldType& aFieldType)
    {
    FUNC_LOG;
    TInt fieldTypeResId = aFieldType.FieldTypeResId();
    TInt index = ItemIndex(aFieldType.FieldTypeResId() );
    return iOrderingItems[index].iOrderingItem;
    }

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::AddItemOrdering
// --------------------------------------------------------------------------
//
TInt CFscFieldOrderingManager::AddItemOrdering(
        const MVPbkFieldType& aFieldType)
    {
    FUNC_LOG;
    TInt fieldTypeResId = aFieldType.FieldTypeResId();
    TInt index = ItemIndex(aFieldType.FieldTypeResId() );
    return iOrderingItems[index].iAddItemOrdering;
    }

// --------------------------------------------------------------------------
// CFscFieldOrderingManager::ItemIndex
// --------------------------------------------------------------------------
//
TInt CFscFieldOrderingManager::ItemIndex(TInt aFieldTypeResId)
    {
    FUNC_LOG;
    const TInt count = iOrderingItems.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iOrderingItems[i].iFieldTypeResId == aFieldTypeResId)
            {
            return i;
            }
        }
    /// Must not come here. If it does then a new field type has been
    /// added to the system but not to Pbk2FieldOrdering.rss
    __ASSERT_DEBUG( EFalse, Panic( EPanic_PostCond_ItemIndex ) );
    return KErrNotFound;
    }

// End of file

