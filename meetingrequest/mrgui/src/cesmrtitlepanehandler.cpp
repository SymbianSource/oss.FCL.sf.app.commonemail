/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ESMR title pane handler implementation

 *
*/


#include "emailtrace.h"
#include "cesmrtitlepanehandler.h"



#include <akntitle.h>

#include <eikspane.h>

#include <avkon.hrh> // EEikStatusPaneUidTitle



CESMRTitlePaneHandler::CESMRTitlePaneHandler( CEikonEnv& aEnv )

: iEikEnv( aEnv )

	{
    FUNC_LOG;
	// Do nothing

	}

	

void CESMRTitlePaneHandler::ConstructL()

	{
    FUNC_LOG;
	// Do nothing

	}	

	

CESMRTitlePaneHandler* CESMRTitlePaneHandler::NewL( CEikonEnv& aEnv )

	{
    FUNC_LOG;
	CESMRTitlePaneHandler* self = new (ELeave) CESMRTitlePaneHandler( aEnv );

	CleanupStack::PushL( self );

	self->ConstructL();

	CleanupStack::Pop( self );

	return self;

	}

	

CESMRTitlePaneHandler::~CESMRTitlePaneHandler()

	{
    FUNC_LOG;
	delete iSaveTitlePaneText;

	}	

	

void CESMRTitlePaneHandler::SetNewTitle( const TDesC* aNewTitle )

	{
    FUNC_LOG;
	TRAPD( error, SetTitlePaneTextL( aNewTitle ) );

	if ( error != KErrNone )

		{

		iEikEnv.HandleError( error );

		}

	}



void CESMRTitlePaneHandler::Rollback()

	{
    FUNC_LOG;
    if ( iTitlePane && iSaveTitlePaneText )

        {

        // iTitlePane takes ownership of iSaveTitlePaneText

        iTitlePane->SetText( iSaveTitlePaneText );

        iSaveTitlePaneText = NULL;

        }	

	}



void CESMRTitlePaneHandler::SetTitlePaneTextL( const TDesC* aNewTitle )

	{
    FUNC_LOG;
	// Reset saved state

    delete iSaveTitlePaneText;

    iSaveTitlePaneText = NULL;



    CEikStatusPane* statusPane = iEikEnv.AppUiFactory()->StatusPane();

    if (statusPane)

        {

        if (statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle))

            .IsPresent())

            {

            iTitlePane = static_cast<CAknTitlePane*>

                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));

            if (iTitlePane->Text())

                {

                iSaveTitlePaneText = iTitlePane->Text()->AllocL();

                }

            if (aNewTitle)

                {

                iTitlePane->SetTextL(*aNewTitle);

                }

            else

                {

                iTitlePane->SetTextToDefaultL();

                }

            }

        }

	}

	

// End of file

