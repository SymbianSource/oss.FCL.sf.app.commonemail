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
* Description: This file implements class CIpsSetUiFolderListArray.
*
*/



#include "emailtrace.h"
#include <miuthdr.h>
#include <msvapi.h>
#include <msvuids.h>

#include "ipssetuifoldersubscription.h"
#include "ipssetutilsconsts.h"

#include "ipssetuifolderlistarray.h"

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::NewL
// ----------------------------------------------------------------------------
//
CIpsSetUiFolderListArray* CIpsSetUiFolderListArray::NewL(
    CMsvSession& aMsvSession,
    TMsvId aMailboxId )
    {
    FUNC_LOG;
    CIpsSetUiFolderListArray* self=new (ELeave) CIpsSetUiFolderListArray(
        aMsvSession, aMailboxId );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::~CIpsSetUiFolderListArray()
// ----------------------------------------------------------------------------
//
CIpsSetUiFolderListArray::~CIpsSetUiFolderListArray()
    {
    FUNC_LOG;
    delete iText;
    delete iEntry;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsSetUiFolderListArray::ConstructL()
    {
    FUNC_LOG;
    RefreshFolderListArrayL();

    iText = HBufC::NewL( KIpsSetUiMaxSettingsTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::CIpsSetUiFolderListArray
// ----------------------------------------------------------------------------
//
CIpsSetUiFolderListArray::CIpsSetUiFolderListArray(
    CMsvSession& aMsvSession,
    TMsvId aMailboxId)
    :
    iSession( aMsvSession ),
    iMailboxId( aMailboxId )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC CIpsSetUiFolderListArray::MdcaPoint( TInt aIndex ) const
    {
    FUNC_LOG;
    const TMsvEmailEntry emailEntry( (*iEntry)[aIndex] );

    TPtr tempText = iText->Des();
    tempText.Zero();

    TInt iconIndex = 0;
    if ( ContextHasChildFolders( emailEntry.Id() ) )
        {
        iconIndex = ( emailEntry.LocalSubscription() ?
            EIpsSetUiFolderSubfoldersSubbed : EIpsSetUiFolderSubfoldersUnsubbed );
        }
    else
        {
        iconIndex = ( emailEntry.LocalSubscription() ?
            EIpsSetUiFolderSingleSubbed : EIpsSetUiFolderSingleUnsubbed );
        }
    tempText.AppendNum( iconIndex );
    tempText.Append( KColumnListSeparator );
    tempText.Append(
        emailEntry.iDetails.Left( KIpsSetUiMaxSettingsTextLength ) );

    return tempText;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::MdcaCount
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiFolderListArray::MdcaCount() const
    {
    FUNC_LOG;
    return iCount;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::Id
// ----------------------------------------------------------------------------
//
TMsvId CIpsSetUiFolderListArray::Id( TInt aIndex )
    {
    FUNC_LOG;
    return (*iEntry)[aIndex].Id();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ChangEIpsSetUiFolderL
// ----------------------------------------------------------------------------
//
void CIpsSetUiFolderListArray::ChangEIpsSetUiFolderL( TMsvId aFolderId )
    {
    FUNC_LOG;
    iEntry->SetEntryL( aFolderId );

    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    iCount = sel->Count();
    delete sel;
    sel = NULL;

    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ContextHasChildren
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::ContextHasChildren( TMsvId aId ) const
    {
    FUNC_LOG;
    return ContextHasChildFolders( aId );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ContextHasChildFolders
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::ContextHasChildFolders( TMsvId aId ) const
    {
    FUNC_LOG;
    TBool hasFolders = EFalse;
    TRAPD( error, hasFolders = DoContextHasChildFoldersL( aId ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    return hasFolders;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::DoContextHasChildFoldersL
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::DoContextHasChildFoldersL( TMsvId aId ) const
    {
    FUNC_LOG;
    TMsvSelectionOrdering selectionOrdering(
        KMsvGroupByType, EMsvSortByDetails, ETrue );
    CMsvEntry* entry = CMsvEntry::NewL(iSession, aId, selectionOrdering);
    CleanupStack::PushL( entry );
    TInt index=entry->Count()-1;
    while (index>=0 && (*entry)[index].iType!=KUidMsvFolderEntry)
        {
        index--;
        }

    CleanupStack::PopAndDestroy( entry );

    if(index!=-1)
        {
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::GetContextChildrenL
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CIpsSetUiFolderListArray::GetContextChildrenL(
    TMsvId aId,
    CMsvEntry& aEntry ) const
    {
    FUNC_LOG;
    TMsvSelectionOrdering selectionOrdering(
        KMsvGroupByType, EMsvSortByDetails, ETrue );
    aEntry.SetEntryL( aId );
    aEntry.SetSortTypeL( selectionOrdering );
    CMsvEntrySelection* sel = aEntry.ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( sel );

    TInt i;
    TInt cnt = sel->Count();

    for( i=0; i<cnt; i++ )
        {
        if( ContextHasChildren( sel->At(i) ) )
            {
            //Note! recursion
            CMsvEntrySelection* selection =
                GetContextChildrenL( sel->At(i), aEntry );
            CleanupStack::PushL( selection );
            TInt c = selection->Count();

            TInt j;
            for( j=0; j<c; j++ )
                {
                sel->AppendL( selection->At(j) );
                }
            CleanupStack::PopAndDestroy( selection );
            }
        }


    CleanupStack::Pop( sel );
    return sel;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::FoldersUpdated
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::FoldersUpdated() const
    {
    FUNC_LOG;
    return ( iEntry->Count() > 0 ? ETrue : EFalse );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::RefreshFolderListArrayL
// ----------------------------------------------------------------------------
//
void CIpsSetUiFolderListArray::RefreshFolderListArrayL()
    {
    FUNC_LOG;
    if( iEntry )
        {
        delete iEntry;
        iEntry = NULL;
        }

    iEntry = iSession.GetEntryL( iMailboxId );
    const TMsvSelectionOrdering originalOrdering=iEntry->SortType();
    TMsvSelectionOrdering newOrdering=originalOrdering;
    newOrdering.SetShowInvisibleEntries(ETrue);
    newOrdering.SetGroupByType(ETrue);
    newOrdering.SetSorting( EMsvSortByDetails );
    iEntry->SetSortTypeL(newOrdering);

    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    iCount = sel->Count();
    delete sel;
    sel = NULL;
    }

// End of File

