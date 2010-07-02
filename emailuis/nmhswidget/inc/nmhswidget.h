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
class QTranslator;
class HbFrameDrawer;
class NmHsWidgetDateTimeObserver;
class HbLabel;
class HbDocumentLoader;

class NmHsWidget : public HbWidget
{
    Q_OBJECT
    
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId)
    Q_PROPERTY(QString accountIconName READ accountIconName WRITE setAccountIconName)
    Q_PROPERTY(QString widgetState READ widgetStateProperty WRITE setWidgetStateProperty)
  
public:
    NmHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidget();

    QPainterPath shape() const;
    
public slots:
    //from home screen fw
    void onInitialize();
    void onShow();
    void onHide();
    //engine
    void updateMailData();
    void onEngineException(const int& exc);
    //properties
    void setAccountId(const QString &text);
    QString accountId() const;
    void setAccountIconName(const QString &text);
    QString accountIconName() const;   
    void setWidgetStateProperty(QString value);
    QString widgetStateProperty();
    //user actions
    void handleExpandCollapseEvent();

signals: 
    void finished();
    void setPreferences(const QStringList &names);
    void error();
private:
    void setupLocalization();
    void setupUi();
    bool loadDocML(HbDocumentLoader &loader);
    void updateMailRowsVisibility(const int visibleCount);
    void addNoMailsLabelToLayout();
    void removeNoMailsLabelFromLayout();
    void addEmailRowsToLayout();
    void removeEmailRowsFromLayout();
    void updateLayout(const int visibleCount);
    void toggleExpansionState();  

protected:
    void createMailRowsList();
    
private:
    NmHsWidgetEmailEngine* mEngine;
    NmHsWidgetTitleRow* mTitleRow;          
    QList<NmHsWidgetEmailRow*> mMailRows;   //list including mail row instances
    NmId mAccountId;                        //mail account id widget is monitoring
    QString mAccountIconName;               //name of mailbox icon shown in titlerow
    QTranslator* mTranslator;
    HbFrameDrawer* mBackgroundFrameDrawer;  //drawer for widget backgound
    bool mIsExpanded;                       //true when widget expanded, false when collapsed
    NmHsWidgetDateTimeObserver* mDateObserver;  //observer for time/locale changes
    HbLabel* mNoMailsLabel;                 //label shown in widget when no mails in inbox
    HbWidget* mWidgetContainer;             //container including title row and content container
    HbWidget* mContentContainer;            //container including mail rows and mNoMailsLabel
    QGraphicsLinearLayout* mContentLayout;  //layout for mail rows
    
public:    
    friend class TestNmHsWidget;     
};

#endif  // NMHSWIDGET_H
