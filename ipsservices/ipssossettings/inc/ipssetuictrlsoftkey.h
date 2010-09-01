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
* Description:  Decleares the softkey controller class.
*
*/


#ifndef IPSSETUICTRLSOFTKEY_H
#define IPSSETUICTRLSOFTKEY_H

#include <e32base.h>
#include <eikcmobs.h>           // MEikCommandObserver

class CEikButtonGroupContainer;

/**
 * Class to control softkeys.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
NONSHARABLE_CLASS( CIpsSetUiCtrlSoftkey ) :
    public CBase,
    public MEikCommandObserver
    {
public:

    /**
     * Flags to track, which softkeys are used.
     */
    enum TIpsSetUiSoftkeyFlags
        {
        /** Observed key. */
        EObservingLeftSoftkey           = 0x01,
        /** Temp key. */
        EObservingUnknownKey            = 0x02,
        /** Observed key. */
        EObservingRightSoftkey          = 0x04,
        /** Observed key. */
        EObservingMiddleSoftkey         = 0x08,
        /** Allow MSK label change. */
        EAllowMiddleSoftkeyLabelChange  = 0x10,
        /** Absorb key presses. */
        EAbsorbing                      = 0x20,
        /** Keep this last. */
        ELastFlag
        };

    /**
     * Keys that are observed
     */
    enum TIpsSetUiObservedKeys
        {
        ELeftSoftkey = 0,
        EUnknownKey,
        ERightSoftkey,
        EMiddleSoftkey
        };

public:  // Constructors and destructor

    /**
     * Creates object from CIpsSetUiCtrlSoftkey and leaves it to cleanup stack
     *
     * @return, Constructed object
     */
    static CIpsSetUiCtrlSoftkey* NewLC(
        CEikButtonGroupContainer& aSoftkeys );

    /**
     * Create object from CIpsSetUiCtrlSoftkey
     *
     * @return, Constructed object
     */
    static CIpsSetUiCtrlSoftkey* NewL(
        CEikButtonGroupContainer& aSoftkeys );

    /**
     * Destructor
     */
    virtual ~CIpsSetUiCtrlSoftkey();

public: // New operators

    /**
     * Get a direct access to softkeys
     *
     * @return Reference to softkey container
     */
    CEikButtonGroupContainer& operator()();

public: // New functions


    /**
     * Sets the observer to the specific key
     *
     * @param aSoftkey Key to be observed
     * @param aCommandObserver The observer for the key events
     * @return KErrNone, when setting the observer is succesful
     */
    TInt SetObserver(
        const TIpsSetUiObservedKeys aSoftkey,
        MEikCommandObserver& aCommandObserver );

    /**
     * Sets/Removes the observers for the softkeys
     *
     * @param aLeftSoftkey Set/Remove left softkey observer
     * @param aRightSoftkey Set/Remove right softkey observer
     * @param aMiddleSoftkey Set/Remove middle softkey observer
     * @param aCommandObserver The observer for the key events
     * @return KErrNone, when setting the observer is succesful
     */
    TInt SetObserver(
        const TBool aLeftSoftkey,
        const TBool aRightSoftkey,
        const TBool aMiddleSoftkey,
        MEikCommandObserver& aCommandObserver );

    /**
     * Removes the observer
     *
     * @param aSoftkey
     */
    void RemoveObserver(
        const TIpsSetUiObservedKeys aSoftkey );

    /**
     * Defines a new softkey
     *
     * @param aLabelResource Resource of the text for the softkey label
     * @param aSoftkeyPos Position of the softkey
     * @param aSoftkeyCmd Command executed by pressing the softkey
     * @param aCommandObserver Observer to receive the events
     */
    void DefineCustomSoftkeyL(
        const TInt aLabelResource,
        const TIpsSetUiObservedKeys aSoftkeyPos,
        const TInt aSoftkeyCmd,
        MEikCommandObserver* aCommandObserver );

    /**
     * Defines a new softkey
     *
     * @param aSoftkeyLabel Text for the softkey label
     * @param aSoftkeyPos Position of the softkey
     * @param aSoftkeyCmd Command executed by pressing the softkey
     * @param aCommandObserver Observer to receive the events
     */
    void DefineCustomSoftkeyL(
        const TDesC& aSoftkeyLabel,
        const TIpsSetUiObservedKeys aSoftkeyPos,
        const TInt aSoftkeyCmd,
        MEikCommandObserver* aCommandObserver );

    /**
     * Checks the current status of MSK label updating availability
     *
     * @return ETrue, when MSK label change is allowed
     * @return EFalse, when MSK label change is not allowed
     */
    TBool MSKLabelling();

    /**
     * Sets the status of MSK label updating
     *
     * @param aNewState Allow/Disallow MSK label change
     */
    void SetMSKLabelling( const TBool aNewState );

    /**
     * Changes the label of the middle softkey
     *
     * @param aLabelResource Resource of the text for the softkey label
     * @param aCommandId Command executed by pressing the softkey
     * @param aCommandObserver Observer to receive the events
     */
    void UpdateMiddleSoftkeyLabelL(
        const TInt aLabelResource,
        const TInt aCommandId,
        MEikCommandObserver* aCommandObserver );

    /**
     * @param aParam Absorption on/off
     */
    void SetAbsorb( const TBool aAbsorb );

protected:  // Constructors

    /**
     * Default constructor for classCIpsSetUiCtrlSoftkey
     *
     * @return, Constructed object
     */
    CIpsSetUiCtrlSoftkey(
        CEikButtonGroupContainer& aSoftkeys );

    /**
     * Symbian 2-phase constructor
     */
    void ConstructL();

private:  // New functions

    /**
     * Changes observer status.
     *
     * @param aOn Turn observing On/Off.
     * @param aSoftkey Key to be controlled.
     * @param aCommandObserver Observer for the key.
     * @return Result of the assignment.
     */
    TInt ChangeObserver(
        const TBool aOn,
        const TIpsSetUiObservedKeys aSoftkey,
        MEikCommandObserver& aCommandObserver );

// From base class MEikCommandObserver

    /**
     * Handle commands. Dummy for absroption.
     *
     * @param aCommandId Executed command.
     */
    void ProcessCommandL( TInt aCommandId );

private:    // Data

    /**
     * Softkey container
     */
    CEikButtonGroupContainer&   iSoftkeys;

    /**
     * Container for the flags
     */
    TUint64                     iFlags;

    /**
     * Container for the last used resource for MSK
     */
    TInt                        iPreviousMskLabelResource;

    };

#endif /* IPSSETUICTRLSOFTKEY_H */

// End of File

