/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  FreestyleEmailUi application's grid menu model
*
*/


// INTERNAL INCLUDE FILES
#include "emailtrace.h"
#include "FreestyleEmailUiLauncherGridModel.h"


CFSEmailUiLauncherGridModel::CFSEmailUiLauncherGridModel()
    {
    FUNC_LOG;
    }
    
void CFSEmailUiLauncherGridModel::ConstructL()
    {
    FUNC_LOG;
    }
    
CFSEmailUiLauncherGridModel::~CFSEmailUiLauncherGridModel()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iItems.Count(); ++i )
        {
        // NOTE: No need to delete iItems[i].iLauncherItem
        // as we don't have the ownership!
        delete iItems[i].iCaption;
        iItems[i].iCaption = NULL;
        }
    iItems.Close();
    }

void CFSEmailUiLauncherGridModel::AddL(
        TItemType aType,
        TInt aId,
        const TDesC& aCaption, 
        CAlfTexture& aIconTexture,
        TInt aParentId,
        CFSEmailLauncherItem* aLauncherItem )
    {
    FUNC_LOG;
    TMenuItem item;
    
    item.iType = aType;
    item.iId = aId;
    item.iParent = aParentId;
    item.iCaption = aCaption.AllocL();
    item.iIconTexture = &aIconTexture;
    item.iLauncherItem = aLauncherItem;
    
    User::LeaveIfError( iItems.Append(item) );
    }

void CFSEmailUiLauncherGridModel::AddL( 
        TItemType aType,
        TInt aId,
        const TDesC& aCaption,
        CAlfTexture& aIconTexture,
        TFSMailMsgId aMailBoxId,
        TFSMailMsgId aMailBoxInboxId )
    {
    FUNC_LOG;
    TMenuItem item;
    
    item.iType = aType;
    item.iId = aId;
    item.iParent = 0;
    item.iCaption = aCaption.AllocL();
    item.iIconTexture = &aIconTexture;
    item.iLauncherItem = NULL;
    item.iMailBoxId = aMailBoxId;
    item.iMailBoxInboxId = aMailBoxInboxId;
    
    User::LeaveIfError( iItems.Append(item) );
    }

TInt CFSEmailUiLauncherGridModel::FindChildren(
        TInt aParentId,
        RArray<TMenuItem>& aItems )
    {
    FUNC_LOG;
    TInt error = KErrNone;
    
    for ( TInt i = 0; i < iItems.Count(); i++ )
        {
        if ( iItems[i].iParent == aParentId )
            {
            error = aItems.Append( iItems[i] );
            if ( error != KErrNone )
                {
                break;
                }
            }
        }

    return error;
    }

TInt CFSEmailUiLauncherGridModel::Parent(TInt aId) const
    {
    FUNC_LOG;
    for(TInt i = 0; i < iItems.Count(); ++i)
        {
        if(iItems[i].iId == aId)
            {
            return iItems[i].iParent;
            }
        }
    return 0;        
    }

TBool CFSEmailUiLauncherGridModel::HasChildren(TInt aParentId) const
    {
    FUNC_LOG;
    for(TInt i = 0; i < iItems.Count(); ++i)
        {
        if(iItems[i].iParent == aParentId)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

TMenuItem* CFSEmailUiLauncherGridModel::Item(TInt aId)
    {
    FUNC_LOG;
    for(TInt i = 0; i < iItems.Count(); ++i)
        {
        if(iItems[i].iId == aId)
            {
            return &iItems[i];
            }
        }
    return NULL;        
    }


