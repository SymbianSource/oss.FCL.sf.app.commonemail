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
* Description: This file defines class CIpsSetDataCtrlCenrep.
*
*/


#ifndef CIPSSETDATACTRLCENREP_H
#define CIPSSETDATACTRLCENREP_H


class CRepository;

/**
 *  Class for Central Repository handling.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetDataCtrlCenrep : public CBase
    {
public:  // Constructors and destructor

    /**
     * Creates object from CIpsSetDataCtrlCenrep and leaves it to cleanup stack
     *
     * @return, Constructed object
     */
    static CIpsSetDataCtrlCenrep* NewLC();

    /**
     * Create object from CIpsSetDataCtrlCenrep
     *
     * @return, Constructed object
     */
    static CIpsSetDataCtrlCenrep* NewL();

    /**
     * Destructor
     */
    virtual ~CIpsSetDataCtrlCenrep();

public: // New functions

    /**
     * @param aRepository Central Repository session to be used.
     */
    void SetActiveCentralRepository(
        CRepository& aRepository );

    /**
     * Sets active base key for the session.
     *
     * @param aAccountId Active base key to be used for storing.
     * @param aProtocol Protocol of the used account.
     */
    void SetActiveBaseKey(
        const TUint32 aAccountId,
        const TUid& aProtocol );

    /**
     * Sets active base key for the session.
     *
     * @param aExtendedAccountId
     */
    void SetActiveBaseKey(
        const TUint32 aExtendedAccountId );

    /**
     * Sets default data key as active key.
     */
    void SetActiveDefaultDataKey();

    /**
     * Creates setting key to Central Repository.
     *
     * @param aSetting Unique key ID of the setting.
     * @param aString String to be stored.
     */
    void CreateToAccountL(
        const TUint32 aSetting,
        const TDesC& aString );

    /**
     * Creates setting key to Central Repository.
     *
     * @param aSetting Unique key ID of the setting.
     * @param aValue Value to be stored.
     */
    void CreateToAccountL(
        const TUint32 aSetting,
        const TInt aValue );

    /**
     * Creates setting key to Central Repository.
     *
     * @param aSettingHigh Unique key ID of the high setting.
     * @param aSettingLow Unique key ID of the low setting.
     * @param aValue Value to be stored.
     */
    void CreateToAccountL(
        const TInt aSettingHigh,
        const TInt aSettingLow,
        const TInt64 aValue );

    /**
     * Removes key from Central Repository.
     *
     * @param aSetting Unique key ID of the setting.
     */
     TInt DeleteFromAccount(
        const TInt aSetting ) const;

    /**
     * Removes key from Central Repository.
     *
     * @param aSettingHigh Unique key ID of the high setting.
     * @param aSettingLow Unique key ID of the low setting.
     */
     TInt DeleteFromAccount(
        const TInt aSettingHigh,
        const TInt aSettingLow ) const;

    /**
     * Sets string in Central Repository.
     *
     * @param aSetting Unique key ID of the setting.
     * @param aString String to be stored.
     */
    void SetToAccountL(
        const TUint32 aSetting,
        const TDesC& aString );
    /**
     * Sets value in Central Repository.
     *
     * @param aSetting Unique key ID of the setting.
     * @param aValue Value to be stored.
     */
    void SetToAccountL(
        const TUint32 aSetting,
        const TInt aValue );

    /**
     * Sets 64-bit value to Central Repository.
     *
     * @param aSettingHigh Unique key ID of the high setting.
     * @param aSettingLow Unique key ID of the low setting.
     * @param aSetting 64-bt value to be stored.
     */
    void SetToAccountL(
        const TInt aSettingHigh,
        const TInt aSettingLow,
        const TInt64 aSetting );

    /**
     * Retrieves 64-bit value from Central Repository.
     * non leaving function
     *
     * @param aSettingHigh Unique key ID of the high setting.
     * @param aSettingLow Unique key ID of the low setting.
     * @param aValue parameter for setting value
     * @return KErrNone or cenrep get error code
     */
    TInt GetFromAccount(
        const TInt aSettingHigh,
        const TInt aSettingLow,
        TInt64& aValue ) const;

    /**
     * Retrieves number value from Central Repository.
     * non leaving function
     *
     * @param aSetting Unique key ID of the setting.
     * @param aValue parameter for setting value
     * @return KErrNone or cenrep get error code
     */
    TInt GetFromAccount(
        const TInt aSetting,
        TInt& aValue ) const;

    /**
     * Retrieves text data from Central Repository.
     * non leaving function
     *
     * @param aSetting Unique key ID of the setting.
     * @param aText Parameter to store the text.
     * @return KErrNone or cenrep get error code
     */
    TInt GetTextFromAccount(
        const TInt aSetting,
        TDes& aText ) const;
    
    /**
     * Retrieves 64-bit value from Central Repository.
     * If setting value can't be found function creates it
     * to extended settings cen rep
     * 
     * @param aSettingHigh Unique key ID of the high setting.
     * @param aSettingLow Unique key ID of the low setting.
     * @param aDefaultValue initial value for created cen rep value
     * @param aValue parameter for setting value, is set to
     *               default if cen rep value can't be found
     * @return KErrNone or cenrep get error code
     */
    TInt GetFromAccountAndCreateIfNotFound(
        const TInt aSettingHigh,
        const TInt aSettingLow,
        const TInt64 aDefaultValue,
        TInt64& aValue );

    /**
     * Retrieves number value from Central Repository.
     * If setting value can't be found function creates it
     * to extended settings cen rep
     * 
     * @param aSetting Unique key ID of the setting.
     * @param aDefaultValue initial value for created cen rep value
     * @param aValue parameter for setting value
     * @return KErrNone or cenrep get error code
     */
    TInt GetFromAccountAndCreateIfNotFound(
        const TInt aSetting,
        const TInt aDefaultValue,
        TInt& aValue );

protected:  // Constructors

    /**
     * Default constructor for classCIpsSetDataCtrlCenrep
     * @return, Constructed object
     */
    CIpsSetDataCtrlCenrep();

    /**
     * Symbian 2-phase constructor
     */
    void ConstructL();

private:    // Data

    /**
     * Pointer to active Central Repository
     * Not owned.
     */
    CRepository* iCenRep;

    /**
     * Currently active account
     */
    TUint32 iBaseKey;
    };

#endif // CIPSSETDATACTRLCENREP_H

// End of File
