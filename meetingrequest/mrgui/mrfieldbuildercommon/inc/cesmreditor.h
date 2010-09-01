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
* Description:  ESMR editor impl.
*
*/


#ifndef CESMREDITOR_H
#define CESMREDITOR_H

#include <eikrted.h>
#include <eikedwob.h>

// Forward declarations
class MESMRListObserver;
class MMRFieldScrollObserver;

NONSHARABLE_CLASS( CESMREditor ): public CEikRichTextEditor,
                                  public MEikEdwinObserver
    {
public:
    /**
     * Two phase constructor.
     */
    IMPORT_C static CESMREditor* NewL();

    /**
     * Two phase constructor.
     *
     * @param aParent parent control
     * @param aNumberOfLines number of visible editor lines
     * @param aTextLimit limit for text length
     * @param aEdwinFlags flags for underlying CEikEdwin
     */
    IMPORT_C static CESMREditor* NewL(
            const CCoeControl* aParent,
            TInt aNumberOfLines,
            TInt aTextLimit,
            TInt aEdwinFlags );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CESMREditor();

public: // From CCoeControl
    IMPORT_C void PositionChanged();
    IMPORT_C TKeyResponse OfferKeyEventL(
    		const TKeyEvent& aKeyEvent,
    		TEventCode aType);
    IMPORT_C void FocusChanged(TDrawNow aDrawNow);
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

public: // From MEikEdwinObserver
    IMPORT_C void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);

public:
    /**
     * Clears selection and sets new text.
     *
     * @param aText new text
     */
    IMPORT_C void ClearSelectionAndSetTextL( const TDesC& aText );

    /**
     * Set font for editor.
     *
     * @param aFont new font
     * @param aLayout
     */
    IMPORT_C void SetFontL( const CFont* aFont );

    /**
     * Returns cursor line number.
     *
     * @return cursor line number
     */
    IMPORT_C TInt CursorLinePos() const;

    /**
     * Returns text line count.
     *
     * @return line count
     */
    IMPORT_C TInt LineCount() const;

    
    /**
	 * Set observer.
	 *
	 * @param aObserver pointer to observer
	 */
    IMPORT_C void SetListObserver( MESMRListObserver* aObserver );

    /**
     * Returns height of one row.
     *
     * @return row's height
     */
    IMPORT_C TInt RowHeight();
        
    /**
     * Returns line number of currenly 
     * focused row starting from one.
     * 
     * @return row's height
     */
    IMPORT_C TInt CurrentLineNumber();

    /**
    * Sets the default text for editor field
    * Ownership is transferred
    * @param aDefaultText default text.
    */
    IMPORT_C void SetDefaultTextL( HBufC* aDefaultText);

    /**
    * Returns the default text, if not set, KNullDesC is returned
    * @return TDesC default text
    */
    IMPORT_C const TDesC& DefaultText();

    /**
     * Returns the limited length
     * @return max length of field
     */
    IMPORT_C TInt GetLimitLength() const;
    
private:
    /**
     * Constructor.
     */
    CESMREditor();

    /**
     * Second phase constructor.
     *
     * @param aParent parent control
     * @param aNumberOfLines number of visible lines in editor
     * @param aTextLimit limit for editor text length
     * @param aEdwinFlags flags passed to underlying CEikEdwin
     */
    void ConstructL(
            const CCoeControl* aParent,
            TInt aNumberOfLines,
            TInt aTextLimit,
            TInt aEdwinFlags );

    void TryToSetSelectionL();

private:    
	/// Ref:
    MESMRListObserver* iObserver;
    /// Own:
    HBufC* iDefaultText;
    /// Own;
    TInt iLimitLength;
    };

#endif  // CESMREDITOR_H

