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
class NmCheckOutboxOperation;
class NmAttachmentPicker;


class NmEditorView : public NmBaseView, public NmActionObserver
{
    Q_OBJECT

public:

    NmEditorView(NmApplication &application,
                 NmUiStartParam* startParam,
                 NmUiEngine &uiEngine,
                 QGraphicsItem *parent = 0);
    ~NmEditorView();


public:

    void reloadViewContents(NmUiStartParam* startParam);
    NmUiViewId nmailViewId() const;
    HbWidget* scrollAreaContents();
    bool okToExitView();
    void aboutToExitView();
    void viewReady();

public slots:

    void orientationChanged(Qt::Orientation orientation);
    void createOptionsMenu();
    void setButtonsDimming(bool enabled);
    void attachmentLongPressed(NmId attachmentPartId, QPointF point);


public: // From NmActionObserver

    void handleActionCommand(NmActionResponse &menuResponse);


private slots:

    void messageCreated(int result);
    void adjustViewDimensions();
    void oneAttachmentAdded(const QString &fileName,
                            const NmId &msgPartId,
                            int result);

    void allAttachmentsAdded(int result);
    void attachmentRemoved(int result);
    void outboxChecked(int result);
    void removeAttachmentTriggered();
    void handleSendOperationCompleted();
    void openAttachmentTriggered();  
    void onAttachmentReqCompleted(const QVariant &value);

private:

    void loadViewLayout();
    void setMailboxName();
    void setMessageData();
    void startMessageCreation(NmUiEditorStartMode startMode);
    void startSending();
    void createToolBar();
    QPointF viewCoordinateToEditCoordinate(QPointF orgPoint);
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

    void sendMousePressEventToScroll(QGraphicsSceneMouseEvent *event);
    void sendMouseReleaseEventToScroll(QGraphicsSceneMouseEvent *event);
    void sendMouseMoveEventToScroll(QGraphicsSceneMouseEvent *event);
    void sendLongPressGesture(const QPointF &point);
    void contextButton(NmActionResponse &result);


private: // Data

    NmApplication &mApplication;
    NmUiEngine &mUiEngine;
    HbDocumentLoader *mDocumentLoader;  // Owned
    QObjectList mWidgetList;            // Owned
    NmBaseViewScrollArea *mScrollArea;  // Not owned
    HbWidget *mScrollAreaContents;      // Not owned
    NmEditorTextEdit *mEditWidget;      // Not owned
    NmEditorHeader *mHeaderWidget;      // Not owned
    NmMessage *mMessage;                // Owned
    NmEditorContent *mContentWidget;    // Owned
    HbMenu *mPrioritySubMenu;           // Owned
    HbMenu *mAttachmentListContextMenu; // Owned
    NmId mSelectedAttachment;

    QPointer<NmMessageCreationOperation> mMessageCreationOperation;  // Not owned
    QPointer<NmAddAttachmentsOperation> mAddAttachmentOperation;     // Not owned 
    QPointer<NmOperation> mRemoveAttachmentOperation;                // Not owned 
    QPointer<NmCheckOutboxOperation> mCheckOutboxOperation;          // Not owned 

    HbProgressDialog *mWaitDialog; // Owned.
    
    NmAttachmentPicker* mAttachmentPicker;    // Owned    
};


#endif /* NMEDITORVIEW_H_ */
