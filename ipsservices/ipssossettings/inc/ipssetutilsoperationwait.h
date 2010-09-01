/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class CIpsSetUtilsOperationWait.
*
*/


#ifndef IPSSETUTILSOPERATIONWAIT_H
#define IPSSETUTILSOPERATIONWAIT_H

// <cmail>
#include <AknProgressDialog.h> //MProgressDialogCallback
// </cmail>
/**
 * Allows a synchronous wait on a operation.
 *
 * @since FS v1.0
 * @lib IpsSosSetting.lib
 */
class CIpsSetUtilsOperationWait: public CActive,
								 public MProgressDialogCallback

    {
public:

    /**
     * Symbian constructor.
     */
    static CIpsSetUtilsOperationWait* NewLC(
        TInt aPriority = EPriorityStandard );

    /**
     * Destructor.
     */
    virtual ~CIpsSetUtilsOperationWait();

    /**
     * Starts the operation and shows the wait dialog
     */
    void Start();

    // <cmail>
    /**
     * Starts the operation and shows the wait dialog
     */
    void StartAndShowWaitDialogL();
    // </cmail>
    
protected:

    /**
     * Default constructor.
     */
    CIpsSetUtilsOperationWait( TInt aPriority );

    /**
     * Run the operation.
     */
    void RunL();

    /**
     * Cancel the operation.
     */
    void DoCancel();
    /**
     * From MProgressDialogCallback
     */
    virtual void DialogDismissedL( TInt aButtonId );

    /**
     * Creates a wait dialog and executes it
     */
    void StartWaitDialogL();

    /**
     * Finalizes the wait dialog
     */
    void StopWaitDialogL();

protected:
	
	/**
	 * Active scheduler.
	 */
	CActiveSchedulerWait iWait;
	
	// <cmail>
	/**
	 * Wait dialog
	 */
	CAknWaitDialog* iWaitDialog;
	// </cmail>
    };

#endif /* IPSSETUTILSOPERATIONWAIT_H */

// End of file
