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
* Description: Utility class for launching different file pickers
*
*/

#ifndef NMATTACHMENTPICKER_H_
#define NMATTACHMENTPICKER_H_

#include <QObject>

#define IMAGE_FETCHER_INTERFACE "Image"
#define AUDIO_FETCHER_INTERFACE "com.nokia.services.media.Music"

#define IMAGE_FETCHER_OPERATION "fetch(QVariantMap,QVariant)"
#define AUDIO_FETCHER_OPERATION "fetch(QString)"

class XQAiwRequest;

/**
 * This class manages launching of different attachment pickers 
 * 
 */
class  NmAttachmentPicker : public QObject
{
    Q_OBJECT

public:
    NmAttachmentPicker(QObject* parent = 0);
    ~NmAttachmentPicker();
        
public slots:
    void fetchImage();
    void fetchAudio();    
    void fetchVideo();
    void fetchOther();       

signals:
    void attachmentsFetchOk(const QVariant& fileNames);
    void attachmentsFetchError(int errorCode, const QString& errorMessage);
    
private:   
    void fetch(const QString& interface, const QString& operation); 
    
private:
    XQAiwRequest *mRequest;    
};

#endif // NMATTACHMENTPICKER_H_
