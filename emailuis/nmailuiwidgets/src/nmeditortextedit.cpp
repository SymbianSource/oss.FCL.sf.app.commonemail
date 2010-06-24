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
* Description: Message editor body text field
*
*/

#include "nmailuiwidgetsheaders.h"

static const QString FILE_PATH_CSS = ":nmeditortextedit.css";
static const QString FILE_PATH_WIDGETML = ":nmeditortextedit.widgetml";

/*!
    Constructor
*/
NmEditorTextEdit::NmEditorTextEdit(QGraphicsItem *parent) :
    HbTextEdit(parent)
{
    NM_FUNCTION;
    
    HbStyleLoader::registerFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::registerFilePath(FILE_PATH_CSS);

    mCustomTextColor = QPair<bool, QColor>(false, Qt::black);
    
    // Enable scrolling using cursor
    setScrollable(true);
    scrollArea()->setScrollDirections(Qt::Horizontal);

    // set background colour to plain white
    QPixmap whitePixmap(10,10);
    whitePixmap.fill(Qt::white);
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem (whitePixmap);
    setBackgroundItem(pixmapItem); 
    
    // disables highlight frame for now - new api to set the frame item should be release somewhere wk26
    setFocusHighlight(HbStyle::P_TextEdit_frame_highlight, HbWidget::FocusHighlightNone);
    
    connect(this, SIGNAL(contentsChanged()), this, SLOT(updateCustomTextColor()));
}

/*!
    Destructor
*/
NmEditorTextEdit::~NmEditorTextEdit()
{
    NM_FUNCTION;
    
    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_CSS);    
}

/*!
	This slot applies custom text color for user - entered text
    It does not affect the text originally inserted into editor
 */
void NmEditorTextEdit::updateCustomTextColor()
{
    NM_FUNCTION;
    
    if (mCustomTextColor.first) {
        QTextCursor tcursor = textCursor();
        QTextCharFormat fmt;
        int pos = tcursor.position();
        if (pos > 0) {
            // If there isn't any user-made selection - apply custom color
            if (!tcursor.hasSelection() && !tcursor.hasComplexSelection()) {
                // Select last added char and set its color to custom
                if (tcursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1)
                    && tcursor.hasSelection()) {
                    fmt = tcursor.charFormat();
                    fmt.setForeground(mCustomTextColor.second);
                    tcursor.mergeCharFormat(fmt);
                }
            }
        }
        else {
            fmt = tcursor.charFormat();
            fmt.setForeground(mCustomTextColor.second);
            tcursor.mergeCharFormat(fmt);
            setTextCursor(tcursor);
        }
    }
}

/*!
   Sets flag is custom text color should be used and sets the custom color.
   
   Function does not affect the color of existing content, only text that will be entered later.
 */
void NmEditorTextEdit::setCustomTextColor(const QPair<bool, QColor> &customColor)
{
    NM_FUNCTION;
    
    mCustomTextColor = customColor;
}

/*!
    Reimplemented function \sa NmEditorTextEdit::setCustomTextColor(const QPair<bool, QColor> &customColor).
    
    \arg info about using of custom color
    \arg color to be used as custom. If \arg useCustom is set to false then color is not changed
 */
void NmEditorTextEdit::setCustomTextColor(bool useCustom, const QColor& color)
{   
    NM_FUNCTION;
    
    mCustomTextColor.first = useCustom;
    //check and set custom color
    mCustomTextColor.first ? mCustomTextColor.second = color : 
                             mCustomTextColor.second = mCustomTextColor.second;
    
}

/*!
 *  Return current custom color and if it must be used.
 *  
 *  \return Current custem color.
 */
QPair<bool, QColor> NmEditorTextEdit::customTextColor() const
{
    NM_FUNCTION;
    
    return mCustomTextColor;
}

/*!
 *  Returns the calculated rect in item coordinates of the editor for the the given \a position inside a document.
 */
QRectF NmEditorTextEdit::rectForPosition(int position)
{
    return HbTextEdit::rectForPosition(position);
}

