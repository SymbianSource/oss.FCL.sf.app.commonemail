/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NMHSWIDGETEMAILROW_H_
#define NMHSWIDGETEMAILROW_H_

#include <QDateTime>
#include <hbwidget.h>
#include "nmcommon.h"
#include "nmmessageenvelope.h"

//FORWARD DECLARATIONS:
class HbLabel;
class HbFrameItem;

class NmHsWidgetEmailRow : public HbWidget
{
    Q_OBJECT
  
public:
    NmHsWidgetEmailRow(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidgetEmailRow();
    NmId messageId();
    bool setupUI();
    void gestureEvent(QGestureEvent *event);

public slots:
    void updateMailData( const NmMessageEnvelope& envelope );
    void updateDateTime();
    
signals:
    void mailViewerLaunchTriggered(const NmId& messageId);
    
private:
    void setIconsToWidget( const NmMessageEnvelope& envelope );
    void hideIcons();
    bool loadDocML();
    bool setupGraphics();
    void setFontsSize( bool read );
    void setHighlighedFontsColor( bool pressed );
    void showHighlight( bool pressed );
    
protected:
    bool event( QEvent *event );
    
private:
    HbLabel *mSenderLabel;
    HbLabel *mSubjectLabel;
    HbLabel *mTimeLabel;
    HbWidget *mNewMailIcon;
    HbLabel *mSeparatorIcon;
    NmId mMessageId;
    QDateTime mMessageSentTime;
    QList <HbLabel*> mStatusIcons;  
    NmMessageEnvelope mEnvelope;
    HbFrameItem* mBackgroundLayoutItem;
    
};

#endif // NMHSWIDGETEMAILROW_H_
