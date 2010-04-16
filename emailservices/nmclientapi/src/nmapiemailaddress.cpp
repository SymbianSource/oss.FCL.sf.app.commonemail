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

#include "nmapiemailaddress.h"
namespace EmailClientApi
{
NmEmailAddressPrivate::NmEmailAddressPrivate()
{
}

NmEmailAddressPrivate::~NmEmailAddressPrivate()
{
}

/*
 * Constructor for NmEmailAddress class
 */
NmEmailAddress::NmEmailAddress()
{
    d = new NmEmailAddressPrivate();
}

NmEmailAddress::~NmEmailAddress()
{

}

/*
 * getter for displayname
 */
QString NmEmailAddress::displayName() const
{
    return d->displayName;
}

/*
 * getter for address
 */
QString NmEmailAddress::address() const
{
    return d->address;
}

/*
 * setter for displayname 
 */
void NmEmailAddress::setDisplayName(const QString& displayName)
{
    d->displayName = displayName;
}

/*
 * setter for address
 */
void NmEmailAddress::setAddress(const QString& address)
{
    d->address = address;
}
}
