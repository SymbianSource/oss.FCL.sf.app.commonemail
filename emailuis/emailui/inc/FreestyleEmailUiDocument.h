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
* Description:  FreestyleEmailUi document definition
*
*/


#ifndef __FREESTYLEEMAILUI_DOCUMENT_H__
#define __FREESTYLEEMAILUI_DOCUMENT_H__

// SYSTEM INCLUDES
//<cmail>
#include <AknDoc.h>
#include <alf/alfenv.h>
//</cmail>

// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
class CEikApplication;

class CFreestyleEmailUiDocument : public CAknDocument
    {
public:
    static CFreestyleEmailUiDocument* NewL(CEikApplication& aApp);
    static CFreestyleEmailUiDocument* NewLC(CEikApplication& aApp);
  	~CFreestyleEmailUiDocument();

public: // from CAknDocument
    CEikAppUi* CreateAppUiL();

private:
    void ConstructL();
    CFreestyleEmailUiDocument(CEikApplication& aApp);
    
    CAlfEnv* iEnv;
    };


#endif // __FREESTYLEEMAILUI_DOCUMENT_H__
