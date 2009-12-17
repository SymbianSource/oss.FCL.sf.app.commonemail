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
* Description: This file defines class CFileSystemInfo.
*
*/

 
#ifndef FILE_SYSTEM_INFO_H
#define FILE_SYSTEM_INFO_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* CFileSystemInfo is a convenience class that is able to provide
* application private path information.
*/
class CFileSystemInfo : public CBase
    {
    public:
    	/**
	     * Constructs new instance of CFileSystemInfo class.
	     * 
	     * @return Pointer to a new CFileSystemInfo object.
	     * @leave KErrNotFound If multimediamenu application is not installed.
	     */
	    static CFileSystemInfo* NewL(const TUid & aApplicationUid);

	    /**
	     * Constructs new instance of CFileSystemInfo class and leaves it in the cleanup stack.
	     * 
	     * @return Pointer to a new CFileSystemInfo object.
	     * @leave KErrNotFound If multimediamenu application is not installed.
	     */    
	    static CFileSystemInfo* NewLC(const TUid & aApplicationUid);

	    /**
	     * Default destructor.
	     */
	    virtual ~CFileSystemInfo();
	               	
		/**
		 * Returns the application private path. The returned path will have a trailing backslash.
		 */
		TFileName PrivatePath() const;
	               	
   	private:

	    /**
	    * Default constructor
	    */
	    CFileSystemInfo();
			
	    /**
	     * Second phase constructor.
	     * 
	     * @leave KErrNotFound If multimediamenu application is not installed.
	     */
	    void ConstructL(const TUid & aApplicationUid);
   	
   	private:
   	
   		/** Current private path */
   		TFileName iPrivatePath;
   		   	
    };
    
#endif // FILE_SYSTEM_INFO_H

// End of file
