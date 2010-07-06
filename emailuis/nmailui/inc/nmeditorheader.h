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

#ifndef NMEDITORHEADER_H_
#define NMEDITORHEADER_H_

#include <hbwidget.h>
#include "nmactionresponse.h"

class HbGroupBox;
class HbLabel;
class HbDocumentLoader;
class NmHtmlLineEdit;
class NmRecipientLineEdit;
class QGraphicsLinearLayout;
class NmRecipientField;
class NmAttachmentList; 

class NmEditorHeader : public QObject
{
Q_OBJECT

public:
    NmEditorHeader(QObject *parent, HbDocumentLoader *documentLoader);
    virtual ~NmEditorHeader();
    qreal headerHeight() const;
    NmHtmlLineEdit *subjectEdit() const;
    NmRecipientLineEdit *toEdit() const;
    NmRecipientLineEdit *ccEdit() const;
    NmRecipientLineEdit *bccEdit() const;
    void setPriority(NmMessagePriority priority=NmMessagePriorityNormal);
    void setPriority(NmActionResponseCommand prio=NmActionResponseCommandNone);
    void addAttachment(const QString &fileName, const QString &fileSize, const NmId &nmid);
    void removeAttachment(const QString &fileName);
    void removeAttachment(const NmId &nmid);
    void setAttachmentParameters(
        const QString &fileName,
        const NmId &msgPartId,
        const QString &fileSize,
        int result);
    void setFieldVisibility(bool isVisible);

private:
    void loadWidgets();
    void createConnections();

signals:
    void headerHeightChanged(int);
    void recipientFieldsHaveContent(bool recipientFieldsHaveContent);
    void attachmentLongPressed(NmId attachmentPartId, QPointF point);
    void attachmentShortPressed(NmId attachmentId);

public slots:
	void sendDelayedHeaderHeightChanged();
    void sendHeaderHeightChanged();
    void editorContentChanged();
    void attachmentActivated(int arrayIndex);
    void attachmentLongPressed(int arrayIndex, QPointF point);

private:
    HbDocumentLoader* mDocumentLoader;  // Not owned
    qreal mHeaderHeight;
    HbLabel *mPriorityIcon; // Not owned
    HbLabel *mFollowUpIcon; // Not owned
    bool mIconVisible;
    NmRecipientLineEdit *mToEdit; // Not owned
    NmRecipientLineEdit *mCcEdit; // Not owned
    NmRecipientLineEdit *mBccEdit; // Not owned
    NmHtmlLineEdit *mSubjectEdit; // Not owned
    bool mRecipientFieldsEmpty;
    QGraphicsLinearLayout *mLayout; // Not owned
    NmAttachmentList *mAttachmentList; // Not owned
    NmRecipientField *mToField;    // Not owned
    NmRecipientField *mCcField;    // Not owned
    HbWidget *mCcWidget;    // Not owned
    NmRecipientField *mBccField;   // Not owned
    HbWidget *mBccWidget;    // Not owned
    HbWidget *mSubjectWidget;    // Not owned
    QGraphicsLinearLayout *mSubjectLayout; // Not owned
    bool mCcBccFieldVisible;
};

#endif /* NMEDITORHEADER_H_ */
