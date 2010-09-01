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
* Description:  Definition of the class CFsMtmUi.
*
*/


#ifndef CFSUIMTM_H
#define CFSUIMTM_H

// INCLUDES
#include <mtmuibas.h>

/**
 * Freestyle Email UI dll
 *
 * @since S60 3.1
 * @lib fsuimtm.lib
 */
class CFsMtmUi : public CBaseMtmUi
    {
public: // Construction and destruction 

	/**
	 * Constructs new object.
	 * @return created object.
	*/
    static CFsMtmUi* NewL( CBaseMtm& aBaseMtm, 
        CRegisteredMtmDll& aRegisteredMtmDll );

    /**
     * Destructor.
    */
    ~CFsMtmUi( );

public: // From CBaseMtmUi

    CMsvOperation* CancelL( TRequestStatus& aStatus, 
        const CMsvEntrySelection& aSelection );

    // Context-specific
    CMsvOperation* OpenL( TRequestStatus& aStatus ); 
    
    CMsvOperation* OpenL( TRequestStatus& aStatus, 
        const CMsvEntrySelection& aSelection ); 

    CMsvOperation* CloseL( TRequestStatus& aStatus ); 

    CMsvOperation* CloseL( TRequestStatus& aStatus, 
        const CMsvEntrySelection& aSelection );

    // Launches editor/settings dialog as appropriate  
    CMsvOperation* EditL( TRequestStatus& aStatus );

    CMsvOperation* EditL( TRequestStatus& aStatus, 
        const CMsvEntrySelection& aSelection ) ;

    // Launches viewer/settings dialog as appropriate  
    CMsvOperation* ViewL( TRequestStatus& aStatus );

    CMsvOperation* ViewL( TRequestStatus& aStatus, 
        const CMsvEntrySelection& aSelection );

    // Copy and move
    CMsvOperation* CopyToL( const CMsvEntrySelection& aSelection, 
            TRequestStatus& aStatus );
    
    CMsvOperation* MoveToL( const CMsvEntrySelection& aSelection, 
            TRequestStatus& aStatus );
    
    CMsvOperation* CopyFromL( const CMsvEntrySelection& aSelection, 
        TMsvId aTargetId, TRequestStatus& aStatus );
    
    CMsvOperation* MoveFromL( const CMsvEntrySelection& aSelection, 
            TMsvId aTargetId, TRequestStatus& aStatus );

    // Create/delete/undelete
    CMsvOperation* CreateL( const TMsvEntry& aEntry, CMsvEntry& aParent, 
        TRequestStatus& aStatus );
    
    CMsvOperation* DeleteFromL( const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

    // MTM-specific
    void InvokeSyncFunctionL( TInt aFunctionId, 
        const CMsvEntrySelection& aSelection, TDes8& aParameter );
    
    CMsvOperation* InvokeAsyncFunctionL( TInt aFunctionId, 
        const CMsvEntrySelection& aSelection, 
        TRequestStatus& aCompletionStatus, TDes8& aParameter );

    TInt GetProgress( const TDesC8& aProgress, 
        TBuf<EProgressStringMaxLen>& aReturnString, TInt& aTotalEntryCount, 
        TInt& aEntriesDone, TInt& aCurrentEntrySize, 
        TInt& aCurrentBytesTrans ) const;

    // Reply/forward
    // CMsvOperation* ReplyL( long,unsigned int,class TRequestStatus & );
    // CMsvOperation* ForwardL( long,unsigned int,class TRequestStatus & );

    CMsvOperation* ReplyL( TMsvId aDestination, TMsvPartList aPartList, 
        TRequestStatus& aCompletionStatus );
    
    CMsvOperation* ForwardL( TMsvId aDestination, TMsvPartList aPartList, 
        TRequestStatus& aCompletionStatus );


protected: // from CBaseMtmUi
    void GetResourceFileName( TFileName& aFileName ) const;

protected: 
    /**
     * Constructor.
     *
     * @param aBaseMtm For manipulating a Message Server entry. 
     * @param aRegisteredMtmDll To access self registry information.
     */
    CFsMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll );

    /**
     * Second phase constructor.
     */    
    void ConstructL( );
    };


#endif // CFSUIMTM_H