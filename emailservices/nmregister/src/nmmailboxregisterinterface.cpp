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

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>
#include "nmmailboxregisterinterface.h" 
#include "nmmailboxregisterinterface_p.h" 

//mandatory keys 
const QString hsItemName          ("item:name"); 
const QString hsitemLaunchUri     ("item:launchuri"); 
const QString hsitemPublisherId   ("item:publisherId"); 
//not mandatory 
const QString hsItemWidgetUri     ("widget:uri"); 
const QString hsItemDescription   ("item:description"); 
//for icon - not mandatory 
const QString hsIconFileName      ("icon:filename");//to display specific icon from file 
const QString hsIconName          ("icon:name");// HbIcon 
const QString hsIconApplicationId ("icon:applicationid"); // icon from associated application 

//Custom for nmHsWidgets
//NmHsWidgetParam is used to identify what email account will be monitored by 
//the registered hs widget
const QString NmHsWidgetParam     ("widgetparam:accountId");
//same as above but for pushing the widget to homescreen
const QString NmHsAccountId       ("accountId");
//This is used to identify nmhswidget in homescreen
const QString NmHsWidget          ("nmhswidget");

//parameter values
const QString NmPublisherName     ("NmMailboxRegisterInterface");
const QString NmLaunchUri         ("application://0x2002DD17");
//URI for the defined service
const QLatin1String interfaceUri("com.nokia.symbian.IEmailRegisterAccount");



/*!
    Constructor
*/
NmMailboxRegisterInterface::NmMailboxRegisterInterface(QObject *parent) :
    QObject(parent), m_d( new NmMailboxRegisterInterfacePrivate(this) )
    {
    }

/*!
    Destructor
*/
NmMailboxRegisterInterface::~NmMailboxRegisterInterface()
    {
    delete m_d;
    }

/*!
    registerNewMailbox
*/
bool NmMailboxRegisterInterface::registerNewMailbox(quint64 accountId, 
        QString accountName, QString accountIconName)
    {
    return m_d->registerNewMailbox( accountId, accountName, accountIconName );
    }

/*!
    updateMailboxName
*/
bool NmMailboxRegisterInterface::updateMailboxName(quint64 accountId, 
        QString newName)
    {
    return m_d->updateMailboxName( accountId, newName );
    }


/*!
    unregisterMailbox
*/
bool NmMailboxRegisterInterface::unregisterMailbox(quint64 accountId)
    {
    return m_d->unregisterMailbox( accountId );
    }





/*!
    Constructor
*/
NmMailboxRegisterInterfacePrivate::NmMailboxRegisterInterfacePrivate(QObject *parent) :
    XQServiceProvider(interfaceUri, parent ), 
    mManager(0),
    mService(0)
    {
    mManager = new QServiceManager();
    QServiceFilter filter("com.nokia.symbian.IMenuClient");
    QList<QServiceInterfaceDescriptor> interfaces = mManager->findInterfaces(filter);
    mService = mManager->loadInterface(interfaces.first()); 
    }

/*!
    Destructor
*/
NmMailboxRegisterInterfacePrivate::~NmMailboxRegisterInterfacePrivate()
    {
    if (mService) 
        {
        delete mService;
        }
    if (mManager) 
        {
        delete mManager;
        }
    }


bool NmMailboxRegisterInterfacePrivate::registerNewMailbox(quint64 accountId,
        QString accountName, QString accountIconName)
    {
    QMap<QString, QVariant> map;
    //------------------------------
    map[hsItemName]        = accountName;
    map[hsitemLaunchUri]   = NmLaunchUri;
    map[hsitemPublisherId] = NmPublisherName;
    map[hsItemWidgetUri]   = NmHsWidget;
    map[hsItemDescription] = "test description for widget"; //TODO: Localization
    map[hsIconFileName]    = accountIconName;
    // to add widget params that are mapped to widgets properties
    map[NmHsWidgetParam]   = QString::number(accountId);
    //------------------------------
    
    bool retVal(false);
    
    bool ret = QMetaObject::invokeMethod(  mService,"add",
                    Qt::DirectConnection,
                    Q_RETURN_ARG(bool, retVal),
                    Q_ARG(QVariantMap, map)); 

    //Push the registered widget also to homescreen right away
    bool pushRetVal = pushWidgetToHomescreen(accountId);
    
    return (retVal && ret && pushRetVal);
    }

bool NmMailboxRegisterInterfacePrivate::updateMailboxName(quint64 accountId,
        QString newName)
    {
    QMap<QString, QVariant> map;
     //------------------------------
     map[hsitemLaunchUri]   = NmLaunchUri;
     map[hsitemPublisherId] = NmPublisherName;
     map[hsItemWidgetUri]   = NmHsWidget;
     map[NmHsWidgetParam]   = QString::number(accountId);
     //------------------------------
     
     QList<QVariantMap> list; //list of items that mach the query
     bool retVal(false);
     bool ret = QMetaObject::invokeMethod(  mService,"getList",
                     Qt::DirectConnection,
                     Q_RETURN_ARG(QList<QVariantMap>, list),
                     Q_ARG(QVariantMap, map)); 

     
     //Normally there should be only one instance, but just in case
     while( !list.isEmpty() ) 
         {  
         //update the account name
         list.first()[hsItemName] = newName;
         //commit changes
         ret = QMetaObject::invokeMethod(  mService,"add",
                         Qt::DirectConnection,
                         Q_RETURN_ARG(bool, retVal),
                         Q_ARG(QVariantMap, list.first()));  
         list.removeFirst();
         }
     //if either invoke or "getList" or "add" return failure
     return (retVal && ret);
    }

bool NmMailboxRegisterInterfacePrivate::unregisterMailbox(quint64 accountId)
    {
    
    QMap<QString, QVariant> map;
     //------------------------------
     map[hsitemLaunchUri]   = NmLaunchUri;
     map[hsitemPublisherId] = NmPublisherName;
     map[hsItemWidgetUri]   = NmHsWidget;
     map[NmHsWidgetParam]     = QString::number(accountId);
     //------------------------------
     
     QList<QVariantMap> list; //list of items that mach the query
     bool retVal(false);
     bool ret = QMetaObject::invokeMethod(  mService,"getList",
                     Qt::DirectConnection,
                     Q_RETURN_ARG(QList<QVariantMap>, list),
                     Q_ARG(QVariantMap, map)); 
    
    
    //Normally there should be only one instance, but just in case
    //Otherwise there will be ghost instances in the application list
     while( !list.isEmpty() ) 
         {  

         //commit changes
         ret = QMetaObject::invokeMethod(  mService,"remove",
                         Qt::DirectConnection,
                         Q_RETURN_ARG(bool, retVal),
                         Q_ARG(QVariantMap, list.first()));  
         list.removeFirst();
         }

     return (retVal && ret);
    }

bool NmMailboxRegisterInterfacePrivate::pushWidgetToHomescreen( quint64 accountId )
    {
    // load plugin
    QServiceManager manager;
    QServiceFilter filter("com.nokia.symbian.IHomeScreenClient");
    filter.setServiceName("hshomescreenclientplugin");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);
    
    if(interfaces.isEmpty()) 
        {
        QServiceManager::Error errori = manager.error();
        return false;
        }
    QObject* service = manager.loadInterface( interfaces.first() );
    if(!service){qDebug() << "service is null ";}
    
    //--------------------------------
    QVariantHash map;
    map[NmHsAccountId] = QString::number(accountId);
    //--------------------------------
    
    // invoke function synchronously
    bool retVal(false);
    bool ret = QMetaObject::invokeMethod(  service, "addWidget",
                    Qt::DirectConnection,
                    Q_RETURN_ARG(bool, retVal),
                    Q_ARG(QString, NmHsWidget),
                    Q_ARG(QVariantHash, map));
   
    if(!ret){
        qDebug()<< "method invoke failed!";
    }
    if(!retVal){
        qDebug() << "addWidget() failed!!";
    }
    
    if(service)
        {
        delete service;
        }
    
    return (retVal && ret);
    }

