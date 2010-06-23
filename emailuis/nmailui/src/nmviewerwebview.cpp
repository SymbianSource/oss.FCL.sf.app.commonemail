/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
    Constructor.
*/
NmMailViewerWK::NmMailViewerWK()
: QGraphicsWebView()
{
    grabGesture(Qt::PinchGesture);
    installEventFilter(new NmEventFilterWK(this));
    setFlag(QGraphicsItem::ItemIsFocusable,false);
}

/*!
    Destructor.
 */
NmMailViewerWK::~NmMailViewerWK()
{
    mContent.clear();
}

/*!
    Sets the parent view.
 */
void NmMailViewerWK::setParentView(NmViewerView *parentView)
{
    mParentView = parentView;
}

/*!
    Adds content into web view.
 */
void NmMailViewerWK::addContent(QString key, QVariant val, NmId partId, bool isFetched) 
{
    mContent[key] = NmMailViewerWkContentItem(val, partId, isFetched);
}

/*!
    Returns resource from added content.
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
    Filter class' constructor.
 */
NmEventFilterWK::NmEventFilterWK(QObject* parent)
: QObject(parent)
{
}

/*
    Filters events if this object has been installed as an event filter.
 */
bool NmEventFilterWK::eventFilter(QObject* object, QEvent* event) {
    Q_UNUSED(object);
    bool consumed = false;
    if (event) {
        switch (event->type()) {
        case QEvent::Gesture:
            consumed = gestureEvent(static_cast<QGestureEvent*>(event));
            break;
        case QEvent::GraphicsSceneMouseDoubleClick:
            // Handle double click (instant zoom).
            // At the moment we simply consume the event.
            event->accept();
            consumed = true;
            break;
        case QEvent::GraphicsSceneContextMenu:
        case QEvent::GraphicsSceneMouseMove:
            event->accept();
            consumed = true;
            break;
        default:
            break;
        }
    }
    return consumed;
}

/*!
    Handles gesture events. This function is invoked by the eventFilter()
    function in case of gesture events.
 */
bool NmEventFilterWK::gestureEvent(QGestureEvent* event) {
    bool consumed = false;
    if (event) {
        if (QPinchGesture* pinch = static_cast<QPinchGesture*>(event->gesture(Qt::PinchGesture))) {
            // Handle pinch (zoom).
            // At the moment we simply consume the event.
            event->accept();
            consumed = true;
        }
    }
    return consumed;
}
