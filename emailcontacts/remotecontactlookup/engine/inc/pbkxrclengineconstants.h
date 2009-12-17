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
* Description:  Definition of the class CPbkxRclSearchEngine.
*
*/


#ifndef PBKXRCLENGINECONSTANTS_H
#define PBKXRCLENGINECONSTANTS_H

// engine resource file
_LIT( KResourceFile, "pbkxrclengine.rsc" );

// engine icon file
_LIT( KEngineIconFile, "pbkxrclengine.mif" );

// Max matches.
const TInt KMaxMatches = 50;

// Default array granularity in RCL.
const TInt KArrayGranularity = 10;

// Add recipient action menu icon.
// <cmail> S60 UID update
#define KPbkxRclAddRecipientActionUid TUid::Uid( 0x2001FE14 )
// </cmail> S60 UID update

// Add recipient action menu priority.
const TInt KPbkxRclAddRecipientPriority = KMaxTInt;

// Path to avkon mif file.
//<cmail> removing hard coded paths
_LIT( KAvkonMif, "avkon2.mif" );
//</cmail>

// Unique identified to rcl setting item.
const TInt KPbkxRclSettingItemId = 0x53EF82CB;

// Extra space in list box entry item because of inserted tabulators
const TInt KExtraSpaceInListboxEntry = 10;

// Name format.
_LIT( KNameFormat, "%S %S" );

#endif
