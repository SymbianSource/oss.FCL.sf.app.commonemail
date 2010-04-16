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

#include "nmbaseview.h"
#include "nmactionobserver.h"
#include "nmactionresponse.h"

class QGraphicsLinearLayout;
class HbTextEdit;
class HbDocumentLoader;
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


public slots:

    void orientationChanged(Qt::Orientation orientation);
    void createOptionsMenu();
    void setButtonsDimming(bool enabled);


public: // From NmActionObserver

    void handleActionCommand(NmActionResponse &menuResponse);


private slots:

    void messageCreated(int result);
    void adjustViewDimensions();
    void oneAttachmentAdded(const QString &fileName,
                            const NmId &msgPartId,
                            int result);

    void allAttachmentsAdded(int result);
    void outboxChecked(int result);
    void removeAttachment(const NmId attachmentPartId);

#ifdef Q_OS_SYMBIAN    
    void onAttachmentReqCompleted(const QVariant &value);
    void attachImage();
#endif    


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


public slots:

    void sendMousePressEventToScroll(QGraphicsSceneMouseEvent *event);
    void sendMouseReleaseEventToScroll(QGraphicsSceneMouseEvent *event);
    void sendMouseMoveEventToScroll(QGraphicsSceneMouseEvent *event);
    void sendLongPressGesture(const QPointF &point);


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
    HbMenu  *mPrioritySubMenu;          // Owned
    HbMenu *mAttachContextMenu;         // Owned

    NmMessageCreationOperation *mMessageCreationOperation;  // Owned
    NmAddAttachmentsOperation *mAddAttachmentOperation;     // Owned
    NmOperation *mRemoveAttachmentOperation;                // Owned
    NmCheckOutboxOperation *mCheckOutboxOperation;          // Owned
};


#endif /* NMEDITORVIEW_H_ */
