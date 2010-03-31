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
*  Description : CEikRichTextEditor based Rich Text viewer
*  Version     : %version: e002sa32#19 %
*
*/

#ifndef CESMRRICHTEXTVIEWER_H
#define CESMRRICHTEXTVIEWER_H

#include <eikrted.h>
#include <cntitem.h>
#include <eikcmobs.h>

#include "resmrstatic.h"
#include "mmrcontactmenuobserver.h"

class CESMRRichTextLink;
class MESMRFieldEventQueue;

// SCROLLING_MOD: List observer forward declaraion
class MESMRListObserver;
class MMRFieldScrollObserver;

/*
 * Pure virtual link selection observer.
 */
class MESMRRichTextObserver
    {
public:
    /*
     * CESMRichTextViewer calls this function with selected link before
     * showing Contact Action Menu. If observer handles link selection it
     * should return ETrue, in which case CESMRRichTextViewer won't handle
     * it.
     */
    virtual TBool
            HandleRichTextLinkSelection(const CESMRRichTextLink* aLink ) = 0;
    };

/*
 * CEikRichTextEditor based Rich Text viewer.
 */
NONSHARABLE_CLASS( CESMRRichTextViewer ) : public CEikRichTextEditor,
                                           public MEikCommandObserver,
                                           public MMRContactMenuObserver
    {
public:
    /*
     * Two-phase constructor.
     * @param aParent if aParent is NULL,
     * 				  CEikRichTextEditor is constucted as
     * 				  window owning control.
     * @return Created object
     */
    IMPORT_C static CESMRRichTextViewer* NewL(
            const CCoeControl* aParent = NULL);

    /*
     * Destructor.
     */
    IMPORT_C ~CESMRRichTextViewer( );

    // From CCoeControl
    IMPORT_C void PositionChanged( );
    IMPORT_C void FocusChanged( TDrawNow aDrawNow );
    IMPORT_C TKeyResponse OfferKeyEventL(
    		const TKeyEvent &aKeyEvent,
    		TEventCode aType );
    IMPORT_C void SetMargins( TInt sMargin );

    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /*
     * Sets the given font for the rich text component. Also
     * sets the default main area color. Changes take effect
     * after AppliyLayoutChanges is called.
     *
     * @param aFont, the font to be set for the rich text component.
     */
    IMPORT_C void SetFontL( const CFont* aFont );

    /*
     * Sets CEikRichTextEditor's text and searches text for emails,
     * phone numbers and urls if aSearchLinks is set to ETrue.
     *
     * @param aText text to be set for CEikRichTextEditor
     * @param aSearchLinks search emails, phone numbers and urls
     */
    IMPORT_C void SetTextL(const TDesC* aText, TBool aSearchLinks = EFalse );

    /*
     * Sets observer for link selections.
     * @param aLinkObserver pointer to observer object
     */
    IMPORT_C void SetLinkObserver(MESMRRichTextObserver* aLinkObserver);

    /*
     * Adds link to text.
     * @param aLink link to be added
     */
    IMPORT_C void AddLinkL(CESMRRichTextLink* aLink );

    /**
     * Inserts link to text.
     * @param aLink link to be added
     * @param aPosition position where link is added
     */
    IMPORT_C void InsertLinkL( CESMRRichTextLink* aLink, TInt aPosition );

    /*
     * Returns selected link, or NULL if none selected.
     */
    IMPORT_C const CESMRRichTextLink* GetSelectedLink( ) const;

    /*
     * Returns text for selected link from underlying CEikRichTextViewer.
     *
     * @param aLink link
     */
    IMPORT_C HBufC* GetLinkTextLC( const CESMRRichTextLink& aLink ) const;

    /*
     * Returns pointer to new CContactItem constructed from aLink.
     * Function uses CESMRRichTextLink::Value to create contact
     * with CESMRRichTextLink::Type information.
     *
     * @param aLink link
     */
    IMPORT_C CContactItem* CreateContactItemL( const CESMRRichTextLink& aLink );

    /*
    * Method for cesmrviewerdescriptionfield to set the list observer
    *
    * @param aObserver observer that should be notified
    */
    IMPORT_C void SetListObserver( MESMRListObserver* aObserver );

    /**
     * Returns height of one row.
     *
     * @return row's height
     */
    IMPORT_C TInt RowHeight();
    /**
     * Returns text line count.
     *
     * @return line count
     */
    IMPORT_C TInt LineCount();
    /**
     * Returns line number of currenly
     * focused row starting from one.
     *
     * @return row's height
     */
    IMPORT_C TInt CurrentLineNumber();

    /**
     * Enables and disables action menu
     * @param aStatus Enables or disables action menu
     */
    IMPORT_C void SetActionMenuStatus( TBool aStatus );

    /**
     * Copies currently selected richtext link associated with
     * action menu to the clipboard
     */
    IMPORT_C void CopyCurrentLinkToClipBoardL() const;

    /**
     * convenience method to reset TLS static contactactionmenuhandler
     * for classes that only have access to richtextviewer.
     * Needed for richtextviewer fields when they lose focus.
     */
    IMPORT_C void ResetActionMenuL() const;

    /**
     * Sets event queue for field. Observer can be used
     * for triggering commands and notifying events.
     * @param aEventObserver the event observer
     */
    IMPORT_C void SetEventQueue( MESMRFieldEventQueue* aEventQueue );

    /**
     * Selects current link.
     * @return ETrue if link selected
     */
    IMPORT_C TBool LinkSelectedL();

    /**
     * Handles long tap event in rich text viewer.
     * @param aPosition position of long tap event.
     */
    IMPORT_C void HandleLongtapEventL( TPoint& aPosition );

    /*
     * Sets the given line spacing for the rich text component.
     * Changes take effect after AppliyLayoutChanges is called.
     *
     * @param aLineSpacingInTwips, the line spacing to be used.
     */
    IMPORT_C void SetLineSpacingL( TInt aLineSpacingInTwips );

    /*
     * Applies the layout changes to the rich text component.
     */
    IMPORT_C void ApplyLayoutChangesL();
    
    /*
     * Set selected link according to the index in the array.
     * @param aLinkIndex the index of link need to be selected.
     */
    IMPORT_C void SetFocusLink( TInt aLinkIndex );

    /*
     * Get the index of selected link in the array.
     * @return The index of selected link in the array.
     */
    IMPORT_C TInt GetFocusLink( ) const;

protected: // From MEikCommandObserver

    /**
     * Process commands from contact menu handler.
     * Forwards commands to event observer.
     */
    void ProcessCommandL( TInt aCommandId );

protected: // From CEikEdwin
    /**
     * Sets the control as ready to be drawn.
     */
    void ActivateL();

protected: // From MMRContactMenuObserver

    void ContactActionQueryComplete();

private:
    /*
     * Private constuctor.
     */
    CESMRRichTextViewer( );

    /*
     * Two-phase constructor.
     * @param aParent if aParent is NULL, CEikRichTextEditor
     * 				  is constucted as window owning control.
     */
    void ConstructL(const CCoeControl* aParent );

    /*
     * Highlights link in CEikRichTextEditor.
     *
     * @param aLink link to be highlighted
     */
    void HighlightLinkL(const CESMRRichTextLink& aLink );

    /*
     * Private internal function which searches for emails,
     * phone numbers and urls from given text using CFindItemEngine.
     *
     * @param aText text which is shown in CEikRichTextEditor
     */
    void SearchLinksL(const TDesC& aText );

    TInt FindTextLinkBetweenNextScrollArea(
    		TInt aStartRow,
    		TInt aEndRow,
    		TCursorPosition::TMovementType aDirection);

    TInt ValidLinkForFocusing(	TInt aIndex,
                                TCursorPosition::TMovementType aDirection,
                                TInt aStartRow,
                                TInt aEndRow);

    void SetValueL( const CESMRRichTextLink& aLink );

    void ShowContextMenuL();

    void GetLinkAreaL( TRegion& aRegion,
                       const CESMRRichTextLink& aLink ) const;

    void ChangeMiddleSoftkeyL( const CESMRRichTextLink& aLink );

    void UpdateViewL( const TKeyEvent &aKeyEvent );

private: // Data

	/// Own: Array of hyperlinks in viewer
    RPointerArray<CESMRRichTextLink> iLinkList;
    /// Ref:
    MESMRRichTextObserver* iLinkObserver;
    /// Own:
    RESMRStatic iESMRStatic;
    /// Ref:
    MESMRListObserver* iObserver;
    /// Own:
    TInt iApproximatelyRowHeight;
    /// Ref: pointer to font
    const CFont* iFont;
    /// Ref: Contact Menu handler
    CESMRContactMenuHandler* iCntMenuHdlr;
    /// Ref: Pointer to event queue
    MESMRFieldEventQueue* iEventQueue;
    /// Own: Rich text link format
    TCharFormat iRichTextLinkFormat;
    /// Own: Rich text link format mask
    TCharFormatMask iRichTextLinkFormatMask;
    /// Own:
    TBool iOpenActionMenu;
    // Own: Paragraph formatter
    CParaFormat* iParaFormat;
    // Own: Paragraph formatter mask
    TParaFormatMask iParaFormatMask;
    // Own: Character formatter
    TCharFormat iCharFormat;
    // Own: Character formatter mask
    TCharFormatMask iCharFormatMask;
    };

#endif /*CESMRRICHTEXTVIEWER_H*/
