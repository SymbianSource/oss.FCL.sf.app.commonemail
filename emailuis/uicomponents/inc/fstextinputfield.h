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
* Description:  Declaration of CFsTextInputField class
*
*/


#ifndef C_TEXTINPUTFIELD_H
#define C_TEXTINPUTFIELD_H

#include <eikrted.h>

class MFsTextInputFieldObserver;
class CHuiControl;
class CHuiLayout;
class CFsTextInputVisual;
class CCoeControl;

/**
 * CFsTextInputField class is responsible for providing to the user
 * interface for the typying text. The entered text is passed to the
 * text input field observer which informs the FAL controller about
 * changes in the text. The new itemset in the list is set. The itemset
 * is based on the typed text.
 */

NONSHARABLE_CLASS( CFsTextInputField ) : public CEikRichTextEditor
    {
public:

    /**
     * Input text mode:
     * EAtomicInput - atomic mode
     * ENotAtomicInput - normal mode
     */
    enum TFsTextInputMode
        {
        ENotAtomicInput=1,
        EAtomicInput
        };

    /**
     * Two-phased constructor
     *
     * @param aObserver Observer which will receive events
     * @return Resturns pointer to the instanse of the CFsTextInputField
     */
    static CFsTextInputField* NewL( MFsTextInputFieldObserver& aObserver );

    /**
     * Two-phased constructor
     *
     * @param aObserver Observer which will receive events
     * @return Resturns pointer to the instanse of the CFsTextInputField
     */ 
    static CFsTextInputField* NewLC( MFsTextInputFieldObserver& aObserver );
    
    /**
     * Destructor
     */
    virtual ~CFsTextInputField();
    
    /**
     * Set Text input mode
     *
     * @param aMode Input mode {ENotAtomicInput, EAtomicInput}
     */
    void SetTextInputMode( const TFsTextInputMode aMode );
    
    /**
     * Returns Text input mode
     *
     * @return TFsTextInputMode Input mode {ENotAtomicInput, EAtomicInput}
     */
     TFsTextInputMode TextInputMode() const;

    /**
     * Set Text input mode
     *
     * @param aMode Input mode {ENotAtomicInput, EAtomicInput}
     */
     TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent, 
         TEventCode aType );

private:

    /**
     * Constructor
     *
     * @param aObserver Observer which will receive events
     * @param aOwner Control which will own text input field
     */
    CFsTextInputField( MFsTextInputFieldObserver& aObserver );
    
    /**
     * Two phase constructor
     *
     * @param aParentLayout Parent layout to which append 
     *                      the text input field
     */
    void ConstructL();    
    
protected:

    /**
     * From CEikRichTextEditor
     * Invoked when changes in text field are made.
     */
    void EditObserver ( TInt aStartEdit, TInt aEditLength );

private: //data

    /**
     * Text input field observer. 
     * The observer is informed about changes in the text.
     * Not owned.
     */
    MFsTextInputFieldObserver& iObserver;
    
    /**
     * Current input mode of the text input field.
     */
    TFsTextInputMode iFsTextInputMode;
    
    };


#endif  // C_TEXTINPUTFIELD_H
