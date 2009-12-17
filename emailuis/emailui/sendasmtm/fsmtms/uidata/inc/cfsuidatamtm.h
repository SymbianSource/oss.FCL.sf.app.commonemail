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
* Description:  Definition of the class CFsUiDataMtm.
*
*/


#ifndef CFSUIDATAMTM_H
#define CFSUIDATAMTM_H

// INCLUDES
#include <mtudcbas.h>

// =========
// Templates
// =========
template <class T> class CleanupResetAndDestroyClose
    {
public:
    inline static void PushL(T& aRef);
private:
    static void Close(TAny *aPtr);
    };

template <class T> inline void CleanupResetAndDestroyClosePushL(T& aRef);

template <class T> inline void CleanupResetAndDestroyClose<T>::PushL(T& aRef)
    {
    CleanupStack::PushL(TCleanupItem(&Close, &aRef));
    }

template <class T> void CleanupResetAndDestroyClose<T>::Close(TAny *aPtr)
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    static_cast<T*>(aPtr)->Close();
    }

template <class T> inline void CleanupResetAndDestroyClosePushL(T& aRef)
    {
    CleanupResetAndDestroyClose<T>::PushL(aRef);
    }

/**
*  Base class for UI Data MTM components.
*
 * @since S60 3.1
 * @lib fsuidatamtm.lib 
*/
class CFsUiDataMtm :
    public CBaseMtmUiData, public MMsvSessionObserver
    {
public: // New Functions
    /**
    * Factory method. Called from MTM ui data registry.
    */
    IMPORT_C static CFsUiDataMtm* NewL(
        CRegisteredMtmDll& aRegisteredMtmDll);


private: // From CBaseMtmUiData
    HBufC* StatusTextL(const TMsvEntry& aContext) const;
    
    TInt QueryCapability(TUid aFunctionId, TInt& aResponse) const;
    
    TInt OperationSupportedL(
        TInt aOperationId, const TMsvEntry& aContext) const;

    TBool CanUnDeleteFromEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanViewEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanReplyToEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanOpenEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanForwardEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanEditEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanDeleteServiceL(
        const TMsvEntry& aService, TInt& aReasonResourceId) const;

    TBool CanDeleteFromEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanCreateEntryL(
        const TMsvEntry& aParent,
        const TMsvEntry& aNewEntry, TInt& aReasonResourceId) const;

    TBool CanCreateEntryL(
        const TMsvEntry& aParent,
        TMsvEntry& aNewEntry, TInt& aReasonResourceId) const;

    TBool CanCopyMoveToEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanCopyMoveFromEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanCloseEntryL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    TBool CanCancelL(
        const TMsvEntry& aContext, TInt& aReasonResourceId) const;

    const CBitmapArray& ContextIcon(
        const TMsvEntry& aContext, TInt aStateFlags) const;

    const CArrayFix<TMtmUiFunction>& MtmSpecificFunctions() const;

private: // From CBaseMtmUiData
    void GetResourceFileName(TFileName& aFileName) const;
    
    void PopulateArraysL();

private: // From MMsvSessionObserver
    void HandleSessionEventL(
        TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

private: // Constructor & Destructor
	/**
	 * Constructor.
	 *
	 * @param aRegisteredMtmDll  To access self registry information.
	 */
	CFsUiDataMtm(CRegisteredMtmDll& aRegisteredMtmDll);

	/**
     * Second phase constructor.
     */    
    void ConstructL();
    
    ~CFsUiDataMtm();

private: // Data
	RArray<TUint> iMailboxIds;

    };


#endif // CFSUIDATAMTM_H
// End of File
    