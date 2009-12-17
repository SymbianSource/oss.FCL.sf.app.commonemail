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
* Description:  CFsTreeControl is a HUI control that handles UI events for
 *                tree list.
 *
*/


#ifndef C_FSTREECONTROL_H
#define C_FSTREECONTROL_H

//////SYSTEM INCLUDES
#include <e32base.h>

//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

//////TOOLKIT INCLUDES
//<cmail> SF
#include <alf/alfcontrol.h>
//</cmail>

//////TOOLKIT CLASSES
class TAlfEvent;
class CAlfVisual;

//////PROJECT CLASSES
class CFsTree;
class MFsTreeVisualizer;

/**
 * CFsTreeControl is a HUI control that handles UI events for the tree list.
 * 
 * The control is a lightweight class mostly mediating events between other 
 * classes. When receiving an event, the Control checks the event type and 
 * does one of the following actions: 
 * - Ignores the event
 * - Forwards the event to the MFsTreeVisualizer interface
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsTreeControl ): public CAlfControl
    {
public: // Construction

    /**
     * Two-phased constructor
     * 
     * @param aEnv Reference to the UI toolkit environment
     * @param aTree Tree data model.
     * @param aVisualizer Tree's visualizer.
     */
    IMPORT_C static CFsTreeControl* NewL( CAlfEnv& aEnv,
            CFsTree& aTree,
            MFsTreeVisualizer& aVisualizer );

    /**
     * C++ destructor
     */
    virtual ~CFsTreeControl();

    /**
     * From CHuiControl
     * Called when an input event is being offered to the control.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  <code>ETrue</code>, if the event was handled.
     *          Otherwise <code>EFalse</code>.
     */
    IMPORT_C TBool OfferEventL(const TAlfEvent& aEvent);

    /**
    * Sets focus state of list component
    * @param aFocused status of the focus to be set
    */
    
    void SetFocused( const TBool aFocused );
    /**
    * Returns state of the focus
    * @return State of the focus
    */
    
    TBool IsFocused() const;

    /**
     */
    void VisualPrepareDrawFailed(CAlfVisual& aVisual, TInt aErrorCode);
    
protected:
    /**
     * From CAlfControl
     */
    void NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& aDisplay );
    
private: //Construction

    /**
     * Default C++ constructor.
     * 
     * @param aEnv Reference to the UI toolkit environment
     * @param aTree Tree data model.
     * @param aVisualizer Tree's visualizer.
     */
    CFsTreeControl( //CAlfEnv& aEnv,
            CFsTree& aTree,
            MFsTreeVisualizer& aVisualizer );

    /**
     * Second phase constructor.
     *
     */
    void ConstructL( CAlfEnv& aEnv );
    
    void SetDragEvents(TBool aEnable);

private: // data

    /**
     * Tree data model.
     */
    CFsTree& iTree;

    /**
     * Tree visualizer.
     */
    MFsTreeVisualizer& iTreeVisualizer;

    /*
    * Determines if list is focused or not
    */
    TBool iFocused;
        
// <cmail> Touch
    /*
    * Control getting extra touch events 
    */
    TBool iGetExtraPointerEvents;
// </cmail>
    };

#endif  // C_FSTREECONTROL_H

