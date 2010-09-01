/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  esmrui definitions used internally in mrui
*
*/
#ifndef ESMRINTERNALUID_H
#define ESMRINTERNALUID_H

//<cmail>
#include "mruiuids.h" //added by Cmail
//</cmail>

// MRUI MR UTILS Calendar plug-in
// <cmail> S60 UID update
#define KESMRUtilsCalPluginDllUid3           0x2001E27F
#define KESMRUtilsCalPluginImplUid3Imap      0x2001F40D
#define KESMRUtilsCalPluginImplUid3Smtp      0x2001F40E
#define KESMRUtilsCalPluginImplUid3Fs        0x2001F40F
// </cmail> S60 UID update
#define KESMRUtilsCalPluginDataImap          "[1000102a]" //For plugin resource file
#define KESMRUtilsCalPluginDataSmtp          "[10001028]" //For plugin resource file
// <cmail> S60 UID update
#define KESMRUtilsCalPluginDataFs            "[2001f406]"  //For plugin resource file
#define KESMRUtilsCalPluginResource          2001E27F.rss // for mmp
// </cmail> S60 UID update

// MRUI MR VIEWER calendar plug-in
// <cmail> S60 UID update
#define KESMRViewerCalPluginDllUid3          0x2001E27E
#define KESMRViewerCalPluginImplUid3Imap     0x2001F410
#define KESMRViewerCalPluginImplUid3Smtp     0x2001F411
#define KESMRViewerCalPluginImplUid3Fs       0x2001F412
// </cmail> S60 UID update
#define KESMRViewerCalPluginPluginDataImap   "[1000102a]"  //For plugin resource file
#define KESMRViewerCalPluginPluginDataSmtp   "[10001028]"  //For plugin resource file
// <cmail> S60 UID update
#define KESMRViewerCalPluginPluginDataFs     "[2001F406]"  //For plugin resource file
#define KESMRViewerCalPluginResource         2001E27E.rss // for mmp
// </cmail> S60 UID update

// MRUI field builder uids
#define KESMRUIFieldBuilderInterfaceUId           0x2001F4AB
#define KESMRUIFieldBuilderPluginImplUId          0x2001F4AC
#define KESMRUIFieldBuilderPluginDllUId           0x2001F4AD
#define KESMRUIFieldBuilderPluginExtensionImplUId 0x2001F4AE
#define KESMRUIFieldBuilderPluginExtensionDllUId  0x2001F4AF

// MRUI Alf layout
#define KESMRAlfLayoutPluginInterfaceUid    0x20018428
#define KESMRAlfLayoutPluginImplUid         0x2001846C
#define KESMRAlfLayoutPlugiDllUid           0x20018427

// MRUI scheduling view
#define KESMRSchedulingviewInterfaceUid 0x2001842C
#define KESMRSchedulingviewImplUid      0x2001842C
#define KESMRSchedulingviewDllUid       0x2001842D

// MRUI common DLL uid
// <cmail> S60 UID update
#define KESMRDllUid3                0x2001E27F
// </cmail> S60 UID update

// MRUI Extension UIDs
// <cmail> S60 UID update
#define KESMRPolicyMgrUid           0x2001FE10
#define KESMRMREntryProcessorUid    0x2001FE11
#define KESMRTaskExtensionUid       0x2001FE12
// </cmail> S60 UID update

// Common system UIDs
#define KEPOCNullUID                0x0
#define KEPOCStaticDllUID2          0x1000008d
#define KEComRecognitionUID2        0x10009D8D

// MRUI specific application and view UIDs
#define KUidMceMailApplication      0x101F4CE4
// <cmail> S60 UID update
#define KUidEsMailApplication       0x2001E277
// </cmail> S60 UID update
#define KUidBVAApplication          0x101F4CE5
#define KUidCalendarApplication     0x10005901
#define KUidEsEmailEditor           0x00000004

// Plugin specific IDs
#define KEasFreestylePlugin         0x2000b020
#define KIntellisync                0x2000e4fb
#define KFSEmailPop3                0x2000e53e
#define KFSEmailImap4               0x2000e53f

// Location plugin UIDs
#define KESMRLocationPluginInterfaceUid			0x2001E60A
#define KESMRLocationPluginImplementationUid	0x2001E60B
#define KESMRLocationPluginDllUid				0x2001E60C

// MR Mailbox Utils plugin UIDs
#define KESMRMButilsExtensionImplementationUid  0x2002137D
#define KESMRMButilsExtensionDllUid             0x2002137D
#define KMRDefaultCMailMRMailbox                0x2002137C

// MR BC Plugin UIDs
#define KMRBCPluginImplementationUid    0x2002137B
#define KMRBCPluginDllUid               0x2002137B

#define KS60CalenEditorsImplementation 0x102071F4
#define KCalEntryUIImplmentation       0x200025B4

#define KMREntryProcessorInterfaceUID               0x20026F6D
#define KMRMeetingRequestProcessorImplementationUID 0x20026F6E
#define KMRCalEventProcessorImplementationUID       0x20026F6F

// MRUI Private CenRep UIDs
#define KESMRUIPreviousLocationsCRUid   0x2001E611

// Definitions for time operations
#define KMaxHoursInDay                  23
#define KMaxMinutesInHour               59
#define KMaxSecondsInMinute             59

// Definition for BC plug-in resources
#define KBCCalMRPluginResource "bcmreventplugin.rsc"

#define KMRBCEventPluginInterfaceUID        0x20022D89
#define KMRBCMREventPluginImplementationUID 0x20022D8A

// Definition of MR Tasks ECom plug-in UIDs
#define KMRTaskInterfaceUID                     0x20022D8C
#define KMRTaskPluginImplementationUID          0x20022D8D
#define KMRCalEventTaskPluginImplementationUID  0x20026F6C

// Definition of MR Policy resolver ECom plug-in UIDs
#define KMRPolicyResolverInterfaceUID           0x20026F70
#define KMREntryPolicyImplementationUID         0x20026FB8
#define KMRCalEntryPolicyImplementationUID      0x20026FB9

#endif // ESMRINTERNALUID_H
