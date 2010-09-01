/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ESMR viewer dialog
*
*/


#ifndef CESMRRESPONSEDIALOG_H
#define CESMRRESPONSEDIALOG_H

// INCLUDES
#include <AknDialog.h>

// FORWARD DECLARATIONS
class CAknsBasicBackgroundControlContext;
class CESMRResponseDialogView;
class TLogicalFont;

/**
 *  CESMRResponseDialog implements the response dialog for meeting requests.
 *
 *  @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRResponseDialog ) : public CAknDialog
    {
public:
    /**
     * Two-phased constructor.
     * @param aText reference pointer to the text that user writes as a response
     */
    IMPORT_C static CESMRResponseDialog* NewL( HBufC*& aText );

    /*
     * Destructor.
     */
    ~CESMRResponseDialog();

public:// From CAknDialog
    SEikControlInfo CreateCustomControlL( TInt aType );
    TBool OkToExitL (TInt aButtonId);
    void ProcessCommandL( TInt aCommand );

public:// From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    IMPORT_C TInt ExecuteDlgLD();

private://Implementation
    CESMRResponseDialog( HBufC*& aText );
    void ConstructL();

private: // data
    /**
    * Ref: View for dialog, deletion handed by framework
    */
    CESMRResponseDialogView* iView;

    /**
    * Ref: Pointer to buffer where the text is written
    */
    HBufC*& iText;
    /// To record if the toolbar in previous view is visible.
    TBool iPreviousVisibility;
    };

#endif // CESMRRESPONSEDIALOG_H
