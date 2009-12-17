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
* Description:  Definition of the class CFscContactAction.
*
*/


#ifndef C_FSCCONTACTACTION_H
#define C_FSCCONTACTACTION_H

//<cmail>
#include "mfsccontactaction.h"
#include "mfscreasoncallback.h"
//</cmail>

/**
 *  ContactAction implementation.
 *
 *  @lib 
 *  @since S60 3.1
 */
class CFscContactAction : public CBase, public MFscContactAction
    {
    
public:

    /**
     * Two-phased constructor.
     *
     * @param aReasonCallback is reference to Reason callback interface
     * @param aContactActionUid is Uid of action
     * @param aActionType specifies action type
     * @param aActionMenuTextResourceId action menu text
     * @param aIcon specifies icon for the action
     */
    static CFscContactAction* NewL(
        MFscReasonCallback& aReasonCallback,
        TUid aContactActionUid,
        TUint64 aActionType,
        TInt aActionMenuTextResourceId,
        CGulIcon* aIcon );
                                        
   
    /**
     * Destructor.
     */
    virtual ~CFscContactAction();
    
public: // From MFscContactAction

    /**
     * @see MFscContactAction::Uid
     */
    virtual TUid Uid() const;
    
    /**
     * @see MFscContactAction::Type
     */
    virtual TUint64 Type() const;

    /**
     * @see MFscContactAction::Icon
     */
    virtual const CGulIcon* Icon() const;

    /**
     * @see MFscContactAction::ActionMenuText
     */
    virtual const TDesC& ActionMenuText() const;

    /**
     * @see MFscContactAction::ReasonL
     */
    virtual void ReasonL( TInt aReasonId, HBufC*& aReason ) const;

public: // methods

    /**
     * Sets action icon
     */
    virtual void SetIcon( CGulIcon* aIcon );
    
private:

    /**
     * Constructor
     *
     * @param aReasonCallback is reference to Reason callback interface
     * @param aContactActionUid is Uid of action
     * @param aActionType specifies action type
     * @param aIcon specifies icon for the action   
     */
    CFscContactAction(
        MFscReasonCallback& aReasonCallback,
        TUid aContactActionUid,
        TUint64 aActionType,
        CGulIcon* aIcon );

    /**
     * Second phase constructor
     *
     * @param aActionMenuTextResourceId action menu text
     */
    void ConstructL( TInt aActionMenuTextResourceId );

private: // data

    /**
     * Uid of action
     */    
    TUid iContactActionUid;

    /**
     * Type of action
     */    
    TUint64 iActionType;

    /**
     * Own: Icon for action
     */    
    CGulIcon* iIcon;

    /**
     * Own: Action menu text of ation
     */    
    TDesC* iActionMenuText;

    /**
     * Reference to Reason callback interface
     */    
    MFscReasonCallback& iReasonCallback;

    };

#endif // C_FSCCONTACTACTION_H
