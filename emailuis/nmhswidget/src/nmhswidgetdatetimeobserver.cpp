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

#include <QDebug>
#include "nmhswidgetdatetimeobserver.h"
#include "nmhswidgetdatetimeobserver_p.h"

/*!
    \fn NmHsWidgetDateTimeObserver::NmHsWidgetDateTimeObserver(QObject *parent)

    /post Private observer is created and listening to environment changes
*/
NmHsWidgetDateTimeObserver::NmHsWidgetDateTimeObserver(QObject *parent) :
    QObject(parent),
    d_ptr(0)
{
    qDebug() << "NmHsWidgetDateTimeObserver::NmHsWidgetDateTimeObserver --> IN";
    d_ptr = new NmHsWidgetDateTimeObserverPrivate(this);
    qDebug() << "NmHsWidgetDateTimeObserver::NmHsWidgetDateTimeObserver <-- OUT";
}

/*!
    \fn NmHsWidgetDateTimeObserver::~NmHsWidgetDateTimeObserver()

    /post Private observer is destroyed
*/
NmHsWidgetDateTimeObserver::~NmHsWidgetDateTimeObserver()
{
    qDebug() << "NmHsWidgetDateTimeObserver::~NmHsWidgetDateTimeObserver --> IN";
    delete d_ptr;
    qDebug() << "NmHsWidgetDateTimeObserver::~NmHsWidgetDateTimeObserver <-- OUT";
}

/*!
    \fn void NmHsWidgetDateTimeObserver::handleCompletion()

    /post Valid change event received by private observer and DateTimeChange signal is emitted
*/
void NmHsWidgetDateTimeObserver::handleCompletion()
{
    qDebug() << "NmHsWidgetDateTimeObserver::handleCompletion --> IN";
    emit dateTimeChanged();
    qDebug() << "NmHsWidgetDateTimeObserver::handleCompletion <-- OUT";
}
