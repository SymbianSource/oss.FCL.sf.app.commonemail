/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Library to handle resource loading when 
*              CCoeEnv is not available.
*
*/


#include <barsread.h>
#include <bautils.h>
#include "basepluginresourceloader.h"

EXPORT_C CResourceLoader* CResourceLoader::NewL( const TDesC& aName )
    {
    CResourceLoader* temp = new( ELeave ) CResourceLoader();
    CleanupStack::PushL( temp );
    temp->ConstructL( aName );
    CleanupStack::Pop( temp );
    return temp;
    }

CResourceLoader::CResourceLoader()
    {
    }

void CResourceLoader::ConstructL( const TDesC& aFilename )
    {
    User::LeaveIfError( iFs.Connect() );
    TFileName aFile(aFilename);
    BaflUtils::NearestLanguageFile( iFs, aFile );
    iResFile.OpenL( iFs, aFile );
    iResFile.ConfirmSignatureL();
    }

EXPORT_C CResourceLoader::~CResourceLoader()
    {
    iResFile.Close();
    iFs.Close();
    }

EXPORT_C RFs& CResourceLoader::Fs() 
    {
    return iFs;
    }

EXPORT_C HBufC* CResourceLoader::LoadLC(TInt aResourceId)
    {
    TResourceReader reader;
    HBufC8* readBuffer = CreateResourceReaderLC( reader, aResourceId );
    TPtrC textdata = reader.ReadTPtrC();
    
    HBufC16* textBuffer = HBufC16::NewL( textdata.Length() );
    *textBuffer = textdata;
    CleanupStack::PopAndDestroy(readBuffer);
    CleanupStack::PushL( textBuffer );
    return textBuffer;
    }

EXPORT_C HBufC8* CResourceLoader::CreateResourceReaderLC(TResourceReader& aReader,TInt aResourceId) const
    {
    HBufC8* readBuffer = iResFile.AllocReadLC( aResourceId );
    aReader.SetBuffer( readBuffer );
    return readBuffer;
    }
        
