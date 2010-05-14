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

#include "nmuiheaders.h"

static const QString NmViewerViewNetManagerScheme("cid");

/*!
	\class NmViewerViewNetManager
	\brief Mail viewer network access manager class
*/


/*!
    Constructor
*/
NmViewerViewNetManager::NmViewerViewNetManager()
    : QNetworkAccessManager(),
      mMessageView(NULL)
{
}

/*!
    Destructor
*/
NmViewerViewNetManager::~NmViewerViewNetManager()
{
}

/*!
    Setter for view.
*/
void NmViewerViewNetManager::setView(NmViewerView *viewerView)
{
    mMessageView=viewerView;
}

/*!
    createRequest
*/
QNetworkReply *NmViewerViewNetManager::createRequest(
    Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest myRequest(request);
    // Set request attribute to prefer cachevar  
    const QVariant cacheControl((int)QNetworkRequest::PreferCache);
    myRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, 
                         cacheControl);
    const QUrl requestUrl = myRequest.url();
    // Check whether request is for embedded image
    if (mMessageView&&mMessageView->webView()&&op==QNetworkAccessManager::GetOperation
        && requestUrl.scheme()==NmViewerViewNetManagerScheme) {
        NmViewerViewNetReply* reply = new NmViewerViewNetReply(
            mMessageView->webView()->loadResource(QTextDocument::ImageResource, requestUrl));
        reply->setOriginalRequest(myRequest);
        return reply;
        }
    // If request is not for embedded image, forward to base class
    return QNetworkAccessManager::createRequest(op, myRequest, outgoingData);
}
