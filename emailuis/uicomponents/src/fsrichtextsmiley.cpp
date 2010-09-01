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
* Description:  Implementation of class CFsRichTextSmiley
*
*/


#include "emailtrace.h"
#include "fsrichtextsmiley.h"
#include "fsrichtexttext.h"
#include "fsrichtextpicture.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFsRichTextSmiley* CFsRichTextSmiley::NewL()
    {
    FUNC_LOG;
    CFsRichTextSmiley* self = new ( ELeave ) CFsRichTextSmiley();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CFsRichTextSmiley* CFsRichTextSmiley::NewL( 
        CFsRichTextText* aRichTextText, 
        CFsRichTextPicture* aRichTextPicture )
    {
    FUNC_LOG;
    CFsRichTextSmiley* self = new ( ELeave ) CFsRichTextSmiley(
            aRichTextText, 
            aRichTextPicture);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;   
    }

CFsRichTextSmiley::~CFsRichTextSmiley()
    {
    FUNC_LOG;
    delete iRichTextText;
    delete iRichTextPicture;
    }

void CFsRichTextSmiley::SetTextObject(CFsRichTextText* aRichTextText)
    {
    FUNC_LOG;
    iRichTextText = aRichTextText;
    }
        
void CFsRichTextSmiley::SetPictureObject(CFsRichTextPicture* aRichTextPicture)
    {
    FUNC_LOG;
    iRichTextPicture = aRichTextPicture;
    }
        
CFsRichTextText* CFsRichTextSmiley::GetTextObject()
    {
    FUNC_LOG;
    return iRichTextText;
    }
        
CFsRichTextPicture* CFsRichTextSmiley::GetPictureObject()
    {
    FUNC_LOG;
    return iRichTextPicture;
    }
        
void CFsRichTextSmiley::ConstructL()
    {
    FUNC_LOG;
    
    }
        
CFsRichTextSmiley::CFsRichTextSmiley()
: iRichTextText(NULL), iRichTextPicture(NULL)
    {
    FUNC_LOG;
    iType = EFsSmiley;
    }
        
CFsRichTextSmiley::CFsRichTextSmiley(
        CFsRichTextText* aRichTextText, 
        CFsRichTextPicture* aRichTextPicture )
: iRichTextText(aRichTextText), iRichTextPicture(aRichTextPicture)
    {
    FUNC_LOG;
    iType = EFsSmiley;
    }



