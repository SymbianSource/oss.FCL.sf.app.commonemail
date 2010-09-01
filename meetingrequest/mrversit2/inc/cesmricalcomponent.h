/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Holds the definition of CESMRICalComponent.
*
*
*/


#ifndef CESMRICALCOMPONENT_H
#define CESMRICALCOMPONENT_H


// User includes.
#include "cesmricalbase.h"	// CESMRICalBase

/**
Abstract class of classes representing specific iCalendar components.
@publishedPartner
*/
class CESMRICalComponent : public CESMRICalBase
	{
public:	// Construction/destruction.
	~CESMRICalComponent();

	// Factory construction methods for concrete derived classes.
	IMPORT_C static CESMRICalComponent* CreateICalComponentL(const TDesC& aName, TICalMethod aMethod);
	IMPORT_C static CESMRICalComponent* CreateICalComponentLC(const TDesC& aName, TICalMethod aMethod);

	IMPORT_C static CESMRICalComponent* CreateICalComponentL(CESMRICalBase::TICalComponentType aType, TICalMethod aMethod);
	IMPORT_C static CESMRICalComponent* CreateICalComponentLC(CESMRICalBase::TICalComponentType aType, TICalMethod aMethod);

protected:	// Construction.
	CESMRICalComponent();
	};

#endif // CESMRICALCOMPONENT_H

// End of File
