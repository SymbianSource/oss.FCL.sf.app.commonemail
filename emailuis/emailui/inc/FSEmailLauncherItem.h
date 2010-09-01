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
* Description:  Launcher item plugin API definition.
*
*/


#ifndef __FSEMAILLAUNCHERITEM_H__
#define __FSEMAILLAUNCHERITEM_H__

#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <AknIconUtils.h>

/**
 * UID of this ECOM interface.
 */
// <cmail> S60 UID update
const TUid KFSEmailLauncherItemUid = {0x2001E285};
// </cmail> S60 UID update

class MFSEmailLauncherItemObserver 
    { 
public: 

    /** 
     * Gets called when an item requests an view refresh. The launcher should
     * re-read all cached data from the item plug-in interface.
     */ 
    virtual void RefreshLauncherViewL() = 0; 
    };

class CFSEmailLauncherItem : public CBase
    {
public: 

    /**
     * Constructs CFSEmailLauncherItem.
     *
     * @param aImplementationUid UID of the implementation to be constructed.
     * @param aObserver Observer for refreshing launcher view.
     * @return Launcher item.
     */
    static CFSEmailLauncherItem* NewL(
        TUid aImplementationUid,
        MFSEmailLauncherItemObserver* aObserver );

    /**
     * Destructor
     */
    virtual ~CFSEmailLauncherItem();

    /**
     * Request a list of all available implementations which satisfy this
     * given interface.
     *
     * @param aImplInfoArray Info array of implementations.
     */
    static void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );

public:

    /**
     * Set observer for refreshing launcher view.
     *
     * @param aObserver Observer for refreshing launcher view.
     */
    virtual void SetObserver( MFSEmailLauncherItemObserver* aObserver ) = 0;

    /**
     * Returns the ID of the item. The IDs are allocated by the EMail UI
     * project. The IDs are used to match the plug-in API implementations with
     * the placeholders, so that we get the items to the correct locations on
     * the grid defined in the UI spec. The implementation could also return
     * some constant in which case the UI would place the item at the end of
     * the grid or something. 
     *
     * @return Id of the item.
     */ 
    virtual TInt Id() const = 0;

    /**
     * Returns the icon displayed in the launcher grid. The UI will call
     * SetSize for it with appropriate values to accomodate to different
     * screen sizes.
     *
     * @return Icon to be displayed.
     */ 
    virtual CAknIcon* Icon() const = 0;

    /**
     * Returns a long or short caption for the item that gets displayed in the
     * UI.
     *
     * @param aLongCaption If EFalse, the short caption is returned.
     * @return Caption for the item.
     */ 
    virtual TPtrC Caption( TBool aLongCaption ) const = 0;

    /**
     * Returns information about whether the item should be visible or not.
     *
     * @return Visibility of the item. 
     */ 
    virtual TBool IsVisible() const = 0;

    /**
     * Gets called if the user selects the item.
     */ 
    virtual void ExecuteActionL() = 0;

private:

    /** 
     * Instance identifier key.
     */
    TUid iDtor_ID_Key;
    };


inline CFSEmailLauncherItem::~CFSEmailLauncherItem()
    {
    // Inform the ECOM framework that this specific instance of the interface
    // has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CFSEmailLauncherItem* CFSEmailLauncherItem::NewL(
        TUid aImplementationUid,
        MFSEmailLauncherItemObserver* aObserver )
    {
    TAny* ptr = REComSession::CreateImplementationL(
        aImplementationUid,
        _FOFF( CFSEmailLauncherItem, iDtor_ID_Key ) );

    CFSEmailLauncherItem* item = REINTERPRET_CAST( CFSEmailLauncherItem*, ptr );

    item->SetObserver( aObserver );

    // The type of TAny* ptr should be CFSEmailLauncherItem.
    return item;
    }

inline void CFSEmailLauncherItem::ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
    {
    REComSession::ListImplementationsL( KFSEmailLauncherItemUid, aImplInfoArray );
    }

#endif // __FSEMAILLAUNCHERITEM_H__
