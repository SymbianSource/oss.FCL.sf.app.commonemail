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
* Description:  Declaration of MFsTextInputFieldObserver interface
*
*/


#ifndef M_FSFTEXTINPUTFIELDOBSERVER_H
#define M_FSFTEXTINPUTFIELDOBSERVER_H

class CFsTextInputField;


/**
 *  Observer interface for CFsTextInputField.
 *  The observer is informed about changes in a text.
 *  
 */
class MFsTextInputFieldObserver
    {
public:

    /**
     * ...
     */
    enum TFsTextInputEventType
        {
        EEventTextUpdated
        };

    /**
     * This will be called when the user enters some text into the text
     * input field. This will also called when the text input field is
     * cleared.
     *
     * @param aTextInputField The text input field which invoked the method.
     * @param aEventType The event passed to the observer.
     */
    IMPORT_C virtual void HandleTextInputEventL(
                             CFsTextInputField* aTextInputField,
                             TFsTextInputEventType aEventType )=0;
    };


#endif // M_FSFTEXTINPUTFIELDOBSERVER_H
