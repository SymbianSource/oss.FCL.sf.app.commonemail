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
* Description:  FreestyleEmailUi attachments list visualiser definition 
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_SENDATTACHMENTSLISTVISUALISER_H__
#define __FREESTYLEEMAILUI_SENDATTACHMENTSLISTVISUALISER_H__

// SYSTEM INCLUDES
#include <e32base.h>
//<cmail>
#include "fscontrolbarobserver.h"
#include "fstreelistconstants.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiScrollbarClet.h"
#include "FreestyleEmailUiListVisualiser.h"
#include "FreestyleEmailUiAppui.h"

// FORWARD DECLARATIONS
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CFreestyleEmailUiTextureManager;
class CFsTreeList;
class CFsTreeVisualizerBase;
class MFsTreeItemData;
class MFsTreeItemVisualizer;
class CFreestyleEmailUiTextureManager;
class CFreestyleEmailUiAppUi;
class CFsTreePlainOneLineNodeVisualizer;
class CFreestyleEmailUiSendAttachmentsListControl;
class TVwsViewId;


// CLASS DECLARATION
class CFSEmailUiSendAttachmentsListVisualiser : public CFsEmailUiViewBase
	{
public:
    static CFSEmailUiSendAttachmentsListVisualiser* NewL(
        CAlfEnv& aEnv, 
        CFreestyleEmailUiAppUi* aAppUi, 
        CAlfControlGroup& aMailListControlGroup );
		
    static CFSEmailUiSendAttachmentsListVisualiser* NewLC(
        CAlfEnv& aEnv, 
        CFreestyleEmailUiAppUi* aAppUi, 
        CAlfControlGroup& aMailListControlGroup );
		
    virtual ~CFSEmailUiSendAttachmentsListVisualiser();
	
public:
    // From aknview  
    // <cmail> Toolbar
    /*void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );*/
    // </cmail> Toolbar

    TUid Id() const;			   
    void ChildDoDeactivate();                
    void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
		
    void DynInitMenuPaneL(
        TInt aResourceId, 
        CEikMenuPane* aMenuPane );

    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;
    
public:
    // Refresh
    void RefreshL();
	
    // Sets softkeys
    void SetViewSoftkeysL( TInt aResourceId );
    void SetMskL();
    // Highlighted index
    void ReScaleUiL();
	
    // Event handling forwarded from control
    TBool OfferEventL( const TAlfEvent& aEvent ); 
    void HandleCommandL( TInt aCommand );
		
    CAlfControl* ViewerControl();
		
    TEditorLaunchParams EditorParams();

// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
   
// </cmail> Toolbar    
    
private:
    CFSEmailUiSendAttachmentsListVisualiser( CFreestyleEmailUiAppUi* aAppUi, CAlfEnv& aEnv, CAlfControlGroup& aAttachmentsListControlGroup );
    void ConstructL();
	
private:
    /**
    * Pointer to Alf environment
    * not owned
    */
    CAlfEnv* iEnv;

    /**
    * ALF screen control rect
    * not owned (control passed to control group)
    */
    CFreestyleEmailUiSendAttachmentsListControl* iScreenControl;
	    
    /**
    * Stores compose view params so that edited
    * message can be opened from drafts when
    * returning from this view
    */
    TEditorLaunchParams iEditorParams;
	    
    /**
    * 
    * 
    */
    TVwsViewId iPrevViewId;
  	};

#endif // __FREESTYLEEMAILUI_SENDATTACHMENTSLISTVISUALISER_H__
