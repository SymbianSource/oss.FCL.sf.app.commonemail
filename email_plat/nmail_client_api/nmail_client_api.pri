#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description:
#
#

symbian*: {
    # Build.inf rules
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
            "nmail_client_api/nmcommon_api.h           APP_LAYER_PLATFORM_EXPORT_PATH(nmcommonheader_api.h)" \
            "nmail_client_api/nmapiemailaddress.h      APP_LAYER_PLATFORM_EXPORT_PATH(nmapiemailaddress.h)" \
            "nmail_client_api/nmapiemailservice.h      APP_LAYER_PLATFORM_EXPORT_PATH(nmapiemailservice.h)" \
            "nmail_client_api/nmenginedef.h            APP_LAYER_PLATFORM_EXPORT_PATH(nmenginedef.h)" \
            "nmail_client_api/nmapienvelopelisting.h   APP_LAYER_PLATFORM_EXPORT_PATH(nmapienvelopelisting.h)" \
            "nmail_client_api/nmapieventnotifier.h     APP_LAYER_PLATFORM_EXPORT_PATH(nmapieventnotifier.h)" \
            "nmail_client_api/nmapifolder.h            APP_LAYER_PLATFORM_EXPORT_PATH(nmapifolder.h)" \
            "nmail_client_api/nmapifolderlisting.h     APP_LAYER_PLATFORM_EXPORT_PATH(nmapifolderlisting.h)" \
            "nmail_client_api/nmapimailbox.h           APP_LAYER_PLATFORM_EXPORT_PATH(nmapimailbox.h)" \
            "nmail_client_api/nmapimailboxlisting.h    APP_LAYER_PLATFORM_EXPORT_PATH(nmapimailboxlisting.h)" \
            "nmail_client_api/nmapimessagebody.h       APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagebody.h)" \
            "nmail_client_api/nmapimessageenvelope.h   APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessageenvelope.h)" \
            "nmail_client_api/nmapimessagetask.h       APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagetask.h)"
}
