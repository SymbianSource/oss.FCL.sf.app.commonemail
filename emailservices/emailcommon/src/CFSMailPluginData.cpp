/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSMailPluginData.
*
*/

// <qmail>
#include <nmcommonheaders.h>
// </qmail>

#include "emailtrace.h"
#include "CFSMailPluginData.h"

// -----------------------------------------------------------------------------
// CFSMailPluginData::CFSMailPluginData
// -----------------------------------------------------------------------------
CFSMailPluginData::CFSMailPluginData()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSMailPluginData::~CFSMailPluginData
// -----------------------------------------------------------------------------
CFSMailPluginData::~CFSMailPluginData()
{
    FUNC_LOG;
	if(iPlugin)
		{
		delete iPlugin;
		iPlugin = NULL;
		CFSMailPlugin::Close();
		}
}
