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
* Description:  Specialization of CFsRichTextObject for text storing
*
*/


#ifndef C_FSRICHTEXTSMILEY_H
#define C_FSRICHTEXTSMILEY_H

#include <e32base.h>

#include "fsrichtextobject.h"

class CFsRichTextText;
class CFsRichTextPicture;

class CFsRichTextSmiley : public CBase, public MFsRichTextObject
    {
    public:
        /**
        * NewL();
        * 
        * 
        */
        static CFsRichTextSmiley* NewL();
    
        /**
        * NewL( 
        *       CFsRichTextText* aRichTextText, 
        *       CFsRichTextPicture* aRichTextPicture );
        * 
        * @param aRichTextText
        * @param aRichTextPicture
        */
        static CFsRichTextSmiley* NewL( 
                CFsRichTextText* aRichTextText, 
                CFsRichTextPicture* aRichTextPicture );

        /**
        * ~CFsRichTextSmiley();
        * 
        * 
        */
        ~CFsRichTextSmiley();

    public:
        /**
        * SetTextObject(CFsRichTextText* aRichTextText);
        * 
        * @param aRichTextText
        */
        void SetTextObject(CFsRichTextText* aRichTextText);
        
        /**
        * SetPictureObject(CFsRichTextPicture* aRichTextPicture);
        * 
        * @param aRichTextPicture
        */
        void SetPictureObject(CFsRichTextPicture* aRichTextPicture);
        
        /**
        * GetTextObject();
        * 
        * 
        */
        CFsRichTextText* GetTextObject();
        
        /**
        * GetPictureObject();
        * 
        * 
        */
        CFsRichTextPicture* GetPictureObject();
        
    private:
        /**
        * ConstructL();
        * 
        * 
        */
        void ConstructL();
        
        /**
        * CFsRichTextSmiley();
        * 
        * 
        */
        CFsRichTextSmiley();
        
        /**
        * CFsRichTextSmiley(
        *       CFsRichTextText* aRichTextText,
        *       CFsRichTextPicture* aRichTextPicture );
        * 
        * @param aRichTextText
        * @param aRichTextPicture
        */
        CFsRichTextSmiley(
                CFsRichTextText* aRichTextText,
                CFsRichTextPicture* aRichTextPicture );

    private:    
        CFsRichTextText* iRichTextText;
        CFsRichTextPicture* iRichTextPicture;
    };

#endif // C_FSRICHTEXTSMILEY_H



