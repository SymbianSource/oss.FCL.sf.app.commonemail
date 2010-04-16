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

#ifndef NMVIEWERWEBVIEW_H_
#define NMVIEWERWEBVIEW_H_

#include <QGraphicsWebView>

class NmViewerView;
class NmMessage;

class QMouseEvent;

class NmMailViewerWK : public QGraphicsWebView
{
    Q_OBJECT
public:
	NmMailViewerWK();
	~NmMailViewerWK();
    virtual QVariant loadResource ( int type, const QUrl & name );
    void setParentView(NmViewerView *parentView);
    void addContent(QString key, QVariant val);
    void sendMousePressEvent(QGraphicsSceneMouseEvent *event);
    void sendMouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QMap<QString,QVariant> mContent;
    NmViewerView *mParentView;      // Not owned
};

#endif /* NMVIEWERWEBVIEW_H_*/
