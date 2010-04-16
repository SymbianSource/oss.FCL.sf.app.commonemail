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

/*!
    Constructor
*/
NmMailViewerWK::NmMailViewerWK()
	:QGraphicsWebView()
{
}

/*!
    Destructor
*/
NmMailViewerWK::~NmMailViewerWK()
{
    mContent.clear();
}

/*!

*/
void NmMailViewerWK::setParentView(NmViewerView *parentView)
{
    mParentView = parentView;
}

/*!
    addContent. Function adds content into web view.
*/
void NmMailViewerWK::addContent(QString key, QVariant val) {
    mContent[key] = val;
}

/*!
    loadResource. Function returns resource from added content (added with addContent)
*/
QVariant NmMailViewerWK::loadResource(int type, const QUrl &name)
{
    if (type == QTextDocument::ImageResource) {
        QString key = '<' + name.path() + '>';
        if (!mContent.contains(key)) {
            key = name.path();
        }
        if (mContent.contains(key)) {
            return mContent[key];
        }
        return 0;
    }
    return 0;
}

/*!
    sendMousePressEvent. Function is used to relay mouse event to base class
*/
void NmMailViewerWK::sendMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event){
        QGraphicsWebView::mousePressEvent(event);
    }
}

/*!
    sendMouseReleaseEvent. Function is used to relay mouse event to base class
*/
void NmMailViewerWK::sendMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event) {
        QGraphicsWebView::mouseReleaseEvent(event);
    }
}



