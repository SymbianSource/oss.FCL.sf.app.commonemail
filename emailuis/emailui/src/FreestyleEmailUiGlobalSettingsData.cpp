/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email source file
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <e32base.h>
#include <StringLoader.h>
#include <barsread.h>
#include <FreestyleEmailUi.rsg>

// LOCAL INCLUDES
#include "FreestyleEmailUiGlobalSettingsData.h"

/**
 * C/C++ constructor for settings data, cannot throw
 */
CFsEmailUiGlobalSettingsData::CFsEmailUiGlobalSettingsData()
	{
    FUNC_LOG;
	}

/**
 * Two-phase constructor for settings data
 */
CFsEmailUiGlobalSettingsData* CFsEmailUiGlobalSettingsData::NewL()
	{
    FUNC_LOG;

	CFsEmailUiGlobalSettingsData* data = new( ELeave ) CFsEmailUiGlobalSettingsData;
	CleanupStack::PushL( data );
	data->ConstructL();
	CleanupStack::Pop( data );

	return data;
	}
	
/**
 *	Second phase for initializing settings data
 */
void CFsEmailUiGlobalSettingsData::ConstructL()
	{
    FUNC_LOG;

	SetMessageListLayout( 1 );
	SetBodyPreview( 1 );
	SetTitleDividers( 1 );
	SetDownloadNotifications( 0 );
	SetActiveIdle( 0 );
	SetWarnBeforeDelete( 1 );
	//SetDownloadHTMLImages( 1 );
	}
	
TBool& CFsEmailUiGlobalSettingsData::WarnBeforeDelete()
	{
    FUNC_LOG;
	return iWarnBeforeDelete;
	}

void CFsEmailUiGlobalSettingsData::SetWarnBeforeDelete(const TBool& aValue)
	{
    FUNC_LOG;

	iWarnBeforeDelete = aValue;

	}

TBool& CFsEmailUiGlobalSettingsData::MessageHeader()
	{
    FUNC_LOG;
	return iMessageHeader;
	}

void CFsEmailUiGlobalSettingsData::SetMessageHeader(const TBool& aValue)
	{
    FUNC_LOG;

	iMessageHeader = aValue;
	}

TBool& CFsEmailUiGlobalSettingsData::MessageListLayout()
	{
    FUNC_LOG;
	return iMessageListLayout;
	}

void CFsEmailUiGlobalSettingsData::SetMessageListLayout(const TBool& aValue)
	{
    FUNC_LOG;
	iMessageListLayout = aValue;
	}

TBool& CFsEmailUiGlobalSettingsData::BodyPreview()
	{
    FUNC_LOG;
	return iBodyPreview;
	}

void CFsEmailUiGlobalSettingsData::SetBodyPreview(const TBool& aValue)
	{
    FUNC_LOG;

	iBodyPreview = aValue;
	}

TBool& CFsEmailUiGlobalSettingsData::TitleDividers()
	{
    FUNC_LOG;
	return iTitleDividers;
	}

void CFsEmailUiGlobalSettingsData::SetTitleDividers( const TBool& aValue )
	{
    FUNC_LOG;

	iTitleDividers = aValue;
	}

TBool& CFsEmailUiGlobalSettingsData::DownloadNotifications()
	{
    FUNC_LOG;
	return iDownloadNotifications;
	}

void CFsEmailUiGlobalSettingsData::SetDownloadNotifications( const TBool& aValue )
	{
    FUNC_LOG;

	iDownloadNotifications = aValue;
	}

TInt& CFsEmailUiGlobalSettingsData::ActiveIdle()
	{
    FUNC_LOG;
	return iActiveIdle;
	}

void CFsEmailUiGlobalSettingsData::SetActiveIdle( const TInt& aValue )
	{
    FUNC_LOG;
	iActiveIdle = aValue;

	}

void CFsEmailUiGlobalSettingsData::SetDownloadHTMLImages( const TBool& aValue )
    {
    FUNC_LOG;
    iDownloadHTMLImages = aValue;
    }

TBool& CFsEmailUiGlobalSettingsData::DownloadHTMLImages()
    {
    FUNC_LOG;
    return iDownloadHTMLImages;
    }
    
    
