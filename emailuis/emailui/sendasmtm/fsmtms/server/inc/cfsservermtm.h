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
* Description:  Definition of the class CFsServerMtm.
*
*/


#ifndef CFSSERVERMTM_H
#define CFSSERVERMTM_H

// INCLUDES
#include <mtsr.h>
//<cmail>
//<cmail>
#include "fsmtmsuids.h"
//</cmail>
//</cmail>

//<cmail>
#include <msvstd.hrh>
class CMsgConverter;
class CMsgActive;
enum TEasEmailMTMCmds
	{
	KEasEmailMTMReadEntry = KMtmFirstFreeMtmFunctionId, //from <msvstd.hrh>
	KEasEmailMTMAddEmailAttachment,
	KEasEmailMTMDeleteEntry,
	// KEEP THIS ALWAYS LAST!
	KEasEmailMTMLastCommand
	};
//</cmail>

/**
 * Freestyle Email server side dll
 *
 * @since S60 3.1
 * @lib fsservermtm.lib
*/
class CFsServerMtm : public CBaseServerMtm
    {
public: // Construction and destruction 

    /**
     * Constructs new object.
     * @return created object.
    */
    IMPORT_C static CFsServerMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, 
        CMsvServerEntry*  aEntry);
    
    /**
     * Destructor.
    */
    ~CFsServerMtm();

public: // From CBaseServerMtm    
    void CopyToLocalL(const CMsvEntrySelection& aSelection,
            TMsvId aDestination, TRequestStatus& aStatus);
    
    void MoveToLocalL(const CMsvEntrySelection& aSelection,
        TMsvId aDestination, TRequestStatus& aStatus);
    
    void CopyFromLocalL(const CMsvEntrySelection& aSelection,
        TMsvId aDestination, TRequestStatus& aStatus);
    
    void MoveFromLocalL(const CMsvEntrySelection& aSelection,
        TMsvId aDestination, TRequestStatus& aStatus);
    
    void CopyWithinServiceL(const CMsvEntrySelection& aSelection,
        TMsvId aDestination, TRequestStatus& aStatus);
    
    void MoveWithinServiceL(const CMsvEntrySelection& aSelection,
        TMsvId aDestination, TRequestStatus& aStatus);
    
    void DeleteAllL(const CMsvEntrySelection& aSelection, 
        TRequestStatus& aStatus);
    
    void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
    
    void ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
    
    void StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand,             
           const TDesC8& aParameter, TRequestStatus& aStatus);
    
    TBool CommandExpected();
    
    const TDesC8& Progress();
    //<cmail>
    void AddEmailAsAttachmentL( const TMsvId& aParentEmailId, const TDesC& aAttachmentPath );
    //</cmail>

protected: // From CBaseServerMtm   
    void DoCancel();
    
    void DoRunL();
    
    void DoComplete(TInt aError);

private: // construction
    /**
     * Constructor.
     *
     * @param aRegisteredMtmDll  To access self registry information.
     * @param aInitialEntry
     */
    CFsServerMtm(CRegisteredMtmDll& aRegisteredMtmDll, 
        CMsvServerEntry* aInitialEntry);

	// <cmail>
    /**
     * Tells if feature Email Attached In Email is ON or OFF.
     * State is looked from cenrep.
     */
	TBool 				iEmailAttachedInEmail;
	
	CMsvServerEntry*    iMsvServerEntry;
	TRequestStatus*		iConvertToNestedEmailRequestStatus;
	TBool               iProcessingNestedEmail;
	RLibrary 			iConvLibrary;
	CMsgConverter* 		iConverter;
	CMsgActive*       	iConvertToNestedEmail; 
	// </cmail>

    /**
     * Second phase constructor.
     */
    void ConstructL();
    };

#endif //CFSSERVERMTM_H     

// End of File
