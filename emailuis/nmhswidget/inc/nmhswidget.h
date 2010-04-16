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

#ifndef NMHSWIDGET_H
#define NMHSWIDGET_H

#include <hbwidget.h>

class NmHsWidgetEmailEngine;
class NmHsWidgetTitleRow;
class NmHsWidgetEmailRow;
class QGraphicsLinearLayout;

class NmHsWidget : public HbWidget
{
    Q_OBJECT
  
public:
    NmHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidget();
    
public slots:
    void onInitialize();
    void onUninitialize();
    
    void updateAccountName(const QString& accountName);
    void updateUnreadCount(const int& unreadCount);
    void updateMailData();

protected:
    void updateMailRowsList(int mailCount);
    void paint(QPainter *painter, 
               const QStyleOptionGraphicsItem *option, 
               QWidget *widget);
    
private:
    NmHsWidgetEmailEngine* mEngine;
    QGraphicsLinearLayout *mRowLayout;
    NmHsWidgetTitleRow* mTitleRow;
    QList<NmHsWidgetEmailRow*> mMailRows;
    
public:    
    friend class TestNmHsWidget;     
};

#endif  // NMHSWIDGET_H
