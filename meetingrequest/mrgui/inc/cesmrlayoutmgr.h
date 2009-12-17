/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR checkbox component for es mr gui (editor)
*
*/


#ifndef CESMRLAYOUTMGR_H
#define CESMRLAYOUTMGR_H

#include <e32base.h>
#include <fbs.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <layoutmetadata.cdl.h>
#include <cenrepnotifyhandler.h>

class CFont;
class CCoeEnv;
class TMargins;
class TLogicalFont;
class CAknLayoutFont;
class CRepository;
class TAknsItemID;
class MESMRFontSizeObserver;

/**
 * MRUI mif file that includes all icon fallbacks for sis installations
 */
_LIT( KESMRMifFile, "esmrgui.mif");

/**
 * Namespace for layout constants
 */
namespace ESMRLayout // codescanner::namespace
    {
    /**
     * common icon value all around the application
     */
    #define KIconSize ::TSize(20,20) // codescanner::baddefines
    
    /**
     * Icon border margin
     */
    const TInt KIconBorderMargin = 3;

    /**
     * Field end margin
     */
    const TInt KFieldEndMargin = 6;
    
    /**
     * Mirrored icon margin
     */
    const TInt KMirroredIconMargin = 9;
    }

/**
 * CESMRLayoutManager
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRLayoutManager ) :
    public CBase,
    public MCenRepNotifyHandlerCallback
    {
public:
    // Enumeration for margin ids
    enum TMarginsId
        {
        EFieldMargins = 0x000000
        };

    // Struct for field sizes
    struct SFieldSize
        {
        TESMREntryFieldId iId;
        TInt iFontSmallHeight;
        TInt iFontNormalHeight;
        TInt iFontLargeHeight;
        TInt iWidthLandscape;
        TInt iWidthPortrait;
        };

public: // Construction and destruction

    /**
     * Creates and initialzes new CESMRLayoutManager object.
     * @return Pointer to CESMRLayoutManager object.
     */
    static CESMRLayoutManager* NewL();

    /**
     * C++ destructor
     */
    virtual ~CESMRLayoutManager();

public: // Interface

    /**
     * Fetches field size information.
     * @param aId Field id.
     * @return Field size information.
     */
    virtual TSize FieldSize( TESMREntryFieldId aId );

    /**
     * Returns margin values used in the fields
     *
     * @param TMarginsId - Not currently in use
     * @return TMargins margin values (pixels)
     */
    virtual TMargins Margins( TMarginsId aId );

    /**
     * Fetches font for field.
     *
     * @param aEnv Pointer to coe environment
     * @param aId Field id.
     * @return Pointer to font object. Ownership is not transferred.
     *         Can return NULL pointer.
     */
    virtual const CFont* Font(
            CCoeEnv* aEnv,
            TESMREntryFieldId aId);

    /**
     * Fetces icon margin from the top of the field.
     * This depends on the font size
     *
     * @return Margin value (pixels)
     */
    virtual TInt IconTopMargin();

    /**
     * Fetces text margin from the top of the field.
     * This depends on the font size
     *
     * @return Margin value (pixels)
     */
    virtual TInt TextTopMargin();

    /**
     * Fetces text margin from the side of the field.
     * This depends on the font size.
     *
     * @return Margin value (pixels)
     */
    virtual TInt TextSideMargin();

    /**
     * Returns the size of the control where response
     * area answer text is written.
     *
     * @return TSize Size of the control
     */
    virtual TSize ResponseAreaAnswerTextSize();

    /**
     * Returns the size of the control where response
     * area topic text is written.
     *
     * @return TSize Size of the control
     */
    virtual TSize ResponseAreaTopicTextSize();

    /**
     * Returns the position of the control where response
     * area answer text is written.
     *
     * @return TPoint position of the control
     */
    virtual TPoint ResponseAreaAnswerTextPosition();

    /**
     * Returns the position of the icon in the response area item
     *
     * @return TPoint position of the control
     */
    virtual TPoint ResponseAreaAnswerIconPosition();

    /**
     * Returns the height of the response area item
     *
     * @return TInt height of the control
     */
    virtual TInt ResponseAreaAnswerItemheight();

    /**
     * Returns the height of standard list field
     *
     * @return TInt height of the control
     */
    virtual TInt RegularFieldHeight();

    /**
     * Returns the position of the text control used in organizer field
     *
     * @return TInt height of the control
     */
    virtual TPoint OrganizerTextPosition();

    /**
     * Returns the width of the text control used in organizer field
     *
     * @return TInt width of the control
     */
    virtual TInt OrganizerTextWidth();

    /**
     * Returns the height of the title pane
     *
     * @return TInt height of the control
     */
    virtual TInt TitlePaneHeight();

    /**
     * Returns the skin/theme based color used as a background
     * color in the list.
     *
     * @return TRgb background color
     */
    virtual TRgb ListAreaBgColor();

    /**
     * Returns the skin/theme based color used as a text
     * color.
     *
     * @return TRgb text color
     */
    virtual TRgb GeneralListAreaTextColor();

    /**
     * Returns the skin/theme based color used as a text
     * color.
     *
     * @return TRgb text color
     */
    virtual TRgb ViewerListAreaTextColor();

    /**
     * Returns the skin/theme based color used as a text
     * color when text is "highlighted".
     *
     * @return TRgb text color
     */
    virtual TRgb ViewerListAreaHighlightedTextColor();

    /**
     * Returns the skin/theme based text color used as a text
     * color in theme documentation specified places.
     *
     * @return TRgb text color
     */
    virtual TRgb NormalTextColor();

    /**
     * Returns the skin/theme based text color used as a text
     * color in theme documentation specified places.
     *
     * @return TRgb text color
     */
    virtual TRgb HighlightedTextColor();
    
    /**
     * Returns the skin/theme based text color used as a text
     * color in title pane.
     *
     * @return TRgb text color
     */
    virtual TRgb TitleAreaTextColor();

    /**
     * Returns the skin/theme based text background color to be used
     * as a color in theme documentation specified places.
     *
     * @return TRgb text color
     */
    virtual TRgb NormalTextBgColor();

    /**
     * Returns the skin/theme based field border color to be used
     * as a color in theme documentation specified places.
     *
     * @return TRgb text color
     */
    virtual TRgb FieldBorderColor();

    /**
     * Returns the text color ID for normal text.
     *
     * @return TInt text color ID
     */
    virtual TInt NormalTextColorID();

    /**
     * Returns the text color ID for viewer area text.
     *
     * @return TInt text color ID
     */
    virtual TInt ViewerListAreaTextColorID();

    /**
     * Creates bitmap and bitmap mask. Bitmap is skin/theme based
     * and it's retrived by the given ID.Bitmap is also resized to
     * the given size.
     * Ownership of these created bitmaps is transferred to the caller.
     *
     * @param aIconID - id of the wanted bitmap
     * @param aBitmap - reference to the pointer where new bitmap is stored
     * @param aMask - reference to the pointer where new bitmap mask is stored
     * @param aSize - Desired size for the bitmap
     * @return TInt - KErrNone if bitmap was constructed succesfully
     *                KErrNotFound if any problems occured
     */
    virtual TInt GetSkinBasedBitmap( TAknsItemID aIconID,
                                     CFbsBitmap*& aBitmap,
                                     CFbsBitmap*& aMask,
                                     TSize aSize );
    /**
     * Checks if Font settings were changed since last font update query
     * Note: This function does not make a query/poll from the cenrep
     * @see CESMRLayoutManager::Font
     *
     * @return TBool - True, if font was changed
     */
    virtual TBool IsFontChanged();

   /**
     * Checks current font zoom/size of this layoutmanager
     *
     * @return TAknUiZoom - font zoom state
     */
    virtual TInt CurrentFontZoom();

    /**
     * Sets the Font size settings observer.
     * @param MESMRFontSizeObserver Observer instance to be informed.
     */
    void SetObserver( MESMRFontSizeObserver* aObserver );

    /**
     * Returns the position of the conflict popup control
     *
     * @return TPoint control position
     */
    IMPORT_C static TPoint ConflictPopupPosition();

    /**
     * Returns information about mirrored layout.
     *
     * @return TBool - ETrue if mirrored layout is set, otherwise EFalse.
     */
    IMPORT_C static TBool IsMirrored();

    /**
     * Sets fallback icon from esmrgui.mif file if no matching icon from skin
     * also used by several other fields as a utility
     *
     * @param aIconID - ID for the icon that this field should show
     * @param aFileIndex - id of the icon (from esmrgui.mif)
     * @param aFileMaskIndex - id of the icon mask (from esmrgui.mif)
     */
    IMPORT_C static void SetBitmapFallback( const TAknsItemID& aIconID,
                                            TInt& aFileIndex,
                                            TInt& aFileMaskIndex);
    
private:
    // Default c++ constructor
    CESMRLayoutManager();
    // Second phase constructor
    void ConstructL();
    // From MCenRepNotifyHandlerCallback
    // Notifies the observer that font size has changed
    virtual void HandleNotifyInt( TUint32 aId, TInt aNewValue );

    /**
     * Method is used to add sizes of the fields for different font sizes and
     * orientations. This is called only once at the construct phase.
     *
     * @param aId - ID of the field
     * @param aFontSmallHeight - height of the field when small fonts are set.
     * @param aFontNormalHeight - height of the field when normal fonts are set.
     * @param aFontLargeHeight - height of the field when large fonts are set.
     * @param aWidthLandscape - width of the field when orientation is landscape.
     * @param aWidthPortrait - width of the field when orientation is portrait.
     *
     */
    void AddSize(
            TESMREntryFieldId aId,
            TInt aFontSmallHeight,
            TInt aFontNormalHeight,
            TInt aFontLargeHeight,
            TInt aWidthLandscape,
            TInt aWidthPortrait );

private: // Data
    /// Own: Field size array
    RArray<SFieldSize> iSize;
    /// Own: Pointer to text field fonts
    CAknLayoutFont* iFont;
    /// Own: Pointer to mfne field fonts
    CAknLayoutFont* iMfneFont;
    /// Own: Pointer to cenrep object fetching font settings information
    CRepository* iFontSettings;
    /// Own: Font settings value
    TInt iFontSettingsValue;
    /// Own: Font settings change status
    TBool iFontSettingsChanged;
    /// Own: Active object which listens Font Size Key from a CenRep
    CCenRepNotifyHandler* iNotifyHandler;
    /// Ref: Observer for Font Size
    MESMRFontSizeObserver* iFontSizeObserver;
    };

#endif //CESMRLAYOUTMGR_H

// EOF
