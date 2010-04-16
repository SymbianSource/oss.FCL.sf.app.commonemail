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

#include <QString>

#include "nmaddress.h"

/*!
  Constructor of NmAddressPrivate object
 */
NmAddressPrivate::NmAddressPrivate()
:mDisplayName(""),
mAddress("")
{
}

/*!
  Destructor of NmAddressPrivate object
 */
NmAddressPrivate::~NmAddressPrivate()
{
}

/*!
    \class NmMailAddress
    \brief Container class for email address having displayname and address
    strings separated

    NmMailAddress class encapsulates RFC 2822 style email adrress - display name
    pair. Example: "FirstName LastName" <FirstName.LastName(at)email.com> where display name is
    'FirstName LastName' and address is 'FirstName.LastName(at)email.com' Class does not parse 
    " and < marks away, those marks should be removed before setting data to
    NmMailAddress object

 */

/*!
  Constructs new empty NmAddress object
 */
NmAddress::NmAddress()
{
    d = new NmAddressPrivate();
}

/*!
  Constructs new NmAddress object with \a displayName and \a address
 */
NmAddress::NmAddress(const QString &displayName, const QString &address)
{
    d = new NmAddressPrivate();
    d->mDisplayName = displayName;
    d->mAddress = address;
}

/*!
  Constructs new NmAddress object with \a address display name is leaved empty
 */
NmAddress::NmAddress(const QString &address)
{
    d = new NmAddressPrivate();
    d->mAddress = address;
}

/*!
  Constructs new NmAddress object by copying data from \a nmAddress
 */
NmAddress::NmAddress(const NmAddress &nmAddress) : d(nmAddress.d)
{
}

/*!
  Constructs new NmAddress object from private address data
 */
NmAddress::NmAddress(QExplicitlySharedDataPointer<NmAddressPrivate> nmPrivateAddress)
{
    d = nmPrivateAddress;
}

/*!
  Copy operator sets data from \a nmAddress
 */
NmAddress &NmAddress::operator=(const NmAddress &nmAddress)
{
    if (this != &nmAddress) {
      d = nmAddress.d;
    }
    return *this;
}

/*!
  Destructor
 */
NmAddress::~NmAddress()
{
}


/*!
  Equal operator returns true if display name and address are same,
  function is case insensitive
 */
bool NmAddress::operator==(const NmAddress &otherAddress) const
{
    bool ret = false;
    if (otherAddress.address().compare(d->mAddress, Qt::CaseInsensitive) == 0 &&
        otherAddress.displayName().compare(
        d->mDisplayName, Qt::CaseInsensitive) == 0) {
        ret = true;
    }
    return ret;
}

/*!
  Not equal operator return true if address or display name is not equal,
  function is case insensitive
 */
bool NmAddress::operator!=(const NmAddress &otherAddress) const
{
    bool ret = true;
    if (otherAddress.address().compare(d->mAddress, Qt::CaseInsensitive) == 0 &&
        otherAddress.displayName().compare(
        d->mDisplayName, Qt::CaseInsensitive) == 0) {
        ret = false;
    }
    return ret;
}

/*!
  Sets display name from \a displayName
 */
void NmAddress::setDisplayName(const QString &displayName)
{
    d->mDisplayName = displayName;
}

/*!
  Returns display name string
 */
QString NmAddress::displayName() const
{
    return d->mDisplayName;
}

/*!
  Sets address from \a address
 */
void NmAddress::setAddress(const QString &address)
{
    d->mAddress = address;
}

/*!
  Returns address string
 */
QString NmAddress::address() const
{
    return d->mAddress;
}
