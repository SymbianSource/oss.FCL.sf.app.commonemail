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
* Description:  Implementation of class CFsRichTextPicture
*
*/


//#include <huiFsTexture.h>

#include "emailtrace.h"
#include "fsrichtextpicture.h"
#include "fstexture.h"

CFsRichTextPicture* CFsRichTextPicture::NewL()
    {
    CFsRichTextPicture* self = new ( ELeave ) CFsRichTextPicture();
    return self;
    }

CFsRichTextPicture* CFsRichTextPicture::NewL( 
        const TSize aSize )
    {
    CFsRichTextPicture* self = new ( ELeave ) CFsRichTextPicture();
    CleanupStack::PushL( self );
    self->ConstructL( aSize );
    CleanupStack::Pop( self );
    return self;
    }

CFsRichTextPicture::~CFsRichTextPicture()
    {
    
    }
        
TSize CFsRichTextPicture::GetTextureSize() const
    {
    FUNC_LOG;
    return iSize;
    }
    
void CFsRichTextPicture::SetTextureSize( const TSize aSize )
    {
    FUNC_LOG;
    iSize = aSize;
    }
    
void CFsRichTextPicture::ConstructL()
    {
    FUNC_LOG;
    }
    

void CFsRichTextPicture::ConstructL( const TSize aSize )
    {
    FUNC_LOG;
    iSize = aSize;
    }

CFsRichTextPicture::CFsRichTextPicture()
    {
    FUNC_LOG;
    iType = EFsTexture;
    }


