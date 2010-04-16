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
* Description:
*
*/


#include "nmuiheaders.h"

/*!
	\class NmBaseView
	\brief Base class for views, contains general data about view
*/

/*!
    Constructor
*/
NmBaseView::NmBaseView(NmUiStartParam* startParam,
                       QGraphicsItem *parent)
: HbView(parent),
mStartParam(startParam)
{
}

/*!
    Destructor
*/
NmBaseView::~NmBaseView()
{
    delete mStartParam;
}

/*!
    Is it ok to exit current view. Function is called when exiting the view. 
    Views can override this function and return false to stay in current view. 
*/
bool NmBaseView::okToExitView()
{
    return true;
}

/*!
    About to exit view. Function is called before view is removed from
    view stack and deleted. Views can override this function and do pre-exit
    functions. For example editor can store message to drafts at this stage.
*/
void NmBaseView::aboutToExitView()
{
}

/*!
    About to change orientation. View can override this function if special
    handling is needed for the view when orientation is about to change.
*/
void NmBaseView::aboutToChangeOrientation()
{
}

/*!
    Orientation changed. View can override this function if special
    handling is needed for the view when orientation is changed.
*/
void NmBaseView::orientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
}

/*!
    Mouse release event. View can override this function if special
    handling is required because of  overlaying scroll area (viewer and editor)
*/
void NmBaseView::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

/*!
    Mouse press event. View can override this function if special
    handling is required because of  overlaying scroll area (viewer and editor)
*/
void NmBaseView::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}


/*!
    Mouse move event. View can override this function if special
    handling is required because of overlaying scroll area (viewer and editor)
*/
void NmBaseView::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}


