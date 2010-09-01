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
* Description:  Freestyle action menu component
 *
*/


#ifndef C_FSACTIONMENU_H
#define C_FSACTIONMENU_H


#include <e32base.h>
#include <eikcmobs.h>

#include "fsactionmenudefines.h"
#include "fstreelist.h"
#include "fstreelistobserver.h"


//////TOOLKIT CLASSES
class CAlfEnv;
class CAlfControlGroup;
class CAlfDeckLayout;
class CAlfControl;
class CAlfDisplay;

//////PROJECT CLASSES
class CFsTreeVisualizerBase;
class CFsActionMenuControl;
class CFsLayoutManager;

/////SYSTEM CLASSES
class CEikButtonGroupContainer;

/**
 * CFsActionMenu is a component for creating and displaying an action menu.
 *
 * @code
 *
 * @endcode
 *
 * @lib 
 */
class CFsActionMenu : public CFsTreeList,
                      public MEikCommandObserver,
                      public MFsTreeListObserver
    {

public: // Construction
    
    /**
     * Two-phased constructor.
     * 
     * @param aDisplay Display.
     * @param aCbaResource Softkey pane to display while pop-up is active.
     *
     */
    IMPORT_C static CFsActionMenu* NewL( CAlfDisplay& aDisplay, TInt aCbaResource );
    
    /**
     * C++ destructor
     */
    IMPORT_C virtual ~CFsActionMenu();
    
public: //API    
    
    /**
     * Exetutes action menu at predefined vertical position.
     * The method returns with TFsActionMenuResult result code.
     *                        
     * @param aPos Action Menu's vertical position.
     */
    IMPORT_C virtual TFsActionMenuResult ExecuteL( TFsVActionMenuPosition aPos );

    
    /**
     * Exetutes action menu at a custom vertical position.
     * The method returns with TFsActionMenuResult result code.
     * 
     * @param aYPos Action Menu's top edge custom vertical position.
     */    
    IMPORT_C TFsActionMenuResult ExecuteL( const TInt aYPos );
    
    /**
     * The function returns size of Action Menu with the curent number of item.
     * This can be useful to calculate AM's custom vertical position.
     * 
     * @return Size of action menu.
     */
    IMPORT_C TSize ActionMenuSize ( );
    
public: //From CFsTreeList

    /**
     * Sets visiblity of the border
     */
    IMPORT_C void SetBorderL( const TBool aVisible, CAlfTexture* aBorderTexture = 0 );
    
    /**
     * The function returns a button group container.
     * 
     * @return A button group container.
     */
    IMPORT_C CEikButtonGroupContainer* ButtonGroupContainer();     
    
protected: //From MEikCommandObserver

    /**
     * From MEikCommandObserver
     * Processes user commands.
     */
    virtual void ProcessCommandL(TInt aCommandId);
    
    
protected: //From MFsTreeListObserver

    /**
     * From MFsTreeListObserver
     * Observer of the tree list is notified of changes in the tree list
     * through this function.
     */
    // <cmail> Touch
    virtual void TreeListEventL( const TFsTreeListEvent aEvent, 
                                 const TFsTreeItemId aId,
                                 const TPoint& aPoint );
    // </cmail>
    
public: //used by Action Menu's control

    void RootLayoutUpdatedL();
    
protected:

    /**
     * Shows action menu's cba.
     */
    void ShowCbaButtonsL();
    
    /**
     * Hides action menu's cba.
     */
    void HideCbaButtonsL();
        
    /**
     * Executes action menu.
     * 
     */
    void DoExecuteL();
    
    /**
     * Dismiss action menu.
     */
    void DoDismissL();
    
    /**
     * 
     */
    void StartWait();
    
    /**
     * 
     */
    void StopWait();
    
    /**
     * The function sets size and position of Action Menu. 
     */
    void UpdateActionMenuWindowL( );
    
    /**
     * The function retrieves Action Menu's size from Layout Manager. The size
     * depends on a number of items in the list.
     */
    virtual void SetAMSizeFromLayoutMgrL( );

    /**
     * The function calculates AM's position based on it's size and predefined vertical
     * position (top,center,bottom) or custom vertical position set by the user.
     *      
     */
    virtual void CalculatePosition( );
    
    /**
     * The function returns whether the slide in effect is set.
     */
    virtual TBool IsSlideIn();
    
    /**
     * The function returns whether the fade in effect is set.
     */
    virtual TBool IsFadeIn();
    
    /**
     * The function returns whether the slide out effect is set.
     */
    virtual TBool IsSlideOut();
    
    /**
     * The function returns whether the fade out effect is set.
     */
    virtual TBool IsFadeOut();
    
    /**
     * The function sets first list item as focused.
     */
    virtual void SetFirstItemAsFocusedL();
    
    /**
     * The function fades screen behind popup.
     * @param aFaded ETrue to fade behind Action Menu, EFalse to un-fade. 
     */
    virtual void FadeBehindActionMenuL( TBool aFaded );
    
protected: //Construction

    /**
     * C++ constructor
     */
    IMPORT_C CFsActionMenu( CAlfDisplay& aDisplay );

    /**
     * Second phase constructor
     *
     * @param aCbaResource Softkey pane to display while pop-up is active.     
     */
    IMPORT_C void ConstructL( TInt aCbaResource );

private:
    
    /**
     * A display in use.
     * Not own.
     */
    CAlfDisplay* iDisplay;
    
    /**
     * A control group for AM.
     * Not own.
     */
    CAlfControlGroup* iCtrlGroup;
    
    /**
     * A layout used for fading AM's background.
     * Own.
     */
    CAlfDeckLayout* iFadeLayout;
    
    /**
     * A root layout for AM.
     * Not own.
     */
    CAlfDeckLayout* iRootLayout;
    
    /**
     * A visualizer for tree list.
     * Not own. Ownership passed to a baseclasses.
     */
    CFsTreeVisualizerBase* iVisualizer;
    
    /**
     * A parent control for AM's control
     * Not own.
     */
    CFsActionMenuControl* iControl;
    
    /**
     * Action Menu's position.
     */
    TFsVActionMenuPosition iVPos;
    
    /**
     * AM's size.
     */
    TAlfRealSize iSize;
        
    /**
     * AM's position. 
     */
    TAlfRealPoint iPos;
    
    /**
     * Action Menu's custom vertical position.
     */
    TInt iCustVPos;
    
    /**
     * Custom position is used or not.
     */
    TBool iIsCustVPos;
    
    /**
     * Button group container for menu's softkeys
     *  Owned
     */
    CEikButtonGroupContainer* iButtonGroupContainer;
    
    /**
     * Wait loop for ExecuteL
     *  Owned
     */
    CActiveSchedulerWait* iWait;
    
    /**
     * Action Menu's result code.
     */
    TFsActionMenuResult iResult;
    
    /**
     * Temporary, before layout mgr taken into use
     */
    const TInt KListBorderPadding;
    
    /**
     * Temporary, before layout mgr taken into use
     */
    TInt iAmXPadd;
    
    /**
     * Temporary, before layout mgr taken into use
     */
    TInt iAmYPadd;
    
    };

#endif //C_FSACTIONMENU_H
