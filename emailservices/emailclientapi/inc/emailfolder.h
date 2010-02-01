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
* Description: CEmailMailbox.
*
*/

#ifndef CEMAILFOLDER_H
#define CEMAILFOLDER_H

#include <emailapidefs.h>
#include "CFSMailCommon.h"
#include <memailfolder.h>
#include "mfsmaileventobserver.h"

using namespace EmailInterface;

class CFSMailPlugin;
class CPluginData;
class CFSMailFolder;
class EmailInterface::MMessageIterator;

NONSHARABLE_CLASS( CEmailFolder ) : 
    public CBase, 
    public MEmailFolder
{
public:
    
    static CEmailFolder* NewLC( 
        CPluginData& aPluginData,
        const TFolderId& aFolderId,
        CFSMailFolder *aFolder );
    
    static CEmailFolder* NewL( 
        CPluginData& aPluginData,
        const TFolderId& aFolderId,
        CFSMailFolder *aFolder );

    ~CEmailFolder();           

public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();
    
public:    // from MEmailFolder
    virtual TFolderId FolderId() const;
    
    virtual TFolderId ParentFolderId() const;
    
    virtual TFolderType FolderType() const;    

    virtual TPtrC Name() const;

    /**
    * Returns direct childs of this folder, i.e. this is not recursive.
    * @return number of subfolders or an error code
    */    
    virtual TInt GetSubfoldersL( 
        RFolderArray& aSubfolders ) const;

    virtual EmailInterface::MMessageIterator* MessagesL( 
        const RSortCriteriaArray& aCriteria );
    
    virtual void DeleteMessagesL( const REmailMessageIdArray& aMessageIds );
    
private: // constructors
    CEmailFolder( CPluginData& aPluginData, const TFolderId& aFolderId, CFSMailFolder *aFolder );
    
    void ConstructL();

private: // Internal methods
    static void ToFsSortCriteriaL( 
        const RSortCriteriaArray& aSortCriteria, 
        RArray<TFSMailSortCriteria>& aFsCriteria );

private:
    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TFolderType     iFolderType;
       
    TFolderId       iFolderId;
    
    TFolderId       iParentId;
    
    CFSMailFolder   *iFolder;
};
    
#endif // CEMAILFOLDER_H

// End of file
