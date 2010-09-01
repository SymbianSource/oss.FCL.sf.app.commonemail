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
* Description:  FSMailServer document definition
*
*/


#ifndef C_FSEMAILGLOBALDIALOGSDOCUMENT_H
#define C_FSEMAILGLOBALDIALOGSDOCUMENT_H

// SYSTEM INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CEikApplication;


class CFsEmailGlobalDialogsDocument : public CAknDocument
    {
public:
    static CFsEmailGlobalDialogsDocument* NewL( CEikApplication& aApp );
    static CFsEmailGlobalDialogsDocument* NewLC( CEikApplication& aApp );
  	~CFsEmailGlobalDialogsDocument();

public: // from CAknDocument
    CEikAppUi* CreateAppUiL();

private:
    void ConstructL();
    CFsEmailGlobalDialogsDocument( CEikApplication& aApp );
    };


#endif // C_FSEMAILGLOBALDIALOGSDOCUMENT_H
