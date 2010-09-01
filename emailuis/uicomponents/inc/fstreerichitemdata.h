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
* Description:  Tree item data derived class able to store rich text.
*
*/



#ifndef C_FSTREERICHITEMDATA_H
#define C_FSTREERICHITEMDATA_H


#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstreeitemdata.h"

const TFsTreeItemDataType KFsTreeRichItemDataType = 4;

class CAlfTexture;
class CFsRichText;

/**
 *  ?one_line_short_description
 *  ?more_complete_description
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CFsTreeRichItemData )
    : public CBase, public MFsTreeItemData
    {

public:

    /**
     * Two-phased constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    IMPORT_C static CFsTreeRichItemData* NewL( CFsRichText* aText );

    /**
    * Destructor.
    */
    virtual ~CFsTreeRichItemData();

    /**
     * Set the FsRichText object. Ownership of this object is gained.
     *
     * @param aText Object containing the text.
     */
    virtual void SetText( CFsRichText* aText );

    /**
     * Return the text.
     *
     * @return Refence to the text.
     */
    IMPORT_C const CFsRichText& Text() const;

    /**
     * Determines if the icon has been set.
     *
     * @return Logical value to inform if the icon is set or not.
     */
    virtual TBool IsIconSet() const;

    /**
     * Icon in use.
     *
     * @return Refence to the icon in use.
     */
    virtual const CAlfTexture& Icon() const;

    /**
     * Set an icon to use.
     *
     * @param aIcon Icon to use.
     */
    virtual void SetIcon( CAlfTexture& aIcon );

// from base class MFsTreeItemData

    /**
     * From MFsTreeItemData.
     * Type of the data item.
     */
    virtual TFsTreeItemDataType Type() const;

protected:

    CFsTreeRichItemData( CFsRichText* aText );

    void ConstructL();

protected: //Data

    /**
     * Text data holder.
     * Own.
     */
    CFsRichText* iFsRichText;

    /**
     * Has the set icon.
     * Not own.
     */
    CAlfTexture* iIcon;

    };


#endif  // C_FSTREERICHITEMDATA_H
