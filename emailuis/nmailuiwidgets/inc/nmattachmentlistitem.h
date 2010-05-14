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
* Description: Attachment list item widget
*
*/

#ifndef NMATTACHMENTLISTITEM_H_
#define NMATTACHMENTLISTITEM_H_


#include <hbwidget.h>
#include"nmailuiwidgetsdef.h"

class HbProgressBar;
class HbTextItem;
class QTimer;

class NMAILUIWIDGETS_EXPORT NmAttachmentListItem : public HbWidget
{
    Q_OBJECT

public:
    explicit NmAttachmentListItem(QGraphicsItem *parent = 0);
    virtual ~NmAttachmentListItem();
    void setTextColor(const QColor color);
    void setFileNameText(const QString &fileName);
    void setFileSizeText(const QString &fileSize);
    void resetFileNameLength(Qt::Orientation orientation);
    void setProgressBarValue(const int value);
    int progressBarValue() const;
    void hideProgressBar();

protected: 
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    
private slots:
    void removeProgressBar();
    void longPressedActivated();

signals: 
    void itemActivated();
    void itemLongPressed(QPointF point);
    
private: 
    void init( );
    void constructUi();
    QSize screenSize(Qt::Orientation orientation);
    
private:
    Q_DISABLE_COPY(NmAttachmentListItem)
    HbTextItem *mFileNameText; //owned
    HbTextItem *mFileSizeText; //owned
    HbProgressBar *mProgressBar; //owned
    QTimer *mTimer; //owned
    bool mButtonPressed;
    QPointF mLongPressedPoint;
    QColor mTextColor;
};

#endif /* NMATTACHMENTLISTITEM_H_ */