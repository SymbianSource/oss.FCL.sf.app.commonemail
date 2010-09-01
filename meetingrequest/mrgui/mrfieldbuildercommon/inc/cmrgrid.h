/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MR grid impl.
*
*/
#ifndef CMRGRID_H
#define CMRGRID_H

// INCLUDES
#include <e32base.h>
//#include <akngrid.h>
//#include <akngridm.h>

// FORWARD DECLARATIONS
class CAknGrid;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRGrid ) : public CBase
    {
    public:
        /**
         * Executes selection grid.
         *
         * @param aSelectedOption on return contains the selection
         * @return ETrue if user selected an option. Otherwise EFalse.
         */
        static TBool ExecuteL( TInt& aSelectedOption );

    private:
        CMRGrid();
        virtual ~CMRGrid();
        void AddGridIconsL();
        TBool ShowGridL( TInt& aSelectedOption );

    private:
        /// Own: Grid control
        CAknGrid*  iGrid;
    };

#endif  // CMRGRID_H
// End of file


