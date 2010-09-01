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
* Description:  Defines class, which controls the note behavior.
*
*/


#ifndef IPSSETUINOTES_H
#define IPSSETUINOTES_H


#include <AknQueryDialog.h>

#include "ipssetutilsconsts.h"

/**
 * Supported note types.
 */
enum TIpsSetUiNotes
    {
    EIpsSetUiConfirmationNote = 0,
    EIpsSetUiInformationNote,
    EIpsSetUiErrorNote,
    EIpsSetUiWarningNote
    };

class CIpsSetUi;
class CIpsSetUiItem;

/**
 *  Controller for notes in settings ui.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiNotes : public CBase
    {
public:  // Constructors and destructor

    /**
     * Creates object from CIpsSetUiNotes and leaves it to cleanup stack
     * @return, Constructed object
     */
    static CIpsSetUiNotes* NewLC();

    /**
     * Create object from CIpsSetUiNotes
     * @return, Constructed object
     */
    static CIpsSetUiNotes* NewL();

    /**
     * Destructor
     */
    virtual ~CIpsSetUiNotes();

public: // New functions

    /**
     * Shows the query for user and waits for user answer.
     *
     * @param aTextResource Resource string, that shall be shown in note
     * @param aDialogResource ResourceId of dialog, which shall be used
     *        Locate the resource dialogs from emailutils.ra
     * @param aTone Tone to be played with query
     *        Class CAknQueryDialog defines the TTone type, which contains
     *        following enumerations:
     *           - ENoTone
     *           - EConfirmationTone
     *           - EWarningTone
     *           - EErrorTone
     *        Example usage: CAknQueryDialog::EConfirmationTone
     * @return Command returned from the query
     */
    static TInt ShowQueryL(
        const TUint aTextResource,
        const TInt  aDialogResource,
        const CAknQueryDialog::TTone aTone = CAknQueryDialog::ENoTone );

    static TInt ShowQueryL(
        const TDesC& aText,
        const TInt  aDialogResource,
        const CAknQueryDialog::TTone aTone = CAknQueryDialog::ENoTone );

    /**
     * This function fetches the text from given resource file and shows
     * the string found from it in small note box.
     * More info in file aknnotewrappers.h
     *
     * @param aNoteType Type of note to be shown
     *        Avkon offers several type of notes, which differs from each
     *        other. There are following types of notes available:
     *           - EMsvConfirmationNote
     *           - EMsvInformationNote
     *           - EMsvErrorNote
     *           - EMsvWarningNote
     * @param aNoteWaiting Parameter to create the note
     *        When parameter is TRUE, created note will be waitnote
     *        When parameter is FALSE, note is not waitnote
     * @param aResource Resource string, that shall be shown in note
     * @param aFile File, from the string shall be read
     */
    static void ShowNoteL(
        const TUint      aResource,
        const TIpsSetUiNotes& aNoteType = EIpsSetUiErrorNote,
        const TBool      aNoteWaiting = EFalse );

    static void ShowNoteL(
        const TDesC&      aText,
        const TIpsSetUiNotes& aNoteType = EIpsSetUiErrorNote,
        const TBool      aNoteWaiting = EFalse );

    /**
     * Creates a string from resource using string loader.
     *
     * @param aTextResource Resource which contains the text.
     * @return String allocated in heap. Caller owns.
     */
    static HBufC* MakeStringLC( const TUint aTextResource );

    /**
     * Creates string from resource and includes the value in to the text.
     *
     * @param aTextResource Resource which contains the text.
     * @param aValue which contains the number.
     * @return Formatted string.
     */
    static TIpsSetUtilsTextPlain MakeString(
        const TUint aTextResource,
        const TInt aValue );

    /**
     * Creates string from resource and includes the string in to the text.
     *
     * @param aTextResource Resource which contains the text.
     * @param aText Text to be included in the string.
     * @return Formatted string.
     */
    static TIpsSetUtilsTextPlain MakeString(
        const TUint aTextResource,
        const TDesC& aText );

    /**
     * Shows the dialog to user.
     *
     * @param aBaseItem Item being edited.
     * @param aError Error identified from the user input.
     * @param Text that should be shown.
     * @return User input to dialog.
     */
    TIpsSetUiEventResult ShowDialog(
        const CIpsSetUiItem& aBaseItem,
        const TIpsSetUiNoteErrors aError,
        const TDesC& aNewText );

protected:  // Constructors

    /**
     * Default constructor for classCIpsSetUiNotes
     *
     * @return, Constructed object
     */
    CIpsSetUiNotes();

    /**
     * Symbian 2-phase constructor
     */
    void ConstructL();

private:  // New functions

    /**
     * Displays email address error note on screen.
     *
     * @param aError Type of error.
     * @return User action.
     */
    TIpsSetUiEventResult ShowEmailAddressErrorNoteL(
        const TIpsSetUiNoteErrors aError );

    /**
     * Displays server address error note on screen.
     *
     * @param aError Type of error.
     * @return User action.
     */
    TIpsSetUiEventResult ShowServerErrorNoteL(
        const TIpsSetUiNoteErrors aError );

    };

#endif /* IPSSETUINOTES_H */
