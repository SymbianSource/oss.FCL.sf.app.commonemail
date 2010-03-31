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
* Description: This file defines class CCalenEditorsPlugin.
*
*/


#ifndef C_CALENEDITORSPLUGIN_H
#define C_CALENEDITORSPLUGIN_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <magnentryui.h>

/**
 * Resolver for loading correct ECOM Implementation for 
 * calendar entry editors.  
 */ 
NONSHARABLE_CLASS( CCalenEditorsPlugin ) : public CBase,
                                           public MAgnEntryUi
    {
public: // factory methods and destructor
    /**
     * Loads and creates default Calendar editors implementation 
     * @return New instance of CCalenEditorsPlugin implementation
     */
    static CCalenEditorsPlugin* NewL();

    /**
    * Destructor.
    */
    virtual ~CCalenEditorsPlugin();

private: // data
    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
     * Plugin implementations should leave this untouched, as 
     * inline destructor and constructor method take care of 
     * ECom releasing.
     */
    TUid iDtor_ID_Key;

    };

#include "caleneditorsplugin.inl"

#endif // C_CALENEDITORSPLUGIN_H
// EOF
