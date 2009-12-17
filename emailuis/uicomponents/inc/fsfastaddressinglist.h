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
* Description:  Declaration of CFsFastAddressingList class
*
*/



#ifndef C_FSFASTADDRESSINGLIST_H
#define C_FSFASTADDRESSINGLIST_H

#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//#ifdef __FS_ALFRED_SUPPORT
#include <alf/alfcontrol.h>
#include <alf/alfenv.h>
/*#else // !__FS_ALFRED_SUPPORT
#include <HuiControl.h>
#include <HuiImage.h>
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstextinputfieldobserver.h"

class MFsFastAddressingListObserver;
class CFsTextInputField;
class CFsFastAddressingVisualizer;
class CFsTreeList;
//<cmail> removed __FS_ALFRED_SUPPORT flag
/*#ifdef __FS_ALFRED_SUPPORT
#else // !__FS_ALFRED_SUPPORT
class CHuiEnv;
class CHuiVisual;
class THuiEvent;
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag
class CFbsBitmap;
class CCoeControl;
class CFsTreeVisualizerBase;
class CFsTreeList;
class MFsTreeItemData;
class MFsTreeItemVisualizer;


/**
 *  Fast Addresssing List (FAL).
 *
 *  This is the UI component part of fast addressing list. FAL is heavily
 *  dependant on an associated component, Fast Addressing Controller
 *  The FAL is created and owned by the Controller.
 *
 *  The FAL consists of four parts: a list component, two icons and a text
 *  input field. When the FAL is created the text input field is always
 *  visible. The icons reside on the either side of the text input field.
 *  They are specified by the Controller.  The controller can choose to
 *  omit both icons, load either one of them or both.
 *
 *  The text input field can be used the end user to input text, e.g. email
 *  addresses. The text is passed to the Controller which is responsible
 *  for searching for matching items in e.g. the phonebook. If it finds
 *  matches, it will ask the FAL to display the list component. The end
 *  user can then either enter more text or select any item from the list.
 *
 *  The controller will search for the matching text in the background so
 *  that the end user can enter more text during the searching.
 *
 */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
NONSHARABLE_CLASS( CFsFastAddressingList ) : public CAlfControl,
                                             public MFsTextInputFieldObserver
/*#else // !__FS_ALFRED_SUPPORT
NONSHARABLE_CLASS( CFsFastAddressingList ) : public CHuiControl,
                                             public MFsTextInputFieldObserver
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag
    {

public: // icon manipulation

    enum TIconID
        {
        EIconA,
        EIconB
        };

public:
    /**
     * Two phase constructor.
     *
     * @param aEnv Reference to CHuiEnv environment
     * @param aObserver Observer which will receive events
     *                  from Fast Addressing List
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    IMPORT_C static CFsFastAddressingList* NewL( CAlfEnv& aEnv,
        MFsFastAddressingListObserver& aObserver );
/*#else // !__FS_ALFRED_SUPPORT
    IMPORT_C static CFsFastAddressingList* NewL( CHuiEnv& aEnv,
        MFsFastAddressingListObserver& aObserver );
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag

    /**
     * Destructor.
     */
    virtual ~CFsFastAddressingList();

    /**
     * Sets selected icon using a filename.
     *
     * @param aIconID point to which icon is to be set {EIconA, EIconB}
     * @param aFilename Filename containing an icon graph.
     */
    IMPORT_C void SetIconL( const TIconID aIconID, const TDesC& aFilename );

    /**
     * Sets selected icon using a Bitmap.
     *
     * @param aIconID point to which icon is to be set {EIconA, EIconB}
     * @param aBitmap Icon bitmap.
     */
    IMPORT_C void SetIconL( const TIconID aIconID, const CFbsBitmap* aBitmap );

    /**
     * Sets visibility of the icon.
     *
     * @param aIconID point to which icon is to be set {EIconA, EIconB}
     * @param aVisible Visibility status to set {ETrue, EFalse}
     */
    IMPORT_C void SetIconVisible( const TIconID aIconID,
                                  const TBool aVisible );

    /**
     * Returns the status of icon visibility.
     *
     * @param aIconID point to which icon is to be set {EIconA, EIconB}
     * @return Visibility status.
     */
    IMPORT_C TBool IconVisible( const TIconID aIconID ) const;

public: // text input field manipulation

    /**
     * Sets status of Atomic mode of the text input field.
     *
     * @param aAtomic Status to be set {ETrue, EFalse}
     */
    IMPORT_C void SetTextInputFieldAtomic( const TBool aAtomic );

    /**
     * Returns the status of Atomic mode of the text input field.
     *
     * @return Status of Atomic mode {ETrue, EFalse}
     */
    IMPORT_C TBool TextInputFieldAtomic() const;


    /**
     * Sets the ReadOnly mode of the text input field.
     *
     * @param aReadOnly Status to be set {ETrue, EFalse}
     */
    IMPORT_C void SetTextInputFieldReadOnly( const TBool aReadOnly );

    /**
     * Returns the status of ReadOnly mode of the text input field.
     *
     * @return Status of ReadOnly mode {ETrue, EFalse}
     */
    IMPORT_C TBool TextInputFieldReadOnly() const;

    /**
     * Sets content of the text input field to aText.
     *
     * @param aText Text to be set in the text input field
     */
    IMPORT_C void SetTextFieldContentsL( const TDesC& aText );

    /**
     * Returns content of the text input field.
     *
     * @param aText TDesC to which the content will be copied.
     */
    IMPORT_C void TextFieldContents( TDes& aText ) const;


    /**
     * Sets the text input field visibility.
     *
     * @param aVisibility of the text input field.
     */
    IMPORT_C void SetTextFieldVisibile( const TBool aVisibility );

    /**
     * Retruns the text input field visibility
     *
     * @return TBool visibility of the text input field.
     */
    IMPORT_C TBool IsTextFieldVisible() const;


public: // list manipulation

    enum TFsListPosition
        {
        EListOnTopOfTextField,
        EListBelowTextField
        };

    /**
     * Sets visibility of the list.
     *
     * @param aVisible Visibility status to set {ETrue, EFalse}
     */
    IMPORT_C void SetListVisibility( const TBool aVisible );

    /**
     * Returns the status of the list visibility.
     *
     * @return Visibility status.
     */
    IMPORT_C TBool ListVisibile() const;

    /**
     * Sets items in the list.
     *
     * @param aItemTexts Items to be set in the list.
     */
    IMPORT_C void SetListItemsL( const RPointerArray<HBufC>& aItemTexts );

    /**
     * Adds item to the list.
     *
     * @param aItemText Item to be added to the list.
     */
    IMPORT_C void AddListItemL( const TDesC& aItemText );

    /**
     * Highilght passed text.
     *
     * @param aText Text to be highlighted.
     */
    IMPORT_C void HighlightText( const TDesC& aText );


    /**
     * Sets position of the list
     *
     * @param aPos Position to be set {EListOnTopOfTextField,
     *                                 EListBelowTextField}
     */
    IMPORT_C void SetListPosition( const TFsListPosition aPos );

    /**
     * Returns the position of the list.
     *
     * @return Position of the list {EListOnTopOfTextField,
     *                               EListBelowTextField}
     */
    TFsListPosition ListPosition() const;

    /**
     * Sets visibility of the scrollbar.
     *
     * @param aVisible Visibility status to set {ETrue, EFalse}
     */
    // use enum from CFsTreeList: on, off, automatic
    IMPORT_C void SetScrollbarVisibility( const TBool aVisible );

    /**
     * Returns the status of the scrollbar visibility.
     *
     * @return Visibility status.
     */
    // use enum from CFsTreeList: on, off, automatic
    IMPORT_C TBool ScrollbarVisibility() const;

    /**
     * Access to the list component is provided as a convenience -
     * this allows the controller the liberty to use all public methods
     * of CFsTreeList directly.
     *
     * @return Reference to the CFsTreeList component.
     */
    IMPORT_C CFsTreeList& List() const;

public: // from class CHuiControl
    /**
     * From CHuiControl.
     * Called when an input event is being offered to the control.
     *
     * @param aEvent  Event to be handled.
     * @return ETrue, if the event was handled. Otherwise EFalse.
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    IMPORT_C TBool OfferEventL( const TAlfEvent &aEvent );
/*#else // !__FS_ALFRED_SUPPORT
    IMPORT_C TBool OfferEventL( const THuiEvent &aEvent );
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag

    /**
     * From CHuiControl
     * Notifies the control that one of the visuals it ows has been laid out.
     * Called automatically from CHuiVisual::UpdateChildrenLayout().
     * For example, occurs when a parent layout is resized.
     *
     * @param  aVisual  Visual whose layout was updated.
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    IMPORT_C void VisualLayoutUpdated( CAlfVisual& aVisual );
/*#else // !__FS_ALFRED_SUPPORT
    IMPORT_C void VisualLayoutUpdated( CHuiVisual& aVisual );
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag


    /**
     * From CHuiControl.
     * Notifies the control that its visible has been changed on a display.
     * This is the earliest time when the control knows the dimensions of the
     * display it is being shown on.
     *
     * @param aIsVisible  ETrue, if the control is now visible on the display.
     *                    EFalse, if the control is about to the hidden
     *                    on the display.
     * @param aDisplay The display on which the control's
     *                 visibility is changing.
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    IMPORT_C void NotifyControlVisibility( TBool aIsVisible,
        CAlfDisplay& aDisplay );
/*#else // !__FS_ALFRED_SUPPORT
    IMPORT_C void NotifyControlVisibility( TBool aIsVisible,
        CHuiDisplay& aDisplay );
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag


public: // From class MFsTextInputFieldObserver

    /**
     * From MFsTextInputFieldObserver
     * Called by input text field when an event occures.
     *
     * @param aTextInputField Event sender
     * @param aEventType Event type
     */
    IMPORT_C void HandleTextInputEventL( CFsTextInputField* aTextInputField,
        MFsTextInputFieldObserver::TFsTextInputEventType aEventType );



private:
    /**
     * Constructor
     *
     * @param aEnv Reference to CHuiEnv environment
     * @param aObserver Observer which will receive events from
     *                  Fast Addressing List
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    CFsFastAddressingList( CAlfEnv& aEnv,
                           MFsFastAddressingListObserver& aObserver );
/*#else // !__FS_ALFRED_SUPPORT
    CFsFastAddressingList( CHuiEnv& aEnv,
                           MFsFastAddressingListObserver& aObserver );
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag

    /**
     * Two phase constructor.
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag#ifdef __FS_ALFRED_SUPPORT
    void ConstructL( CAlfEnv& aEnv );
#else // !__FS_ALFRED_SUPPORT
    void ConstructL( CHuiEnv& aEnv );
#endif // __FS_ALFRED_SUPPORT

    /**
     * Select highlighted text and copies into text input field
     */
    void MakeSelection();

    /**
     * Select next item on the list.
     */
    void MoveSelectionDown();

    /**
     * Select previous item on the list.
     */
    void MoveSelectionUp();

    /**
     * Sets text input mode
     */
    void SetTextInputMode();

    /**
     * Called when text input field was modified.
     */
    void TextInputFieldModified();

private: // data

    /**
     * Fied where the text is typed.
     * Owned.
     */
    CFsTextInputField* iTextInputField;

    /**
     * FAL observer. It is informed about changes in FAL.
     * Not owned.
     */
    MFsFastAddressingListObserver& iObserver;

    /**
     * Image for Icon A
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    TAlfImage iIconA;
/*#else // !__FS_ALFRED_SUPPORT
    THuiImage iIconA;
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag
    /**
     * Image for Icon B
     */
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#ifdef __FS_ALFRED_SUPPORT
    TAlfImage iIconB;
/*#else // !__FS_ALFRED_SUPPORT
    THuiImage iIconB;
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag
    /**
     * List with set of items that matches the typed text.
     * Owned.
     */
    CFsTreeList*    iTreeList;

    /**
     * Visualisation of the FAL component.
     * Owned.
     */
    CFsFastAddressingVisualizer* iVisualizer;

    /**
     * Visualisation of the list passed to tree list component.
     * Not owned.
     */
    CFsTreeVisualizerBase* iTreeVisualizer;

    /**
     * Array of items added to the tree list.
     */
    RPointerArray<MFsTreeItemData> iPlainItemDataArr;

    /**
     * Array of visualizers for items added to the tree list.
     */
    RPointerArray<MFsTreeItemVisualizer> iPlainItemVisualizerArr;

    };

#endif // C_FSFASTADDRESSINGLIST_H
