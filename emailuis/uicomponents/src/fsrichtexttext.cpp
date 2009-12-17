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
* Description:  Implementation of class CFsRichTextText
*
*/


#include "emailtrace.h"
#include "fsrichtexttext.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFsRichTextText* CFsRichTextText::NewL()
    {
    FUNC_LOG;
    CFsRichTextText* self = new ( ELeave ) CFsRichTextText();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//    
CFsRichTextText* CFsRichTextText::NewL( const TDesC& aText )
    {
    FUNC_LOG;
    CFsRichTextText* self = new ( ELeave ) CFsRichTextText();
    CleanupStack::PushL( self );
    self->ConstructL( aText );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFsRichTextText* CFsRichTextText::NewL( 
                    const TDesC& aText, 
                    TInt aTextStyle )
    {
    FUNC_LOG;
    CFsRichTextText* self = new ( ELeave ) CFsRichTextText( aTextStyle );
    CleanupStack::PushL( self );
    self->ConstructL( aText );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsRichTextText
// ---------------------------------------------------------------------------
//
CFsRichTextText::~CFsRichTextText()
    {
    FUNC_LOG;
    if ( iText )
        {
        delete iText; 
        }
    }

// ---------------------------------------------------------------------------
// GetText
// ---------------------------------------------------------------------------
//    
TDesC& CFsRichTextText::GetText() const 
    {
    FUNC_LOG;
    return *iText;
    }

// ---------------------------------------------------------------------------
// SetText
// ---------------------------------------------------------------------------
//    
void CFsRichTextText::SetTextL( const TDesC& aText )
    {
    FUNC_LOG;
    iText = aText.AllocL();
    }

// ---------------------------------------------------------------------------
// GetStyleId
// ---------------------------------------------------------------------------
//
TInt CFsRichTextText::GetStyleId() const
    {
    FUNC_LOG;
    return iStyle;
    }

// ---------------------------------------------------------------------------
// SetStyleId
// ---------------------------------------------------------------------------
//
void CFsRichTextText::SetStyleId( const TInt aTextStyleId )
    {
    FUNC_LOG;
    iStyle = aTextStyleId;
    }    

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CFsRichTextText::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CFsRichTextText::ConstructL( const TDesC& aText )
    {
    FUNC_LOG;
    iText = aText.AllocL();
    }

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//
CFsRichTextText::CFsRichTextText()
    {
    FUNC_LOG;
    iType = EFsText; 
    }

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//
CFsRichTextText::CFsRichTextText( const TInt aTextStyle ) 
        : iStyle( aTextStyle )
    {
    FUNC_LOG;
    iType = EFsText; 
    }
   

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//  
void CFsRichTextText::SetTextColor(TRgb aColor)
    {
    FUNC_LOG;
    iColor = aColor;
    }

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//     
TRgb CFsRichTextText::GetTextColor() const
    {
    FUNC_LOG;
    return iColor;
    }

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//  
void CFsRichTextText::SetTextDirection(TBidiText::TDirectionality aTextDirection)
    {
    FUNC_LOG;
    iTextDirection = aTextDirection;
    }

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//     
TBidiText::TDirectionality CFsRichTextText::GetTextDirection() const
    {
    FUNC_LOG;
    return iTextDirection;
    }

