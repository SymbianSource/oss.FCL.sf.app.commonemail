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
* Description: This file defines class CESMRNcsEditor.
*
*/

#ifndef CESMRNCSEDITOR_H
#define CESMRNCSEDITOR_H

#include <eikrted.h>
#include <AknUtils.h>

/**
 *  CESMRNcsEditor is a specialised CEikRichTextEditor for 
 *  the email address lists for attendee fields.
 */
NONSHARABLE_CLASS( CESMRNcsEditor ) : public CEikRichTextEditor
    {
public:
    /** 
     * Constructor of this class
     * @param default text of the editor
     * @param aHeaderField set if editor contains a headerfield
     * @returns CESMRClsItem instance
     */
    CESMRNcsEditor( HBufC* aDefaultText );

    /**
     * C++ Virtual Destructor.
     */
    virtual ~CESMRNcsEditor();

public: // new functions
    /**
     * Get the size and position rectangle of the editor
     * @param aLineRect rectangle of the editor
     */
    void GetLineRectL( TRect& aLineRect ) const;

    /**
     * Get the number of scrollable lines of the rich text editor
     * @return number of scrollable lines
     */
    virtual TInt ScrollableLines() const;

    /**
     * Get the current line number of the selection
     * @return current selected line number 
     */
    virtual TInt CursorLineNumber() const;

    /**
     * Get the number of lines in the editor
     * @return number of lines in the editor
     */
    virtual TInt LineCount() const;

    /**
     * Get the current position in the editor line
     * @return position in the current line of the editor
     */
    virtual TInt CursorPositionL() const;

    /**
     * Get the descriptor content of the editor label
     * @return descriptor of the label
     */
    virtual const TDesC& GetLabelText() const;

    /**
     * Set the maximum length for the label
     * @param aSize maximum length for the label
     */
    void SetMaximumLabelLength( TSize aSize );

    /**
     * Initialise/reset the rich text editor
     */
    void SetupEditorL();

    /**
     * Update editor content
     */
    void UpdateEditorL();

    /**
     * Update editor position
     */
    void PositionChanged();

    /**
     * Get the height of the text in the editor
     * @return height of the text in the editor
     */
    TInt TextHeight() const;

    /**
     * Get the height of the single line of text in the editor
     * @return height of the single line of text in the editor
     */
    TInt GetLineHeightL() const;

    /**
     * Get the number of characters in the editor
     * @return number of characters in the editor
     */
    TInt GetNumChars() const;

    /**
      * Get the length of the editor document
      * @return length of the editor document
      */
    TInt DocumentLength() const;

    /**
      * Get the number of pixels above the current editor line
      * @return number of pixels above the current editor line
      */
    TInt PixelsAboveBand() const;

    /**
      * Change the number of pixels above the current editor line
      * @param aPixels set the  number of pixels above the current editor line
      */
    TInt ChangeBandTopL(TInt aPixels);

    /**
      * Set the color of the text in the editor
      * @param aColor color of the text in the editor
      */
    void SetTextColorL( TLogicalRgb aColor );

    /**
      * check if the editor has default text available
      * @param ETrue if editor has default text available
      */
    TBool HasDefaultText();

    /**
      * Get length of text after trim
      * return length of text after trim
      */
    TInt TrimmedTextLengthL();

    /**
      * Set the font of the editor
      * @param aFont font of the editor
      */
    void SetFontL( const CFont* aFont );
    
#ifdef _DEBUG
    void DebugDump();
#endif

public: // from CoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void SetTextL( const TDesC* aDes );
    void FocusChanged( TDrawNow aDrawNow );
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    

protected: // from CEikEdwin
    virtual void HandleResourceChange( TInt aType );

protected: // from MEditObserver
    virtual void EditObserver( TInt aStart, TInt aExtent );

private: // Implementation
    void UpdateColors();
    void UpdateGraphics();
    void DoUpdateGraphicsL();

private: // data
    TInt iPreviousFontHeight;
    TInt32 iPreviousLineSpacingInTwips;
    HBufC* iDefaultText;//not own
    TBool iHasDefaultText;
    TRgb iTextColor;
    };


#endif
