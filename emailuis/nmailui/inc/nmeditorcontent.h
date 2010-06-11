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
* Description: Message editor contents widget
*
*/

#ifndef NMEDITORCONTENT_H_
#define NMEDITORCONTENT_H_

#include <hbwidget.h>
#include "nmuiviewids.h"

enum MessageBodyType { PlainText, HTMLText };

class HbAnchorLayout;
class HbTextEdit;
class HbDocumentLoader;
class NmBaseViewScrollArea;
class NmEditorView;
class NmMessage;
class NmMessageEnvelope;
class NmEditorHeader;
class NmEditorTextEdit;
class QNetworkAccessManager;

class NmEditorContent : public HbWidget
{
Q_OBJECT

public:
    NmEditorContent(QGraphicsItem *parent,
                    NmEditorView *parentView,
                    HbDocumentLoader *documentLoader,
                    QNetworkAccessManager &manager);
    virtual ~NmEditorContent();

    void setMessageData(const NmMessage &originalMessage);
    NmEditorTextEdit* editor() const;
    NmEditorHeader* header() const;

private:
    void createConnections();

signals:
    void setPlainText(const QString&);
    void setHtml(const QString&);

public slots:
    void setEditorContentHeight();

private:
    NmEditorHeader *mHeaderWidget;   // Owned
    NmEditorView *mParentView;       // Not owned
    HbAnchorLayout *mEditorLayout;   // Not owned
    MessageBodyType mMessageBodyType;
    NmEditorTextEdit *mEditorWidget; // Not owned
    NmBaseViewScrollArea *mBackgroundScrollArea;
};

#endif /* NMEDITORCONTENT_H_ */
