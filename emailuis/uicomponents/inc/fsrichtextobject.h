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
* Description:  Interface class for storing objects embedded in CFsRichText
*
*/


#ifndef M_FSRICHTEXTOBJECT_H
#define M_FSRICHTEXTOBJECT_H

#include <e32base.h>
class MFsRichTextObject
    {
    public:
        enum TFsRichTextObjectType
            {
                EFsText = 1,
                EFsTexture,
                EFsHotspot,
                EFsSmiley,
                EFsNewLine
            };
        
    public:
        /**
        * GetType() const;
        * 
        * 
        */
        TFsRichTextObjectType GetType() const;
        
        /**
        * GetBeginOfObject() const;
        * 
        * 
        */
        TInt GetBeginOfObject() const;
        
        /**
        * GetEndOfObject() const;
        * 
        * 
        */
        TInt GetEndOfObject() const;
        
        /**
        * SetBeginOfObject(TInt aBeginOfObject);
        * 
        * @param aBeginOfObject
        */
        void SetBeginOfObject(TInt aBeginOfObject);
        
        /**
        * SetEndOfObject(TInt aEndOfObject);
        * 
        * @param aEndOfObject
        */
        void SetEndOfObject(TInt aEndOfObject);
        
        /**
        * SetEmbeded(TBool aEmbeded);
        * 
        * @param aEmbeded
        */
        void SetEmbeded(TBool aEmbeded);
        
        /**
        * SetIdOfEmbeded(TInt aIdOfEmbeded);
        * 
        * @param aIdOfEmbeded
        */
        void SetIdOfEmbeded(TInt aIdOfEmbeded);
        
        /**
        * GetEmbeded() const;
        * 
        * 
        */
        TBool GetEmbeded() const;
        
        /**
        * GetIdOfEmbeded() const;
        * 
        * 
        */
        TInt GetIdOfEmbeded() const;
        
        /**
        * SetHotSpot(TBool aHotSpot);
        * 
        * @param aHotSpot
        */
        void SetHotSpot(TBool aHotSpot);
        
        /**
        * SetIdOfHotSpot(TInt aIdOfHotSpot);
        * 
        * @param aIdOfHotSpot
        */
        void SetIdOfHotSpot(TInt aIdOfHotSpot);
        
        /**
        * SetTypeOfHotSpot(TInt aTypeOfHotSpot);
        * 
        * @param aTypeOfHotSpot
        */
        void SetTypeOfHotSpot(TInt aTypeOfHotSpot);

        /**
        * SetWhiteSpace(TBool aWhiteSpace);
        * 
        * @param aWhiteSpace
        */
        void SetIsWhiteSpace(TBool aIsWhiteSpace);        

        /**
        * GetHotSpot() const;
        * 
        * 
        */
        TBool GetHotSpot() const;
        
        /**
        * GetIdOfHotSpot()const;
        * 
        * 
        */
        TInt GetIdOfHotSpot()const;
        
        /**
        * GetTypeOfHotSpot()const;
        * 
        * 
        */
        TInt GetTypeOfHotSpot()const;

	    /**
        *  GetIsWhiteSpace()const;
        * 
        * 
        */
	    TBool GetIsWhiteSpace() const;       

        /**
        * ~MFsRichTextObject()
        * 
        * 
        */
        virtual ~MFsRichTextObject() {};

    protected:
        TFsRichTextObjectType iType;
        TInt iBeginOfObject;
        TInt iEndOfObject;
        TBool iEmbeded;
        TInt iIdOfEmbeded;
        
        TBool iHotSpot;
        TInt iIdOfHotSpot;
        TInt iTypeOfHotSpot;
	    TBool iIsWhiteSpace;
    };

#endif // M_FSRICHTEXTOBJECT_H

