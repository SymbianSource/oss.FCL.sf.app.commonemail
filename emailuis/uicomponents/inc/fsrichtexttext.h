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


#ifndef C_FSRICHTEXTTEXT_H
#define C_FSRICHTEXTTEXT_H

#include <e32base.h>
#include <gdi.h>

#include "fsrichtextobject.h"
#include <biditext.h>

class CFsRichTextText : public CBase, public MFsRichTextObject
    {
    public:
        /**
        * NewL();
        * 
        * 
        */
        static CFsRichTextText* NewL();
    
        /**
        * NewL( 
        *       const TDesC& aText );
        * 
        * @param aText
        */
        static CFsRichTextText* NewL( 
                const TDesC& aText );
    
        /**
        * NewL( 
        *       const TDesC& aText, 
        *       const TInt aTextStyleId );
        * 
        * @param aText
        * @param aTextStyleId
        */
        static CFsRichTextText* NewL( 
                const TDesC& aText, 
                const TInt aTextStyleId );

        /**
        * ~CFsRichTextText();
        * 
        * 
        */
        ~CFsRichTextText();

    public:
        /**
        * GetText() const;
        * 
        * 
        */
        TDesC& GetText() const;
    
        /**
        * SetTextL( const TDesC& aText );
        * 
        * @param aText
        */
        void SetTextL( const TDesC& aText );
        
        /**
        * GetStyleId() const;
        * 
        * 
        */
        TInt GetStyleId() const;
    
        /**
        * SetStyleId( const TInt aTextStyleId );
        * 
        * @param aTextStyleId
        */
        void SetStyleId( const TInt aTextStyleId );
        
        /**
        * SetTextColor(TRgb aColor);
        * 
        * @param aColor
        */
        void SetTextColor(TRgb aColor);
        
        /**
        * GetTextColor() const;
        * 
        * 
        */
        TRgb GetTextColor() const;
    
        /**
        * SetTextDirection(TInt aTypeOfHotSpot);
        * 
        * @param aTypeOfHotSpot
        */
        void SetTextDirection(TBidiText::TDirectionality aTextDirection);
        
        /**
        * GetTextDirection() const;
        * 
        * 
        */
        TBidiText::TDirectionality GetTextDirection() const;
    private:
        /**
        * ConstructL();
        * 
        * 
        */
        void ConstructL();
    
        /**
        * ConstructL( const TDesC& aText );
        * 
        * @param aText
        */
        void ConstructL( const TDesC& aText );

        /**
        * CFsRichTextText();
        * 
        * 
        */
        CFsRichTextText();
    
        /**
        * CFsRichTextText( const TInt aTextStyle );
        * 
        * @param aTextStyle
        */
        CFsRichTextText( const TInt aTextStyle );

    private:    
        HBufC* iText;
        
        TInt iStyle;
        
        TRgb iColor;
        
        TBidiText::TDirectionality iTextDirection;
    };

#endif // C_FSRICHTEXTTEXT_H

