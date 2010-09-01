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
* Description:  ESMR observer for the custom list component
*
*/

#ifndef __MESMRRESPONSEOBSERVER_H__
#define __MESMRRESPONSEOBSERVER_H__

//#include "esmrguidef.h"

/**
 *  Observer interface for the response area
 *
 *  @lib esmrgui.lib
 */
 class MESMRResponseObserver
	{
public:
	/**
	 * Called when user selects the response for MR. It can be decline, tentatibe or accept.
	 *
	 * @param aType - Response value
	 * @return Boolean ETrue/EFalse to inform was the response succesfully sent.
	 */
	virtual TBool Response( TInt aType ) = 0;	
	/**
	 * Method to be called when UI needs to be closed.
	 */
	virtual void ExitDialog() = 0;	
	/**
	 * Method to be called when user wants to change the response value.
	 * Called from CESMRResponseReadyField.
	 */
	virtual void ChangeReadyResponseL() = 0;
	};
	
#endif // __MESMRRESPONSEOBSERVER_H__
