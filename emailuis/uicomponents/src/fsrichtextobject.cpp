/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class MFsRichTextObject
*
*/


#include "emailtrace.h"
#include "fsrichtextobject.h"

// ---------------------------------------------------------------------------
// GetType
// ---------------------------------------------------------------------------
//
MFsRichTextObject::TFsRichTextObjectType MFsRichTextObject::GetType() const
    {
    FUNC_LOG;
    return iType;
    }
    
// ---------------------------------------------------------------------------
// GetBeginOfObject
// ---------------------------------------------------------------------------
//
TInt MFsRichTextObject::GetBeginOfObject() const
    {
    FUNC_LOG;
    return iBeginOfObject;   
    }

// ---------------------------------------------------------------------------
// GetEndOfObject
// ---------------------------------------------------------------------------
//
TInt MFsRichTextObject::GetEndOfObject() const
    {
    FUNC_LOG;
    return iEndOfObject;    
    }
  
// ---------------------------------------------------------------------------
// SetBeginOfObject
// ---------------------------------------------------------------------------
//  
void MFsRichTextObject::SetBeginOfObject(TInt aBeginOfObject)
    {
    FUNC_LOG;
    iBeginOfObject = aBeginOfObject;
    }

// ---------------------------------------------------------------------------
// SetEndOfObject
// ---------------------------------------------------------------------------
//        
void MFsRichTextObject::SetEndOfObject(TInt aEndOfObject)
    {
    FUNC_LOG;
    iEndOfObject = aEndOfObject;
    }

// ---------------------------------------------------------------------------
// SetEmbeded
// ---------------------------------------------------------------------------
//   
void MFsRichTextObject::SetEmbeded(TBool aEmbeded)
    {
    FUNC_LOG;
    iEmbeded = aEmbeded;    
    }
   
// ---------------------------------------------------------------------------
// SetIdOfEmbeded
// ---------------------------------------------------------------------------
//    
void MFsRichTextObject::SetIdOfEmbeded(TInt aIdOfEmbeded)
    {
    FUNC_LOG;
    iIdOfEmbeded = aIdOfEmbeded;
    }
    
// ---------------------------------------------------------------------------
// GetEmbeded
// ---------------------------------------------------------------------------
//       
TBool MFsRichTextObject::GetEmbeded() const
    {
    FUNC_LOG;
    return iEmbeded;
    }

// ---------------------------------------------------------------------------
// GetIdOfEmbeded
// ---------------------------------------------------------------------------
//      
TInt MFsRichTextObject::GetIdOfEmbeded() const
    {
    FUNC_LOG;
    return iIdOfEmbeded;
    }




// ---------------------------------------------------------------------------
// SetHotSpot
// ---------------------------------------------------------------------------
//    
void MFsRichTextObject::SetHotSpot(TBool aHotSpot)
    {
    FUNC_LOG;
    iHotSpot = aHotSpot;
    }
// ---------------------------------------------------------------------------
// SetIdOfHotSpot
// ---------------------------------------------------------------------------
//        
void MFsRichTextObject::SetIdOfHotSpot(TInt aIdOfHotSpot)
    {
    FUNC_LOG;
    iIdOfHotSpot = aIdOfHotSpot;
    }
// ---------------------------------------------------------------------------
// GetHotSpot
// ---------------------------------------------------------------------------
//        
TBool MFsRichTextObject::GetHotSpot() const
    {
    FUNC_LOG;
    return iHotSpot;
    }
// ---------------------------------------------------------------------------
// GetIdOfHotSpot
// ---------------------------------------------------------------------------
//        
TInt MFsRichTextObject::GetIdOfHotSpot() const
    {
    FUNC_LOG;
    return iIdOfHotSpot;
    }
// ---------------------------------------------------------------------------
// SetTypeOfHotSpot
// ---------------------------------------------------------------------------
//       
void MFsRichTextObject::SetTypeOfHotSpot(TInt aTypeOfHotSpot)
    {
    FUNC_LOG;
    iTypeOfHotSpot = aTypeOfHotSpot;
    }
// ---------------------------------------------------------------------------
// GetTypeOfHotSpot
// ---------------------------------------------------------------------------
//  
TInt MFsRichTextObject::GetTypeOfHotSpot()const
    {
    FUNC_LOG;
    return iTypeOfHotSpot;
    }

// ---------------------------------------------------------------------------
// SetIsWhiteSpace
// ---------------------------------------------------------------------------
//  
void MFsRichTextObject::SetIsWhiteSpace(TBool aIsWhiteSpace)
    {
    FUNC_LOG;
    iIsWhiteSpace = aIsWhiteSpace;
    } 

// ---------------------------------------------------------------------------
// GetWhiteSpace
// ---------------------------------------------------------------------------
//      
TBool MFsRichTextObject::GetIsWhiteSpace() const
    {
    FUNC_LOG;
    return iIsWhiteSpace;
    }  

