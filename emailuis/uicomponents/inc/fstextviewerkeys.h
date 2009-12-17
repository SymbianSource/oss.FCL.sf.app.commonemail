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
* Description:  Custom key definition
*
*/


#ifndef C_FSTEXTVIEWERKEYS_H
#define C_FSTEXTVIEWERKEYS_H

#include <e32base.h>

class CFsTextViewerKeys : public CBase
    {
    public:
        IMPORT_C static CFsTextViewerKeys* NewL ();
        IMPORT_C ~CFsTextViewerKeys();

        IMPORT_C void SetKeyUp( TInt aKey );
        IMPORT_C void SetKeyDown( TInt aKey );
        IMPORT_C void SetKeyScrollUp( TInt aKey );
        IMPORT_C void SetKeyScrollDown( TInt aKey );
        IMPORT_C void SetKeyPgUp( TInt aKey );
        IMPORT_C void SetKeyPgDown( TInt aKey );
        IMPORT_C void SetKeyRight( TInt aKey );
        IMPORT_C void SetKeyLeft( TInt aKey );
        IMPORT_C void SetKeyMark( TInt aKey );
        IMPORT_C void SetKeyClick( TInt aKey );

        IMPORT_C TInt GetKeyUp();
        IMPORT_C TInt GetKeyDown();
        IMPORT_C TInt GetKeyScrollUp();
        IMPORT_C TInt GetKeyScrollDown();
        IMPORT_C TInt GetKeyPgUp();
        IMPORT_C TInt GetKeyPgDown();
        IMPORT_C TInt GetKeyRight();
        IMPORT_C TInt GetKeyLeft();
        IMPORT_C TInt GetKeyMark();
        IMPORT_C TInt GetKeyClick();

    public:

    private:
        CFsTextViewerKeys();
        void ConstructL();

    private:
        TInt iKeyUp;
        TInt iKeyDown;
        TInt iKeyScrollUp;
        TInt iKeyScrollDown;
        TInt iKeyPgUp;
        TInt iKeyPgDown;
        TInt iKeyRight;
        TInt iKeyLeft;
        TInt iKeyMark;
        TInt iKeyClick;
    };

#endif //C_FSTEXTVIEWERKEYS_H
