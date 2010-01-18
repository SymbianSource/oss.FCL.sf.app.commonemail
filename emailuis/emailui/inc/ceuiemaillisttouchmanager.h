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

#ifndef C_EUIEMAILLISTTOUCHMANAGER_H
#define C_EUIEMAILLISTTOUCHMANAGER_H

#include <e32base.h>

#include "fscontrolbarobserver.h"
#include "fstreelistobserver.h"

class CFSEmailUiMailListVisualiser;

/**
 *  Part of FreestyleEmailUi application.
 *  Helper class for touch management to handling different touch events. 
 *
 *  @code
 *   CEUiEmailListTouchManager* manager = CEUiEmailListTouchManager::NewL(*this); 
 *   CFsControlBar* controlBar = CFsControlBar::NewL( iEnv );
 *   controlBar->AddObserverL( *manager );
 *  @endcode
 *
 *  @lib FreestyleEmailUi.exe
 *  @since S60 v9.1
 */
class CEUiEmailListTouchManager : public CBase,
        public MFsControlBarObserver,
        public MFsTreeListObserver
    {
public:

    /**
     * Two-phased constructor.
     * 
     * @param aVisualiser Reference to the mail list visualiser.
     */
    static CEUiEmailListTouchManager* NewL(
            CFSEmailUiMailListVisualiser& aVisualiser);

    /**
     * Destructor.
     */
    ~CEUiEmailListTouchManager();

    /**
     * Disable/Activate manager
     *
     * @since S60 v9.1
     * @param TBool for state. 
     */
    void SetDisabled(TBool aState);

private:

    // from base class MFsControlBarObserver

    /**
     * From MFsControlBarObserver.
     * Invoked when event occurs.
     *
     * @param aEvent event type.
     * @param aData additional data. For EButton - id of pressed button.
     */
    void HandleControlBarEvent(TFsControlBarEvent aEvent, TInt aData);

    // from base class MFsTreeListObserver

    /**
     * From MFsTreeListObserver.
     * Observer of the tree list is notified of changes in the tree list
     * through this function.
     * 
     * @param aEvent Event which happend in the tree list.
     * @param aId Id of the node/item associated with the event. KFsTreeNoneID
     *            means that the event is not associated with a node/item.
     */
    void TreeListEventL( const TFsTreeListEvent aEvent,
                         const TFsTreeItemId aId,
                         const TPoint& aPoint );

private:

    CEUiEmailListTouchManager(CFSEmailUiMailListVisualiser& aVisualiser);

    void ConstructL();

    /**
     * Helper class for handling long tap actions. 
     *
     * @since S60 v9.1
     * @param aPoint Coordinates of the long tap event.
     */
    void DoHandleLongTapL( const TPoint& aPoint );

    /**
     * Helper class for handling actions. (Open viewer, expand/collapse etc.)
     * 
     * @since S60 v9.1
     * @param TInt used for pass which folderlist or shortlist should be opened.    
     */
    void DoHandleActionL(TInt aData = 0);

    /**
     * Helper class for changing focus.  
     *
     * @since S60 v9.1		 
     */
    void DoHandleTreelistFocusChangeL();

private:
    // data

    /**
     * Mail list visualise
     * Not own.
     */
    CFSEmailUiMailListVisualiser& iVisualiser;

    /**
     * Disable actions. Used when folderlist is launched. 
     */
    TBool iDisableActions;
    };

#endif //C_EUIEMAILLISTTOUCHMANAGER_H
