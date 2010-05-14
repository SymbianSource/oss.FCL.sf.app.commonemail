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
* Description: Nmail plugin for custom widgets
*
*/

#include "nmailuiwidgetsplugin.h"
#include <nmrecipientlineedit.h>
#include <nmhtmllineedit.h>
#include <nmeditortextedit.h>
#include <nmbaseviewscrollarea.h>
#include <nmattachmentlistwidget.h>

QObject *NMailUiWidgetsPlugin::createObject(const QString& type, const QString &name )
{
    if( type == NmBaseViewScrollArea::staticMetaObject.className() ) {
        QObject *object = new NmBaseViewScrollArea;
        object->setObjectName(name);
        return object;
    }
    if( type == NmRecipientLineEdit::staticMetaObject.className() ) {
        QObject *object = new NmRecipientLineEdit;
        object->setObjectName(name);
        return object;
    }
    if( type == NmHtmlLineEdit::staticMetaObject.className() ) {
        QObject *object = new NmHtmlLineEdit;
        object->setObjectName(name);
        return object;
    }
    if( type == NmEditorTextEdit::staticMetaObject.className() ) {
        QObject *object = new NmEditorTextEdit;
        object->setObjectName(name);
        return object;
    }
    if( type == NmAttachmentListWidget::staticMetaObject.className() ) {
        QObject *object = new NmAttachmentListWidget;
        object->setObjectName(name);
        return object;
    }
    return 0;
}

QList<const QMetaObject *> NMailUiWidgetsPlugin::supportedObjects()
{
    QList<const QMetaObject *> result;
    result.append( &NmBaseViewScrollArea::staticMetaObject );
    result.append( &NmRecipientLineEdit::staticMetaObject );
    result.append( &NmHtmlLineEdit::staticMetaObject );
    result.append( &NmEditorTextEdit::staticMetaObject );
    result.append( &NmAttachmentListWidget::staticMetaObject );
    return result;
}

// end of file

