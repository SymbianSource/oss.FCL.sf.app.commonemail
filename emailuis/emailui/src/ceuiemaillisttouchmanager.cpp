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
 * Description:  Take care of touch event handling and component states.
 *
 */

#include <alf/alfenv.h>

#include "emailtrace.h"
#include "ceuiemaillisttouchmanager.h"
#include "FreestyleEmailUiMailListVisualiser.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CEUiEmailListTouchManager* CEUiEmailListTouchManager::NewL(
        CFSEmailUiMailListVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CEUiEmailListTouchManager* self = new (ELeave) CEUiEmailListTouchManager(
            aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;

    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CEUiEmailListTouchManager::~CEUiEmailListTouchManager()
    {
    }

// ---------------------------------------------------------------------------
// CEUiEmailListTouchManager::SetDisabled
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::SetDisabled(TBool aState)
    {
    iDisableActions = aState;
    }

// ---------------------------------------------------------------------------
// From class MFsControlBarObserver.
// CEUiEmailListTouchManager::HandleControlBarEvent
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::HandleControlBarEvent(
        TFsControlBarEvent aEvent, TInt aData)
    {
    FUNC_LOG;

    //If folderlist is opened. Ignore all events. 
    if (iDisableActions)
        return;

    switch (aEvent)
        {

        case MFsControlBarObserver::EEventTouchFocused:
            //change focus
        TRAP_IGNORE(iVisualiser.SetControlBarFocusedL())
            ;
            break;
        case MFsControlBarObserver::EEventButtonTouched:
            //action
        TRAP_IGNORE(DoHandleActionL( aData ))
            ;
            break;

        default:
            //Just ignore rest of events
            break;
        }

    }

// ---------------------------------------------------------------------------
// From class MFsTreeListObserver.
// CEUiEmailListTouchManager::TreeListEventL
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::TreeListEventL(const TFsTreeListEvent aEvent,
        const TFsTreeItemId /*aId*/)
    {
    FUNC_LOG;

    //If folderlist is opened. Ignore all events. 
    if (iDisableActions)
        return;

    switch (aEvent)
        {
        case MFsTreeListObserver::EFsTreeListItemTouchAction:
            DoHandleActionL();
            break;
        case MFsTreeListObserver::EFsTreeListItemTouchLongTap:
            DoHandleLongTapL();
            break;
        case MFsTreeListObserver::EFsTreeListItemTouchFocused:
            DoHandleTreelistFocusChangeL();
            break;
        case MFsTreeListObserver::EFsTreeListItemWillGetFocused:
            {
            iVisualiser.SetMskL();
            }
            break;

        default:
            //Just ignore rest of events
            break;
        }
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CEUiEmailListTouchManager::CEUiEmailListTouchManager(
        CFSEmailUiMailListVisualiser& aVisualiser) :
    iVisualiser(aVisualiser), iDisableActions(EFalse)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CEUiEmailListTouchManager::DoHandleLongTapL
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::DoHandleLongTapL()
    {
    FUNC_LOG;

    //Supported only for list component
    switch (iVisualiser.GetFocusedControl())
        {
        case EMailListComponent:
            iVisualiser.DoHandleListItemLongTapL();
            break;
        default:
            //Ignore rest
            break;
        }

    }
// ---------------------------------------------------------------------------
// CEUiEmailListTouchManager::DoHandleActionL
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::DoHandleActionL(TInt aData)
    {
    FUNC_LOG;

    switch (iVisualiser.GetFocusedControl())
        {
        case EMailListComponent:
            iVisualiser.DoHandleListItemOpenL();
            break;
        case EControlBarComponent:
            iVisualiser.DoHandleControlBarOpenL(aData);
            break;
        default:
            //Ignore rest
            break;
        }
    }
// ---------------------------------------------------------------------------
// CEUiEmailListTouchManager::DoHandleTreelistFocusChangeL
// ---------------------------------------------------------------------------
//
void CEUiEmailListTouchManager::DoHandleTreelistFocusChangeL()
    {
    FUNC_LOG;

    if (iVisualiser.GetFocusedControl() != EMailListComponent)
        iVisualiser.SetTreeListFocusedL();
    }

