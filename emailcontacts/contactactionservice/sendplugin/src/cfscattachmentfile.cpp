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
* Description: This file implements classes CFscAttachmentFile, CFscAttachmentFileArray. 
*
*/

// INCLUDES
#include "emailtrace.h"
#include  <e32std.h>

#include "cfscattachmentfile.h"

/// Unnamed namespace for local definitions
namespace {

    // LOCAL CONSTANTS
    _LIT(KRamDrive, "d:");
    _LIT(KPersistentDrive, "c:");
    _LIT(KTempFilePath, "\\system\\temp\\");
    _LIT(KInvalidFileNameChars, "?*<>/\"|\\:");
    _LIT(KDefaultTempFileName, "TEMP.tmp");
    const TInt KMaxDriveNameLength = 2;
    const TInt KMaxNumberLength = 5;
    const TInt KMaxFileNumber = 99999;

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPostCond_Constructor = 1,
        EPanicPreCond_ConstructL,
        EPanicPostCond_ConstructL,
        EPanicPreCond_CreateFileL,
        EPanicPostCond_CreateFileL
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CFscAttachmentFile");
        User::Panic(KPanicText,aReason);
        }
#endif // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

CFscAttachmentFile* CFscAttachmentFile::NewL(
    const TDesC& aBaseName, 
    RFs& aRfs, 
    TUint aFileMode )
    {
    FUNC_LOG;
    CFscAttachmentFile* self = 
        CFscAttachmentFile::NewLC(aBaseName, aRfs, aFileMode);
    CleanupStack::Pop( self );
    return self;
    }

CFscAttachmentFile* CFscAttachmentFile::NewLC( 
    const TDesC& aBaseName, 
    RFs& aRfs, 
    TUint aFileMode )
    {
    FUNC_LOG;
    CFscAttachmentFile* self = new(ELeave) CFscAttachmentFile;
    CleanupStack::PushL(self);
    self->iRfs = aRfs;
    self->iFileMode = aFileMode;
    self->ConstructL(aBaseName);
    return self;
    }

CFscAttachmentFile::~CFscAttachmentFile()
    {
    FUNC_LOG;
    DeleteFile();
    delete iFileName;
    delete iBaseName;
    }

const TDesC& CFscAttachmentFile::FileName() const
    {
    FUNC_LOG;
    if (iFileName)
        {
        return *iFileName;
        }
    else
        {
        return KNullDesC;
        }
    }

RFile& CFscAttachmentFile::File()
    {
    FUNC_LOG;
    return iFile;
    }

void CFscAttachmentFile::Release()
    {
    FUNC_LOG;
    iFile.Close();
    iOwnsFile = EFalse;
    }

void CFscAttachmentFile::SwitchDriveL()
    {
    FUNC_LOG;
    TPtrC drive;
    if (iFileName)
        {
        // File already created successfully: switch the drive
        TParsePtrC fileNameParser(*iFileName);
        drive.Set(fileNameParser.Drive()==KRamDrive ? KPersistentDrive : KRamDrive);
        }
    else
        {
        // Default is RAM drive in ConstructL
        drive.Set(KPersistentDrive);
        }

    // Create new file
    CreateFileL(drive, KTempFilePath, *iBaseName);
    }

CFscAttachmentFile::CFscAttachmentFile()
    {
    FUNC_LOG;
    // CBase::operator new will reset members
    __ASSERT_DEBUG(!iBaseName && !iFileName && iFile.SubSessionHandle()==KNullHandle, 
        Panic(EPanicPostCond_Constructor));
    }

void CFscAttachmentFile::ConstructL(const TDesC& aBaseName)
    {
    FUNC_LOG;
    __ASSERT_DEBUG(!iBaseName && !iFileName && iFile.SubSessionHandle()==KNullHandle, 
        Panic(EPanicPreCond_ConstructL));

    // Clean up aBaseName
    const TInt maxLength = KMaxFileName - KMaxDriveNameLength - KTempFilePath().Length() - KMaxNumberLength;
    iBaseName = HBufC::NewL(maxLength);
    TPtr baseName = iBaseName->Des();
    for (TInt i=0; i < aBaseName.Length() && i < maxLength; ++i)
        {
        TChar ch = aBaseName[i];
        if (KInvalidFileNameChars().Locate(ch) == KErrNotFound)
            {
            baseName.Append(ch);
            }
        }
    baseName.TrimAll();
    if (baseName.Length() == 0)
        {
        baseName = KDefaultTempFileName;
        }

    // Try to create the file
    TRAPD(err, CreateFileL(KRamDrive, KTempFilePath, baseName));
    if (err != KErrNone)
        {
        // Try on different drive
        SwitchDriveL();
        }

    __ASSERT_DEBUG(iBaseName && iFileName, Panic(EPanicPostCond_ConstructL));
    }

void CFscAttachmentFile::CreateFileL
        (const TDesC& aDrive, const TDesC& aDir, const TDesC& aBaseName)
    {
    FUNC_LOG;
    __ASSERT_DEBUG(iBaseName && iRfs.Handle() != KNullHandle, 
        Panic( EPanicPreCond_CreateFileL ));

    // Create and init a local buffer for the file name
    HBufC* fileNameBuf = HBufC::NewLC( KMaxFileName );
    TPtr fileName = fileNameBuf->Des();
    fileName = aDrive;
    fileName.Append( aDir );
    // Create directory
    TInt error = iRfs.MkDirAll( fileName );
    if ( error != KErrNone && error != KErrAlreadyExists )
        {
        User::Leave( error );
        }
    fileName.Append( aBaseName );

    TInt number = 0;
    FOREVER
        {
        RFile file;
        const TInt error = file.Create( iRfs, fileName, iFileMode );
        
        TParsePtrC parsePtr( fileName );
                
        if ( error == KErrNone && parsePtr.Name().Length() > 0 )
            {
            // File created succesfully: switch state
            DeleteFile();  // Delete previous file
            iFile = file;
            delete iFileName;
            iFileName = fileNameBuf;
            iOwnsFile = ETrue;
            CleanupStack::Pop();  // fileNameBuf
            __ASSERT_DEBUG( iFileName && iFile.SubSessionHandle() != 
                            KNullHandle, 
                            Panic( EPanicPostCond_CreateFileL ) );
            return;
            }
        else if ( error != KErrAlreadyExists && error != KErrNone )
            {
            User::Leave( error );
            }

        // File name was reserved: append number to basename to make it unique
        fileName = aDrive;
        fileName.Append( aDir );
        TParsePtrC baseNameParser( aBaseName );
        fileName.Append( baseNameParser.Name() );
        fileName.AppendNum( number );
        fileName.Append( baseNameParser.Ext() );
        if ( ++number > KMaxFileNumber )
            {
            User::Leave( KErrTooBig );
            }
        }
    }

void CFscAttachmentFile::DeleteFile()
    {
    FUNC_LOG;
    iFile.Close();
    if ( iFileName && iOwnsFile )
        {
        // Delete the file. If the deletion fails for some reason trust that the
        // system will delete the file when it cleans up temp file directories.
        iRfs.Delete( *iFileName );
        }
    }

CFscAttachmentFileArray::CFscAttachmentFileArray(TInt aGranularity)
    : CArrayPtrFlat<CFscAttachmentFile>(aGranularity)
    {
    FUNC_LOG;
    }

CFscAttachmentFileArray::~CFscAttachmentFileArray()
    {
    FUNC_LOG;
    // Delete all objects in the array
    ResetAndDestroy();
    }

TInt CFscAttachmentFileArray::MdcaCount() const
    {
    FUNC_LOG;
    return Count();
    }

TPtrC CFscAttachmentFileArray::MdcaPoint(TInt aIndex) const
    {
    FUNC_LOG;
    return At(aIndex)->FileName();
    }

// End of File

