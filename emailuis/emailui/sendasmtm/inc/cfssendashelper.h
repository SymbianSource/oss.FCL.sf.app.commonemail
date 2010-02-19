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
* Description:  Definition of the class CFsSendAsHelper.
*
*/


#ifndef CFSSENDASHELPER_H
#define CFSSENDASHELPER_H

#include <e32base.h>
#include <apmstd.h>

#include <cmsvrecipientlist.h>
//<cmail>
#include "cfsmailcommon.h"
//</cmail>
// ---------------------------------------------------------------------------

class CFSMailAddress;

/**
 * This class is having functions which are used by Integration MTM to create
 * a message and to launch Freestyle UI editor.
 *
 * @since S60 3.1
 * @lib fssendashelper.lib
 */
class CFsSendAsHelper : public CBase
{
public: // constructors and destructor
    /**
     * Constructs new object.
     * @return created object.
    */
    IMPORT_C static CFsSendAsHelper* NewL();

    /** 
     * Destructor
    */
    IMPORT_C ~CFsSendAsHelper();

public:    
    /**
     * Called from MTM when a file is selected to through sendas functionality.
     * The file full path and mimetype are stored which will be later used to
     * create mail and launch FS UI.
     * @param aFileName Filename including full path
     * @param aMimeType Mime type of the file that need to be attached
     * @param aCharSet Charcaterset of the file
    */
    IMPORT_C void AddAttachmentL(const TDesC& aFileName,
        const TDesC8& aMimeType, TUint aCharSet);    
    /**
     * Called from MTM when a file is selected to through sendas functionality.
     * The file full path and mimetype are stored which will be later used to
     * create mail and launch FS UI.
     * @param aFile Handle to an attachment file. Ownership is transferred
     * @param aMimeType Mime type of the file that need to be attached
     * @param aCharSet Charcaterset of the file
    */
    IMPORT_C void AddAttachmentL( RFile& aFile,
        const TDesC8& aMimeType, TUint aCharSet);    
   
    /**
     * Called from MTM when a address is selected to send an e-mail.
     * The Address and Alias of the address if present are stored 
     * which will be later used to create mail and launch FS UI
     * @param aAddress Mail address of the recipient
     * @param aAlias Alias of the address, this can be NULL
     * @param aType Type of recipient.
    */
    IMPORT_C void AddRecipientL(const TDesC& aAddress, const TDesC& aAlias, 
        TMsvRecipientType aType);
    
    /**
     * Called from MTM when a subject need to be set for the mail. The subject
     * will be stored which will be later used to create mail and launch FS UI.
     * @param aSubject Subject of the mail.
    */
    IMPORT_C void SetSubjectL(const TDesC& aSubject);

    /**
     * Called from MTM when certain text need to be made as body of the mail.
     * Usually the case when notepad file is selected to send using sendas
     * functionality. The contents of body will be stored which will be later 
     * used to create mail while launching FS UI.
     * @param aBody body part of the mail.
    */
    IMPORT_C void SetBodyL(const TDesC& aBody);

    /**
     * Called from MTM after either of setting subject, recipients,  
     * attachments or/and body. The message will be created with the right
     * data and will be stored to drafts of mail box. The messageid of the 
     * createdmessage will be externalized which is used later to launch UI.
     *
     * @param aPluginId Plug-in Id
     * @param aId Mailbox Id. 
    */
    IMPORT_C void StoreDataL( TUint aPluginId, TUint aId );

    /**
     * Called from MTM either to create empty mail or to create mail with some
     * contents.
     * @param aPluginId Plug-in Id
     * @param aId Mailbox Id.
     * @param aIsEmptyMail True if empty message need to be created.
     * @param aIsSettings True if settings view need to be launched.
    */
    IMPORT_C void LaunchUiL(TUint aPluginId, TUint aId, 
        TBool aIsEmptyMail = EFalse, TBool aIsSettings = EFalse );

private: // constructors
    /**
     * Constructor
     */
    CFsSendAsHelper();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /*
     * Displays mailbox not available note.
     */
    void DisplayMailboxNotAvailableNoteL();

    /*
     * Checks whether the mailbox is available or not
     * @param aPluginId Plugin ID or the mailbox. 
     * @return KErrNone if mailbox available, KErrNotSupported otherwise.
     */
    TInt IsMailboxAvailable( TInt aPluginId );
private: // data
    // Subject of the message
    HBufC* iSubject;

    // Body of the message    
    HBufC* iBody;

    // Filehandles that need to be attached.
    RArray< RFile > iFiles;
    
    // Filenames that need to be attached
    RPointerArray<HBufC> iFileNames;

    // Mimetypes of the attached filehandles
    RPointerArray<HBufC8> iMimeTypesFile;
    // Mimetypes of the attached filenames
    RPointerArray<HBufC8> iMimeTypesFileName;

    // To recipients of the mail
    RPointerArray<CFSMailAddress> iToAddresses;
    // Cc recipients of the mail
    RPointerArray<CFSMailAddress> iCcAddresses;  
    // Bcc recipients of the mail
    RPointerArray<CFSMailAddress> iBccAddresses;

    //If Filehandles are passed to the framework (plugin)
    TBool isPassed;
    
    // Message to be displayed when trying to use MfE or Ozone mailbox
    // while in mass storage mode. Own.
    HBufC* iMassStorageNote;
};
#endif  //CFSSENDASHELPER_H
