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
* Description:  ESMR mr entry processor inline methods 
*
*/


// -----------------------------------------------------------------------------
// CESMREntryProcessor::ScenarioData
// -----------------------------------------------------------------------------
//
inline const TESMRScenarioData& CESMREntryProcessor::ScenarioData() const
    {
    return iScenData;
    }

// -----------------------------------------------------------------------------
// CESMREntryProcessor::ContainsProcessedEntry
// -----------------------------------------------------------------------------
//
inline TBool CESMREntryProcessor::ContainsProcessedEntry() const
    {
    return (TBool) iESMREntry;
    }
