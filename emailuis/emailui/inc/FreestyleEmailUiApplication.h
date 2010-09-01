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
* Description:  FreestyleEmailUi application entry point definitions
*
*/


#ifndef __FREESTYLEEMAILUI_APPLICATION_H__
#define __FREESTYLEEMAILUI_APPLICATION_H__

#include <aknapp.h>


class CFreestyleEmailUiApplication : public CAknApplication
    {
public:  // from CAknApplication
    TUid AppDllUid() const;

protected: // from CAknApplication
    CApaDocument* CreateDocumentL();
    };

#endif 
