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
* Description:  ESMR observer for the font size settings
*
*/

#ifndef MESMRFONTSIZEOBSERVER_H
#define MESMRFONTSIZEOBSERVER_H

/**
 *  Observer interface for the font size settings.
 *
 *  @see cesmrview.h
 *  @lib esmrgui.lib
 */
 class MESMRFontSizeObserver
    {
public:
    /**
     * Method to be called when font size settings have changed
     */
    virtual void FontSizeSettingsChanged() = 0;
    };

#endif // MESMRFONTSIZEOBSERVER_H
