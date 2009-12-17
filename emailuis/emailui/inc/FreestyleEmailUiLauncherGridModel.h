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
* Description:  FreestyleEmailUi grid menu model
*
*/


#ifndef __FREESTYLEEMAILUI_LAUNCHERGRID_MODEL_H__
#define __FREESTYLEEMAILUI_LAUNCHERGRID_MODEL_H__

#include <e32base.h>
//<cmail>
#include <alf/alftexture.h>
//</cmail>
#include "FSEmailLauncherItem.h"
#include "CFSMailCommon.h"


enum TItemType
    {
    EShortcut,
    EFolder
    };


struct TMenuItem
    {
    TInt iId;   
    TInt iParent;  
    TItemType iType; 
    HBufC* iCaption;  
    CAlfTexture* iIconTexture;
    CFSEmailLauncherItem* iLauncherItem;
    TFSMailMsgId iMailBoxId;
    TFSMailMsgId iMailBoxInboxId;
    };


class CFSEmailUiLauncherGridModel : public CBase
    {
public:
    CFSEmailUiLauncherGridModel();   
    void ConstructL();

    virtual ~CFSEmailUiLauncherGridModel();

    void AddL( 
        TItemType aType,
        TInt aId,
        const TDesC& aCaption,
        CAlfTexture& aIconTexture,
        TInt aParentId = 0, 
        CFSEmailLauncherItem* aLauncherItem = NULL ); // Doesn't take ownership!
    
    void AddL( 
        TItemType aType,
        TInt aId,
        const TDesC& aCaption,
        CAlfTexture& aIconTexture,
        TFSMailMsgId aMailBoxId,
        TFSMailMsgId aMailBoxInboxId );
    
    TInt Parent(TInt aId) const;
    
    TInt FindChildren(TInt aParentId, RArray<TMenuItem>& aItems);

    TBool HasChildren(TInt aParentId) const;
    
    TMenuItem* Item(TInt aId); // Ownership not transferred!


private:

    RArray<TMenuItem> iItems;    

    };


#endif // __FREESTYLEEMAILUI_LAUNCHERGRID_MODEL_H__
