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
* Description:  Priority field for viewers
*
*/


#ifndef __CESMRVIEWERPRIORITYFIELD_H__
#define __CESMRVIEWERPRIORITYFIELD_H__

#include <eikrted.h>
#include <eikedwob.h>

#include "cesmrviewerlabelfield.h"

/**
 * This class shows the priority of the event
 */
NONSHARABLE_CLASS( CESMRViewerPriorityField ) : public CESMRViewerLabelField
    {
public:
    /**
     * Creates new CESMRViewerPriorityField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerPriorityField* NewL();
    
    /**
     * C++ Destructor.
     */
    ~CESMRViewerPriorityField();
    
public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    
private: // Implementation
    CESMRViewerPriorityField();
    void ConstructL();
    };

#endif //__CESMRVIEWERPRIORITYFIELD_H__
