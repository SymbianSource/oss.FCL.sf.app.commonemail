/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message header URL event handler
*
*/

#ifndef __CFREESTYLE_MESSAGE_HEADER_EVENTHANDLER_URL_H__
#define __CFREESTYLE_MESSAGE_HEADER_EVENTHANDLER_URL_H__

#include <e32base.h>
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiAttachmentsListModel.h"

class CFreestyleEmailUiAppUi;
class CFSMailMessage;
class CFsEmailUiHtmlViewerView;
class CFreestyleMessageHeaderURL;
class TAttachmentData;

enum TLinkType
    {
    EMailHeaderTo,
    EMailHeaderCC,
    EMailHeaderFrom,
    EMailHeaderAttachment
    };

class MFreestyleMessageViewEventHandler
    {
public:
    virtual TBool HandleEventL( const TDesC& aUri ) = 0;
    };

class CFreestyleMessageHeaderURLEventHandler : public CBase, 
                                               public MFreestyleMessageViewEventHandler
    {
public:
    IMPORT_C static CFreestyleMessageHeaderURLEventHandler * NewL( 
            CFreestyleEmailUiAppUi& aAppUi, 
            CFsEmailUiHtmlViewerView& aView );
    ~CFreestyleMessageHeaderURLEventHandler ();
    
    IMPORT_C TBool HandleEventL( const TDesC& aUri );
    
protected:
    CFreestyleMessageHeaderURLEventHandler( CFreestyleEmailUiAppUi& aAppUi, 
                                            CFsEmailUiHtmlViewerView& aView );
    void ConstructL();
    
private:
    const TAttachmentData& FindAttachmentL( const CFreestyleMessageHeaderURL& aAttachmentUrl );
    void LaunchAttachmentMenuL( const TAttachmentData& aAttachment );
    void HandAttachmentActionMenuCommandL( TActionMenuCustomItemId aSelectedActionMenuItem,
                                           const TAttachmentData& aAttachment );
    
    void LaunchEmailAddressMenuL( const CFreestyleMessageHeaderURL& iMessageHeaderURL );    
    void HandleEmailAddressActionMenuCommandL( TActionMenuCustomItemId aSelectedActionMenuItem, 
                                               const CFreestyleMessageHeaderURL& iMessageHeaderURL );
    
    void SaveEmailAsContactL( const TDesC& aEmailAddress );
    
private:
    CFreestyleMessageHeaderURL*     iMessageHeaderURL;
    CFreestyleEmailUiAppUi&         iAppUi;
    CFsEmailUiHtmlViewerView&       iView;
    CFSMailMessage*                 iMailMessage;
    CFSEmailUiAttachmentsListModel* iAttachmentsListModel;
    };

#endif //__CFREESTYLE_MESSAGE_HEADER_EVENTHANDLER_URL_H__
