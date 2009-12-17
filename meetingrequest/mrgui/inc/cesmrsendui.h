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
* Description:  ESMR send ui wrapper
*
*/


#ifndef CESMRSENDUI_H
#define CESMRSENDUI_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;
class CEikMenuPane;
class CSendUi;
class TParse;
class RFile;
class CCalenGlobalData;

/**
* SendUI wrapper class. Used to send entry information forward. CCalEntry
* is converted to VCalendar format before sending.
*/
NONSHARABLE_CLASS( CESMRSendUI ) : public CBase
    {
public: // Factory method and destructor
    IMPORT_C static CESMRSendUI* NewL(TInt aMenuCommandId);
    IMPORT_C virtual ~CESMRSendUI();

public: // API
    /**
     * Can entry be sent or not.
     *
     * @param aCommandId - NOT IN USE
     * @return boolean ETrue/Efalse
     */
    IMPORT_C TBool CanSendL(TInt aCommandId);

    /**
     * Adds the send ui menu items into menu pane.
     *
     * @param aMenuPane - menu pane where items are added
     * @param aIndex - index of the menu item
     */
    IMPORT_C void DisplaySendMenuItemL(CEikMenuPane& aMenuPane, TInt aIndex);

    /**
     * Displays the send ui sub menu
     *
     * @param aMenuPane - NOT IN USE
     */
    IMPORT_C void DisplaySendCascadeMenuL(CEikMenuPane& aMenuPane);

    /**
     * Sends the converted entry via selected protocol
     *
     * @param aCommand - information about how entry needs to be sent
     * @param aEntry - entry to be sent.
     */
    IMPORT_C void SendAsVCalendarL(TInt aCommand, CCalEntry& aEntry);

private: // utility functions
    void DoSendAsAttachmentFileL(TInt aCommand, TParse& aFilename);

    void DoSendAsAttachmentHandleL(const RFile& aHandle);

private: // constrution
    CESMRSendUI();
    void ConstructL(TInt aMenuCommandId);
private: // data
	/// Own:
    CSendUi* iSendUi;
    /// Own:
    TInt iSendAsCmdId;
    /// Own:
    TUid iSelectedMtmUid;
    /// Own:
    CArrayFixFlat<TUid>* iSendMtmsToDim;
    // Own: calendar session
    CCalSession* iSession;
    };

#endif // CESMRSENDUI_H


// End of File
