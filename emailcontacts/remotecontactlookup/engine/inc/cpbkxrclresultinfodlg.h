/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclResultInfoDlg
*
*/


#ifndef CPBKXRCLRESULTINFODLG_H
#define CPBKXRCLRESULTINFODLG_H

#include "cpbkxrclbasedlg.h"
#include "pbkxrclengineconstants.h"
#include <eiklbo.h>                 // MEikListBoxObserver

class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknTabGroup;
class CAknTitlePane;
class MPbkxRclContactRetrieval;
class CPbkContactItem;
class CPbkContactEngine;
class CPbkxRclCbRSender;
class CPbkxRclActionServiceWrapper;
class TPbkContactItemField;
class CPbkFieldArray;

/**
 *  Result information view implementation.
 *
 *  @lib pbkxrclengine.lib
 *  @since S60 3.1
 */
class CPbkxRclResultInfoDlg : public CPbkxRclBaseDlg,
							  public MEikListBoxObserver
    {
public: // constructor and destructor

    /**
    * Two-phase constructor.
    *
    * @param aIndex Selected item index, is updated by dialog after selection
    *               is made.
    * @param aArray Array of list box items.
    * @param aCommand Observer which executes dialog menu commands.
    * @param aContactRetrieval Used to get more information about contacts.
    * @param aContactEngine Used to create pbk contacts.
    * @param aActionService Contact action service.
    * @param aContactSelectorEnabled Flag indicating whether contact
    *                                selection is enabled.
    * @param aActiveItemIndex Initial active item index.
    * @return Created object.
    */
    static CPbkxRclResultInfoDlg* NewL( 
        TInt& aIndex, 
        CDesCArray* aArray,
        MEikCommandObserver* aCommand,
        MPbkxRclContactRetrieval* aContactRetrieval,
        CPbkContactEngine& aContactEngine,
        CPbkxRclActionServiceWrapper& aActionService,
        TBool aContactSelectorEnabled,
        TInt aActiveItemIndex );
    
    /**
    * Destructor.
    */
    virtual ~CPbkxRclResultInfoDlg();

public: // methods from base classes
    
    /**
    * Hides option menu items that are not supposed to be shown.
    *
    * @param aResourceId Menu resource id.
    * @param aMenuPane Menu pane.
    */
    virtual void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
    * Gets help context for this view.
    *
    * @param aContext On return, contains help context information
    */
    virtual void GetHelpContext( TCoeHelpContext& aContext ) const;

public: // new methods

    /**
    * Executes dialog and destroys it after it is closed.
    */
    void ExecuteLD();

    /**
    * Closes dialog.
    */
    void Close();

    /**
    * Sends callback request to selected contact.
    */
    void SendCallbackRequestL();

    /**
    * Sends bussiness card of currently selected contact.
    */ 
    void SendBusinessCardL();

    /**
    * Updates existing contact with currently selected field.
    *
    * If update is successful, a string is constructed which is shown
    * to the user.
    *
    * @return Constructed string to be shown to the user or NULL, if
    *         operation failed.
    */
    HBufC* UpdateExistingContactL();
    
protected: // redefinition of functions in the base class

    /**
    * Handles key events.
    *
    * @param aKeyEvent Key event.
    * @param aType Type of the key event.
    * @return Key response (handled/not handled).
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

	// MEikListBoxObserver

    /**
     * Listbox event handler.
     *
     * @param aListBox that invoked the event.
     * @param aEventType Type of the event.
     */
    virtual void HandleListBoxEventL(
       CEikListBox* aListBox,
       TListBoxEvent aEventType );

    TBool OkToExitL( TInt aButtonId );
    
private: // methods used internally

    /**
    * Setup status pane (tab group and title pane text).
    */
    void SetupStatusPaneL();

    /**
    * Setup tab group.
    *
    * Each contact gets its own tab.
    */
    void SetupTabGroupL();

    /**
    * Sets the visibility of navi indicators.
    */
    void SetNaviIndicatorsL();

    /**
    * Sets title pane text.
    */
    void SetTitlePaneTextL();

    /**
    * Reverts changes made to status pane.
    */
    void ResetStatusPane();

    /**
    * Removes tab group and frees the resources.
    */
    void ResetTabGroup();

    /**
    * Creates list box entries of the currently selected contact.
    */
    void CreateListBoxEntriesL();

    /**
    * Fetches current contact from contact retrieval interface.
    *
    * @return ETrue if fetching is successful, EFalse otherwise.
    */
    TBool FetchCurrentContactL();

    /**
    * Updates both list box entries and status pane.
    */
    void UpdateDialogL();

    /**
    * Returns ETrue if there are call actions available, EFalse
    * otherwise.
    *
    * @return ETrue if there is call actions available.
    */
    TBool CallActionsAvailable() const;

    /**
    * Returns ETrue if focus is on phone number field.
    *
    * @return ETrue if focus is on phone number field.
    */
    TBool FocusOnPhoneNumberField() const;

    /**
    * Returns ETrue if focus is on email address field.
    *
    * @return ETrue if focus is on email address field.
    */
    TBool FocusOnEmailAddressField() const;
        
    /**
    * Returns ETrue if focus is on voip field.
    *
    * @return ETrue if focus is on voip field.
    */
    TBool FocusOnVoipField() const;

    /**
    * Returns current field focused.
    */
    TPbkContactItemField& CurrentField() const;

private: // constructors

    /**
    * Constructor.
    *
    * @param aIndex Selected item index, is updated by dialog after selection
    *               is made.
    * @param aArray Array of list box items.
    * @param aCommand Observer which executes dialog menu commands.
    * @param aContactRetrieval Used to get more information about contacts.
    * @param aContactEngine Used to create pbk contacts.
    * @param aActionService Contact action service.
    * @param aContactSelectorEnabled Flag indicating whether contact
    *                                selection is enabled.
    * @param aActiveItemIndex Initial active item index.
    */
    CPbkxRclResultInfoDlg( 
        TInt& aIndex, 
        CDesCArray* aArray, 
        MEikCommandObserver* aCommand,
        MPbkxRclContactRetrieval* aContactRetrieval,
        CPbkContactEngine& aContactEngine,
        CPbkxRclActionServiceWrapper& aActionService,
        TBool aContactSelectorEnabled,
        TInt aActiveItemIndex );
    
    /**
    * Second-phase constructor.
    */
    void ConstructL();    
    
private: // data

    // Array of items. Not owned.
    CDesCArray* iItems;

    // Contacts which information are shown. Owned.
    CPbkContactItem* iCurrentContact;
    
    // Navi pane of the current UI application. Not owned.
    CAknNavigationControlContainer* iNaviPane;

    // Navigation decorator. Owned.
    CAknNavigationDecorator* iNaviDecorator;

    // Navigation tab group. Not owned.
    CAknTabGroup* iTabGroup;

    // Title pane of the application. Not owned.
    CAknTitlePane* iTitlePane;
    
    // Currently active tab.
    TInt iActiveTabIndex;

    // Flag indicating whether contact selector is enabled or not.
    TBool iContactSelectorEnabled;

    // Interface through which contact details are retrieved. Not owned.
    MPbkxRclContactRetrieval* iContactRetrieval;

    // Contact engine to fetch the correct item priorities.
    CPbkContactEngine& iContactEngine;

    // Action service wrapper for handling menu commands.
    CPbkxRclActionServiceWrapper& iContactActionService;

    // Text shown in wait note when details are retrieved. Owned.
    RBuf iWaitNoteText;

    // Original title pane text. Owned.
    HBufC* iTitlePaneText;

    // Class for making callback requests. Owned.
    CPbkxRclCbRSender* iCallbackReqSender;

    // Currently visible fields. Owned.
    CPbkFieldArray* iVisibleFields;

    // Flag indicating that we are exiting dialog. After this is set
    // no key presses are handled.
    TBool iExit;

    };

#endif // CPBKXRCLRESULTINFODLG_H

