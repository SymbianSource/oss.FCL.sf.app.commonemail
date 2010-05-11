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
* Description: This file defines class CNcsPopupListBox.
*
*/



#ifndef CNCSPOPUPLISTBOX_H
#define CNCSPOPUPLISTBOX_H


// INCLUDES
#include <aknlists.h>
#include "FreestyleEmailUiContactHandlerObserver.h"

// FORWARD DECLARATION
class CNcsEmailAddressObject;
class CAknsFrameBackgroundControlContext;
class CFSEmailUiContactHandler;
class CFSMailBox;
class CNcsHeaderContainer;
class CFSEmailUiLayoutHandler;
class CFreestyleEmailUiAppUi; //<cmail>

// CLASS DECLARATION

/**
*  CNcsPopupListBox
*/
class CNcsPopupListBox : public CEikTextListBox, 
                         public MEikListBoxObserver,
                         public MFSEmailUiContactHandlerObserver
    {
public: // Constructors and destructor

		/*
		* @param aMailBox reference to current mailbox item
		*/
    static CNcsPopupListBox* NewL( const CCoeControl* aParent, CFSMailBox& aMailBox, 
                                   CNcsHeaderContainer& aHeaderContainer,
                                   TBool aRemoteLookupSupported );

    /*
    * @param aMailBox reference to current mailbox item
    */
    void ConstructL( const CCoeControl* aParent );

    /*
    * 
    *
    * @param aText
    */
    void InitAndSearchL( const TDesC& aText, TInt aMode = -1 );
		
    // Destructor
    virtual ~CNcsPopupListBox();

public: // from CEikTextListBox

    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    void SizeChanged();
    void HandleResourceChange( TInt aType );
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

public: // from MEikListBoxObserver

    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

public: // from MFSEmailUiContactHandlerObserver

    void OperationCompleteL( TContactHandlerCmd aCmd, 
        const RPointerArray<CFSEmailUiClsItem>& aMatchingItems );
    void OperationErrorL( TContactHandlerCmd aCmd, TInt aError );

public: // new methods for the header container

    /*
    * 
    * @param aText
    */
    void SetSearchTextL( const TDesC& aText, TInt aMode = -1 );
 
    /*
    * 
    * @param 
    */
    CNcsEmailAddressObject* ReturnCurrentEmailAddressLC();
 
    /*
    *
    *
    */
    void SetPopupMaxRect( const TRect& aPopupMaxRect );
 		
    /*
    *
    *
    */
    TBool IsPopupEmpty() const;


    /*
    *
    *
    */
    TBool IsRemoteLookupItemSelected() const;
	
    /*
    * Returs reference to current cls items array
    *
    * @return RPointerArray<CFSEmailUiClsItem>& reference to current cls items array
    */
    const RPointerArray<CFSEmailUiClsItem>& CurrentPopupClsItemsArray() const;

    /*
    * Return index of the "Search from remote" item in the list
    *
    * @return iRemoteLookupItemPos value; 
    */
    TInt RemoteLookupItemPos() const;
 
    /*
    * Conveniency method to get Layout handler reference.
    */
    CFSEmailUiLayoutHandler& LayoutHandler() const;
    
private: // to populate new list
    
    void SetListItemsFromArrayL();

    TInt RoundToItemHeight(const TInt aPopupHeight) const;

    void SetPopupRect();
 
    void SetScrollBarVisibilityL();

    void SetRemoteLookupItemFirstToTheListL();
		
    enum TRemoteLookupItemMoveDirection
        {
        ERemoteLookupItemUp,
        ERemoteLookupItemDown
        };

    void MoveRemoteLookupItemL( TRemoteLookupItemMoveDirection aDirection );
 
    void CreateTextArrayAndSetToTheListboxL( const TBool& aResetIfExists );

private:

    CNcsPopupListBox( CNcsHeaderContainer& aHeaderContainer,
        TBool aRemoteLookupSupported, CFSMailBox& aMailbox );

    virtual void CreateItemDrawerL();

    void UpdateListL();

    void UpdateTextColors();

    // From CCoeControl.
    void Draw( const TRect& aRect ) const;

private: // data

    CNcsHeaderContainer&				iHeaderContainer;

    CAknsFrameBackgroundControlContext* iBackgroundContext;

    RPointerArray<CFSEmailUiClsItem>	iMatchingItems;

    CFSEmailUiContactHandler*			iContactHandler; // Owned
    CFSMailBox& 						iMailBox;

    TRect								iPopupMaxRect;
    
    CDesCArray*							iItemTextsArray;
    HBufC*								iCurrentSearchText;
    TInt								iRemoteLookupItemPos;
    
    TBool 								iRemoteLookupSupported;
    TBool 								iCachingInProgress;
    
    CFreestyleEmailUiAppUi*             iAppUi;  //<cmail>  not owned
    };


/**
*  CNcsListItemDrawer
*/
class CNcsListItemDrawer: public CListItemDrawer
    {
public: // constructor
	CNcsListItemDrawer( const CNcsPopupListBox& aListBox );

protected: // from CListItemDrawer
	void DrawActualItem( TInt aItemIndex, const TRect& aActualItemRect,
                         TBool aItemIsCurrent, TBool aViewIsEmphasized, 
                         TBool aViewIsDimmed, TBool aItemIsSelected ) const;

private: // new methods
    void DrawPartOfItem( const TRect& aItemRect, const CFont& aFont,
                         TInt aStartPos, TInt aLength, const TDesC& aDes,
                         TBool aUnderlined, TInt aBaselineOffsetFromTop ) const; 

private: // data
	const CNcsPopupListBox& iListBox;
    };


#endif // CNCSPOPUPLISTBOX_H


// End of File
