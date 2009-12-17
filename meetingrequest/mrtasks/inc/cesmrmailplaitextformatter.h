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
* Description:  Plain text formatter for ES MR entries
*
*/


#ifndef C_ESMRMAILPLAINTEXTFORMATTER_H
#define C_ESMRMAILPLAINTEXTFORMATTER_H

#include <e32base.h>

class CCalEntry;
class CMRMailboxUtils;

/**
 * CESMRMailPlainTextFormatter is responsible for constructing
 * plain text part for meeting request email messages.
 *
 * CESMRMailPlainTextFormatter is ised internally by esmrtasks
 * module.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS(CESMRMailPlainTextFormatter) : public CBase
    {
public: // Construction and destruction
    
    /**
     * Creates and initialzes new CESMRMailPlainTextFormatter object.
     * Ownership is transferred to caller. 
     * @param aMailboxUtils Reference to MR mailbox utilities.
     * @return Pointer to CESMRMailPlainTextFormatter object.
     */
    static CESMRMailPlainTextFormatter* NewL(
            CMRMailboxUtils& aMailboxUtils );

    /**
     * Creates and initialzes new CESMRMailPlainTextFormatter object.
     * Ownership is transferred to caller. Created object is left to
     * cleanup stack.
     * @param aMailboxUtils Reference to MR mailbox utilities.
     * @return Pointer to CESMRMailPlainTextFormatter object.
     */
    static CESMRMailPlainTextFormatter* NewLC(
            CMRMailboxUtils& aMailboxUtils );    
    
    /**
     * C++ destructor
     */
    ~CESMRMailPlainTextFormatter();

    /**
     * Formats plain text part body from ES calendar entry. Ownership of the 
     * created descriptor is transferred to caller. Created object is also
     * left to cleanup stack.
     * @param aEntry Reference to calendar entry.
     * @return Plain text buffer.
     */
    HBufC* Body16LC(
            CCalEntry& aEntry );
    
    /**
     * Formats plain text body part from ES calendar entry. Ownership of the 
     * created descriptor is transferred to caller. Created object is also
     * left to cleanup stack.
     * @param aEntry Reference to calendar entry.
     * @return Plain text buffer.
     */    
    HBufC8* Body8LC(
            CCalEntry& aEntry );

    /**
     * Formats the subject line for meeting request email.O wnership of the 
     * created descriptor is transferred to caller. Created object is also
     * left to cleanup stack.
     * @param aEntry Reference to calendar entry.
     * @param aIsForwarded ETrue if entry is forwarded
     * @param aIsUpdate ETrue if entry is update to existing entry
     * @return Subject line buffer.
     */
    HBufC* Subject16LC(
            CCalEntry& aEntry,
            TBool aIsForwarded,
            TBool aIsUpdate );
            
    /**
     * Formats the subject line for meeting request email.O wnership of the 
     * created descriptor is transferred to caller. Created object is also
     * left to cleanup stack.
     * @param aEntry Reference to calendar entry.
     * @param aIsForwarded ETrue if entry is forwarded
     * @param aIsUpdate ETrue if entry is update to existing entry
     * @return Subject line buffer.
     */
     HBufC8* Subject8LC(
            CCalEntry& aEntry,
            TBool aIsForwarded,
            TBool aIsUpdate );
     
     /**
      * Fetches the localized updated string for entry
      * @rerurn Localized updated string
      */
     HBufC* UpdatedStringLC();

     /**
      * Fetches the localized canceled string for entry
      * @rerurn Localized updated string
      */
     HBufC* CanceledStringLC();
     
     /**
      * Fetches the localized Reply string for entry
      * @param aEntry Reference to entry
      * @rerurn Localized reply string
      */
     HBufC* ReplyStringLC( CCalEntry& aEntry );     
     
private: // Implementation

    CESMRMailPlainTextFormatter(
            CMRMailboxUtils& aMailboxUtils );
    void ConstructL();
    
    HBufC* SubjectLinePrefix16LC( 
    		CCalEntry& aEntry,
            TBool aIsForwarded,
            TBool aIsSToredToDB );
    
private: // data
    /**
    * Reference to MR mailbox utilities.
    * Not own.
    */
    CMRMailboxUtils& iMailboxUtils;
    
    /**
    * Resource file offset
    */
    TInt iResourceOffset;
    };
    
#endif  // C_ESMRMAILPLAINTEXTFORMATTER_H
