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
* Description:  Declaration of CFsTextInputVisual class
*
*/



#ifndef C_FSTEXTINPUTVISUAL_H
#define C_FSTEXTINPUTVISUAL_H

//<cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)
//#include <fsconfig.h>
//#ifdef __FS_ALFRED_SUPPORT
#include <alf/alfvisual.h>
/*#else // !__FS_ALFRED_SUPPORT
#include <huivisual.h>
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)

//Forward declarations
class CCoeControl;
class CAlfControl;
class CAlfLayout;
class CAlfGc;
class MAlfVisualOwner;

/**
 * CFsTextInputVisual is responsible for controlling the
 * TRect and redrawing of the CCoeControl. The text input
 * field inherits from CEikRichTextEditor which inherits form
 * CCoeControl (not directly).
 *
 */

NONSHARABLE_CLASS(CFsTextInputVisual) : public CAlfVisual
    {
public:
    /**
     * Creates instance of Text Input Visual and adds it to the HuiControl.
     *
     * @param aOwnerControl HuiControl which will own the visual
     * @param aCoeControl CoeConotrol of which the TRect will be managed.
     * @param aParentLayout Parent layout to which the visual will be appended.
     * @return Pointer to istance of the Text Input Visual
     */
    static CFsTextInputVisual* AddNewL( CAlfControl& aOwnerControl,
                                        CCoeControl& aCoeControl,
                                        CAlfLayout* aParentLayout = 0);
    /**
     * Virtual destructor
     */
    virtual ~CFsTextInputVisual();
    
    /**
     * From CHuiVisual. Invoked to redraw its content.
     *
     * @param aGc Graphic Context
     * @param aDisplayRect in which the control should be drawn.
     */
    virtual void DrawSelf( CAlfGc& aGc, const TRect& aDisplayRect ) const;
    
    /**
     * Cotrols the visibility status of the text input field.
     * @param aVisibility Visbility status to be set.
     */
    void SetVisibility( TBool aVisibility );
    
private:
    /**
     * Constructor
     *
     * @param aOwnerControl HuiControl which will own the visual.
     * @param aCoeControl CoeConotrol of which the TRect will be managed.
     */
    CFsTextInputVisual( CAlfControl& aOwner, 
                        CCoeControl& aCoeControl );


    void ConstructL( CAlfControl& aOwner );

private: //data
    /**
     * Text input field - CoeControl.
     * Must be mutable becouse it is changed in the DrawSelf method.
     * DrawSelf method must be const becouse it's inherited from 
     * CHuiVisual class.
     * Not owned.
     */
    mutable CCoeControl& iCoeControl;
    
    /**
     * Visibility status
     */
    TBool iVisible;
    };


#endif //C_FSTEXTINPUTVISUAL_H
