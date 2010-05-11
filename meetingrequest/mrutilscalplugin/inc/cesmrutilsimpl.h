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
* Description:  Implementation of CMRUtils ECom API.
*
*/


#ifndef CESMRUTILSIMPL_H
#define CESMRUTILSIMPL_H

//  INCLUDE FILES
#include <CMRUtils.h>

// FORWARD DECLARATIONS
class CESMRUtils;

/**
 * ECom implementation for es meeting request utilities.
 */
NONSHARABLE_CLASS(CESMRUtilsImpl) : 
        public CMRUtils,
        public MMRUtilsObserver
    {
public: // Constructors and destructors

    /**
     * ECom plug-in instantiation method.
     * @return instantiated ECom plug-in
     */
    static CESMRUtilsImpl* NewL();

    /**
     * Destructor.
     */
      ~CESMRUtilsImpl();

public: // From CMRUtils    	
    TInt DeleteWithUiL(
            const CCalEntry& aEntry,
            TMsvId aMailbox );
    TInt DeleteWithUiL( 
            CCalInstance* aInstance,
            TMsvId aMailbox );                            
    void SendWithUiL( 
            const CCalEntry& aEntry,
            TMsvId aMailbox );                            
    TAny* ExtensionL( 
            TUid aExtensionUid );        
    void SessionSetupL( 
            CCalSession& aCalSession,
            CMsvSession* aMsvSession );
    
public: // From MMRUtilsObserver

	void HandleCalEngStatus( 
	        TMRUtilsCalEngStatus aStatus );    	
	void HandleOperation( 
	        TInt aType,
            TInt aPercentageCompleted, 
            TInt aStatus );
    
private: // Implementation
    CESMRUtilsImpl();
    void ConstructL();            
    
private: // data            
    /// Own: ESMRUtils object
	CESMRUtils* iEsmrUtils;
    };

#endif // CESMRUTILSIMPL_H

// End of File
