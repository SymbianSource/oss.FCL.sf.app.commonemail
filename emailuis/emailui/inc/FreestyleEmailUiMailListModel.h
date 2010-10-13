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
* Description:  FreestyleEmailUi mail list model and model item definition.
*
*/


#ifndef __FREESTYLEEMAILUI_MAILLISTMODEL_H__
#define __FREESTYLEEMAILUI_MAILLISTMODEL_H__

// SYSTEM INCLUDES
#include <e32base.h>

// INTERNAL INCLUDES
#include "FreestyleEmailUiListModel.h"
#include "fstreelistconstants.h"

// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
class CFSMailMessage;

enum TModelItemType
	{
	ETypeMailItem=0,
	ETypeSeparator
	};

/**
 * Mail list model item.
 */
class CFSEmailUiMailListModelItem : public CBase, public MFSListModelItem
    {
public:
    static CFSEmailUiMailListModelItem* NewL( CFSMailMessage* aMessagePtr, TModelItemType aModelItemtype );
    static CFSEmailUiMailListModelItem* NewLC( CFSMailMessage* aMessagePtr, TModelItemType aModelItemtype );
    virtual ~CFSEmailUiMailListModelItem();

private:
    CFSEmailUiMailListModelItem::CFSEmailUiMailListModelItem( CFSMailMessage* aMessagePtr, TModelItemType aModelItemtype );
public:
    inline void SetMessagePtr( CFSMailMessage* aMessagePtr ) { iMessagePtr = aMessagePtr; }
	inline CFSMailMessage& MessagePtr() { return *iMessagePtr; }
	inline const CFSMailMessage& MessagePtr() const { return *iMessagePtr; }
	inline TModelItemType ModelItemType() const { return iModelItemType; }
	void SetSeparatorTextL( const TDesC& aSeparatorText );
	inline HBufC* SeparatorText() { return iSeparatorText; }
    inline const HBufC* SeparatorText() const { return iSeparatorText; }
	inline void AddCorrespondingListId( TInt aFsTreeListId ) { iFsTreeListId = aFsTreeListId; }
	inline TInt CorrespondingListId() const { return iFsTreeListId; }
	
private:
	CFSMailMessage* iMessagePtr;   //!< ownded IFF item type is ETypeMailItem
	TModelItemType iModelItemType;
	HBufC* iSeparatorText;         //!< owned
	TInt iFsTreeListId;
  }; 
 

/**
 * Mail list model.
 */
class CFSEmailUiMailListModel : public CBase, public MFSEmailListModel
  {
public: // construction and destruction
    
    /** Two-phased constructor. */
	static CFSEmailUiMailListModel* NewL( CFreestyleEmailUiAppUi* aAppUi, TBool aSearchList = EFalse );
	/** Two-phased constructor. */
	static CFSEmailUiMailListModel* NewLC( CFreestyleEmailUiAppUi* aAppUi, TBool aSearchList = EFalse );
	/** Destructor. */
	~CFSEmailUiMailListModel();

public: // from base class MFSEmailListModel

	void AppendL(MFSListModelItem* aItem);
	void InsertL(MFSListModelItem* aItem, TInt aIndex);
	void RemoveAndDestroy(TInt aIndex);
	MFSListModelItem* Item(TInt aIndex);
    const MFSListModelItem* Item(TInt aIndex) const;
	TInt Count() const;
	TBool ContainsItem(const CFSMailMessage& aMessage) const;

public: // new methods
	
	CFreestyleEmailUiAppUi* AppUi();
	
	void SetSortCriteria( TFSMailSortCriteria aSortCriteria );
	TFSMailSortCriteria SortCriteria();

	void SortL();
	
	/**
	 * Replaces the message pointer of the given item. Previous message is destroyed
	 * if owned by the item. Also message pointer in the parent node is updated if
	 * necessary.
	 * @param  aIndex  Index of the item for which message pointer is replaced
	 * @param  aNewPtr Pointer to the new message for the item. Ownership is transferred
	 *                 if the updated item is a message item (not divider).
	 */
	void ReplaceMessagePtr( TInt aIndex, CFSMailMessage* aNewPtr );
	
	/**
	 * Get index for item to be inserted in case the title dividers are not in use
	 * @param  aMessageToInsert    The message which should be inserted
	 * @param  aSortCriteria       The sorting criteria to be used in insertion
	 * @return The index within the model for the new item.
	 */
    TInt GetInsertionPointL( const CFSEmailUiMailListModelItem& aItemToInsert ) const;
    
    /**
     * Get index for item to be inserted in case the title dividers are in use.
     * also gets the index of the parent node and place of the new item under its parent.
     * @param  aMessageToInsert    The message which should be inserted
     * @param  aSortCriteria       The sorting criteria to be used in insertion
     * @param  aChildIdx           On return, contains index of the item to be inserted relative to its parent node.
     * @param  aParentNodeIdx      On return, contains the index of the parent node within the model.
     *                             KErrNotFound if a new node is needed for the item.
     * @return The index within the model for the new item.
     */ 
	TInt GetInsertionPointL( const CFSEmailUiMailListModelItem& aItemToInsert, //!< input arg
	                         TInt& aChildIdx,                                  //!< output arg
	                         TInt& aParentNodeIdx ) const;                     //!< output arg

	static TInt CompareModelItemsL( const CFSEmailUiMailListModelItem& aItem1,
	                                const CFSEmailUiMailListModelItem& aItem2 );
	
	/**
	 * Compares given messages using the given sorting mode. 
	 * @return Positive value if aMessage1 is greater, negative value if aMessage1 is smaller, and 0 
	 *         if the messages are equal.
	 */
	static TInt CompareMessagesL( const CFSMailMessage& aMessage1, 
	                              const CFSMailMessage& aMessage2, 
	                              TFSMailSortField aSortField );	    

	static TBool MessagesBelongUnderSameSeparatorL( const CFSMailMessage& aMessage1, 
	                                                const CFSMailMessage& aMessage2,
	                                                TFSMailSortField aSortField );
	
	/**
	 * Resets the data. Use this instead of deleting the model to avoid NULL
	 * pointer checks.
	 */
    void Reset();

    /**
     * Returns ItemIds for items under specific node.
     */
    void GetItemIdsUnderNodeL( const TFsTreeItemId aNodeId, RFsTreeItemIdList& aMessageIds ) const;
    
protected: // construction
    
    // C++ constructor.
	CFSEmailUiMailListModel( CFreestyleEmailUiAppUi* aAppUi );
	// Second phase constructor.
	void ConstructL( TBool aSearchList );

private: // data

    // Pointer to app ui. Not own.
	CFreestyleEmailUiAppUi* iAppUi;
  	RPointerArray<CFSEmailUiMailListModelItem> iItems;   
  	TFSMailSortCriteria iSortCriteria;
  };

#endif //__FREESTYLEEMAILUI_MAILLISTMODEL_H__

