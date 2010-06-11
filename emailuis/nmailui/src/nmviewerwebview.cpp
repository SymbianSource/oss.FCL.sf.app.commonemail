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
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmMailViewerWK::~NmMailViewerWK()
{
    NM_FUNCTION;
    
    mContent.clear();
}

/*!

*/
void NmMailViewerWK::setParentView(NmViewerView *parentView)
{
    NM_FUNCTION;
    
    mParentView = parentView;
}

/*!
    addContent. Function adds content into web view.
*/
void NmMailViewerWK::addContent(QString key, QVariant val, NmId partId, bool isFetched) 
{
    NM_FUNCTION;
    
    mContent[key] = NmMailViewerWkContentItem(val, partId, isFetched);
}

/*!
    loadResource. Function returns resource from added content (added with addContent)
*/
QVariant NmMailViewerWK::loadResource(int type, const QUrl &name, NmId &partId, bool &isFetched)
{
    NM_FUNCTION;
    
    if (type == QTextDocument::ImageResource) {
        QString key = '<' + name.path() + '>';
        if (!mContent.contains(key)) {
            key = name.path();
        }
        if (mContent.contains(key)) {
            partId = mContent[key].mPartId;
            isFetched = mContent[key].mIsFetched;
            return mContent[key].mData;
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
    NM_FUNCTION;
    
    if (event){
        QGraphicsWebView::mousePressEvent(event);
    }
}

/*!
    sendMouseReleaseEvent. Function is used to relay mouse event to base class
*/
void NmMailViewerWK::sendMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    NM_FUNCTION;
    
    if (event) {
        QGraphicsWebView::mouseReleaseEvent(event);
    }
}



