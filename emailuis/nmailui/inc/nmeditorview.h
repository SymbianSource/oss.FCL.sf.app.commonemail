/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Message editor view
*
*/

#ifndef NMEDITORVIEW_H_
#define NMEDITORVIEW_H_

#include <QPointer>

#include "nmbaseview.h"
#include "nmactionobserver.h"
#include "nmactionresponse.h"
#include "nmattachmentfetchobserver.h"

class QGraphicsLinearLayout;
class HbTextEdit;
class HbDocumentLoader;
class HbProgressDialog;
class HbWidget;
class NmBaseViewScrollArea;
class NmApplication;
class NmUiEngine;
class NmUiStartParam;
class NmMessage;
class NmEditorContent;
class NmEditorTextEdit;
class NmEditorHeader;
class NmAction;
class NmOperation;
class NmMessageCreationOperation;
class NmAddAttachmentsOperation;
class NmAttachmentPicker;
class HbMessageBox;
class NmAttachmentManager;


class NmEditorView : public NmBaseView,
                     public NmActionObserver,
                     public NmAttachmentFetchObserver
{
    Q_OBJECT

public:

    NmEditorView(NmApplication &application,
                 NmUiStartParam* startParam,
                 NmUiEngine &uiEngine,
                 NmAttachmentManager &attaManager,
                 QGraphicsItem *parent = 0);
    ~NmEditorView();


public:

    void reloadViewContents(NmUiStartParam* startParam);
    NmUiViewId nmailViewId() const;
    HbWidget* scrollAreaContents();
    void okToExitView();
    void aboutToExitView();
    void viewReady();

public slots:

    void orientationChanged(Qt::Orientation orientation);
    void createOptionsMenu();
    void setButtonsDimming(bool enabled);
    void attachmentLongPressed(NmId attachmentPartId, QPointF point);
    void invalidAddressQuery(HbAction* action);
    void okToExitQuery(HbAction* action);

public: // From NmActionObserver

    void handleActionCommand(NmActionResponse &menuResponse);

public: // From NmAttachmentFetchObserver
    void progressChanged(int value);
    void fetchCompleted(int result);

private slots:

    void messageCreated(int result);
    void adjustViewDimensions();
    void oneAttachmentAdded(const QString &fileName,
                            const NmId &msgPartId,
                            int result);

    void allAttachmentsAdded(int result);
    void attachmentRemoved(int result);
    void removeAttachmentTriggered();
    void handleSendOperationCompleted();
    void openAttachmentTriggered();  
    void onAttachmentReqCompleted(const QVariant &value);
    void onAttachmentsFetchError(int errorCode, const QString& errorMessage);
    void switchCcBccFieldVisibility();
	void fetchProgressDialogCancelled();

private:

    void loadViewLayout();
    void setMailboxName();
    void fetchProgressDialogShow();
    void fetchMessageIfNeeded();
    void startMessageCreation(NmUiEditorStartMode startMode);
    void startSending();
    void finalizeSending();
    void createToolBar();
    void updateMessageWithEditorContents();
    void fillEditorWithMessageContents();
    void initializeVKB();
    QString addSubjectPrefix(NmUiEditorStartMode startMode, const QString &subject);
    void addAttachments(const QStringList &fileNames);
    void setPriority(NmActionResponseCommand priority);
    QString addressListToString(const QList<NmAddress*> &list) const;
    QString addressListToString(const QList<NmAddress> &list) const;
    void enableToolBarAttach(bool enable);

public slots:

    void sendProgressDialogCancelled();

private: // Data

    NmApplication &mApplication;
    NmUiEngine &mUiEngine;
    NmAttachmentManager  &mAttaManager;
    HbDocumentLoader *mDocumentLoader;  // Owned
    QObjectList mWidgetList;            // Owned
    NmBaseViewScrollArea *mScrollArea;  // Not owned
    HbWidget *mScrollAreaContents;      // Not owned
    NmEditorTextEdit *mEditWidget;      // Not owned
    NmEditorHeader *mHeaderWidget;      // Not owned
    NmMessage *mMessage;                // Owned
    QGraphicsLinearLayout *mLayout;
    NmEditorContent *mContentWidget;    // Owned
    HbMenu *mPrioritySubMenu;           // Owned
    HbMenu *mAttachmentListContextMenu; // Owned
    NmId mSelectedAttachment;

    QPointer<NmMessageCreationOperation> mMessageCreationOperation;  // Not owned
    QPointer<NmAddAttachmentsOperation> mAddAttachmentOperation;     // Not owned 
    QPointer<NmOperation> mRemoveAttachmentOperation;                // Not owned 

    HbProgressDialog *mWaitDialog;         // Owned.    
    HbMessageBox* mQueryDialog;            // Owned
    NmAttachmentPicker* mAttachmentPicker; // Owned    
    bool mCcBccFieldVisible;
    QPointer<HbProgressDialog> mServiceSendingDialog; // Owned.
};


#endif /* NMEDITORVIEW_H_ */
