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
* Description:  ESMR base field for viewer fields in CESMRListComponent
*
*/


#ifndef CESMRVIEWERLABELFIELD_H
#define CESMRVIEWERLABELFIELD_H

#include <eikedwin.h>
#include <eikrted.h>
#include <eikedwob.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

#include "cesmrfield.h"
#include "cesmrlayoutmgr.h"
#include "cesmriconfield.h"

class CEikLabel;
class CESMREditor;

/**
 * Base class for most of the fields in viewer mode.
 */
class CESMRViewerLabelField : public CESMRIconField
    {
public:
    /**
     * Creates new CESMRViewerLabelField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    IMPORT_C static CESMRViewerLabelField* NewL();

    /**
     * C++ destructor
     */
    IMPORT_C ~CESMRViewerLabelField();

public: //from CESMRField
    IMPORT_C void InitializeL();

    IMPORT_C void FocusChanged( TDrawNow aDrawNow );

protected: //Implementation
    IMPORT_C CESMRViewerLabelField();
    IMPORT_C void ConstructL( TAknsItemID aIconID );
    IMPORT_C void ConstructL();

protected:
    /// Ref: Label for the text in the field
    CEikLabel* iLabel;
    };

#endif  // __CESMRVIEWERLABELFIELD_H__
