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
* Description: Message editor body text field
*
*/

#ifndef NMEDITORTEXTEDIT_H_
#define NMEDITORTEXTEDIT_H_

#include <hbtextedit.h>
#include"nmailuiwidgetsdef.h"

class NmEditorContent;
class NmBaseViewScrollArea;
class HbScrollArea;


class NMAILUIWIDGETS_EXPORT NmEditorTextEdit : public HbTextEdit
{
Q_OBJECT

public:
    NmEditorTextEdit(QGraphicsItem *parent = 0);
    virtual ~NmEditorTextEdit();

    void init(NmEditorContent *parent, NmBaseViewScrollArea *bgScrollArea);
    qreal contentHeight() const;

    void sendMousePressEvent(QGraphicsSceneMouseEvent *event);
    void sendMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void sendMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void sendLongPressEvent(const QPointF &point);
    void setCustomTextColor(const QPair<bool, QColor> &customColor);
    void setCustomTextColor(bool useCustom, const QColor& color = Qt::black);
    QPair<bool, QColor> customTextColor() const;

signals:
    void setEditorContentHeight();

public slots:
    void updateEditorHeight();
    void setHeaderHeight(int);
    void setScrollPosition(int oldPos, int newPos);
    void updateScrollPosition(const QPointF &newPosition);
    void updateCustomTextColor();

private:
    NmEditorContent *mParent;
    HbScrollArea *mScrollArea;
    NmBaseViewScrollArea *mBackgroundScrollArea;
    qreal mPreviousContentsHeight;
    int mHeaderHeight;
    QPointF mBgScrollPosition;
    bool mFirstTime;
    
    QPair<bool, QColor> mCustomTextColor;//!<This property keeps info about using customTextColor and color to use. 
};

#endif /* NMEDITORTEXTEDIT_H_ */
