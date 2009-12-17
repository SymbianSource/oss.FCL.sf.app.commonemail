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
* Description: This file implements classes CPictureField, CSmileyField, CNewLineField. 
*
*/

#include "emailtrace.h"
#include "fsrichtextfields.h"

CPictureField::CPictureField(TSize aSize):iSize(aSize)
    {
    FUNC_LOG;
        
    }
    
TInt CPictureField::Value(TPtr& /*aValueText*/)
    {
    FUNC_LOG;
    return 0;  
    }
    
void CPictureField::InternalizeL(RReadStream& /*aStream*/)
    {
    FUNC_LOG;
        
    }
    
TUid CPictureField::Type() const
    {
    FUNC_LOG;
    return KFsRichTextPictureFieldUid;    
    }
    
void CPictureField::GetSize(TSize& aSize)
    {
    FUNC_LOG;
    aSize.iHeight = iSize.iHeight;
    aSize.iWidth = aSize.iWidth;   
    }



CSmileyField::CSmileyField(TInt aSmileyIndex):iSmileyIndex(aSmileyIndex)
    {
    FUNC_LOG;
    
    }
    
TInt CSmileyField::Value(TPtr& /*aValueText*/)
    {
    FUNC_LOG;
    return 0;  
    }
    
void CSmileyField::InternalizeL(RReadStream& /*aStream*/)
    {
    FUNC_LOG;
    
    }
    
TUid CSmileyField::Type()const
    {
    FUNC_LOG;
    return KFsRichTextSmileyFieldUid;
    }
    
TInt CSmileyField::GetSmileyIndex()
    {
    FUNC_LOG;
    return iSmileyIndex;
    }


CNewLineField::CNewLineField()
    {
    FUNC_LOG;
    
    }
    
TInt CNewLineField::Value(TPtr& /*aValueText*/)
    {
    FUNC_LOG;
    return 0;  
    }
    
void CNewLineField::InternalizeL(RReadStream& /*aStream*/)
    {
    FUNC_LOG;
    
    }
    
TUid CNewLineField::Type()const
    {
    FUNC_LOG;
    return KFsRichTextNewLineFieldUid;
    }
    

