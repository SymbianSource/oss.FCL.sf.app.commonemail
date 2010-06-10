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
class HbTapGesture;

class NMAILUIWIDGETS_EXPORT NmAttachmentListItem : public HbWidget
{
    Q_OBJECT

public:
    explicit NmAttachmentListItem(QGraphicsItem *parent = 0);
    virtual ~NmAttachmentListItem();
    void setTextColor(const QColor color);
    void setFileNameText(const QString &fileName);
    void setFileSizeText(const QString &fileSize);
    void setProgressBarValue(const int value);
    int progressBarValue() const;
    void hideProgressBar();

protected: 
    virtual void gestureEvent(QGestureEvent *event);
    
private slots:
    void removeProgressBar();

signals: 
    void itemActivated();
    void itemLongPressed(QPointF point);
    
private: 
    void init( );
    void constructUi();
    
private:
    Q_DISABLE_COPY(NmAttachmentListItem)
    HbTextItem *mFileNameText; //owned
    HbTextItem *mFileSizeText; //owned
    HbProgressBar *mProgressBar; //owned
    QColor mTextColor;
};

#endif /* NMATTACHMENTLISTITEM_H_ */
