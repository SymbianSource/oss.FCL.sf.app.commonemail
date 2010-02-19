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
* Description: This file implements class CFSEmailUiShortcutBinding.
 *
 */


#include "emailtrace.h"
#include <PtiEngine.h>
#include <PtiKeyMappings.h>
#include <centralrepository.h>
#include <AvkonInternalCRKeys.h>
#include <eikmenup.h>

#include "FreestyleEmailUiShortcutBinding.h"
#include "FSEmail.pan"


//////////////////////////////
// The shortcut binding tables
// ---------------------------
// In case the same shortcut is assigned for multiple commands, the shortcut to 
// command conversion will always return the first mapping.
//
// Shortcuts with modifiers must be listed before unmodifed shortcuts, at least when
// the same key has different meaning as plain key and with modifier. Otherwise,
// the unmodified mapping will be used also for the modified key.

// Shortcuts for the mail list view
const CFSEmailUiShortcutBinding::TShortcutBinding KMailListShortcuts[] = 
    {
        { 0,  'A', 0,     EFsEmailUiCmdActionsReplyAll },
        { 0,  'R', 0,     EFsEmailUiCmdActionsReply },
        { 0,  'F', 0,     EFsEmailUiCmdActionsForward }, 
        { 0,  'M', 0,     EFsEmailUiCmdActionsMove },
        { 0,    'Y', 0,     EFsEmailUiCmdActionsCopyMessage },
        { 0,    'D', 0,     EFsEmailUiCmdActionsDelete },
        { 0, EStdKeyBackspace, 0, EFsEmailUiCmdActionsDelete },
        { 0,  'T', 0,     EFsEmailUiCmdGoToTop },
        { 0,  'B', 0,     EFsEmailUiCmdGoToBottom },
        { 0,    'J', 0,     EFsEmailUiCmdPageUp },
        { 0,    'K', 0,     EFsEmailUiCmdPageDown },
        { 0,    'L', 0,     EFsEmailUiCmdActionsFlag },
        { 0,  'C', 0,     EFsEmailUiCmdCompose },
        { 0,    'E', 0,     EFsEmailUiCmdCalActionsAccept }, 
        { 0,    'G', 0,     EFsEmailUiCmdCalActionsTentative },
        { 0,    'V', 0,     EFsEmailUiCmdCalActionsDecline },
        { 0,    'O', 0,     EFsEmailUiCmdActionsOpen },
        { 0,  'Z', 0,     EFsEmailUiCmdSync },
        { 0,    'S', 0,     EFsEmailUiCmdSearch },
        { 0,    'Q', 0,     EFsEmailUiCmdGoToSwitchFolder },
        { 0,    'W', 0,     EFsEmailUiCmdGoToSwitchSorting },
        { 0,    'H', 0,     EFsEmailUiCmdHelp },
        
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsReadUnreadToggle },
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsRead },   
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsUnread },
        
        { 0,    'I', 0,     EFsEmailUiCmdActionsCollapseExpandAllToggle },
        { 0,    'I', 0,     EFsEmailUiCmdActionsCollapseAll },
        { 0,    'I', 0,     EFsEmailUiCmdActionsExpandAll },
        
        { 0,  0, 0,       EFsEmailUiCmdMarkUnmarkToggle },
        { 0,  0, 0,       EFsEmailUiCmdMarkMark }, 
        { 0,  0, 0,       EFsEmailUiCmdMarkUnmark }
    };

// Shortcuts for the mail viewer
const CFSEmailUiShortcutBinding::TShortcutBinding KMailViewerShortcuts[] = 
    {
        { 0,    'C', EModifierCtrl, EFsEmailUiCmdActionsCopyToClipboard },

        { 0,  'A', 0,     EFsEmailUiCmdActionsReplyAll },
        { 0,  'R', 0,     EFsEmailUiCmdActionsReply },
        { 0,  'F', 0,     EFsEmailUiCmdActionsForward }, 
        { 0,  'F', 0,     EFsEmailUiCmdActionsForward2 }, 
        { 0,  'M', 0,     EFsEmailUiCmdActionsMoveMessage },
        { 0,    'Y', 0,     EFsEmailUiCmdActionsCopyMessage },
        { 0,    'D', 0,     EFsEmailUiCmdActionsDelete },
        { 0, EStdKeyBackspace, 0, EFsEmailUiCmdActionsDelete },
        { 0,  'T', 0,     EFsEmailUiCmdGoToTop },
        { 0,  'B', 0,     EFsEmailUiCmdGoToBottom },
        { 0,    'J', 0,     EFsEmailUiCmdPageUp },
        { 0,    'K', 0,     EFsEmailUiCmdPageDown },
        { 0,  'N', 0,     EFsEmailUiCmdNextMessage },
        { 0,  'P', 0,     EFsEmailUiCmdPreviousMessage },
        { 0,    'L', 0,     EFsEmailUiCmdActionsFlag },
        { 0,  'C', 0,     EFsEmailUiCmdCompose },
        { 0,    'H', 0,     EFsEmailUiCmdHelp },
        
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsReadUnreadToggle },
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsRead },   
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsUnread }
    };

// Shortcuts for the HTML viewer
const CFSEmailUiShortcutBinding::TShortcutBinding KHtmlViewerShortcuts[] = 
    {
        { 0,  'A', 0,     EFsEmailUiCmdActionsReplyAll },
        { 0,  'R', 0,     EFsEmailUiCmdActionsReply },
        { 0,  'F', 0,     EFsEmailUiCmdActionsForward }, 
        { 0,    'D', 0,     EFsEmailUiCmdActionsDelete },
        { 0, EStdKeyBackspace, 0, EFsEmailUiCmdActionsDelete },
        { 0,    'H', 0,     EFsEmailUiCmdHelp },
        { 0,  '+', 0,     EFsEmailUiCmdZoomIn },
        { 0,  '-', 0,     EFsEmailUiCmdZoomOut },
        { 0,  'N', 0,     EFsEmailUiCmdNextMessage },
        { 0,  'P', 0,     EFsEmailUiCmdPreviousMessage },
        { 0,  EStdKeyLeftArrow, EModifierShift,     EFsEmailUiCmdPreviousMessage },
        { 0,  EStdKeyRightArrow, EModifierShift,     EFsEmailUiCmdNextMessage }
    };

// Shortcuts for the search results view    
const CFSEmailUiShortcutBinding::TShortcutBinding KSearchResultsShortcuts[] = 
    {
        { 0,    'O', 0,     EFsEmailUiCmdOpen },
        { 0,    'S', 0,     EFsEmailUiCmdSearch },
        { 0,    'S', 0,     EFsEmailUiCmdNewSearch },
        { 0,    'H', 0,     EFsEmailUiCmdHelp },
        { 0,  'T', 0,     EFsEmailUiCmdGoToTop },
        { 0,  'B', 0,     EFsEmailUiCmdGoToBottom },
        { 0,    'J', 0,     EFsEmailUiCmdPageUp },
        { 0,    'K', 0,     EFsEmailUiCmdPageDown },
        { 0,  'A', 0,     EFsEmailUiCmdActionsReplyAll },
        { 0,  'R', 0,     EFsEmailUiCmdActionsReply },
        { 0,  'F', 0,     EFsEmailUiCmdActionsForward }, 
        { 0,    'D', 0,     EFsEmailUiCmdActionsDelete },
        { 0, EStdKeyBackspace, 0, EFsEmailUiCmdActionsDelete },
        { 0,    'E', 0,     EFsEmailUiCmdCalActionsAccept }, 
        { 0,    'G', 0,     EFsEmailUiCmdCalActionsTentative },
        { 0,    'V', 0,     EFsEmailUiCmdCalActionsDecline },        
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsReadUnreadToggle },
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsRead },   
        { 0,  'U', 0,     EFsEmailUiCmdMarkAsUnread }
    };

// Shortcuts for generic list views. These are used with Mail details, attachment list, etc.    
const CFSEmailUiShortcutBinding::TShortcutBinding KGenericListShortcuts[] = 
    {
        { 0,    'C', EModifierCtrl, EFsEmailUiCmdActionsCopyToClipboard },

        { 0,    'O', 0,     EFsEmailUiCmdOpen },
        { 0,    'D', 0,     EFsEmailUiCmdActionsDelete },
        { 0, EStdKeyBackspace, 0, EFsEmailUiCmdActionsDelete },
        { 0,  'T', 0,     EFsEmailUiCmdGoToTop },
        { 0,  'B', 0,     EFsEmailUiCmdGoToBottom },
        { 0,    'J', 0,     EFsEmailUiCmdPageUp },
        { 0,    'K', 0,     EFsEmailUiCmdPageDown },
        { 0,    'H', 0,     EFsEmailUiCmdHelp },

        { 0,    'I', 0,     EFsEmailUiCmdActionsCollapseExpandAllToggle },
        { 0,    'I', 0,     EFsEmailUiCmdActionsCollapseAll },
        { 0,    'I', 0,     EFsEmailUiCmdActionsExpandAll }
    };

// Shortcuts for the main grid
const CFSEmailUiShortcutBinding::TShortcutBinding KMainGridShortcuts[] = 
    {
        { 0,    'O', 0,     EFsEmailUiCmdOpen },
        { 0,  'Z', 0,     EFsEmailUiCmdSync },
        { 0,  'Z', 0,     EFsEmailUiCmdSyncAll },
        { 0,    'H', 0,     EFsEmailUiCmdHelp },
    };

// Shortcuts for send attachments list view
const CFSEmailUiShortcutBinding::TShortcutBinding KSendAttachmentListShortcuts[] =
    {
        { 0,    'O', 0,     ESendAttachmentMenuOpen },
        { 0,    'D', 0,     ESendAttachmentMenuRemoveAttachment },
        { 0, EStdKeyBackspace, 0, ESendAttachmentMenuRemoveAttachment },
        { 0,  'T', 0,     EFsEmailUiCmdGoToTop },
        { 0,  'B', 0,     EFsEmailUiCmdGoToBottom },
        { 0,    'J', 0,     EFsEmailUiCmdPageUp },
        { 0,    'K', 0,     EFsEmailUiCmdPageDown },
        { 0,    'H', 0,     ESendAttachmentMenuHelp },
        { 0,    'I', 0,     EFsEmailUiCmdActionsCollapseExpandAllToggle }
    };
    
// Shortcuts for settings and global settings views
const CFSEmailUiShortcutBinding::TShortcutBinding KSettingsShortcuts[] =
    {
        { 0,    'O', 0,     EFsEmailUiCmdOpen },
        { 0,  'T', 0,     EFsEmailUiCmdGoToTop },
        { 0,  'B', 0,     EFsEmailUiCmdGoToBottom },
        { 0,    'J', 0,     EFsEmailUiCmdPageUp },
        { 0,    'K', 0,     EFsEmailUiCmdPageDown },
        { 0,    'H', 0,     EFsEmailUiCmdHelp }
    };

///////////////////////
// The class definition 
CFSEmailUiShortcutBinding* CFSEmailUiShortcutBinding::NewL()
    {
    FUNC_LOG;
    CFSEmailUiShortcutBinding* self = new (ELeave) CFSEmailUiShortcutBinding();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------    
CFSEmailUiShortcutBinding::~CFSEmailUiShortcutBinding()
    {
    FUNC_LOG;
    delete iPtiEngine;
    iQwertyMappings = NULL; // owned by iPtiEngine
    iHalfQwertyMappings = NULL; // ownned by iPtiEngine
    iKbLayoutProperty.Close();
    }
        
        
// ----------------------------------------------------------------------------    
TInt CFSEmailUiShortcutBinding::CommandForShortcutKey( const TKeyEvent& aKey, 
                                                       TShortcutContext aContext ) const
    {
    FUNC_LOG;
    TText shortcut = aKey.iScanCode;
    TBuf<1> keyData; // we are only interested about the first char of the key
    
    TKeyBoardType kbType = KeyBoardType();

    if ( kbType == EQwerty4x12 || kbType == EQwerty4x10 || 
         kbType == EQwerty3x11 || kbType == ECustomQwerty )
        {
        // One of the QWERTY modes
        // Map the key to the corresponding character on the current keyboard
        // If Fn is held depressed, use the Fn mappings, otherwise use upper case mappings.
        TPtiTextCase caseMode = EPtiCaseUpper;
#ifdef RD_INTELLIGENT_TEXT_INPUT        
        if ( aKey.iModifiers&EModifierRightFunc )
            {
            caseMode = EPtiCaseFnLower;
            }
#endif // RD_INTELLIGENT_TEXT_INPUT
        if ( iQwertyMappings )
            {
            iQwertyMappings->GetDataForKey( static_cast<TPtiKey>(aKey.iScanCode), 
                    keyData, caseMode );
            }
        }
    else if ( kbType == EHalfQwerty )
        {
        // Half-QWERTY mode
        // Map the key to the corresponding character in the Fn mode
        if ( iHalfQwertyMappings )
            {
#ifdef RD_INTELLIGENT_TEXT_INPUT        
            iHalfQwertyMappings->GetDataForKey( static_cast<TPtiKey>(aKey.iScanCode), 
                    keyData, EPtiCaseFnLower );
#endif // RD_INTELLIGENT_TEXT_INPUT
            }
        }
    else
        {
        // ITU-T mode, use scancode directly without mapping
        }
    
    // Substitute the scan code with the mapped character if available
    if ( keyData.Length() )
        {
        shortcut = keyData[0];
        }
    // Map hash and asterisk keys separately if they have not been mapped in PtiEngine
    else if ( shortcut == EStdKeyHash )
        {
        shortcut = '#';
        }
    else if ( shortcut == EStdKeyNkpAsterisk )
        {
        shortcut = '*';
        }

    return CommandForShortcut( shortcut, aContext, aKey.iModifiers );
    }
        
        
// ----------------------------------------------------------------------------    
TInt CFSEmailUiShortcutBinding::CommandForShortcut( TText aShortcut, 
                                                    TShortcutContext aContext,
                                                    TUint32 aModifiers )
    {
    FUNC_LOG;
    TInt command = KErrNotFound;
    
    if ( aShortcut ) // value 0 is used as mark for no shortcut
        {
        TInt count = 0;
        const TShortcutBinding* bindings = NULL;

        // Select appropriate binding table    
        GetBindingsForContext( bindings, count, aContext );
            
        // Search command from the binding table
        for ( TInt i=0 ; i<count && command==KErrNotFound  ; ++i )
            {
            TBool isItutShortcut = ( aShortcut == bindings[i].iItutShortcut );
            
            TBool isQwertyShortcut = ( aShortcut == bindings[i].iQwertyShortcut );
            // Check the modifier keys for the QWERTY shortcut
            if ( isQwertyShortcut )
                {
                TUint32 matchingModifiers = ( aModifiers & bindings[i].iQwertyModifiers );
                if ( matchingModifiers != bindings[i].iQwertyModifiers )
                    {
                    isQwertyShortcut = EFalse;
                    }
                }
                                       
            if ( isItutShortcut || isQwertyShortcut )
                {
                command = bindings[i].iCommandId;
                }
            }
        }
        
    return command;
    }
    
    
// ----------------------------------------------------------------------------    
TText CFSEmailUiShortcutBinding::ShortcutForCommand( TInt aCommandId, 
                                                     TShortcutContext aContext, 
                                                     TBool aQwertyMode )
    {
    FUNC_LOG;
    TText shortcut = 0;
    TInt count = 0;
    const TShortcutBinding* bindings = NULL;
    
    // Select appropriate binding table
    GetBindingsForContext( bindings, count, aContext );
        
    // Search shortcut from the binding table
    for ( TInt i=0 ; i<count && !shortcut  ; ++i )
        {
        if ( aCommandId == bindings[i].iCommandId )
            {
            if ( aQwertyMode )
                {
                // Only single-key shortcuts are supported here.
                if ( !bindings[i].iQwertyModifiers )
                    {
                    shortcut = bindings[i].iQwertyShortcut;
                    }
                }
            else
                {
                shortcut = bindings[i].iItutShortcut;
                }
            }
        }
        
    return shortcut;
    }
    
    
// ----------------------------------------------------------------------------    
void CFSEmailUiShortcutBinding::AppendShortcutHintsL(
	CEikMenuPane& aMenuPane,
	TShortcutContext aContext ) const
    {
    FUNC_LOG;
    TKeyBoardType kbType = KeyBoardType();
    TBool isQwerty = 
        ( kbType != ENoKeyboard && kbType != EItutKeyPad && kbType != EHalfQwerty );
    
    // Browse through the commands in the menu pane and append shortcuts
    // where available.
    TInt count = aMenuPane.NumberOfItemsInPane();
    
    for ( TInt i = 0; i < count; ++i )
        {
        CEikMenuPaneItem::SData& itemData = aMenuPane.ItemDataByIndexL( i );
        TText hint = ShortcutForCommand(
        	itemData.iCommandId, aContext, isQwerty );        
        if ( hint )
            {
            AppendHintToCaption( itemData.iText, hint );
            }
        }
    }
    
    
// ----------------------------------------------------------------------------    
CFSEmailUiShortcutBinding::CFSEmailUiShortcutBinding()
    {
    FUNC_LOG;
    }
	
	
// ----------------------------------------------------------------------------    
void CFSEmailUiShortcutBinding::ConstructL()
    {
    FUNC_LOG;
    iPtiEngine = CPtiEngine::NewL();
    
    // Always use English mappings for QWERTY keyboards. This is because it is not 
    // certain that the mappings of the current language hold latin alphabet at all.
    // English should be available in all device variants as an optional input language.
    CPtiCoreLanguage* lang = 
        static_cast<CPtiCoreLanguage*>( iPtiEngine->GetLanguage( ELangEnglish ) );
    if (lang)
        {
        iQwertyMappings = static_cast<CPtiKeyMappings*>( lang->GetQwertyKeymappings() );
#ifdef RD_INTELLIGENT_TEXT_INPUT        
        iHalfQwertyMappings = static_cast<CPtiKeyMappings*>( lang->GetHalfQwertyKeymappings() );
#endif // RD_INTELLIGENT_TEXT_INPUT        
        }
    lang = NULL; // owned by iPtiEngine
    
    
    if ( !iQwertyMappings && !iHalfQwertyMappings )
        {
        // In non-QWERTY devices we can free up PtiEngine right away, because
        // it is not needed anyway.
        delete iPtiEngine;
        iPtiEngine = NULL;
        }

    iKbLayoutProperty.Attach( KCRUidAvkon, KAknKeyBoardLayout );
    }
	
	
// ----------------------------------------------------------------------------    
CFSEmailUiShortcutBinding::TKeyBoardType CFSEmailUiShortcutBinding::KeyBoardType() const
    {
    FUNC_LOG;
    TInt keyboardType = ENoKeyboard;

    // RProperty does not define Get() as a constant function. This is
    // probably just a flaw in the interface. Cast away constness to
    // circumvent this.
    RProperty& kbProperty = const_cast<RProperty&>(iKbLayoutProperty);
    kbProperty.Get( keyboardType );
    return static_cast<TKeyBoardType>( keyboardType );
    }

	
// ----------------------------------------------------------------------------    
void CFSEmailUiShortcutBinding::GetBindingsForContext( const TShortcutBinding*& aBindingArray, 
                                                       TInt& aBindingCount,
                                                       TShortcutContext aContext )
    {
    FUNC_LOG;
    switch (aContext)
        {
        case EContextMailList:
            {
            aBindingCount = sizeof(KMailListShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KMailListShortcuts;
            }
        break;

        case EContextSearchResults:
            {
            aBindingCount = sizeof(KSearchResultsShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KSearchResultsShortcuts;
            }
        break;

        case EContextMailViewer:
            {
            aBindingCount = sizeof(KMailViewerShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KMailViewerShortcuts;
            }
        break;

        case EContextHtmlViewer:
            {
            aBindingCount = sizeof(KHtmlViewerShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KHtmlViewerShortcuts;
            }
        break;
        
        case EContextMailDetails:
        case EContextFolderList:
        case EContextAttachmentList:
        case EContextDownloadManager:
            {
            aBindingCount = sizeof(KGenericListShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KGenericListShortcuts;
            }
        break;

        case EContextMainGrid:
            {
            aBindingCount = sizeof(KMainGridShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KMainGridShortcuts;
            }
        break;

        case EContextSendAttachmentList:
            {
            aBindingCount = sizeof(KSendAttachmentListShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KSendAttachmentListShortcuts;
            }
        break;
        
        case EContextSettings:
            {
            aBindingCount = sizeof(KSettingsShortcuts) / sizeof(TShortcutBinding);
            aBindingArray = KSettingsShortcuts;
            }
        break;
        
        default:
            {
            Panic( EFSEmailUiUnexpectedValue );
            }
        break;
        }
    }                                         


// ----------------------------------------------------------------------------    
void CFSEmailUiShortcutBinding::AppendHintToCaption( TDes& aCaption, 
                                                     TText aHint )
    {
    FUNC_LOG;
    static const TInt KHintLength = 4; // e.g. " [X]"
    _LIT( KLeadingSeparator, " [" );
    _LIT( KTailingSeparator, "]" );
    
    if ( aCaption.MaxLength() >= aCaption.Length() + KHintLength )
        {
        aCaption.Append( KLeadingSeparator );
        aCaption.Append( aHint );
        aCaption.Append( KTailingSeparator );
        }
    }

