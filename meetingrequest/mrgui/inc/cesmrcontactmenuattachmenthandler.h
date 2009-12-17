/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Creates and display attachment action menu.
*
*/


#ifndef CESMRCONTACTMENUATTACHMENTHANDLER_H
#define CESMRCONTACTMENUATTACHMENTHANDLER_H

#include <e32base.h>
#include <eikcmobs.h>

// Forward declarations
class CEikMenuPane;
class CFscContactActionMenu;
class CGulIcon;
// <cmail>
class CESMRAttachmentInfo;
// </cmail>

NONSHARABLE_CLASS( CESMRContactMenuAttachmentHandler ) : public CBase
    {
public: // enumarations
    /**
     * Predefined action menu items that are availabe
     */
    enum TESMRAttachmentActionMenuItem
        {
        EActionOpen = 0
        };

public: // Constructors and destructors

    /**
     * Two-phase constructor.
     *
     * @param aContactActionMenu contact action menu
     * @return new instance of the class
     */
    static CESMRContactMenuAttachmentHandler* NewL(
        CFscContactActionMenu& aContactActionMenu );

    /**
     * Destructor
     */
    ~CESMRContactMenuAttachmentHandler();

public: // New methods
    /**
     * Resets/disables attachment contact menu handler.
     */
    void Reset();

    /**
     * Sets attachment info for action menu to show/hide options.
     * Ownership does not change.
     * 
     * @param aAttachmentInfo attachment info object
     */
    void SetAttachmentInfo( CESMRAttachmentInfo* aAttachmentInfo );

    /**
     * Enables contact action menu and options menu for given attachment(s).
     */
    void InitActionMenuL( );

    /**
     * Returns true if options menu / actions is available.
     */
    TBool OptionsMenuAvailable( );

    /**
     * Initializes options menu / actions submenu.
     *
     * @param aActionMenuPane actions submenu
     */
    void InitOptionsMenuL( CEikMenuPane* aActionMenuPane );

    /**
     * Executes contact action menu.
     */
    void ShowActionMenuL( );

    /**
     * Sets command observer for field. Field can use command
     * observer for triggering commands.
     * @param @aCommandObserver Reference to command observer
     */
     void SetCommandObserver( MEikCommandObserver* aCommandObserver );

private:

    /**
     * C++ default Constructor
     * @param aContactActionMenu contact action menu
     */
    CESMRContactMenuAttachmentHandler( 
    		CFscContactActionMenu& aContactActionMenu );

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();

    /**
     * Creates a single actionmenu item with selectable command
     * @param aResourceId
     * @param aCommandUid
     */
    void CreateMenuItemL( TInt aResourceId, TUid aCommandUid );

    /**
     * create icons to actionmenu
     */
    CGulIcon* CreateIconL( TUid aCommandUid );

private: // data

    /// Ref: Contact action menu
    CFscContactActionMenu& iContactActionMenu;

    /// Not owned
    CESMRAttachmentInfo* iAttachmentInfo;

    /// Own:
    TBool iOptionsMenuAvailable;

    /// Own: array of actionmenu icons
    RPointerArray<CGulIcon> iIconArray;

    /// Ref: Pointer to command observer
    MEikCommandObserver* iCommandObserver;
    };

#endif // CESMRCONTACTMENUATTACHMENTHANDLER_H

// End of file
