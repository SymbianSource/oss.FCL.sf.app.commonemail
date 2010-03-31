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
* Description:  Definition of class CESMRContactMenuUrlHandler.
 *
*/


#ifndef CESMRCONTACTMENUURLHANDLER_H
#define CESMRCONTACTMENUURLHANDLER_H

#include <e32base.h>

class CEikMenuPane;
class CFscContactActionMenu;
class CGulIcon;
class CAiwServiceHandler;
class CEikonEnv;

NONSHARABLE_CLASS( CESMRContactMenuUrlHandler ) : public CBase
    {
public:
    /**
     * Two-phase constructor.
     *
     * @param aContactActionMenu contact action menu
     * @return new instance of the class
     */
    static CESMRContactMenuUrlHandler* NewL(
            CFscContactActionMenu& aContactActionMenu );

    /**
     * Destructor.
     */
    ~CESMRContactMenuUrlHandler( );

    /**
     * Resets/disables url contact menu handler.
     */
    void Reset( );

    /**
     * Enables contact action menu and options menu for given link.
     *
     * @param aUrl selected url
     */
    void SetUrlL( const TDesC& aUrl );

    /**
     * Returns true if options menu / actions is available.
     * @return TBool
     */
    TBool OptionsMenuAvailable( );

    /**
     * Initializes options menu / actions submenu.
     *
     * @param aActionMenuPane actions submenu
     */
    void InitOptionsMenuL( CEikMenuPane* aActionMenuPane );

    /**
     * Executes actions submenu selection.
     *
     * @param aCommand selected command id
     */
    void ExecuteOptionsMenuL( TInt aCommand );

    /**
     * Executes contact action menu.
     */
    void ShowActionMenuL( );

private:
    /**
     * Private constructor.
     *
     * @param aContactActionMenu contact action menu
     */
    CESMRContactMenuUrlHandler( CFscContactActionMenu& aContactActionMenu );

    /**
     * Opens selected richtext link in browser.
     */
    void OpenUrlInBrowserL( );

    /**
     * Opens selected richtext link in intranet browser.
     */
    void OpenUrlInIntranetL( );

    /**
     * Bookmarks selected richtext link.
     */
    void BookmarkUrlL( );

    /**
     * Creates a single actionmenu item with selectable command
     * @param aResourceId
     * @param aCommandUid
     */
    void CreateMenuItemL( TInt aResourceId, TUid aCommandUid );

    /**
     * create icons to actionmenu
     */
    CGulIcon* CreateIconL(TUid aCommandUid);

    /**
     * Is service vailable
     * @param aResourceId resource id
     * @return TBool
     */
    TBool IsServiceAvailableL(TInt aResourceId);
    
    /**
     * Service handler
     * @return Reference to CAiwServiceHandler 
     */
    CAiwServiceHandler& ServiceHandlerL();
private: //data

    ///Ref: Contact action menu
    CFscContactActionMenu& iContactActionMenu;

    /// Own: Current Url.
    HBufC* iUrl;

    /// Own: array of actionmenu icons
    RPointerArray<CGulIcon> iIconArray;

    /// Own: instance of aiw servicehandler
    CAiwServiceHandler* iServiceHandler;
    };

#endif
