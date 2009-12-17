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
* Description:  Folder array behavior.
*
*/


#ifndef IPSSETUIFOLDERLISTARRAY_H
#define IPSSETUIFOLDERLISTARRAY_H


#include <msvstd.h>     // TMsvId  
#include <bamdesca.h>   // MDesCArray

class CMsvEntry;
/**
 * Array class to be used by CIpsSetUiSubscriptionDialog
 *
 * @lib IpsSosSettings.lib
 * @since FS v1.0
 */
class CIpsSetUiFolderListArray :
    public CBase,
    public MDesCArray
    {
public:

    /**
     * Two phased constructor NewL
     * @param TMsvId mailbox id which folders we are handling
     * @return CIpsSetUiFolderListArray*
     */
    static CIpsSetUiFolderListArray* NewL(
        CMsvSession& aMsvSession,
        TMsvId aMailboxId );

    /**
     * Destructor
     * ~CIpsSetUiFolderListArray()
     */
    ~CIpsSetUiFolderListArray();

    /**
     * Id()
     * @param TInt, index which Id is to be fetched
     * @return TMsvId, the id of folder identified by parameter
     */
    TMsvId Id( TInt aIndex );

    /**
     * ChangEIpsSetUiFolderL
     * @param TMsvId, id of folder to be changed into
     */
    void ChangEIpsSetUiFolderL( TMsvId aFolderId );

    /**
     * FoldersUpdated
     * @return TBool, ETrue if folders have been updated
     */
    TBool FoldersUpdated() const;

    /**
     * ContextHasChildren
     * @param TMsvId, id of folder to be checked for children
     * @TBool ETrue if folder has children
     */
    TBool ContextHasChildren( TMsvId aId ) const;

    /**
     * GetContextChildrenL
     * This is a recursive function, which will get all folder entries under
     * the entry given in as the first parameter
     * @param TMsvId, id of the folder which children we want
     * @return CMsvEntrySelection*, array of TMsvIds of the children
     */
    CMsvEntrySelection* GetContextChildrenL( TMsvId aId, CMsvEntry& aEntry ) const;

    /**
     * RefreshFolderListArrayL()
     *
     */
    void RefreshFolderListArrayL();

private:

    /**
     * C++ constructor
     * @param TMsvId
     */
    CIpsSetUiFolderListArray(
        CMsvSession& aMsvSession,
        TMsvId aMailboxId );

    /**
     * Second phase constructor
     * ConstructL()
     */
    void ConstructL();


    /**
     * from MDesCArray
     * MdcaCount()
     * @return TInt, count of items to be displayed
     */
    virtual TInt MdcaCount() const;

    /**
     * from MDesCArray
     * MdcaPoint()
     * @param TInt
     * @return TPtrC
     */
    virtual TPtrC MdcaPoint(TInt aIndex) const;

    /**
     * ContextHasChildFolders
     * @param TMsvId, id of folder to be checked for children
     * @return TBool, ETrue if folder has children
     */
    TBool ContextHasChildFolders( TMsvId aId ) const;

    /**
     * DoContextHasChildFoldersL
     * @param TMsvId, id of folder to be checked for children
     * @return TBool ETrue if folder has children
     */
    TBool DoContextHasChildFoldersL( TMsvId aId ) const;

private:

// data

    /**
     * Message server session.
     */
    CMsvSession&    iSession;

    /**
     * Current mailbox id.
     */
    TMsvId          iMailboxId;

    /**
     * Current entry object.
     * Owned.
     */
    CMsvEntry*      iEntry;

    /**
     * Temp text container.
     * Owned.
     */
    HBufC*          iText;

    /**
     * Number of items.
     */
    TInt            iCount;
    };

#endif

// End of File