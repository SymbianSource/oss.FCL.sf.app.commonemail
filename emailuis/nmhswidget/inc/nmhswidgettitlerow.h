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

#ifndef NMHSWIDGETTITLEROW_H_
#define NMHSWIDGETTITLEROW_H_

#include <hbwidget.h>

//FORWARD DECLARATIONS:
class HbLabel;

class NmHsWidgetTitleRow : public HbWidget
{
    Q_OBJECT
  
public:
    NmHsWidgetTitleRow(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidgetTitleRow();
    
private:
    void loadDocML();
    void updateData();
    
public slots: 
    void updateAccountName(const QString& accountName );
    void updateUnreadCount(const int& unreadCount );

private:
    HbLabel *mMailboxIcon;
    HbLabel *mMailboxInfo;
    QString mAccountName;
    int mUnreadCount;
};

#endif // NMHSWIDGETTITLEROW_H_
