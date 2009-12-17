/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for application.
 *
*/

#ifndef __EMAILWIDGETSETTINGSAPPUI_H__
#define __EMAILWIDGETSETTINGSAPPUI_H__

#include <aknapp.h>
#include <aknViewAppUi.h>
#include <aknview.h>

#include <AknDoc.h>
#include <e32std.h>
#include <apgtask.h>
#include <eiklbo.h>
#include <u32std.h>

const TUid KUidWidgetSettApp={ 0x2002429E };

class CEmailWidgetSettingsAppUi : public CAknViewAppUi
	{
public:
	void ConstructL();
	~CEmailWidgetSettingsAppUi();

private: 
	void HandleCommandL(TInt aCommand);

private:
	void CreateListViewL();
	void LaunchEmailWizardL();
	
private:
	TInt			iCloseCount;
	};


class CEmailWidgetSettingsDocument : public CAknDocument
	{
public:
	void ConstructL();
	CEmailWidgetSettingsDocument(CEikApplication& aApp): CAknDocument(aApp) { }
	~CEmailWidgetSettingsDocument() { }

public:  
    
private:
	CEikAppUi* CreateAppUiL();
	};

class CEmailWidgetSettingsApplication : public CAknApplication
	{
private:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
    };

#endif // __EMAILWIDGETSETTINGSAPPUI_H__
// End of File
