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
* Description: This file defines classes CNewLineField, CPictureField, CSmileyField
*
*/
#ifndef C_FSRICHTEXTFIELDS_H
#define C_FSRICHTEXTFIELDS_H

#include <fldbase.h>

const TUid KFsRichTextPictureFieldUid = {268435540};
const TUid KFsRichTextSmileyFieldUid =  {268435539};
const TUid KFsRichTextNewLineFieldUid = {268435538};


class CNewLineField : public CTextField
    {
public:
    /**
    * CNewLineField()
    *
    * 
    */
    CNewLineField();
    
    /**
    * Value(TPtr& aValueText);
    *
    * @param aValueText
    */
    TInt Value(TPtr& aValueText);
    
    /**
    * InternalizeL(RReadStream& aStream);
    *
    * @param aStream
    */
    void InternalizeL(RReadStream& aStream);
    
    /**
    * Type()const;
    *
    * 
    */
    TUid Type()const;
    };


class CPictureField : public CTextField
    {
public:
    /**
    * CPictureField(TSize aSize);
    *
    * @param aSize
    */
    CPictureField(TSize aSize);
    
    /**
    * Value(TPtr& aValueText);
    *
    * @param aValueText
    */
    TInt Value(TPtr& aValueText);
    
    /**
    * InternalizeL(RReadStream& aStream);
    *
    * @param aStream
    */
    void InternalizeL(RReadStream& aStream);
    
    /**
    * Type()const;
    *
    * 
    */
    TUid Type()const;
    
    /**
    * GetSize(TSize& aSize);   
    *
    * @param aSize
    */
    void GetSize(TSize& aSize);    
    
protected:
    TSize iSize;
    };
    
    
class CSmileyField : public CTextField
    {
public:
    /**
    * CSmileyField(TInt aSmileyIndex);
    *
    * @param aSmileyIndex
    */
    CSmileyField(TInt aSmileyIndex);
    
    /**
    * Value(TPtr& aValueText);
    *
    * @param aValueText
    */
    TInt Value(TPtr& aValueText);
    
    /**
    * InternalizeL(RReadStream& aStream);
    *
    * @param aStream
    */
    void InternalizeL(RReadStream& aStream);
    
    /**
    * Type()const;
    *
    * 
    */
    TUid Type()const;
    
    /**
    * GetSmileyIndex();    
    *
    * 
    */
    TInt GetSmileyIndex();    
    
protected:
    TInt iSmileyIndex;
    };    

#endif //FSRICHTEXTFIELDS_H