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
* Description: This file defines class MFsDelayedLoadingInterface.
*
*/



#ifndef __FSDELAYEDLOADER_H__
#define __FSDELAYEDLOADER_H__


#include<e32cmn.h>
#include<coemain.h>
#include "FSEmailBuildFlags.h"

class CFSEmailUiContactHandler;
class CIdle;


/**
 * MFsDelayedLoadingInterface
 */
class MFsDelayedLoadingInterface
	{
	public:
		virtual TInt DelayLoadCallback() = 0;
	};


/**
 * CFsDelayedLoader
 */
class CFsDelayedLoader: public CCoeStatic
{
public:
	static CFsDelayedLoader* InstanceL();
	void Start();
	~CFsDelayedLoader();
	CFSEmailUiContactHandler* GetContactHandlerL();
	void AppendLoader( MFsDelayedLoadingInterface& aLoader );
	void RemoveLoader( MFsDelayedLoadingInterface& aLoader );
public:	
	static TInt IdleCallbackFunction( TAny* aParam );
private:
	CFsDelayedLoader();
	void ConstructL();
private:
  	CIdle* iIdle;
    TCallBack* iIdleCallback;
    CFSEmailUiContactHandler* iContactHandler;
	RPointerArray<MFsDelayedLoadingInterface> iLoaders;
	TInt iLoaderIndex;
};


#endif
