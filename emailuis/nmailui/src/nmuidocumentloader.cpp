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

#include "nmuiheaders.h"

/*!
    Constructor
*/
NmUiDocumentLoader::NmUiDocumentLoader(const HbMainWindow *window) : HbDocumentLoader(window)
{
}

/*!
    Destructor
*/
NmUiDocumentLoader::~NmUiDocumentLoader()
{
}

/*!
    createObject. Function returns Nmail UI specific objects from DocML
*/
QObject *NmUiDocumentLoader::createObject(const QString& type, const QString &name)
{
    QObject *res = NULL;
    if( type == NmMailViewerWK::staticMetaObject.className() ) {
        res = new NmMailViewerWK();
        res->setObjectName(name);
    }
    if( type == NmBaseViewScrollArea::staticMetaObject.className() ) {
        res = new NmBaseViewScrollArea();
        res->setObjectName(name);
    }
    if( type == NmViewerHeader::staticMetaObject.className() ) {
        res = new NmViewerHeader();
        res->setObjectName(name);
    }
    if (res == NULL) {
        res = HbDocumentLoader::createObject(type, name);
    }
    return res;
}
