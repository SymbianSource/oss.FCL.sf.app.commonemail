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
 * Description:  MR attahcment field implementation
 *
 */
#include "cmrattachmentcommand.h"
#include <apgcli.h>
#include <f32file.h>

// DEBUG
#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRAttachmentCommand::CMRAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRAttachmentCommand::CMRAttachmentCommand()
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CMRAttachmentCommand::CMRAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRAttachmentCommand::~CMRAttachmentCommand()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRAttachmentCommand::GetMimeTypeL
// -----------------------------------------------------------------------------
//
TDataType CMRAttachmentCommand::GetMimeType( RFile& aFile )
    {
    FUNC_LOG;
    TDataType dataType;
    RApaLsSession aApaSession;
    if( aFile.SubSessionHandle() )
        {
        TInt error(0);

        if( !aApaSession.Handle() )
            {
            error = aApaSession.Connect();
            }
        if( error == KErrNone )
            {
            TDataRecognitionResult recognitionResult;
            error = aApaSession.RecognizeData( aFile, recognitionResult );
            if( recognitionResult.iConfidence !=
                CApaDataRecognizerType::ENotRecognized &&
                    error == KErrNone )
                {
                dataType = recognitionResult.iDataType;
                }
            }
        }

    aApaSession.Close();
    return dataType;
    }

// EOF
