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
* Description:  Contains headers needed in plugin
*
*/

#ifndef IPSPLGHEADERS_H
#define IPSPLGHEADERS_H

// System includes
#include <e32cmn.h>
#include <e32def.h>
#include <e32base.h>
#include <e32property.h>

#include <f32file.h>

#include <eikenv.h>  // for rich text para and char format layer
#include <eikrutil.h>

#include <coemain.h> // for rich text para and char format layer

#include <charconv.h>

// cenrep
#include <centralrepository.h>
// ecom
#include <ecom/implementationproxy.h>
// mtm
#include <MTMStore.h>
#include <mtmuibas.h>
// mtcl
#include <mtclbase.h>
#include <mtclreg.h>
// msv
#include <msvstd.hrh>
#include <msvapi.h>     // Messaging framework
#include <msvuids.h>
#include <msvstd.h>
// miut
#include <miutset.h>    // MTM identifiers
#include <miuthdr.h>    // TMsvEmailEntry
#include <miut_err.h>   // KErrImapServerBusy
#include <miutmsg.h>
#include <miutconv.h>
// imap
#include <imapset.h>
#include <imapcmds.h>
// pop
#include <pop3set.h>
#include <popcmtm.h>
// smtp
#include <smtpset.h>

#ifndef FF_CMAIL_INTEGRATION
#include <timrfc822datefield.h>
#endif // FF_CMAIL_INTEGRATION

#include <cmsvattachment.h>           // CMsvAttachment
#include <mmsvattachmentmanager.h>    // MMsvAttachmentManager
// cfsmail
//<cmail>
#include "cfsmailcommon.h"
#include "cfsmailmessage.h"
// sendui
#include <SendUiConsts.h>
#include <SenduiMtmUids.h>

#include "fsmailserverconst.h"
// mfsmail
#include "mfsmailboxsearchobserver.h"
#include "mfsmaileventobserver.h"
//</cmail>
// alwaysonline
#include <AlwaysOnlineManagerClient.h>

#include <CoreApplicationUIsSDKCRKeys.h>

//<cmail> defines.h is not used and neither can be found in the environment.
//#include <defines.h>
//</cmail>

#include <cemailaccounts.h>

// Internal headers

// base
#include "ipsplgsosbaseplugin.h"
#include "ipsplgbaseoperation.h"
#include "ipsplgsosbaseplugin.hrh"
#include "ipsplgcommon.h"

// smtp
#include "ipsplgsmtpservice.h"
#include "ipsplgsmtpoperation.h"
#include "ipsplgsmtpoperation.h"


// msg
#include "ipsplgmsgmapper.h"
#include "ipsplgmsgkey.h"
#include "ipsplgmsgiterator.h"      // CIpsPlgMsgIterator
#include "ipsplgmsgswap.h"        // TIpsPlgMsgSwap


// imap4
#include "ipsplgimap4connectop.h"
#include "ipsplgimap4plugin.h"
#include "ipsplgimap4connectop.h"
#include "ipsplgimap4populateop.h"
#include "ipsplgimap4fetchattachmentop.h"
#include "ipsplgimap4moveremoteop.h"
#include "ipsplgimap4fetchattachmentop.h"
#include "ipsplgimap4connectop.h"
#include "ipsplgconnectandrefreshfolderlist.h"


// pop3
#include "ipsplgpop3plugin.h"
#include "ipsplgpop3connectop.h"
#include "ipsplgpop3fetchoperation.h"
#include "ipsplgpop3connectop.h"


//<cmail>
//</cmail>

// search
#include "ipsplgsearch.h"
#include "ipsplgsearchop.h"
#include "ipsplgtextsearcher.h"

// settings
#include "ipsplgsettingsobserver.h"
#include "ipssetdataapi.h"
#include "ipssetdatastorer.h"
#include "ipssetdata.h"
#include "IpsSetDataExtension.h"
#include "ipssetutils.h"
#include "ipssetutilsconsts.hrh"

// mrulist
#include "ipsplgmrulist.h"


// events
#include "ipsplgeventtimer.h"
#include "ipsplgeventhandler.h"
#include "ipsplgsyncstatehandler.h"

// observers
#include "mfsmailrequestobserver.h"
#include "ipsplgsettingsobserver.h"
#include "ipsplgpropertywatcher.h"


// panics
#include "ipsplgpanic.h"


// operations
#include "ipsplgtimeroperation.h"
#include "ipsplgoperationwait.h"
#include "ipsplgonlineoperation.h"
#include "ipsplgdisconnectop.h"
#include "ipsplgservicedeletionop.h"
#include "ipsplgdeleteremote.h"
#include "ipsplgdeletelocal.h"
#include "ipsplgsingleopwatcher.h"









// #include "ipsplgheaders.h"

#endif //IPSPLGHEADERS_H



