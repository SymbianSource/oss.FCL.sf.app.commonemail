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
* Description:  Definition of class CFscContactActionMenuList.
*
*/

#ifndef C_FSCCONTACTACTIONMENULIST_H
#define C_FSCCONTACTACTIONMENULIST_H

// INCLUDES
#include <e32base.h>
#include <coecntrl.h>
#include <eiklbo.h>

#include "fsccontactactionmenudefines.h"
#include "tfsccontactactionmenutimedvalue.h"

// FORWARD DECLARATIONS
class MFscContactActionMenuListObserver;
class MFscContactActionMenuModel;
class CFscContactActionMenuListBox;
class CFont;
class CAknsFrameBackgroundControlContext; 

/**
 *  Contact Action Menu List implementation.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuList 
    : public CCoeControl, 
      public MEikListBoxObserver
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @param aObserver Menu list's observer
     * @param aModel Menu's model
     * @param aAiMode ETrue in AI mode
     * @param aPositionGiver
     * @param aOpenedFromMR ETrue, if menu was opened from meeting request.
     *
     * @return New instance of the component  
     */
    static CFscContactActionMenuList* NewL( 
        MFscContactActionMenuListObserver* aObserver, 
        MFscContactActionMenuModel* aModel,
        TBool aAiMode, 
        MFsActionMenuPositionGiver* aPositionGiver = 0,
        TBool aOpenedFromMR = EFalse );

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuList();
    
public: // From CCoeControl

    /**
     * OfferKeyEventL
     *
     * @param aKeyEvent Key event
     * @param aType Event type
     *
     * @return key response
     */
    virtual TKeyResponse OfferKeyEventL( 
        const TKeyEvent& aKeyEvent,TEventCode aType );
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * HandleResourceChange
     *
     * @param aType resource change type
     */
    virtual void HandleResourceChange( TInt aType );
    
    /**
     * CountComponentControls
     *
     * @return component controls count
     */
    virtual TInt CountComponentControls() const;
    
    /**
     * ComponentControl
     *
     * @param aIndex Control's index
     * @return control
     */
    virtual CCoeControl* ComponentControl( TInt aIndex ) const;

public: // From MEikListBoxObserver

    /**
     * HandleListBoxEventL
     *
     * @param aListBox listbox
     * @param aEventType event type
     */
    void HandleListBoxEventL( 
            CEikListBox* aListBox, TListBoxEvent aEventType );

public: // Public methods

    /*
     * Show list
     *
     * @param aPosition Position of the list
     * @param aIndex Index of selected item when menu list is shown
     */   
    void ShowL( 
        TFscContactActionMenuPosition aPosition,
        TInt aIndex );  

    /*
     * Hide list
     * @param aSlide Boolean param to specify hiding style 
     */
    void Hide( TBool aSlide = ETrue );
    
    /*
     * Selected item
     *
     * @return Selected item index 
     */
    TInt SelectedItemIndex();
      
    /*
     * Item count in the menu
     *
     * @return item count
     */
    TInt ItemCount();

private: // Private constructors

    /**
     * Constructor.
     *
     * @param aObserver Menu list's observer
     * @param aModel Menu's model
     * @param aAiMode ETrue in AI mode
     * @param aPositionGiver
     * @param aOpenedFromMR ETrue, if menu is opened from meeting request.
     */
    CFscContactActionMenuList( 
        MFscContactActionMenuListObserver* aObserver, 
        MFscContactActionMenuModel* aModel,
        TBool aAiMode, 
        MFsActionMenuPositionGiver* aPositionGiver,
        TBool aOpenedFromMR );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /*
     * Layouts popup window. 
     */
    void LayoutPopupWindow( const TBool aLayoutChange = EFalse );
    
    /*
     * Given rect's width is modified by content.
     */
    void AdjustByContent( TRect& aRect );

    /*
     * Layouts list.
     * 
     * @leave In case of error may leave.
     */
    void LayoutListL();

    /**
     * Returns calculated left margin width in pixels.
     */
    TInt LeftMargin( const TRect& aMenuPane, const TRect& aG1 ) const;

    /**
     * Returns calculated right margin width in pixels.
     */
    TInt RightMargin( const TRect& aMenuPane, const TRect& aT1 ) const;

    /**
     * Returns calculated middle margin width in pixels.
     */
    TInt MiddleMargin( const TRect& aMenuPane, const TRect& aG1 ) const;
    
    /**
     * Layouts list.
     * 
     * @return Possible error code.
     */
    TInt LayoutList();

    /**
     * Construct controls of the list
     */
    void ConstructControlsL();
    
    /**
     * Clear controls of the list
     */
    void ClearControls();

private: // Private methods

    /**
     * Update list content
     */
    void UpdateListContentL( const TSize& aIconSize );
            
    /**
     * Update menu offset depending on current menu state
     */    
    void SetupMenuOffset();    
            
    /**
     * Update colors based on current skin
     */
    void UpdateColors();
    
    /**
     * Count maximum text width of current list items
     *
     * @return max text width in pixels
     */
    TInt MaxListTextWidth();
     
    /**
     * Update window position. Method is called by CPeriodic
     *
     * @param aPtr Pointer to menu list instance
     * @return Error code
     */
    static TInt UpdateWindowPosition( TAny *aPtr );

private: // from CCoeControl
        
    /*
     * @see CCoeControl::SizeChanged
     */
    void SizeChanged();

private: // data

    /**
     * State of the menu
     */
    enum TState
        {
        EOut,           // Hidden
        ESlidingOut,
        EIn,            // Visible
        ESlidingIn
        } iState;  

    /**
     * List box
     *   Owned
     */
    CFscContactActionMenuListBox* iListBox;

    /**
     * Menu list's observer
     *   Not owned
     */
    MFscContactActionMenuListObserver* iObserver;

    /**
     * Menu's model
     *   Not owned
     */
    MFscContactActionMenuModel* iModel;

    /**
     * Menu's position
     */
    TFscContactActionMenuPosition iPosition;

    TPoint iCustomPosition;
    
    /**
     * Menu font
     *   Not owned
     */
    const CFont* iFont; 
    
    /**
     * Timer for slide in/out effect
     *   Owned
     */
    CPeriodic* iPeriodic;

    /**
     * Window's original rect
     */
    TRect iOrigMenuRect;

    /**
     * Window's position offset
     */
    TFscContactActionMenuTimedValue iMenuOffset;
    
    /**
     * Boolean flag to tell whether extra key events should 
     * be notified and skipped.
     */
    TBool iAiMode;
    
    /**
     * Skin ids
     */
    TAknsItemID iListItemId;
    TAknsItemID iListBottomId;
    TAknsItemID iHighlightId;
    TAknsItemID iHighlightCenterId;   

    TBool iListHasConsumedPointed;
    TBool iHasBeenDragged;
    
    MFsActionMenuPositionGiver* iPositionGiver;
    
    TBool iOpenedFromMR;
    };

#endif // C_FSCCONTACTACTIONMENULIST_H

    
