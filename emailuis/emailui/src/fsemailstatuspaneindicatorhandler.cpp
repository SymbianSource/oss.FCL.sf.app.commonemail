/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Handles mailiindicator updating
*
*/


#include <centralrepository.h>
#include <coemain.h> // CCoeEnv
#include <coeaui.h> // CCoeAppUi
#include "emailtrace.h"

#include "cfsmailclient.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiConstants.h"
#include "fsemailstatuspaneindicatorhandler.h"
#include "commonemailcrkeys.h"
#include "freestyleemailcenrepkeys.h"




// FUNCTION DEFINITIONS
void TFsEmailStatusPaneIndicatorHandler::StatusPaneMailIndicatorHandlingL( TInt aMailBoxId  )
    {
    FUNC_LOG;
    //emailindicator control    
    CRepository* emailRepository = CRepository::NewL( KFreestyleEmailCenRep );    
    CleanupStack::PushL(emailRepository);
    CRepository* commonEmailRepository = CRepository::NewL( KCmailDataRepository );
    CleanupStack::PushL(commonEmailRepository);
    TInt numberOfMailboxes(0);

    emailRepository->Get(KNumberOfMailboxesWithNewEmails,numberOfMailboxes);
    if(numberOfMailboxes > 0)
        {
        RArray<TInt> repositoryIds;
        CleanupClosePushL(repositoryIds);
        TInt tmp (0); //used to store the mailbox and plugin ids
        for(TInt i = 1 ; i <= numberOfMailboxes * 2; i++ )
            {
            emailRepository->Get(KNumberOfMailboxesWithNewEmails+i, tmp);
            repositoryIds.Append(tmp);
            }
        // Here we check if the current mailbox id is already in the repository:
        TInt index = repositoryIds.Find(aMailBoxId);

        if(index != KErrNotFound)
            {
            repositoryIds.Remove(index);
            repositoryIds.Remove(index-1);
            // Rearrange the mailbox/mailplugin ids
            // in the repository
            for(TInt j = 0; j < repositoryIds.Count(); j++)
                {
                emailRepository->Set(KNumberOfMailboxesWithNewEmails + 1 + j, repositoryIds.operator [](j));
                }
            // And delete last two remaining keys, that contains old information:
            emailRepository->Delete(KNumberOfMailboxesWithNewEmails + ( numberOfMailboxes * 2 - 1 ));
            emailRepository->Delete(KNumberOfMailboxesWithNewEmails + ( numberOfMailboxes * 2 ));

            // Decrease the number of mailboxes and update the repository:
            numberOfMailboxes -= 1;
            emailRepository->Set(KNumberOfMailboxesWithNewEmails,numberOfMailboxes);

            // Finally update the mailbox name to indicator pop-up note if there is still unchecked mailboxes
            if(numberOfMailboxes > 0)
                {
                TInt lastMBoxIdInRepository (0);
                TInt lastMPluginIdInRepository (0);
                emailRepository->Get(KNumberOfMailboxesWithNewEmails+(numberOfMailboxes*2), lastMBoxIdInRepository);
                // Get matching email account.
                TFSMailMsgId prevMboxId;
                prevMboxId.SetId(lastMBoxIdInRepository);
                emailRepository->Get(KNumberOfMailboxesWithNewEmails+(numberOfMailboxes*2-1), lastMPluginIdInRepository);
                TUid lastMPluginUidInRepository = { lastMPluginIdInRepository };
                prevMboxId.SetPluginId(lastMPluginUidInRepository);                
                CFreestyleEmailUiAppUi* appUi = 
                static_cast<CFreestyleEmailUiAppUi*>( CCoeEnv::Static()->AppUi() );
                //Write the name commonEmailRepository
                CFSMailBox* prevMbox = appUi->GetMailClient()->GetMailBoxByUidL( prevMboxId );
                if ( prevMbox )
                    {
                    commonEmailRepository->Set( KCmailNewEmailDisplayText, prevMbox->GetName() );
                    }
                }
            }
        if(numberOfMailboxes == 0)
            {       
            // Set email indicator off.. user has checked the new emails in all the mailboxes
            TFsEmailUiUtility::ToggleEmailIconL(EFalse);
            }
        repositoryIds.Reset();
        CleanupStack::PopAndDestroy();//repositoryIds
        }
    CleanupStack::PopAndDestroy(2); //emailRepository, CommonEmailRepository
    }
