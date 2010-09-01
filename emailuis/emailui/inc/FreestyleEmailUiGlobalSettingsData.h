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
* Description:  Freestyle Email header file
*
*/


#ifndef GLOBALSETTINGSSETTINGITEMLISTSETTINGS_H
#define GLOBALSETTINGSSETTINGITEMLISTSETTINGS_H
			
// SYSTEM INCLUDES
#include <e32std.h>

// CLASS DECLARATION
// <cmail> CBase inheritance
class CFsEmailUiGlobalSettingsData : public CBase
// </cmail>
	{
public:
	// construct and destroy
	static CFsEmailUiGlobalSettingsData* NewL();
	
		
private:
    void ConstructL();
	// constructor
	CFsEmailUiGlobalSettingsData();

public:
	TBool& WarnBeforeDelete();
	void SetWarnBeforeDelete( const TBool& aValue );
	TBool& MessageHeader();
	void SetMessageHeader( const TBool& aValue );
	TBool& MessageListLayout();
	void SetMessageListLayout( const TBool& aValue );
	TBool& BodyPreview();
	void SetBodyPreview( const TBool& aValue );
	TBool& TitleDividers();
	void SetTitleDividers( const TBool& aValue );
	TBool& DownloadNotifications();
	void SetDownloadNotifications( const TBool& aValue );
	TInt& ActiveIdle();
	void SetActiveIdle( const TInt& aValue );
	TBool& DownloadHTMLImages();
    void SetDownloadHTMLImages( const TBool& aValue );

private:
	TBool iWarnBeforeDelete;
	TBool iMessageHeader;
	TBool iMessageListLayout;
	TBool iBodyPreview;
	TBool iTitleDividers;
	TBool iDownloadNotifications;
	TBool iDownloadHTMLImages;
	TInt iActiveIdle;
	
	};
#endif // GLOBALSETTINGSSETTINGITEMLISTSETTINGS_H
