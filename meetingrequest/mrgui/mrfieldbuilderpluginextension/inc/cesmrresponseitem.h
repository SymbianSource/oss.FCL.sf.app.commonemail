/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of response area item
 *
*/

#ifndef CESMRRESPONSEITEM_H
#define CESMRRESPONSEITEM_H

// INCLUDES
#include <coecntrl.h>
#include "esmrcommands.h"

// FORWARD DECLARATIONS
class CESMRLayoutManager;
class CMRLabel;
class CMRImage;

// CLASS DECLARATIONS
/**
 * Response area contains four response items. One item contains icon and text,
 * (e.g. unchecked icon and "Accept" as a text) except the first topic line that
 * doesn't have the icon. Topic item might have text in two lines depending on
 * the text length and screen orientation. Item is highlighted by drawing svg graphic
 * to the background. Only highlighted item can be selected by pressing "OK" key.
 */
NONSHARABLE_CLASS( CESMRResponseItem ) : public CCoeControl
    {
    public:
        static CESMRResponseItem* NewL( TESMRCommand aCmd,
                                        const TDesC& aItemText,
                                        TBool aHasIcon );
        static CESMRResponseItem* NewLC( TESMRCommand aCmd,
                                         const TDesC& aItemText,
                                         TBool aHasIcon );
        ~CESMRResponseItem();
    
    public: // From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aInd ) const;
        void SizeChanged();
    
    public: // New methods
        /**
         * Sets the highlight item on.
         */
        void SetHighlight();
    
        /**
         * Sets the highlight item off.
         */
        void RemoveHighlight();
        /**
         * Changes the icon from checked to unchecked or vice versa
         * depending on the parameter.
         *
         * @param aChecked - ETrue draws the checked icon
         *                   EFalse draws the unchecked icon
         */
    
        void ChangeIconL( TBool aChecked );
        /**
         * Sets font for the text in this item
         *
         * @param Font to be set
         */
        void SetFont( const CFont* aFont );
    
        /**
         * Sets text for this item. When called for the first time
         * text is captured and that text is always wrappen again when
         * calling this method. This is done because orientation of
         * the screen might change.
         *
         * @param Text for the item
         */
        void SetTextL( const TDesC& aItemText );
    
        /**
         * Gives pointer to layoutmanager for this class.
         *
         * @param aLayout- Layout manager
         */
        void SetLayoutManager( CESMRLayoutManager* aLayout );
    
        /**
         * Counts the number of lines that item needs for the text.
         * This is called to give enough space for the item.
         *
         * @return Number of text lines.
         */
        TInt ItemTextLineCount();
    
        /**
         * Returns the command id attached to this item.
         *
         * @return A command id
         */
        TESMRCommand CommandId() const;
    
    private: // Implementation
        CMRImage* IconL(TBool aChecked );
        void Draw( const TRect& aRect ) const;
    
    private: // private construction
        CESMRResponseItem(TESMRCommand aCmd);
        void ConstructL( const TDesC& aItemText, TBool aHasIcon );
        
    private: // data
        /// Own: Label for item's text
        CMRLabel* iSelectionLabel;
        /// Own: Checkbox icon
        CMRImage* iIcon;
        /// Own: Items text
        HBufC* iItemText;
        /// Item's command id
        TESMRCommand iCmd;
        /// Layout data
        CESMRLayoutManager* iLayout;
        /// Flag for the item highlight status
        TBool iHighlighted;
        /// Flag for telling whether the item needs the icon
        TBool iHasIcon;
        /// Original text length
        TInt iOriginalTextLength;
    };

#endif // CESMRRESPONSEITEM_H

// End of file
