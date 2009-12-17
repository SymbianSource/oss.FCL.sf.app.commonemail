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
* Description:  Resource headers for project NCS
*
*/



#ifndef FS_EMAIL_PAN
#define FS_EMAIL_PAN


// NCS application panic codes
enum TFsEmailUiPanics
    {
    ENcsBasicUi = 1,
    EFSEmailUiCLSNotFound,
    EFSEmailUiUnknownView,
    EFSEmailUiUnexpectedValue,
    EFSEmailUiNullPointerException,
    EFSEmailUiBaseViewMethodNotOverriden
    // add further panics here
    };

inline void Panic( TFsEmailUiPanics aReason )
    {
    _LIT( KApplicationName, "FsEmail UI" );
    User::Panic( KApplicationName, aReason );
    }

#endif // FS_EMAIL_PAN

// End of File