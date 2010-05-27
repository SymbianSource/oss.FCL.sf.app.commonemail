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
// Header widget fields
enum
{
    EEditorToLine=0,
    EEditorCcLine,
    EEditorBccLine,
    EEditorSubjectLine,
    EEditorAttachmentLine
};

class HbGroupBox;
class HbLabel;
class HbDocumentLoader;
class NmHtmlLineEdit;
class NmRecipientLineEdit;
class QGraphicsLinearLayout;
class NmRecipientField;
class NmAttachmentList;

class NmEditorHeader : public HbWidget
{
Q_OBJECT

public:
    NmEditorHeader(HbDocumentLoader *documentLoader, QGraphicsItem *parent=0);
    virtual ~NmEditorHeader();
    int headerHeight() const;
    NmHtmlLineEdit* subjectEdit() const;
    NmRecipientLineEdit* toEdit() const;
    NmRecipientLineEdit* ccEdit() const;
    NmRecipientLineEdit* bccEdit() const;
    void setPriority(NmMessagePriority priority=NmMessagePriorityNormal);
    void setPriority(NmActionResponseCommand prio=NmActionResponseCommandNone);
    void addAttachment(const QString &fileName, const QString &fileSize, const NmId &nmid);
    void removeAttachment(const QString &fileName);
    void removeAttachment(const NmId &nmid);
    void launchAttachment(const NmId &nmid);
    void setAttachmentParameters(
        const QString &fileName,
        const NmId &msgPartId,
        const QString &fileSize,
        int result);
    void setFieldVisibility(bool isVisible);

private:
    void loadWidgets();
    void rescaleHeader();
    void createConnections();
    void adjustFieldSizeValues( NmRecipientLineEdit *widget, qreal height );

signals:
    void headerHeightChanged(int);
    void recipientFieldsHaveContent(bool recipientFieldsHaveContent);
    void attachmentLongPressed(NmId attachmentPartId, QPointF point);

public slots:
	void fixHeaderFieldHeights();
    void sendHeaderHeightChanged();
    void editorContentChanged();
    void attachmentActivated(int arrayIndex);
    void attachmentLongPressed(int arrayIndex, QPointF point);

private:
    HbDocumentLoader* mDocumentLoader;  // Not owned
	HbWidget *mHeader;  // Not owned
    int mHeaderHeight;
    HbLabel *mSubjectLabel;
    HbLabel *mPriorityIconLabel;
    bool mIconVisible;
    NmRecipientLineEdit *mToEdit;
    NmRecipientLineEdit *mCcEdit;
    NmRecipientLineEdit *mBccEdit;
    NmHtmlLineEdit *mSubjectEdit;
    bool mRecipientFieldsEmpty;
    QGraphicsLinearLayout *mLayout;
    NmAttachmentList *mAttachmentList;
    NmRecipientField *mToField;    // Owned
    NmRecipientField *mCcField;    // Owned
    NmRecipientField *mBccField;   // Owned
    bool mCcBccFieldVisible;
};

#endif /* NMEDITORHEADER_H_ */
