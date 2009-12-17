/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR observer for the title pane
*
*/


#ifndef MESMRTITLEPANEOBSERVER_H
#define MESMRTITLEPANEOBSERVER_H

class MESMRTitlePaneObserver
{
public:
    /**
     * Updates title pane text.
     * 
     * @param aText new text
     */
    virtual void UpdateTitlePaneTextL( const TDesC& aText ) = 0;
    
    /**
    * Updates the priority icon in title pane
    * 
    * @param aPriority
    */
    virtual void UpdateTitlePanePriorityIconL( TUint aPriority ) = 0;
};

#endif  // MESMRTITLEPANEOBSERVER_H
