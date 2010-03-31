/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMRUI API extension definition
*
*/

#ifndef ESMRUTILSAPIEXT_H
#define ESMRUTILSAPIEXT_H

#include <cmrutils.h>

/**
 * Extension API query function.
 *
 * Usage:
 *  Class providing extension:
 *
 *  class CExtendable
 *      {
 *      public:
 *      .
 *      .
 *      .
 *      TAny* ExtensionL( TUid aExtUid );
 *      }; 
 *
 *   TAny* CExtendable::ExtensionL( TUid aExtUid )
 *      {
 *      if (aExtUid == KOwnExtension)
 *          {
 *          return static_cast<ExtensionApiType*>( this );
 *          }
 *      } 
 *
 *  Class using extension:
 *
 *
 *  CMRUtils* utils = ...;
 *  ExtensionApiType* ext = ExtensionApiL<ExtensionApiType>( iUtils );
 *
 * @param aUtils
 * @return Pointer to the requested extension API
 */
template<class ExtensionApiType>
ExtensionApiType* ExtensionApiL( CMRUtils& aUtils )
    {
    TAny* extApi = aUtils.ExtensionL( ExtensionApiType::ExtensionUid() );
    if ( extApi )
        {
        return static_cast<ExtensionApiType*>( extApi );
        }
    else
        {
        return NULL;        
        }
    }
    
#endif  // ESMRUTILSAPIEXT_H

// End of File
