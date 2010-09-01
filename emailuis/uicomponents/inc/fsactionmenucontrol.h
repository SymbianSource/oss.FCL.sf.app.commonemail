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
* Description:  A control for action menu component
 *
*/


#ifndef C_FSACTIONMENUCONTROL_H
#define C_FSACTIONMENUCONTROL_H

// <cmail> SF
#include <alf/alfcontrol.h>
// </cmail>

class CFsActionMenu;
class CAlfVisual;

/**
 * CFsActionMenuControl is a control for action menu component.
 *
 * @code
 *
 * @endcode
 *
 * @lib 
 */
class CFsActionMenuControl : public CAlfControl
    {
    
public:

    /**
     * Constructor.
     */
    CFsActionMenuControl( CFsActionMenu* aActionMenu );

    /**
     * Second-phase constructor.
     */
    void ConstructL( CAlfEnv& aEnv );

    /**
     * Destructor.
     */
    virtual ~CFsActionMenuControl();
    
public:
    
    /**
     * 
     */
    void SetRootVisual( CAlfVisual* aVisual );
    
protected:

    /**
     * From CAlfControl.
     * Notifies the owner that the layout of a visual has been recalculated.
     * Called only when the EAlfVisualFlagLayoutUpdateNotification flag has
     * been set for the visual.
     *
     * @param aVisual  Visual that has been laid out.
     */
    void VisualLayoutUpdated( CAlfVisual& aVisual );
    
protected:

    /**
     * 
     */
    CFsActionMenu* iActionMenu;
    
    /**
     * 
     */
    CAlfVisual* iVisual;
    
    };


#endif //C_FSACTIONMENUCONTROL_H
