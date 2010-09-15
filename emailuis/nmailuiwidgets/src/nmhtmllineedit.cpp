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
* Description: Message editor header widget
*
*/

#include "nmailuiwidgetsheaders.h"
/*
#include "nmhtmllineedit.h"
*/
/*!
    Constructor
 */
NmHtmlLineEdit::NmHtmlLineEdit(QGraphicsItem *parent) :
    HbLineEdit(parent)
{
    NM_FUNCTION;

    // Disable scrolling so that baunch effect works correctly
    HbAbstractEdit::setScrollable(false);
    HbAbstractEdit::scrollArea()->setScrollDirections(0);
}

/*!
    Destructor
 */
NmHtmlLineEdit::~NmHtmlLineEdit()
{
    NM_FUNCTION;
}

/*!
    Replaces the document used in the editor.

    This allows
 */
void NmHtmlLineEdit::setDocument(QTextDocument *document)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setDocument(document);
}

QTextDocument *NmHtmlLineEdit::document() const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::document();
}

void NmHtmlLineEdit::setTextCursor(const QTextCursor &cursor)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setTextCursor(cursor);
}

QTextCursor NmHtmlLineEdit::textCursor() const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::textCursor();
}

QString NmHtmlLineEdit::toHtml() const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::toHtml();
}

void NmHtmlLineEdit::setHtml(const QString &text)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setHtml(text);
}

QString NmHtmlLineEdit::toPlainText () const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::toPlainText();
}

void NmHtmlLineEdit::setPlainText (const QString &text)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setPlainText(text);
}

/*!
 *  Returns true if this widget has the input focus.
 */
bool NmHtmlLineEdit::hasInputFocus() const
{
    NM_FUNCTION;

    bool ret = false;
    
    HbInputMethod* inputMethod = HbInputMethod::activeInputMethod();
    
    if (inputMethod) {
        HbInputFocusObject *focusObject = inputMethod->focusObject();
        if (focusObject) {
            ret = (focusObject->object() == this);
        }
    }
    
    return ret; 
}


/*!
 *  Returns the rectangle for the cursor.
 */
QRectF NmHtmlLineEdit::rectForCursorPosition() const
{
    NM_FUNCTION;
    
    return HbLineEdit::rectForPosition(cursorPosition());
}
