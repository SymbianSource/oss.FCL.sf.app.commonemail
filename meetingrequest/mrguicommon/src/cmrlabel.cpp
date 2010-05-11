/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  URL Parser ECom plugin entry point
*
*/
#include "cmrlabel.h"
#include "nmrcolormanager.h"

#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <aknsconstants.hrh>
#include <gdi.h>

#include "emailtrace.h"

// ---------------------------------------------------------------------------
// CMRLabel::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMRLabel* CMRLabel::NewL()
    {
    FUNC_LOG;
    
    CMRLabel* self = new (ELeave) CMRLabel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRLabel::ConstructL
// ---------------------------------------------------------------------------
//
void CMRLabel::ConstructL()
    {
    FUNC_LOG;

    NMRColorManager::SetColor( *this, 
                               NMRColorManager::EMRMainAreaTextColor );
    }

// ---------------------------------------------------------------------------
// CMRLabel::CMRLabel
// ---------------------------------------------------------------------------
//
CMRLabel::CMRLabel()
    {
    FUNC_LOG;

    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRLabel::~CMRLabel
// ---------------------------------------------------------------------------
//
EXPORT_C CMRLabel::~CMRLabel()
    {
    FUNC_LOG;

    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRLabel::FocusChanged
// ---------------------------------------------------------------------------
//
void CMRLabel::FocusChanged( TDrawNow aDrawNow )
    {
    FUNC_LOG;

    CEikLabel::FocusChanged( aDrawNow );
    }

// ---------------------------------------------------------------------------
// CMRLabel::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRLabel::SizeChanged()
    {
    FUNC_LOG;

    CEikLabel::SizeChanged();
    
    NMRColorManager::SetColor(
            *this, 
            NMRColorManager::EMRMainAreaTextColor );
    }

// End of file
