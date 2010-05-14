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
* Description:
*
*/


#include "nmuiheaders.h"

static const qreal nmHeaderLineOpacity = 0.4;

/*!
    \class NmViewerHeader
    \brief Mail viewer header area class
*/

/*!
    Constructor
*/
NmViewerHeader::NmViewerHeader(QGraphicsItem *parent) :
    HbWidget(parent),
    mMessage(NULL),
    mSubject(NULL),
    mSent(NULL),
    mPrioIcon(NULL),
    mHeaderBox(NULL),
    mRecipientsBox(NULL),
    mViewerView(NULL)
{
    loadWidgets();
}

/*!
    Destructor
*/
NmViewerHeader::~NmViewerHeader()
{
}

/*!
    setter for nmviewer view
*/
void NmViewerHeader::setView(NmViewerView* view)
{
    mViewerView = view;
}

/*!
    Load widgets from XML for the header
*/
void NmViewerHeader::loadWidgets()
{
    // Scale header widget to screen width
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding); 

    // Add header box
    if (!mHeaderBox) {
        mHeaderBox = new HbGroupBox(this);
        HbStyle::setItemName(mHeaderBox, "headergroupbox");
        mHeaderBox->setObjectName("ViewerHeaderRecipients");
        mHeaderBox->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
        mHeaderBox->setCollapsable(true);
    }

    // Add sent time
    if (!mSent){
        mSent = new HbTextItem(this);
        HbStyle::setItemName(mSent, "sent");
        mSent->setObjectName("ViewerHeaderSent");
    }

    // Add priority icon
    if (!mPrioIcon){
        mPrioIcon = new HbIconItem(this);
        mSent->setObjectName("ViewerHeaderPrioIcon");
        HbStyle::setItemName(mPrioIcon, "prioicon");
    }

    // Add subject
    if (!mSubject){
        mSubject = new HbTextItem(this);
        HbStyle::setItemName(mSubject, "subject");
        mSubject->setObjectName("ViewerHeaderSubject");
        mSubject->setTextWrapping(Hb::TextWordWrap);
    }
}


/*!
    Reimplementation to do some extra painting
*/
void NmViewerHeader::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (painter) {
        painter->setOpacity(nmHeaderLineOpacity);
        QLineF line1( rect().topLeft().x(), rect().bottomRight().y(),
                     rect().bottomRight().x(), rect().bottomRight().y());
        painter->drawLine(line1);
        if (mHeaderBox){
            QRectF headerBoxGeometry = mHeaderBox->geometry();
            QLineF line2( headerBoxGeometry.topLeft().x(), headerBoxGeometry.bottomRight().y(),
                          headerBoxGeometry.bottomRight().x(), headerBoxGeometry.bottomRight().y());
            painter->drawLine(line2);        
        }       
    }
}

/*!
    Setter for message object
*/
void NmViewerHeader::setMessage(NmMessage* message)
{
    mMessage=message;
    setHeaderData();
}


/*!
    Function updates data in already created objects. New message pointer
    comes from viewer view, ownership is not transferred.
    This function gets called when message body is fetched and
    to/cc/bcc data needs to be updated
*/
void NmViewerHeader::updateMessageData(NmMessage* message)
{
    if (message){
        mMessage=message;
        // Set recipients to text edit field as html 
        NmAddress sender = mMessage->envelope().sender();      
        if (mRecipientsBox){  
            mRecipientsBox->setHtml(formatRecipientList(addressToDisplayInHtml(sender),
                                    mMessage->envelope().toRecipients(), 
                                    mMessage->envelope().ccRecipients()));
        }       
    }
}

/*!
    Function sets data to header area based on message data
*/
void NmViewerHeader::setHeaderData()
{
    if (mMessage) {
        // Background is all white always, so force text color to black
        QColor textColor(Qt::black);
        const NmMessageEnvelope &envelope = mMessage->envelope();
        const QString dispName = envelope.sender().displayName();
        if (dispName.length()>0){
            mSenderName = NmUtilities::cleanupDisplayName(dispName);
        }
        else {
            mSenderName = envelope.sender().address();
        }
        if (mHeaderBox) {
            mHeaderBox->setHeading(mSenderName);
        }
        // Set subject text
        if (mSubject){
            QString subjectText = envelope.subject();
            if (subjectText.length()){
                mSubject->setText(subjectText);
            }
            else{
                mSubject->setText(hbTrId("txt_mail_dblist_val_no_subject"));
            }
        mSubject->setTextColor(textColor);
        }
        if (mSent){
            HbExtendedLocale locale = HbExtendedLocale::system();
            QTime time = envelope.sentTime().toLocalTime().time();
            QDate sentLocalDate = envelope.sentTime().toLocalTime().date();
            QString shortDateSpec = r_qtn_date_without_year;
            QString shortTimeSpec = r_qtn_time_usual;
            QString text = locale.format(sentLocalDate, shortDateSpec);
            text += " ";
            text += locale.format(time, shortTimeSpec);
            mSent->setText(text);
            mSent->setTextColor(textColor);
        } 
        if (mPrioIcon){
            switch (envelope.priority()){
                case NmMessagePriorityLow:
                    mPrioIcon->setObjectName("ViewerHeaderPriorityLow");
                    mPrioIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityLow));
                    mPrioIcon->show();
                    break;
                case NmMessagePriorityHigh:
                    mPrioIcon->setObjectName("ViewerHeaderPriorityHigh");
                    mPrioIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
                    mPrioIcon->show();
                    break;
                case NmMessagePriorityNormal:
                default:
                    // Normal priority has no icon so hide the label hide.
                    mPrioIcon->setObjectName("ViewerHeaderPriorityNormal");
                    break;
            }
        }
    }
    createExpandableHeader();
}


/*!
    Function can be used to rescale the header area.
*/ 
void NmViewerHeader::rescaleHeader(const QSizeF layoutReso)
{  
    setMinimumWidth(layoutReso.width());
    setMaximumWidth(layoutReso.width());
}

/*!
    Function creates expandable header area with group box
*/
void NmViewerHeader::createExpandableHeader()
{
    if (mHeaderBox) {        // Initialize recipient box
        if (!mRecipientsBox){
            mRecipientsBox = new HbTextEdit();
            HbStyle::setItemName(mRecipientsBox, "recipients");
            mRecipientsBox->setContextMenuFlags(0);
            mRecipientsBox->setReadOnly(true);
            mRecipientsBox->setFontSpec(HbFontSpec(HbFontSpec::Secondary)); 
            // Set text wrapping for from/to/cc address fields using text document
            QTextOption textOption = mRecipientsBox->document()->defaultTextOption();
            textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            mRecipientsBox->document()->setDefaultTextOption(textOption);
        }
        connect(mRecipientsBox, SIGNAL(cursorPositionChanged(int, int)),
                this, SLOT(cursorPositionChanged(int, int)));
                
        // Set recipients to text edit field as html 
        NmAddress sender = mMessage->envelope().sender();               
        if (mMessage) {
            mRecipientsBox->setHtml(formatRecipientList(addressToDisplayInHtml(sender),
                                    mMessage->envelope().toRecipients(), 
                                    mMessage->envelope().ccRecipients()));
        }
        mRecipientsBox->setCursorVisibility(Hb::TextCursorHidden);
        mHeaderBox->setContentWidget(mRecipientsBox);
        // Set box collapsed as default
        mHeaderBox->setCollapsed(true);
    }
}

/*!
    Function formats recipient list to be displayed in HTML format.
*/
QString NmViewerHeader::formatRecipientList(const QString &sender,
                                            const QList<NmAddress> &to,
                                            const QList<NmAddress> &cc)
{
    QString result;
    result.append("<html><body link=\"blue\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\"");
    result.append("marginwidth=\"0\" bgcolor=\"white\" text=\"black\">");    
    result.append("<font color=\"black\" face=\"");
    result.append("Nokia Sans");
    result.append("\"size=\"3\">"); 
    // Set text in HTML format based on layout direction
    if (qApp->layoutDirection()==Qt::RightToLeft){
        result.append("<p style=\"margin-right: '0'; margin-left: '0'\" dir=\"rtl\"><b>"); 
        result.append(hbTrId("txt_mail_list_from"));
        result.append(" </b>"); 
        result.append(sender);
        result.append("<br><b>"); 
        result.append(hbTrId("txt_mail_list_to"));
        result.append(" </b>");
        int reciCount = to.count();
        for (int i=0; i < reciCount; i++) { 
            result.append(addressToDisplayInHtml(to[i]));
            result.append(" ");
        } 
        reciCount = cc.count();
        if (reciCount) {
            result.append("<br><b>");
            result.append(hbTrId("txt_mail_list_cc"));
            result.append(" </b>");
            for (int i=0; i < reciCount; i++) {
                result.append(addressToDisplayInHtml(cc[i]));
                result.append(" "); 
            }
        }   
    }
    else{
        result.append("<p style=\"margin-right: '0'; margin-left: '0'\" dir=\"ltr\"><b>"); 
        result.append(hbTrId("txt_mail_list_from"));
        result.append(" </b>"); 
        result.append(sender);
        result.append("<br><b>"); 
        result.append(hbTrId("txt_mail_list_to"));
        result.append(" </b>");
        int reciCount = to.count();
        for (int i=0; i < reciCount; i++) { 
            result.append(addressToDisplayInHtml(to[i]));
            result.append("; ");
        }
        reciCount = cc.count();
        if (reciCount) {
            result.append("<br><b>");
            result.append(hbTrId("txt_mail_list_cc"));
            result.append(" </b>");
            for (int i=0; i < reciCount; i++) {
                result.append(addressToDisplayInHtml(cc[i]));
                result.append("; ");
            }
        }   
    } 
    result.append("</p></font></body></html>");     
    return result;
}

/*!
    Function retursn address string from NmAddress to
    be displayed in HTML format
*/
QString NmViewerHeader::addressToDisplayInHtml(const NmAddress &addr)
{
    QString dispName;
    if (addr.displayName().length()!=0){
        dispName.append(NmUtilities::cleanupDisplayName(addr.displayName()));
    }
    else{
        dispName.append(addr.address());
    }
    QString ret="<a href=mailto:";
    ret.append(addr.address());
    ret.append(">");
    ret.append(dispName);
    ret.append("</a>");
    return ret;
}

/*!
    Function handles cursor position changes in header group box.
    E.g link handler.
*/
void NmViewerHeader::cursorPositionChanged(int oldPos, int newPos)
{
    Q_UNUSED(oldPos);
    QString string = mRecipientsBox->anchorAt(newPos); 
    if (mViewerView&&string.contains("mailto:",Qt::CaseSensitive)){
        QUrl url(string);     
        mViewerView->linkClicked(url);
    }
}
