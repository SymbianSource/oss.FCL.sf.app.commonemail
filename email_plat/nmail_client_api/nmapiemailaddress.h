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

#ifndef NMAPIEMAILADDRESS_H
#define NMAPIEMAILADDRESS_H

#include <QString>
#include <QExplicitlySharedDataPointer>
#include "nmenginedef.h"
/**
 * Email addres
 */

namespace EmailClientApi
{
class NmEmailAddressPrivate : public QSharedData
{
public:
    NmEmailAddressPrivate();
    virtual ~NmEmailAddressPrivate();
    QString displayName;
    QString address;
};

class NMENGINE_EXPORT NmEmailAddress
{
public:
    /*
     * Constructor for NmEmailAddress class
     */
    NmEmailAddress();
    virtual ~NmEmailAddress();
    /*
     * getter for displayname
     */
    QString displayName() const;

    /*
     * getter for address
     */
    QString address() const;

    /*
     * setter for displayname 
     */
    void setDisplayName(const QString &displayName);

    /*
     * setter for address
     */
    void setAddress(const QString &address);
private:
    QExplicitlySharedDataPointer<NmEmailAddressPrivate> d;
};
}

#endif
