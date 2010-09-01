/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Declaration of asynchronous text formatter.
*
*/

#ifndef _CFSASYNCTEXTFORMATTER_H_
#define _CFSASYNCTEXTFORMATTER_H_

#include <e32base.h>

class CTextLayout;

/**
 * Observer for informing about active object status.
 */
class MAsyncTextFormatterObserver
    {
public:

    /**
     * Called when text formatting is complete. 
     */
    virtual void FormatAllTextComplete() = 0;
    
    /**
     * Called when text formatting was cancelled.
     */
    virtual void FormatAllTextCancelled() = 0;
    };

/**
 * Asynchronous formatter of text from CTextLayout class.
 */
class CFSAsyncTextFormatter : public CActive
    {
public:
    
    /**
     * NewL for two phase construction.
     */
    static CFSAsyncTextFormatter* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CFSAsyncTextFormatter();
    
    /**
     * Starts active object (formatting).
     * @param aTextLayout pointer to text layout which content needs to be 
     *                    formatted
     * @param aObserver observer which will be informed when formatting 
     *                  finished or cancelled 
     */
    void StartFormatting( CTextLayout* aTextLayout, 
            MAsyncTextFormatterObserver* aObserver );
        
private:
    
    /**
     * constructor.
     */
    CFSAsyncTextFormatter();
    
    /**
     * 2nd phase constructor can leave.
     */
    void ConstructL();
    
protected: // From CActive

    /**
     * From base class CActive
     * Handles an active object's request completion event.
     */
    void RunL();
    
    /**
     * From base class CActive
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();
    
private:
    
    /**
     * Current position of formatted part of document.
     */ 
    TInt iCurrentDocPos;
    
    /**
     * Observer which will be informed when formatting finished.
     * Not own.
     */ 
    MAsyncTextFormatterObserver* iObserver;
       
    /**
     * Text layout which contains text to be formatted.
     * Not own.
     */
    CTextLayout* iTextLayout;
    };

#endif /* _CFSASYNCTEXTFORMATTER_H_ */
