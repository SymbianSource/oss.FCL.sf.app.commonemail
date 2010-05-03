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
 * Description: 
 *
 */

#ifndef NMHSWIDGET_H
#define NMHSWIDGET_H

#include <hbwidget.h>
#include "nmcommon.h"

class NmHsWidgetEmailEngine;
class NmHsWidgetTitleRow;
class NmHsWidgetEmailRow;
class QGraphicsLinearLayout;

class NmHsWidget : public HbWidget
{
    Q_OBJECT
    
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId)
  
public:
    NmHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidget();
    
public slots:
    void onInitialize();
    void onShow();
    void onHide();
    void onUninitialize();
    

    void updateMailData();
    void setAccountId(const QString &text);
    QString accountId() const;

protected:
    void updateMailRowsList(int mailCount);
    void paint(QPainter *painter, 
               const QStyleOptionGraphicsItem *option, 
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
private:
    NmHsWidgetEmailEngine* mEngine;
    QGraphicsLinearLayout *mRowLayout;
    NmHsWidgetTitleRow* mTitleRow;
    QList<NmHsWidgetEmailRow*> mMailRows;
    NmId mAccountId;
    
public:    
    friend class TestNmHsWidget;     
};

#endif  // NMHSWIDGET_H
