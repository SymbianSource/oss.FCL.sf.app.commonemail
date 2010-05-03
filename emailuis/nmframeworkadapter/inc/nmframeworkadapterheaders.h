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
* Description:
*
*/

#ifndef NMFRAMEWORKADAPTERHEADERS_H_
#define NMFRAMEWORKADAPTERHEADERS_H_

// Qt
#include <QtPlugin>
#include <QByteArray>
#include <QTextCodec>
#include <QRegExp>
#include <QTimer>
#ifdef Q_OS_SYMBIAN
#include <xqsharablefile.h>
#endif
// Orbit

// nmailuiengine

// nmailbase
#include <nmmailbox.h>
#include <nmcommon.h>
#include <nmmessageenvelope.h>
#include <nmmessage.h>
#include <nmconverter.h>
#include <nmcleanuputils.h>
#include <nmstoreenvelopesoperation.h>
#include <nmcheckoutboxoperation.h>

// nmframeworkadapter
#include "nmframeworkadapter.h"
#include "nmfwamessagefetchingoperation.h"
#include "nmfwamessagecreationoperation.h"
#include "nmfwaforwardmessagecreationoperation.h"
#include "nmfwareplymessagecreationoperation.h"
#include "nmfwastoreenvelopesoperation.h"
#include "nmfwamessagesendingoperation.h"
#include "nmfwaaddattachmentsoperation.h"
#include "nmfwaremoveattachmentoperation.h"
#include "nmfwastoremessageoperation.h"
#include "nmfwacheckoutboxoperation.h"
#include "nmfwamessagesendingoperation.h"
#include "nmfwamessagepartfetchingoperation.h"

// fs email
#include <CFSMailCommon.h>
#include <CFSMailClient.h>
#include <CFSMailRequestObserver.h>
#include <CFSMailFolder.h>
#include <CFSMailBox.h>
#include <MFSMailIterator.h>

//Symbian
#include <utf.h> 


#endif /* NMFRAMEWORKADAPTERHEADERS_H_ */
