/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Title pane for UI
*
*/

#ifndef CESMRTITLEPANE_H
#define CESMRTITLEPANE_H

#include <coecntrl.h>
#include <AknsItemID.h>
//<cmail>
#include "mesmrcalentry.h"
#include "esmrdef.h"
//</cmail>

class CMRImage;
class CMRLabel;

/**
 * This class is responsibe of showing the current title.
 */
NONSHARABLE_CLASS( CESMRTitlePane ) : public CCoeControl
    {

public:
    /**
     * Constructor for new title pane.
     *
     * @param aLayout - pointer to layout manager
     */
    static CESMRTitlePane* NewL();
    static CESMRTitlePane* NewLC();
    // Destructor
    ~CESMRTitlePane();

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();

public:
	/**
	 * Handle set the priority
	 * @param aPriority
	 */
    void HandleSetPriorityCmdL( TInt aPriority );
    /**
     * Sets the priority indicator for this pane
     *
     * @param aPriority - priority to be set
     * @param aEntryType - entry type
     */
    void SetPriorityL(TUint aPriority );

    /**
     * Sets the text for this pane
     *
     * @param aText - descriptor that contains the text
     */
    void SetTextL( const TDesC& aText );

    /**
     * Called by the view component. Title pane fethces the priority of entry
     * @param aEntry Current calendar entry
     */
    void InternalizeL( MESMRCalEntry& aEntry );

    /**
     * Called by the view component. Title pane stores the priority of entry
     * @param aEntry Current calendar entry
     */
    void ExternalizeL( MESMRCalEntry& aEntry );

    /**
      * Set default icon for the view if needed
      */
    void SetDefaultIcon(TBool aDefault);

private:
    CESMRTitlePane();
    void ConstructL();
    void CreateIconL( TAknsItemID aIconID );

private:
    /// Own: Current priority
    TUint iPriority;
    /// Own: entry type:
    MESMRCalEntry::TESMRCalEntryType iEntryType;
    /// Own: do we have a default icon
    TBool iDefaultIcon;
    /// Own: Field icon
    CMRImage* iIcon;
    /// Own: Field text label
    CMRLabel* iTitle;
    };
#endif // CESMRTITLEPANE_H

// EOF
