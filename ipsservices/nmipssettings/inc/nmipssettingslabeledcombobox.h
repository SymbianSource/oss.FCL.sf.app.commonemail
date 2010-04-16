/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMIPSSETTINGSLABELEDCOMBOBOX_H
#define NMIPSSETTINGSLABELEDCOMBOBOX_H

#include <hbwidget.h>
#include <qhash.h>

class QGraphicsLinearLayout;
class QGraphicsItem;
class HbComboBox;
class HbLabel;

class NmIpsSettingsLabeledComboBox : public HbWidget
{
    Q_OBJECT

public:

    NmIpsSettingsLabeledComboBox(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~NmIpsSettingsLabeledComboBox();

    void setCurrentIndex(int index);
    void setItems(QStringList comboBoxItems, QStringList texts);

signals:

    void currentIndexChanged(int index);

private slots:

    void comboBoxIndexChanged(int index);

private: // data

    // Now owned.
    HbLabel *mLabel;
    // Not owned.
    HbComboBox *mComboBox;

    QHash<int, QString> mLabelTexts;

};

#endif // NMIPSSETTINGSLABELEDCOMBOBOX_H
