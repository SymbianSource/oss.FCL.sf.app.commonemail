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
* Description:  MR titlepane handler header
*
*/

#ifndef CESMRTITLEPANEHANDLER_H
#define CESMRTITLEPANEHANDLER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAknTitlePane;
class CEikonEnv;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CESMRTitlePaneHandler ) : public CBase
    {
    public:
        static CESMRTitlePaneHandler* NewL( CEikonEnv& aEnv );
        ~CESMRTitlePaneHandler();

    public:
        /**
         * Restores the original titlepane text that was saved
         * before setting new one into titlepane. If called before
         * SetNewTitle() does nothing.
         */
        void Rollback();

        /**
         * Sets new title. If aNewTitle is NULL the titlepane
         * default is used.
         * @param aNewTitle New titlepane text as reference
         */
        void SetNewTitle( const TDesC* aNewTitle );

    private:
        CESMRTitlePaneHandler( CEikonEnv& aEnv );
        void ConstructL();
        void SetTitlePaneTextL( const TDesC* aNewTitle );

    private:
        // Own: Original title pane text before change
        HBufC* iSaveTitlePaneText;
        // Ref:
        CEikonEnv& iEikEnv;
        // Ref:
        CAknTitlePane* iTitlePane;
    };

#endif // CESMRTITLEPANEHANDLER_H

// End of file

