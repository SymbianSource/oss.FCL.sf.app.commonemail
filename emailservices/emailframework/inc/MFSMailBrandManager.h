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
* Description: This file defines class MFSMailBrandManager.
*
*/


#ifndef MFSMAILBRANDMANAGER_H
#define MFSMAILBRANDMANAGER_H

// INCLUDES
//<cmail>
#include "mailbrandmanager.hrh"
//</cmail>

// FORWARD DECLARATIONS
class CGulIcon;
class TRgb;
class TFSMailMsgId;

/**
 *  Branding manager.
 *
 *  Branding manager is used to query different branding elements
 *  related to certain mail box.
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class MFSMailBrandManager
{

    public:
    
    /**
     * Destructor.
     */  
     virtual ~MFSMailBrandManager() { }
    
    /**
     * Returns a branded graphical element.
     *
     * @param aElement Id of the branded element. This identifies which
     *                 graphic user wishes to retrieve.
     * @param aMailBoxId mailbox whose branded element is retrieved.
     *
     * @return A pointer to a CGulIcon object. The caller of this method is 
     *         responsible of destroying the object. Returns NULL if the 
     *         brand doesn't contain the requested branding element.
     */
     virtual CGulIcon* GetGraphicL( TFSBrandElement aElement,
                                    const TFSMailMsgId& aMailboxId ) = 0;

     /**
      * Returns a branded graphical element.
      *
      * @param aElement Id of the branded element. This identifies which
      *                 graphic user wishes to retrieve.
      * @param aBrandId is the domain.
      *
      * @return A pointer to a CGulIcon object. The caller of this method is 
      *         responsible of destroying the object. Returns NULL if the 
      *         brand doesn't contain the requested branding element.
      */
      virtual CGulIcon* GetGraphicL( TFSBrandElement aElement,
                                     const TDesC& aBrandId ) = 0;
    
    /**
     * Returns a branded text element.
     *
     * @param aElement Id of the branded element. This identifies which
     *                 text user wishes to retrieve.
     * @param aMailBoxId mailbox whose branded element is retrieved.
     *
     * @return A TPtrC object. Returns TPtrC to a null descriptor if the
     *         brand doesn't contain the requested branding element.
     */
     virtual TPtrC GetTextL( TFSBrandElement aElement,
                            const TFSMailMsgId& aMailboxId ) = 0;
    
    /**
     * Returns a branded text element.
     * This function is for WhiteLabelBranding, because branded mailbox name is
     * needed before mailbox creation.
     * 
     * @param aElement Id of the branded element. This identifies which
     *                 text user wishes to retrieve.
     * @param aMailBoxId mailbox whose branded element is retrieved.
     *
     * @return A TPtrC object. Returns TPtrC to a null descriptor if the
     *         brand doesn't contain the requested branding element.
     */
     virtual TPtrC GetTextL( TFSBrandElement aElement, 
                            const TDesC& aBrandId ) = 0;
    
    /**
     * Returns a branded text element.
     *
     * @param aElement The id of the branded element. This identifies which
     *                 color user wishes to retrieve.
     * @param aMailBoxId mailbox whose branded element is retrieved.
     * @param aColor A reference to a TRgb object. The color is returned
     *               using this reference.
     *
     * @return Error code. KErrNotFound if the brand doesn't contain the requested
     *         branding element.
     */
     virtual TInt GetColorL( TFSBrandElement aElement,
                            const TFSMailMsgId& aMailboxId,
                            TRgb& aColor ) = 0;
    
    /**
     * This function will change 'mailbox name' as branded name.
     * If aMailboxId is NULL function goes through all mailboxes and check if mailbox is branded.
     * If it is, function will change 'mailbox name' as branded name.
     * This function should be called after mailbox settings has changed.
     * 
     * @param aMailboxId Id of the mailbox
     */
     virtual void UpdateMailboxNamesL(  const TFSMailMsgId aMailboxId ) = 0;

    /**
     * Returns branded graphic element of given type.
     *
     * @param aElementId brand element
     * @param aMailBoxId mailbox whose branded element is retrieved.     
     * @param aIconIds Icon path and ids
     *
     * @return Error code
     */
     virtual TInt GetGraphicIdsL( TFSBrandElement aElement,
                                  const TFSMailMsgId& aMailboxId,
                                  TDes& aIconIds  ) = 0;

    /**
     * Returns branded graphic element of given type.
     *
     * @param aElementId brand element
     * @param aBrandId Domain of email address.     
     * @param aIconIds Icon path and ids
     *
     * @return Error code
     */
     virtual TInt GetGraphicIdsL( TFSBrandElement aElement,
                                  const TDesC& aBrandId,
                                  TDes& aIconIds  ) = 0;        
};


#endif  // MFSMAILBRANDMANAGER_H