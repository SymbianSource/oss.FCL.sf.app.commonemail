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
* Description: Declares autosaver class for emailui
*
*/



#ifndef __FSAUTOSAVER_H__
#define __FSAUTOSAVER_H__


class CAlfEnv;


#include<e32base.h>
#include"FreestyleEmailUiUtilities.h"


class CFsAutoSaver: public CBase, public MFSEmailUiGenericTimerCallback
{
public:
	static CFsAutoSaver* NewL( CAlfEnv& aEnv, TInt aTimeDelayMs );
	void ReportActivity();
	~CFsAutoSaver();
	void Enable( TBool aEnable );
	TBool IsEnabled();
public: // from MFSEmailUiGenericTimerCallback
	void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );
private:
	CFsAutoSaver( CAlfEnv& aEnv, TInt aTimeDelayMs );
	void ConstructL();
private:
	CAlfEnv& iEnv;
	TInt iTimeDelayMs;
	CFSEmailUiGenericTimer* iTimer;
	TBool iEnabled;
};


#endif
