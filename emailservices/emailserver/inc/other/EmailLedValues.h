/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Public API for 3rd party applications to monitor and control LED via Publish&Subcsribe.
*
*/


#ifndef __EMAILLEDVALUES_H
#define __EMAILLEDVALUES_H

/**
* Email LED controller category
*/
const TUid KPropertyUidEmailLedCategory = { 0x10210E73 };

/**
* Email LED controller key
*/
const TUint KEmailLEDOnRequest = { 0 };

/**
* Value for EmailLEDOnRequest. Use this value to enable
* blinking. Value is 0 by default (not blinking)
*/
const TInt KSetEmailLedOn = { 1 };

#endif // __EMAILLEDVALUES_H

// End of File
