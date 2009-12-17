/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR Conflict popup for meeting request field
*
*/

#ifndef __CESMRCONFLICTPOPUP_H_
#define __CESMRCONFLICTPOPUP_H_

#include <e32base.h>
#include <e32cmn.h>
#include <calentry.h>

class CAknInfoPopupNoteController;
class MESMRCalEntry;
class MESMRMeetingRequestEntry;

/**
 * This class shows the conflict popup note when topic of the
 * response area is higlighted and there is conflicting meeting 
 */
NONSHARABLE_CLASS(CESMRConflictPopup ) : public CBase
    {
    public: // Construction and Destruction
	    /**
	     * Two-phased constructor. Creates and initializes 
	     * CESMRConflictPopup object.
	     * @return Pointer to Conflict popup object.
	     */
	    static CESMRConflictPopup* NewL( MESMRCalEntry& aEntry);
	    
	    /**
	     * Destructor.
	     */
	    ~CESMRConflictPopup();
	    
	    /**
	     * Shows the popup on the screen.
	     */
		void ShowPopup();
		
    private: // Implementation
		CESMRConflictPopup();
		void ConstructL(MESMRCalEntry& aEntry);
		void PrepareDisplayStringL();
		void InitializeL();

    private :
		CAknInfoPopupNoteController* iNote; // Own
		
		MESMRMeetingRequestEntry*	iEntry; //ref
    };

#endif /*__CESMRCONFLICTPOPUP_H_*/

// EOF