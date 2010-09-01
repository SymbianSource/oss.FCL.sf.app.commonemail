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
* Description:  Specialization of CFsRichTextObject for picture storing
*
*/


#ifndef C_FSRICHTEXTPICTURE_H
#define C_FSRICHTEXTPICTURE_H

#include <e32base.h>

#include "fsrichtextobject.h"

class CFsTexture;

class CFsRichTextPicture : public CBase, public MFsRichTextObject
    {
    public:
        /**
    	* CFsRichTextPicture* NewL( 
    	*       const TSize aSize );
        * 
    	* @param aSize
        */
    	static CFsRichTextPicture* NewL( );
    
        /**
        * CFsRichTextPicture* NewL( 
        *       const TSize aSize );
        * 
        * @param aSize
        */
        static CFsRichTextPicture* NewL( 
                const TSize aSize );
        
        /**
        * ~CFsRichTextPicture();
        * 
        * 
        */
        ~CFsRichTextPicture();

    public:
        /**
        * GetTextureSize() const;
        * 
        * 
        */
        TSize GetTextureSize() const;
    
        /**
        * SetTextureSize( const TSize aSize );
        * 
        * @param aSize
        */
        void SetTextureSize( const TSize aSize );
    
    private:
        /**
        * ConstructL();
        * 
        * 
        */
        void ConstructL();
    
        /**
        * ConstructL( const TSize aSize );
        * 
        * @param aSize
        */
        void ConstructL( const TSize aSize );

        /**
        * CFsRichTextPicture(CFsTexture& aTexture);
        * 
        * @param aTexture
        */
        CFsRichTextPicture();
    
    private:    
        TSize iSize;
    };

#endif // C_FSRICHTEXTPICTURE_H


