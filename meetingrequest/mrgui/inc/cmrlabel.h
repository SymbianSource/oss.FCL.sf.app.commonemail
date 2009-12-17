/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for Location URL Parser plug-in
*
*/

#ifndef CMRLABEL_H
#define CMRLABEL_H

// INCLUDES
#include <eiklabel.h> 

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

class CMRLabel : public CEikLabel
    {
    public:
        /**
         * Static construtor
         * @return New instance of this class
         */
        IMPORT_C static CMRLabel* NewL();
        /**
         * Destructor
         */
        IMPORT_C ~CMRLabel();
        
    public: // From base class
        void FocusChanged(TDrawNow aDrawNow);
        
    private: // Implementation
        CMRLabel();
        void ConstructL();
        
    private: // Data
    };

#endif // CMRLABEL_H

// End of file
