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
* Description:  Specialization of CFsRichTextNewLine for text storing
*
*/


#ifndef C_FSRICHTEXTNEWLINE_H
#define C_FSRICHTEXTNEWLINE_H

#include <e32base.h>

#include "fsrichtextobject.h"

class CFsRichTextNewLine : public CBase, public MFsRichTextObject
    {
    public:
        /**
        * NewL();   
        *
        * 
        */
        static CFsRichTextNewLine* NewL();


        /**
        * ~CFsRichTextNewLine(); 
        *
        * 
        */
        ~CFsRichTextNewLine();
    
    private:
        /**
        * ConstructL();
        *
        * 
        */
        void ConstructL();

        /**
        * CFsRichTextNewLine();
        *
        * 
        */
        CFsRichTextNewLine();
    };

#endif // C_FSRICHTEXTNEWLINE_H

