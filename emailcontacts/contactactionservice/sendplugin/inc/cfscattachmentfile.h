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
* Description: This file defines classes CFscAttachmentFile, CFscAttachmentFileArray
*
*/


#ifndef _CFSCATTACHEMTFILE_H
#define _CFSCATTACHEMTFILE_H

// INCLUDES
#include <f32file.h>     // File modes
#include <bamdesca.h>    // MDesCArray

//  CLASS DECLARATION 

/**
 *  CFscAttachmentFile.
 *   Class which creates a temporary file at construction and deletes the
 *   file when destroyed.
 *
 *  @since S60 3.1
 */
class CFscAttachmentFile : public CBase
	{

public: // Interface
    /// Drive enumeration
    enum TTempDrive
        {
        /// RAM disk. Contents lost at power-down
        ERamDisk = 1,
        /// Persistent file system
        EPersistent
        };

    /**
     * Creates new attachment file.
     *
     * @param aBaseName basename used to build a unique filename. Used
     *                  as-is if the filename is not in use.
     * @param aRfs      open file server session handle.
     * @param aFileMode mode in which to create the file, see RFile::Create.
     */
    static CFscAttachmentFile* NewL(
        const TDesC& aBaseName, 
        RFs& aRfs,
        TUint aFileMode = EFileStream | EFileShareExclusive | EFileWrite );

    /**
     * Like NewL but leaves the created object on the cleanup stack.
     * @see CFscAttachmentFile::NewL
     */
    static CFscAttachmentFile* NewLC( 
        const TDesC& aBaseName, 
        RFs& aRfs,
        TUint aFileMode = EFileStream | EFileShareExclusive | EFileWrite );

    /**
     * Destructor. Closes and deletes the file from the file system.
     * If Release() has been called the file is not deleted.
     */
    ~CFscAttachmentFile();

    /**
     * Returns the full file name.
     */
    const TDesC& FileName() const;

    /**
     * Returns the opened attachment file.
     */
    RFile& File();

    /**
     * Switches drive to use from RAM disk to persistent disk or vice 
     * versa and recreates the attachment file on the other drive. Current
     * file is closed and deleted.
     */
    void SwitchDriveL();

    /**
     * Closes the file and releases ownership of it. After calling this 
     * function the file is no longer deleted in destructor. 
     */
    void Release();

private:  // Implementation

    CFscAttachmentFile();
    void ConstructL(const TDesC& aBaseName);
    void CreateFileL(const TDesC& aDrive, const TDesC& aDir, const TDesC& aBaseName);
    void DeleteFile();
    
private:  // Data
    /// Ref: Handle to file server
    RFs iRfs;
    
    /// Own: file mode
    TUint iFileMode;

    /// Own: cleaned up base name
    HBufC* iBaseName;

    /// Own: name of the file
    HBufC* iFileName;

    /// Own: the opened file
    RFile iFile;
    
    /// Own: ETrue if this object owns the file and should delete it in
    /// destructor
    TBool iOwnsFile;
	};


/**
 * Array of CFscAttachmentFile objects. This array owns the object it contains,
 * in other words it will delete the contained objects in its destructor.
 * Implements MDesCArray interface through which the file names can be 
 * retrieved.
 */
class CFscAttachmentFileArray 
    : public CArrayPtrFlat<CFscAttachmentFile>, public MDesCArray
    {
public:  // Interface
    /**
     * Constructor.
     * @param aGranularity array reallocation granularity.
     */
    CFscAttachmentFileArray(TInt aGranularity);

    /**
     * Destructor. Deletes all CFscAttachmentFile objects in the array.
     */
    ~CFscAttachmentFileArray();

public:  // from MDesCArray
    
    TInt MdcaCount() const;

    TPtrC MdcaPoint(TInt aIndex) const;

    };

#endif // _CFSCATTACHEMTFILE_H

// End of File
