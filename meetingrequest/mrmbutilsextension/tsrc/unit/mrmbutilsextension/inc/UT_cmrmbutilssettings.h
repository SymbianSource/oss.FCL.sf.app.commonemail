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
* Description: 
*
*/

#ifndef UT_CMRMBUTILSSETTINGS_H
#define UT_CMRMBUTILSSETTINGS_H

// INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitdecorators.h>

// FORWARD DECLARATIONS
class CFSMailClient;
class CRepository;
class CMRMBUtilsSettings;
class CFSMailBox;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CMRMBUtilsSettings ) : 
        public CEUnitTestSuiteClass
    {
public:  // Constructors and destructor

    static UT_CMRMBUtilsSettings* NewL();
    static UT_CMRMBUtilsSettings* NewLC();
    ~UT_CMRMBUtilsSettings();

private: // Constructors

    UT_CMRMBUtilsSettings();
    void ConstructL();

private: // New methods

     void SetupL();
     void SetupWithMailboxesL();
     void ResetcenRep();
     void Teardown();
     
     void T_SetDefaultMailboxSettingL();
     void T_GetDefaultMailboxSettingsL();

     void T_CenRepSetDefaultMailboxSettingL();
     void T_CenRepGetDefaultMailboxSettingsL();     
     
private: // Data

    CFSMailClient* iMailClient;
    CMRMBUtilsSettings* iSettings;
    CRepository* iCenRep;
    RPointerArray<CFSMailBox> iMailboxes;
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __MRMBUTILSEXTENSION_H__

// EOF
