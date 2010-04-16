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
}

/*!
    Destructor
 */
NmHtmlLineEdit::~NmHtmlLineEdit()
{
}

/*!
    Replaces the document used in the editor.

    This allows
 */
void NmHtmlLineEdit::setDocument(QTextDocument *document)
{
    HbAbstractEdit::setDocument(document);
}

QTextDocument *NmHtmlLineEdit::document() const
{
    return HbAbstractEdit::document();
}

void NmHtmlLineEdit::setTextCursor(const QTextCursor &cursor)
{
    HbAbstractEdit::setTextCursor(cursor);
}

QTextCursor NmHtmlLineEdit::textCursor() const
{
    return HbAbstractEdit::textCursor();
}

QString NmHtmlLineEdit::toHtml() const
{
    return HbAbstractEdit::toHtml();
}

void NmHtmlLineEdit::setHtml(const QString &text)
{
    HbAbstractEdit::setHtml(text);
}

QString NmHtmlLineEdit::toPlainText () const
{
    return HbAbstractEdit::toPlainText();
}

void NmHtmlLineEdit::setPlainText (const QString &text)
{
    HbAbstractEdit::setPlainText(text);
}
