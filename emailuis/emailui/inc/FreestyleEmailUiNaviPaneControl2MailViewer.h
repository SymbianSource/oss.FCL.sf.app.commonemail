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
* Description: encapsulates navipane controls of mail viewer view
*
*/

#ifndef CFREESTYLEEMAILNAVIPANECONTROL1_H
#define CFREESTYLEEMAILNAVIPANECONTROL1_H

#include <coecntrl.h>
class CCoeControlArray;

NONSHARABLE_CLASS( CFreestyleEmailUiNaviPaneControlContainer2MailViewer ) : public CCoeControl
    {
    public:
        enum TFreestyleEmailNaviControlId
            {
            EFreestyleEmailNaviTimeControlId,
            };

    public:
        static CFreestyleEmailUiNaviPaneControlContainer2MailViewer* NewL();
        virtual ~CFreestyleEmailUiNaviPaneControlContainer2MailViewer();

        /**
        * From CoeControl
        * See the documentation from the coecntrl.h
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

    private:
        CFreestyleEmailUiNaviPaneControlContainer2MailViewer();
        void ConstructL();

        void AddToControlArrayL( CCoeControl* aControl, TFreestyleEmailNaviControlId aControlId );
        void RemoveFromControlArray( TFreestyleEmailNaviControlId aControlId );

    private: // data

        /** Control array */
        CCoeControlArray* iControlArray;
    };

#endif // CFREESTYLEEMAILNAVIPANECONTROL1_H
