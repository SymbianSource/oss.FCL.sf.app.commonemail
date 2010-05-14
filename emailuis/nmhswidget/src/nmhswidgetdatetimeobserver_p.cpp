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
#include <bacntf.h>
#include "nmhswidgetdatetimeobserver.h"
#include "nmhswidgetdatetimeobserver_p.h"

/*!
    \fn NmHsWidgetDateTimeObserverPrivate::NmHsWidgetDateTimeObserverPrivate()

    /param q pointer to public interface
    /post Observer listens to environment change events via callback
*/
NmHsWidgetDateTimeObserverPrivate::NmHsWidgetDateTimeObserverPrivate(NmHsWidgetDateTimeObserver *q) :
    q_ptr(q)
{
    qDebug() << "NmHsWidgetDateTimeObserverPrivate::NmHsWidgetDateTimeObserverPrivate --> IN" ;
    TCallBack callback( LocaleChangeCallback, this );
    //TODO: Handle leave properly
    TRAP_IGNORE(iLocaleNotifier = CEnvironmentChangeNotifier::NewL( CActive::EPriorityStandard, callback ));
    iLocaleNotifier->Start();
    qDebug() << "NmHsWidgetDateTimeObserverPrivate::NmHsWidgetDateTimeObserverPrivate <-- OUT" ;
}

/*!
    \fn NmHsWidgetDateTimeObserverPrivate::~NmHsWidgetDateTimeObserverPrivate()

    Destructor
*/
NmHsWidgetDateTimeObserverPrivate::~NmHsWidgetDateTimeObserverPrivate()
{
    qDebug() << "NmHsWidgetDateTimeObserverPrivate::~NmHsWidgetDateTimeObserverPrivate --> IN" ;
    if(iLocaleNotifier){
        delete iLocaleNotifier;
        iLocaleNotifier = NULL;
    }
    qDebug() << "NmHsWidgetDateTimeObserverPrivate::~NmHsWidgetDateTimeObserverPrivate <-- OUT" ;
}

/*!
    \fn TInt NmHsWidgetDateTimeObserverPrivate::HandleLocaleChange()

    /post If event includes change for system time, locale or midnight crossover
          indicates poblic interface about the change
    /return Returns count of handled change events
*/
TInt NmHsWidgetDateTimeObserverPrivate::HandleLocaleChange()
{
    qDebug() << "NmHsWidgetDateTimeObserverPrivate::HandleLocaleChange";
    TInt handled = 0;
    if( iLocaleNotifier->Change() & ( EChangesSystemTime | EChangesLocale | EChangesMidnightCrossover ))
        {
        q_ptr->handleCompletion();
        handled = 1;
        }
    return handled;
}

/*!
    \fn TInt NmHsWidgetDateTimeObserverPrivate::LocaleChangeCallback(TAny* aThisPtr)
    /param aThisPtr Pointer to tis observer from callback object
    /post Event is checked and handled if valid change
    /return Returns count of handled change events
*/
TInt NmHsWidgetDateTimeObserverPrivate::LocaleChangeCallback(TAny* aThisPtr)
{
    qDebug() << "NmHsWidgetDateTimeObserverPrivate::LocaleChangeCallback" ;
    TInt ret = static_cast<NmHsWidgetDateTimeObserverPrivate*>(aThisPtr)->HandleLocaleChange();
    return ret;
}
