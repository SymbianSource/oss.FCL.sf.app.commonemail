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
* Description: This file defines class CFSEmailUiShortcutBinding.
*
*/


#ifndef __FS_EMAIL_UI_SHORTCUT_BINDING_H
#define __FS_EMAIL_UI_SHORTCUT_BINDING_H

#include <PtiDefs.h>
#include <e32property.h>
#include "FreestyleEmailUi.hrh"


// CONSTANTS

// FORWARD DECLARATIONS
class CEikMenuPane;
class CPtiEngine;
class CPtiKeyMappings;
class CPtiQwertyKeyMappings;
class CPtiHalfQwertyKeyMappings;


// CLASS DECLARATION
class CFSEmailUiShortcutBinding : public CBase
    {
public: // types
    
    //! Defines a view where the shortcut is used
    enum TShortcutContext
        {
        EContextMailList,
        EContextSearchResults,
        EContextMailViewer,
        EContextHtmlViewer,
        EContextMailDetails,
        EContextFolderList,
        EContextAttachmentList,
        EContextDownloadManager,
        EContextMainGrid,
        EContextSettings,
        EContextSendAttachmentList
        };

    //! @internal
    struct TShortcutBinding
        {
        TText iItutShortcut;        //!< Shortcut character on ITU-T. 0 if N/A.
        
        TText iQwertyShortcut;      //!< Shortcut character on QWERTY. 0 if N/A.
        TUint32 iQwertyModifiers;   //!< Modifiers of type TEventModifier required for the QWERTY shortcut. 0 if none.
        
        TInt iCommandId;            //!< The command for the shortcut.
        };

public: // construction & destruction
	
    static CFSEmailUiShortcutBinding* NewL();
    
    ~CFSEmailUiShortcutBinding();
        
public: // methods
    
    /**
    * Gives a command ID for the given shortcut key. In the QWERTY mode, this 
    * depends on the English key mappings of the used device.
    * @param    aKey        Key event for which the command is fetched
    * @param    aContext    The view where the shortcut is used
    * @return   The command ID for the given key. 
    *           KErrNotFound if there is no command binded to the key in the given context.
    */
    TInt CommandForShortcutKey( const TKeyEvent& aKey, TShortcutContext aContext ) const;
    
    /**
    * Gives a command ID for the given shortcut. As opposed to the function 
    * CommandForShortcutKey(), this function does no mapping for the given
    * shortcut.
    * @param    aShortcut   The shortcut character for which the command is fetched.
    * @param    aContext    The view where the shortcut is used.
    * @param    aModifier   Modifiers (like ctrl) which are used when entering the shortcut.
    * @return   The command ID for the given shortcut. 
    *           KErrNotFound if there is no command binded to the shortcut in the given context.
    */
    static TInt CommandForShortcut( TText aShortcut, TShortcutContext aContext, TUint32 aModifiers=0 );
    
    /**
    * Gives a shortcut character for the given command. This function supports only
    * one key shortcuts: if the shortcut is made up of a modifer (like ctrl) and the
    * base key, this function does not find it.
    * @param    aCommandId  The command for which a shortcut is fetched.
    * @param    aContext    The view where the shortcut should be available.
    * @param    aQwertyMode Is the shortcut for QWERTY or ITU-T mode required.
    *                       ETrue means QWERTY mode.
    * @return   The shortcut character for the given command.
    *           0 if no such command was found.
    */
    static TText ShortcutForCommand( TInt aCommandId, TShortcutContext aContext, TBool aQwertyMode );
    
    /**
    * Adds the shortcut hints to the given menu. If the device is currently in the QWERTY
    * mode, the QWERTY shortcuts are used. Otherwise, the ITU-T shortcuts are used.
    * Only one key shortcuts are hinted: if a QWERTY shortcut needs a modifier key (like ctrl),
    * no hint is added to the menu.
    * @param    aMenuPane   The menu where the shortcuts should be added.
    * @param    aContext    The view in which the menu is shown.
    */
    void AppendShortcutHintsL( CEikMenuPane& aMenuPane, TShortcutContext aContext ) const;

    /**
    * Possible keyboard modes. Values match values stored to property key KAknKeyBoardLayout.  
    */
    enum TKeyBoardType
        {
        ENoKeyboard = 0,
        EItutKeyPad = 1,
        EQwerty4x12 = 2,
        EQwerty4x10 = 3,
        EQwerty3x11 = 4,
        EHalfQwerty = 5,
        ECustomQwerty = 6
        };
     
    /**
    * For checking current keyboard mode
    * @return   Type of the currently active keyboard
    */
    TKeyBoardType KeyBoardType() const;


private: // construction

	CFSEmailUiShortcutBinding();
	
	void ConstructL();
	
private: // methods
		
	//! Utility function for selecting the correct binding table for the given view.
	static void GetBindingsForContext( const TShortcutBinding*& aBindingArray, 
	                                   TInt& aBindingCount, 
	                                   TShortcutContext aContext );
	
	//! Appends the given shortcut hint with decorations to the given descriptor.
	static void AppendHintToCaption( TDes& aCaption, TText aHint );
	
private: // data
	
	CPtiEngine* iPtiEngine;
	
    RProperty iKbLayoutProperty;

    CPtiQwertyKeyMappings* iQwertyMappings; // owned by iPtiEngine
    CPtiHalfQwertyKeyMappings* iHalfQwertyMappings; // owned by iPtiEngine
    };
    
#endif // __FS_EMAIL_UI_SHORTCUT_BINDING_H
