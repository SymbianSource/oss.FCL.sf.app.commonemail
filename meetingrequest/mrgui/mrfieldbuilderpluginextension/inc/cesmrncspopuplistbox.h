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
* Description: This file defines classes CESMRNcsPopupListBox, CESMRNcsListItemDrawer
*
*/

#ifndef CESMRNCSPOPUPLISTBOX_H
#define CESMRNCSPOPUPLISTBOX_H

// INCLUDES
#include <aknlists.h>
#include "mesmrcontacthandlerobserver.h"

// FORWARD DECLARATIONS
class CESMRNcsEmailAddressObject;
class CAknsBasicBackgroundControlContext;
class CESMRContactHandler;
class CFSMailBox;
class CESMRLayoutManager;

// CLASS DECLARATION

/**
 *  CESMRNcsPopupListBox is the litbox that is displayed when user types
 *  text to attendee and field and contacts are searched. this class handles
 *  displaying and updating the list of contacts email addresses
 */
NONSHARABLE_CLASS( CESMRNcsPopupListBox ) : public CEikTextListBox,
                                            public MESMRContactHandlerObserver
    {
public: // Constructors and destructor
    /**
     * Creates new CESMRNcsPopupListBox object. Ownership
     * is transferred to caller.
     * @param aParent set parent control of this class
     * @param aContactHandler pointer to contact handles class
     * @return Pointer to created object,
     */
    static CESMRNcsPopupListBox* NewL( const CCoeControl* aParent,
                                       CESMRContactHandler& aContactHandler );

    /**
     * Second phase constructor
     * @param aParent set parent control of this class
     */
    void ConstructL( const CCoeControl* aParent );

    /**
     * C++ Destructor.
     */
    virtual ~CESMRNcsPopupListBox();

public: // from CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    virtual TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    void SizeChanged();

public: // from ContactHandlerObserver
    void OperationCompleteL(
            TContactHandlerCmd aCmd,
            const RPointerArray<CESMRClsItem>* aMatchingItems );
    void OperationErrorL( TContactHandlerCmd aCmd, TInt aError );

public: // new methods for the header container
    /**
     * Start searching contacts for teh email addresses using a string
     * @param aText start contacts database search with this string
     */
    void InitAndSearchL( const TDesC& aText );

    /**
     * Start searching contacts for teh email addresses using a string
     * @param aText start contacts database search with this string
     */
    void SetSearchTextL( const TDesC& aText );

    /**
     * Get the currently selected email/contact address object
     * @return current selected email/contact address object
     */
    CESMRNcsEmailAddressObject* ReturnCurrentEmailAddressLC();

    /**
     * Set maximum area on screen for displaying the popuplistbox
     * @param aPopupMaxRect area rectangle
     */
    void SetPopupMaxRectL( const TRect& aPopupMaxRect );

    /**
     * check if popup has any items
     * @return check if it's empty
     */
    TBool IsPopupEmpty() const;

    /**
     * Test if remote look up item is selected
     */
    TBool IsRemoteLookupItemSelected() const;

    /**
     * Returs reference to current cls items array
     * @return RPointerArray<CFSEmailUiClsItem>& reference to current cls items array
     */
    const RPointerArray<CESMRClsItem>& CurrentPopupClsItemsArray() const;

    /**
     * @return iRMLUItemPosition value;
     */
    TInt RMLUItemPosition() const;

    /**
     * Initialise skin/theme/laf support and set layoutmanager to be used by the list
     * @param aLayout LayoutManager to be used by the list
     */
    void Initialise(CESMRLayoutManager* aLayout);
    
private: // Implementation
    enum TRemoteLookupItemMoveDirection
        {
        ERemoteLookupItemUp,
        ERemoteLookupItemDown
        };
    void SetListItemsFromArrayL();
    void SetPopupHeightL();
    void SetRemoteLookupItemFirstToTheListL();
    void MoveRemoteLookupItemL( TRemoteLookupItemMoveDirection aDirection );
    void CreateTextArrayAndSetToTheListboxL();

private:
    CESMRNcsPopupListBox( CESMRContactHandler& aContactHandler );
    virtual void CreateItemDrawerL();
    
private: // data
    CAknsBasicBackgroundControlContext* iBaseBackroundContext;//own
    RPointerArray<CESMRClsItem>         iMatchingArray; //own
    CDesCArray*                         iItemTextsArray;//own
    HBufC*                              iCurrentSearchText;//own
    CFSMailBox*                         iMailBox;//not own
    CESMRContactHandler&                iContactHandler;//not own
    TRect                               iPopupMaxRect;
    TInt                                iRMLUItemPosition;
    TBool                               iRemoteLookupSupported;
    };

// CLASS DECLARATION

/**
 *  CESMRNcsListItemDrawer handles drawing a single contact/address
 *  item on the CESMRNcsPopupListBox
 */
NONSHARABLE_CLASS( CESMRNcsListItemDrawer ) : public CListItemDrawer
    {
public:
    /**
     * Constructor
     * @param aListBox reference to the list box where the object is located
     */
    CESMRNcsListItemDrawer( CESMRNcsPopupListBox& aListBox );

    void SetLayoutManager( CESMRLayoutManager* aLayout );

private: // from CListItemDrawer
    void DrawActualItem( TInt aItemIndex, const TRect& aActualItemRect,
                         TBool aItemIsCurrent, TBool aViewIsEmphasized,
                         TBool aViewIsDimmed, TBool aItemIsSelected ) const;

private:
    void DoDrawActualItemL( TInt aItemIndex, const TRect& aActualItemRect,
                            TBool aItemIsCurrent, TBool aViewIsEmphasized,
                            TBool aViewIsDimmed, TBool aItemIsSelected ) const;
    void DrawPartOfItem( const TRect& aItemRect, const CFont& aFont,
                         TInt aStartPos, TInt aLength, const TDesC& aDes,
                         TBool aUnderlined, TInt aBaseline ) const;
    
private: //data
    CESMRNcsPopupListBox&               iListBox;
    CESMRLayoutManager*                 iLayout;//not own
    };


#endif // CESMRNCSPOPUPLISTBOX_H


// End of File
