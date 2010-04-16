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
    EEditorGroupBoxRecipient,
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
    NmHtmlLineEdit* subjectField() const;
    NmRecipientLineEdit* toField() const;
    NmRecipientLineEdit* ccField() const;
    NmRecipientLineEdit* bccField() const;
    void setPriority(NmMessagePriority priority=NmMessagePriorityNormal);
    void setPriority(NmActionResponseCommand prio=NmActionResponseCommandNone);
    void setGroupBoxCollapsed( bool collapsed );
    void addAttachment(const QString &fileName, const QString &fileSize, const NmId &nmid);
    void removeAttachment(const QString &fileName);
    void setAttachmentParameters(
        const QString &fileName,
        const NmId &msgPartId,
        const QString &fileSize,
        int result);

private:
    void loadWidgets();
    void rescaleHeader();
    void createConnections();
    HbWidget* createRecipientGroupBoxContentWidget();

signals:
    void headerHeightChanged(int);
    void recipientFieldsHaveContent(bool recipientFieldsHaveContent);
    void attachmentRemoved(const NmId);

public slots:
    void sendHeaderHeightChanged();
    void editorContentChanged();
    void groupBoxExpandCollapse();
    void attachmentSelected(int, QPointF);

private:
    HbDocumentLoader* mDocumentLoader;  // Not owned
    int mHeaderHeight;
    HbLabel *mSubjectLabel;
    HbLabel *mPriorityIconLabel;
    bool mIconVisible;
    NmRecipientLineEdit *mToEdit;
    NmHtmlLineEdit *mSubjectEdit;
    bool mRecipientFieldsEmpty;

    // Recipient GroupBox related
    HbGroupBox *mGroupBoxRecipient;          // Owned
    HbWidget *mGroupBoxRecipientContent;     // Not owned
    QGraphicsLinearLayout *mGbVerticalLayout;// Not owned
    QGraphicsLinearLayout *mCcFieldLayout;   // Not owned
    QGraphicsLinearLayout *mBccFieldLayout;  // Not owned

    NmRecipientField *mToField;    // owned
    NmRecipientField *mCcField;    // Not owned
    NmRecipientField *mBccField;   // Not owned

    NmAttachmentList *mAttachmentList;
};

#endif /* NMEDITORHEADER_H_ */
