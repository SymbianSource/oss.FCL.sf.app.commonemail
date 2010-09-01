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
* Description: 
*
*/
/*
* ============================================================================
*  Name     : CTestAppContainer from TestAppContainer.h
*  Part of  : TestApp
* ============================================================================
*/

#ifndef TestAppCONTAINER_H
#define TestAppCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <badesca.h>
   
// FORWARD DECLARATIONS
class CEikTextListBox;
class MAknsSkinInstance;
class CAknsBasicBackgroundControlContext;


// CLASS DECLARATION

/**
*  CTestAppContainer  container control class.
*  
*/
class CTestAppContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CTestAppContainer();

    public: // New functions
        void AppendDataL( TDesC& buf );
    public: // Functions from base classes

    private: // Functions from base classes
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void Draw(const TRect& aRect) const;
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
    private: //data
        CEikTextListBox* iListBox;
        CDesCArrayFlat* iItems;  
        
        /// Ref: A pointer MAknsSkinInstance
        MAknsSkinInstance* iSkinInstance;

        /// Own: A pointer CAknBasicBackgroundControlContext
		CAknsBasicBackgroundControlContext* iBackgroundSkinContext;
    };

#endif

// End of File
