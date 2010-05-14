/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Utility class for launching different file pickers
*
*/

#include "nmuiheaders.h"


/*!
    \class NmAttachmentPicker
    \brief Attachment picker for mail editor
*/

/*!
    Constructor
*/
NmAttachmentPicker::NmAttachmentPicker(QObject* parent):
    QObject(parent),
    mRequest(NULL)
{
}

/*!
    Destructor
*/
NmAttachmentPicker::~NmAttachmentPicker()
{
    delete mRequest;
}

/*!
    Send request to start Photo picker 
*/
void NmAttachmentPicker::fetchImage()
{
    fetch(IMAGE_FETCHER_INTERFACE, IMAGE_FETCHER_OPERATION);
}

/*!
    Send request to start Music picker 
*/
void NmAttachmentPicker::fetchAudio()
{
    fetch(AUDIO_FETCHER_INTERFACE, AUDIO_FETCHER_OPERATION);    
}

/*!
    Send request to start Video picker 
*/
void NmAttachmentPicker::fetchVideo()
{

}

/*!
    Send request to start general file picker 
*/
void NmAttachmentPicker::fetchOther()
{

}

/*!
    Construct & send appmgr request to start appropriate picker   
*/
void NmAttachmentPicker::fetch(const QString& interface, 
    const QString& operation)
{
    delete mRequest;
    mRequest = NULL;
    XQApplicationManager appMgr;
    mRequest = appMgr.create(interface, operation, true);

    if (mRequest) {
        mRequest->setSynchronous(false);
        connect(mRequest, SIGNAL(requestOk(const QVariant&)),
                this, SIGNAL(attachmentsFetchOk(const QVariant&)));

        connect(mRequest, SIGNAL(requestError(int, const QString&)),
                this, SIGNAL(attachmentsFetchError(int, const QString&)));
        
        mRequest->send();
    } 
}
