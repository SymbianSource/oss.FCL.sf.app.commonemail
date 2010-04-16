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

// Following constants will be removed later when possible
static const double Un = 6.66;
static const double BodyMargin = Un;
static const int ChromeHeight = 160;
static const double FieldHeightWhenSecondaryFont = 5 * Un;
static const int GroupBoxTitleHeight = 42;
static const double HeightOfTheHeaderOnStartup =
    2 * FieldHeightWhenSecondaryFont + GroupBoxTitleHeight;

/*!
    Constructor
*/
NmEditorTextEdit::NmEditorTextEdit(QGraphicsItem *parent) :
    HbTextEdit(parent)
{
}

/*!
    Destructor
*/
NmEditorTextEdit::~NmEditorTextEdit()
{
}

void NmEditorTextEdit::init(NmEditorContent *parent, NmBaseViewScrollArea *bgScrollArea)
{
    mPreviousContentsHeight = 0;
    mFirstTime = true;
    mCustomTextColor = QPair<bool, QColor>(false,Qt::black);
    mParent = parent;
    mBackgroundScrollArea = bgScrollArea;
    mHeaderHeight = (int)HeightOfTheHeaderOnStartup;
    mBgScrollPosition.setX(0);
    mBgScrollPosition.setY(0);
    document()->setDocumentMargin(BodyMargin);

    mScrollArea = this->scrollArea();
    mScrollArea->setLongPressEnabled(true);
    // Enable scrolling using cursor
    setScrollable(true);
    mScrollArea->setScrollDirections(Qt::Horizontal);
    // Let all mouse events go into background scroll area which handles the scrolling
    setAcceptedMouseButtons(Qt::NoButton);
    mScrollArea->setAcceptedMouseButtons(Qt::NoButton);

    connect(this, SIGNAL(contentsChanged()), this, SLOT(updateEditorHeight()));
    connect(this, SIGNAL(cursorPositionChanged(int, int)),
            this, SLOT(setScrollPosition(int, int)));
    connect(this, SIGNAL(setEditorContentHeight()), mParent, SLOT(setEditorContentHeight()));
    connect(this, SIGNAL(contentsChanged()), this, SLOT(updateCustomTextColor()));
}

/*!
    This function returns the height (pixels) of the body fields document content.
 */
qreal NmEditorTextEdit::contentHeight() const
{
    QSizeF s = document()->size();
    return s.height();
}

/*!
   This slot updates the editor height. It is called every time when text edit
   widget content has been changed.
 */
void NmEditorTextEdit::updateEditorHeight()
{
    // Get current body content height
    qreal heightOfTheTextEdit = contentHeight();

    // Check if height is changed
    if (mPreviousContentsHeight != heightOfTheTextEdit) {
        mPreviousContentsHeight = heightOfTheTextEdit;
        setPreferredHeight(heightOfTheTextEdit);
        setMaximumHeight(heightOfTheTextEdit);
    }
    // Inform parent that content height has been changed
    emit setEditorContentHeight();
}

/*!
    This slot is called when cursor position is changed in body area using
    'pointing stick' or keyboard. Function will update the scroll position
    of the content so that cursor does not go outside of the screen or
    behind the virtual keyboard.
 */
void NmEditorTextEdit::setScrollPosition(int oldPos, int newPos)
{
    Q_UNUSED(oldPos);

    if (mFirstTime) {
        // For some reason content height of the HbTextEdit is wrong
        // right after construction. That is the reason why this mFirstTime
        // member is used to set content height bit later.
        mFirstTime = false;
        updateEditorHeight();
    }
    const QSizeF screenReso = HbDeviceProfile::current().logicalSize();
    qreal maxHeight = screenReso.height() - ChromeHeight;

    // Get cursor position coordinates
    QRectF cursorPosPix = rectForPosition(newPos);

    // Calculate the screen top and bottom boundaries, this means the part of the
    // background scroll area which is currently visible.
    qreal visibleRectTopBoundary;
    qreal visibleRectBottomBoundary;

    if (mBgScrollPosition.y() < mHeaderHeight) {
        // Header is completely or partially visible
        visibleRectTopBoundary = mHeaderHeight - mBgScrollPosition.y();
        visibleRectBottomBoundary = maxHeight - visibleRectTopBoundary;
    }
    else {
        // Header is not visible
        visibleRectTopBoundary = mBgScrollPosition.y() - mHeaderHeight;
        visibleRectBottomBoundary = visibleRectTopBoundary + maxHeight;
    }

    // Do scrolling if cursor is out of the screen boundaries
    if (cursorPosPix.y() > visibleRectBottomBoundary) {
        // Do scroll forward
        mBackgroundScrollArea->scrollContentsTo(
            QPointF(0,cursorPosPix.y() - maxHeight + mHeaderHeight));
    }
    else if (cursorPosPix.y() + mHeaderHeight < mBgScrollPosition.y()) {
        // Do scroll backward
        mBackgroundScrollArea->scrollContentsTo(QPointF(0,cursorPosPix.y() + mHeaderHeight));
    }
}

/*!
    This slot is called when background scroll areas scroll position has been shanged.
*/
void NmEditorTextEdit::updateScrollPosition(const QPointF &newPosition)
{
    mBgScrollPosition = newPosition;
}

/*!
	This slot applies custom text color for user - entered text
    It does not affect the text originally inserted into editor
 */
void NmEditorTextEdit::updateCustomTextColor()
{
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
        } else {
            fmt = tcursor.charFormat();
            fmt.setForeground(mCustomTextColor.second);
            tcursor.mergeCharFormat(fmt);
            setTextCursor(tcursor);
        }
    }
}

/*!
    This slot is called when header widget height has been changed. Function performs
    the repositioning of the body field and resizing of the editor and content area.
 */
void NmEditorTextEdit::setHeaderHeight(int newHeight)
{
    mHeaderHeight = newHeight;
    updateEditorHeight();
}

/*!
    sendMousePressEvent. Function is used to relay mouse press event to base class
*/
void NmEditorTextEdit::sendMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event) {
        HbAbstractEdit::mousePressEvent(event);
    }
}

/*!
    sendMouseReleaseEvent. Function is used to relay mouse release event to base class
*/
void NmEditorTextEdit::sendMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event) {
        HbAbstractEdit::mouseReleaseEvent(event);
    }
}

/*!
    sendMouseMoveEvent. Function is used to relay mouse move event to base class
*/
void NmEditorTextEdit::sendMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event) {
        HbAbstractEdit::mouseMoveEvent(event);
    }
}

/*!
    sendLongPressEvent. Opens the context menu of the editor
*/
void NmEditorTextEdit::sendLongPressEvent(const QPointF &point)
{
    showContextMenu(point);
}

/*!
   Sets flag is custom text color should be used and sets the custom color.
   
   Function does not affect the color of existing content, only text that will be entered later.
 */
void NmEditorTextEdit::setCustomTextColor(const QPair<bool, QColor> &customColor)
{
    mCustomTextColor = customColor;
}

/*!
    Reimplemented function \sa NmEditorTextEdit::setCustomTextColor(const QPair<bool, QColor> &customColor).
    
    \arg info about using of custom color
    \arg color to be used as custom. If \arg useCustom is set to false then color is not changed
 */
void NmEditorTextEdit::setCustomTextColor(bool useCustom, const QColor& color)
{   
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
    return mCustomTextColor;
}
