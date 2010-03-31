/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Visualiser class for Control Button component.
*
*/

#include "emailtrace.h"
#include <alf/alfcontrol.h>
#include <alf/alfdecklayout.h>
#include <alf/alfimagevisual.h>
#include <alf/alfgridlayout.h>
#include <alf/alfenv.h>
#include <alf/alftextstylemanager.h>
#include <alf/alftextvisual.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfframebrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfshadowborderbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alftextstyle.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <touchlogicalfeedback.h>
#include <layoutmetadata.cdl.h>

#include "fscontrolbuttonvisualiser.h"
#include "fscontrolbuttonmodel.h"
#include "fsgenericpanic.h"
#include "fstextstylemanager.h"
#include "fslayoutmanager.h"
#include "fscontrolbar.h"

const TInt KFsDefaultFontStyle = EAlfTextStyleNormal;
const TInt KDefaultShadowBorderWidth( 8 );
const TReal KShadowBorderOpacity( 0.08 );
const TInt KButtonBorderSize( 8 );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsControlButtonVisualiser::CFsControlButtonVisualiser(
    CAlfControl& aParent,
    CAlfDeckLayout& aParentLayout,
    CFsTextStyleManager* aTextStyleManager ) :
    iParent( &aParent ),
    iParentLayout( &aParentLayout ),
    iUpdateIconsAlign( EFalse ),
    iFirstDraw( ETrue ),
    iVisible( EFalse ),
    iTextStyleManager( aTextStyleManager )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsControlButtonVisualiser::CFsControlButtonVisualiser() :
    iUpdateIconsAlign( EFalse ),
    iFirstDraw( ETrue ),
    iVisible( EFalse ),
    iTextStyleManager( NULL )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::ConstructL()
    {
    FUNC_LOG;
    if ( iParent )
        {
        // Visual for background content
        iButtonLayout = CAlfLayout::AddNewL( *iParent );
        iButtonLayout->SetTactileFeedbackL( ETouchEventStylusDown, ETouchFeedbackBasic );
        iButtonLayout->EnableBrushesL();

        iButtonContentLayout = CAlfLayout::AddNewL( *iParent );

        // Request callback after image is loaded to refresh it's size and
        // position.
        iParent->Env().TextureManager().AddLoadObserverL( this );

        iParentLayout->SetFlag( EAlfVisualFlagLayoutUpdateNotification );
        iButtonLayout->SetFlags( EAlfVisualFlagManualSize | EAlfVisualFlagManualPosition );
        iButtonContentLayout->SetFlags( EAlfVisualFlagManualSize | EAlfVisualFlagManualPosition );

        }
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsControlButtonVisualiser* CFsControlButtonVisualiser::NewL(
    CAlfControl& aParent,
    CAlfDeckLayout& aParentLayout,
    CFsTextStyleManager* aTextStyleManager )
    {
    FUNC_LOG;
    CFsControlButtonVisualiser* self =
        new(ELeave)CFsControlButtonVisualiser(
            aParent, aParentLayout, aTextStyleManager );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsControlButtonVisualiser::~CFsControlButtonVisualiser()
    {
    FUNC_LOG;
    ClearBackgroundImage();

    // removing from parent
    if ( iButtonLayout )
        {
        // If clase is only needed by debug version only.
        if ( KErrNotFound != iParentLayout->FindVisual( iButtonLayout ) )
            {
            iParentLayout->Remove( iButtonLayout );
            }
        iParent->Remove( iButtonLayout );
        iParent->Env().TextureManager().RemoveLoadObserver( this );
        delete iButtonLayout;
        }

    delete iBgColorBrush;
    delete iShadowBorderBrush;
    delete iTextFontSpec;
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Initializes visualiser with model.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::InitializeL(
    CFsControlButtonModel& aButtonModel )
    {
    FUNC_LOG;
    iButtonModel = &aButtonModel;

    // Add background color brush. This cannot be done in ConstructL as the
    // button model isn't set before.
    if ( !iBgColorBrush )
        {
        iBgColorBrush = CAlfGradientBrush::NewL( iParent->Env() );
        if( iShowShadow )
            {
            iBgColorBrush->SetOpacity( KShadowBorderOpacity ); 
            iBgColorBrush->SetColor(KRgbBlack);
            }
        else
            {
            iBgColorBrush->SetOpacity( 0 );
            }

        iBgColorBrush->SetLayer( EAlfBrushLayerBackground );
        iButtonLayout->Brushes()->AppendL( iBgColorBrush,
										   EAlfDoesNotHaveOwnership );
        }

    iUseDefaultBackground = ETrue;
    
    // Add a shadow border brush to outline the buttons with a shadowed border.
    if ( iShowShadow && !iShadowBorderBrush )
    	{
    	iShadowBorderBrush = CAlfShadowBorderBrush::NewL( 
    		iParent->Env(), TAlfMetric( KDefaultShadowBorderWidth ) );
    	iShadowBorderBrush->SetOpacity( KShadowBorderOpacity );
    	iShadowBorderBrush->SetLayer( EAlfBrushLayerBackground );
    	iButtonLayout->Brushes()->AppendL( iShadowBorderBrush,
										   EAlfDoesNotHaveOwnership );
    	}

    UpdateVisualThemeL();
    
    // create needed visuals
    CreateButtonVisualsL();

    UpdateBarLayout();
    }

// ---------------------------------------------------------------------------
// CFsControlButtonVisualiser::UpdateVisualThemeL
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::UpdateVisualThemeL()
    {
    if( iUseDefaultBackground )
        {
        ClearBackgroundImage();
        iDefaultBgBrush = CAlfFrameBrush::NewL( iParent->Env(), KAknsIIDQsnFrButtonTb );  
        iBgBrush = iDefaultBgBrush;
        iButtonLayout->Brushes()->AppendL( iBgBrush, EAlfDoesNotHaveOwnership );
        UpdateBarLayout();
        }
    else if ( iBgColorBrush )
        {
        SetBackgroundColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonBackground ) );
        }
    }

// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Refreshes button's content.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::Refresh()
    {
    FUNC_LOG;
    // draw only if enabled and ready for drawing
    if ( !IsVisible() )
        {
        return;
        }

    TBool buttonSizeChanged( iButtonModel->ButtonSizeChanged() );

    if ( buttonSizeChanged ||
        iButtonModel->AutoSizeMode()
            == MFsControlButtonInterface::EFsFitToContent ||
        iButtonModel->AutoSizeMode()
            == MFsControlButtonInterface::EFsFitToParent )
        {
        TSize buttonSize( iButtonModel->Size() );
        UpdateButtonSize();
        buttonSizeChanged |= buttonSize != iButtonModel->Size();
        }

    if ( buttonSizeChanged )
        {
        UpdateBarLayout();
        return;
        }

    if ( iButtonModel->ButtonPosChanged() )
        {
        UpdateButtonPos();
        if ( !buttonSizeChanged )
            {
            UpdateBarLayout();
            return;
            }
        }

    // recalculate icons alignement if needed
    if ( iUpdateIconsAlign || iFirstDraw )
        {
        if ( iButtonModel->ContainsElement( ECBElemIconA ) )
            {
            SetImageAlign( iIconA, iAIconHAlign, iAIconVAlign );
            }

        if ( iButtonModel->ContainsElement( ECBElemIconB ) )
            {
            SetImageAlign( iIconB, iBIconHAlign, iBIconVAlign );
            }

        iUpdateIconsAlign = EFalse;
        iFirstDraw = EFalse;
        }

    // if dimmed
    if ( iButtonModel->IsDimmed() )
        {
        PrepareDrawDimmed();
        }
    // if focused
    else if( iButtonModel->IsFocused() && iDrawFocus )
        {
        PrepareDrawFocused();
        }
    // normal - no focus, enabled
    else
        {
        PrepareDrawNormal();
        }
    }

// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Refreshes button's content, sets topleft point of button in specified
// place.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::Refresh( TPoint aStartAt )
    {
    FUNC_LOG;
    if ( iButtonModel->AutoPosition() )
        {
        TAlfRealPoint pos( aStartAt );
        if ( CFsLayoutManager::IsMirrored() )
            {
            TInt width( iParentLayout->Size().Target().iX );
            pos.iX = width - aStartAt.iX - iButtonLayout->Size().Target().iX;
            }

        // Set new position for background visual and it's content.
        iButtonLayout->SetPos( pos );
        iButtonContentLayout->SetPos( pos );
        }

    Refresh();
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Retrieves end X coordinate of the button.
// ---------------------------------------------------------------------------
//
TInt CFsControlButtonVisualiser::GetBottomRight() const
    {
    FUNC_LOG;
    return
        iButtonModel->TopLeftPoint().iX + iButtonLayout->Size().iX.Target();
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Retrieves position of the button.
// ---------------------------------------------------------------------------
//
const TAlfTimedPoint CFsControlButtonVisualiser::Pos() const
    {
    FUNC_LOG;
    return iButtonLayout->Pos();
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Retrieves size of the button.
// ---------------------------------------------------------------------------
//
const TAlfTimedPoint CFsControlButtonVisualiser::Size() const
    {
    FUNC_LOG;
    return iButtonLayout->Size();
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Sets background image of the button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetBackgroundImageL(
    CAlfImageBrush* aImage )
    {
    FUNC_LOG;
    // Release the old background image.
    ClearBackgroundImage();

    // As the ownership of the new brush is gained the pointer can't be lost.
    CleanupStack::PushL( aImage );
    iButtonLayout->Brushes()->InsertL( 0, aImage, EAlfDoesNotHaveOwnership );
    CleanupStack::Pop( aImage );

    iUseDefaultBackground = EFalse;
    iBgBrush = aImage;
    }


// ---------------------------------------------------------------------------
// Sets background color for button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetBackgroundColor(
    const TRgb& aColor )
    {
    FUNC_LOG;
    iBgColorBrush->SetColor( aColor );
    iBgColorBrush->SetOpacity( 1 );
    }


// ---------------------------------------------------------------------------
// Clears button's background color. Button is transparent.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::ClearBackgroundColor()
    {
    FUNC_LOG;
    if( iBgColorBrush )
        {
        iBgColorBrush->SetOpacity( 0 );
        }
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Clears background image of the button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::ClearBackgroundImage()
    {
    FUNC_LOG;
    if ( iBgBrush )
        {
        for ( TInt i( iButtonLayout->Brushes()->Count() - 1 ); 0 <= i; i-- )
            {
            if ( &iButtonLayout->Brushes()->At( i ) == iBgBrush )
                {
                iButtonLayout->Brushes()->Remove( i );
                break;
                }
            }
        delete iBgBrush;
        iBgBrush = NULL;
        iDefaultBgBrush = NULL;
        }
    }


// ---------------------------------------------------------------------------
// From class CFsControlButtonVisualiserBase.
// Sets alignement of element of the button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetElemAlignL(
    TFsControlButtonElem aButtonElem,
    TAlfAlignHorizontal aHAlign,
    TAlfAlignVertical aVAlign )
    {
    FUNC_LOG;
    TFsControlButtonType buttonType( iButtonModel->Type() );

    // check if button contains this element.
    if ( !iButtonModel->ContainsElement( aButtonElem ) )
        {
        FsGenericPanic( EFsControlButtonIncorrectButtonElement );
        User::Leave( KErrNotSupported );
        }

    switch ( aButtonElem )
        {
        case ECBElemIconA:
            {
            iAIconHAlign = aHAlign;
            iAIconVAlign = aVAlign;
            SetImageAlign( iIconA, aHAlign, aVAlign );
            break;
            }

        case ECBElemIconB:
            {
            iBIconHAlign = aHAlign;
            iBIconVAlign = aVAlign;
            SetImageAlign( iIconB, aHAlign, aVAlign );
            break;
            }

        case ECBElemLabelFirstLine:
            {
            SetTextAlign( iLabelFirstLine, aHAlign, aVAlign );
            break;
            }

        case ECBElemLabelSndLine:
            {
            SetTextAlign( iLabelSecondLine, aHAlign, aVAlign );
            break;
            }

        default:
            {
            // no other elements available
            FsGenericPanic( EFsControlButtonIncorrectButtonElement );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// Update elemets after texture loading is completed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::TextureLoadingCompleted(
    CAlfTexture& aTexture,
    TInt /*aTextureId*/,
    TInt aErrorCode )
    {
    FUNC_LOG;
    if ( aErrorCode )
        {
        return;
        }

    // <cmail> Safety check added. Check why it is needed here.
    if ( iIconA &&
            iIconA->Image().HasTexture() &&
            &iIconA->Image().Texture() == &aTexture )
        {
        TRAP_IGNORE( UpdateElementL( ECBElemIconA ) );
        }
    if ( iIconB &&
            iIconB->Image().HasTexture() &&
            &iIconB->Image().Texture() == &aTexture )
        {
        TRAP_IGNORE( UpdateElementL( ECBElemIconB ) );
        }
    // </cmail>
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::MakeFocusVisible( TBool aShow )
    {
    FUNC_LOG;
    if( aShow )
        {
        iDrawFocus = ETrue;
        PrepareDrawFocused();
        }
    else
        {
        iDrawFocus = EFalse;
        PrepareDrawNormal();
        }
    }

// ---------------------------------------------------------------------------
// Updates element content from the model.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::UpdateElementL(
    TFsControlButtonElem aButtonElem )
    {
    FUNC_LOG;
    // check if button contains this element.
    if ( !iButtonModel->ContainsElement( aButtonElem ) )
        {
        FsGenericPanic( EFsControlButtonIncorrectButtonElement );
        User::Leave( KErrNotSupported );
        }

    switch ( aButtonElem )
        {
        case ECBElemIconA:
            {
            iIconA->SetImage(
                TAlfImage( *iButtonModel->Icon( ECBElemIconA ) ) );
            // <cmail> Platform layout change
            //iIconA->SetSize( iIconA->Image().Texture().Size() );
            // </cmail> Platform layout change
            break;
            }

        case ECBElemIconB:
            {
            iIconB->SetImage(
                TAlfImage( *iButtonModel->Icon( ECBElemIconB ) ) );
            // <cmail> Platform layout change
            //iIconB->SetSize( iIconB->Image().Texture().Size() );
            // </cmail> Platform layout change
            break;
            }

        case ECBElemLabelFirstLine:
            {
            TRAP_IGNORE( iLabelFirstLine->SetTextL( iButtonModel->Text(
                MFsControlButtonInterface::EFsButtonFirstLine ) ) );
            break;
            }

        case ECBElemLabelSndLine:
            {
            TRAP_IGNORE( iLabelSecondLine->SetTextL( iButtonModel->Text(
                MFsControlButtonInterface::EFsButtonSecondLine ) ) );
            break;
            }

        default:
            {
            // no other elements available
            FsGenericPanic( EFsControlButtonIncorrectButtonElement );
            break;
            }
        }

    UpdateButtonSize();
    UpdateBarLayout();
    }


// ---------------------------------------------------------------------------
// Retrieves width of the button.
// ---------------------------------------------------------------------------
//
TInt CFsControlButtonVisualiser::Width() const
    {
    FUNC_LOG;
    return iButtonModel->Size().iWidth;
    }


// ---------------------------------------------------------------------------
// Updates size of button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::UpdateButtonSize()
    {
    FUNC_LOG;
    TBool textClipping( ETrue );
    CAlfTextVisual::TLineWrap textWrapping(
        CAlfTextVisual::ELineWrapTruncate );

    const TPoint oldSize( iButtonContentLayout->Size().Target() );

    switch( iButtonModel->AutoSizeMode() )
        {
        case MFsControlButtonInterface::EFsLayout:
            {
            if ( iButtonModel->IsLayoutSize() )
                {
                iButtonModel->SetSize( iButtonModel->GetLayoutSize() );
                }
            break;
            }

        case MFsControlButtonInterface::EFsFitToContent:
            {
            iButtonModel->SetWidth( CalculateButtonSize() );
            textWrapping = CAlfTextVisual::ELineWrapManual;
            textClipping = EFalse;
            break;
            }

        case MFsControlButtonInterface::EFsFitToParent:
            {
            iButtonModel->SetWidth( iParentLayout->Size().Target().iX );
            break;
            }

        case MFsControlButtonInterface::EFsManual:
        default:
            {
            break;
            }
        }

    // New size for the button background.
    iButtonLayout->SetSize( iButtonModel->Size() );
    // Same size for the content
    iButtonContentLayout->SetSize( iButtonModel->Size() );

    if( iDefaultBgBrush )
        {
        TSize size =  iButtonModel->Size();
        TRect fullRect = TRect( size );
        TRect innerRect = fullRect;
        innerRect.Shrink( KButtonBorderSize,KButtonBorderSize );
        TRAP_IGNORE(
                iDefaultBgBrush->SetFrameRectsL( innerRect, fullRect ); );
        }
    
    if ( iLabelFirstLine )
        {
        iLabelFirstLine->SetWrapping( textWrapping );
        iLabelFirstLine->SetClipping( textClipping );
        }

    if ( iLabelSecondLine )
        {
        iLabelSecondLine->SetWrapping( textWrapping );
        iLabelSecondLine->SetClipping( textClipping );
        }

    iButtonLayout->UpdateChildrenLayout();

    TRAP_IGNORE( UpdateElementsSizeL( iButtonModel->Type() ) );

    iButtonContentLayout->UpdateChildrenLayout();
    
    // update text styles if needed (at least one text line)
    const TPoint newSize( iButtonContentLayout->Size().Target() );
    if ( newSize != oldSize )
        {
        iUpdateIconsAlign = ETrue;
        if ( iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
            {
            UpdateTextStyles();
            }
        UpdateBarLayout();
        }
    }


// ---------------------------------------------------------------------------
// Retrieves the background layout of the button.
// ---------------------------------------------------------------------------
//
CAlfLayout* CFsControlButtonVisualiser::Layout()
    {
    FUNC_LOG;
    return iButtonLayout;
    }


// ---------------------------------------------------------------------------
// Retrieves the content layout of the button.
// ---------------------------------------------------------------------------
//
CAlfLayout* CFsControlButtonVisualiser::ContentLayout()
    {
    FUNC_LOG;
    return iButtonContentLayout;
    }


// ---------------------------------------------------------------------------
// Enables (makes visible) button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::ShowL()
    {
    FUNC_LOG;
    UpdateButtonSize();
    UpdateButtonPos();

    if ( KErrNotFound == iParentLayout->FindVisual( iButtonLayout ) )
        {
        User::LeaveIfError( iParentLayout->Append( iButtonLayout ) );
        }
    if ( KErrNotFound == iParentLayout->FindVisual( iButtonContentLayout ) )
        {
        TInt error( iParentLayout->Append( iButtonContentLayout ) );
        if ( error )
            {
            if ( KErrNotFound != iParentLayout->FindVisual( iButtonLayout ) )
                {
                iParentLayout->Remove( iButtonLayout );
                }
            User::Leave( error );
            }
        }
    iVisible = ETrue;
    }


// ---------------------------------------------------------------------------
// Disables (hides) button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::Hide()
    {
    FUNC_LOG;
    if ( !iVisible && iButtonModel->IsFocused() )
        {
        FsGenericPanic( EFsControlButtonCannotDisableFocusedControl );
        }

    TBool refresh( EFalse );

    if ( KErrNotFound != iParentLayout->FindVisual( iButtonLayout ) )
        {
        iParentLayout->Remove( iButtonLayout );
        refresh = ETrue;
        }

    if ( KErrNotFound != iParentLayout->FindVisual( iButtonContentLayout ) )
        {
        iParentLayout->Remove( iButtonContentLayout );
        refresh = ETrue;
        }

    iVisible = EFalse;
    if ( refresh )
        {
        UpdateBarLayout();
        }
    }


// ---------------------------------------------------------------------------
// Checks if button is visible or hidden.
// ---------------------------------------------------------------------------
//
TBool CFsControlButtonVisualiser::IsVisible() const
    {
    FUNC_LOG;
    return iVisible;
    }


// ---------------------------------------------------------------------------
// Changes color of text when button focused.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::PrepareDrawFocused()
    {
    FUNC_LOG;
    if ( iLabelFirstLine )
        {
        TCharFormat charFormat;
        TInt styleId( KFsDefaultFontStyle );

        if ( iTextStyleManager )
            {
            ResolveCharFormat( charFormat, iLabelFirstLine );
            charFormat.iFontPresentation.iUnderline = EUnderlineOff;
            // Use modified style if possible, otherwise use the default
            // style.
            TRAPD( leaveErr,
                styleId = iTextStyleManager->GetStyleIDL( charFormat ) );
            if ( KErrNone == leaveErr )
                {
                iLabelFirstLine->SetTextStyle( styleId );
                }
            }
        iLabelFirstLine->SetColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonFocused ) );
        }

    if ( iLabelSecondLine )
        {
        TCharFormat charFormat;
        TInt styleId( KFsDefaultFontStyle );

        if ( iTextStyleManager )
            {
            ResolveCharFormat( charFormat, iLabelSecondLine );
            charFormat.iFontPresentation.iUnderline = EUnderlineOff;
            TRAPD( leaveErr,
                styleId = iTextStyleManager->GetStyleIDL( charFormat ) );
            // Use modified style if possible, otherwise use the default
            // style.
            if ( KErrNone == leaveErr )
                {
                iLabelSecondLine->SetTextStyle( styleId );
                }
            }
        iLabelSecondLine->SetColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonFocused ) );
        }
    }


// ---------------------------------------------------------------------------
// Changes color of text when button's state is normal.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::PrepareDrawNormal()
    {
    FUNC_LOG;
    TCharFormat charFormat;

    if ( iLabelFirstLine )
        {
        TCharFormat charFormat;
        TInt styleId( KFsDefaultFontStyle );

        if ( iTextStyleManager )
            {
            ResolveCharFormat( charFormat, iLabelFirstLine );
            charFormat.iFontPresentation.iUnderline = EUnderlineOff;
            TRAPD( leaveErr,
                styleId = iTextStyleManager->GetStyleIDL( charFormat ) );
            // Use modified style if possible, otherwise use the default
            // style.
            if ( KErrNone == leaveErr )
                {
                iLabelFirstLine->SetTextStyle( styleId );
                }
            }
        iLabelFirstLine->SetColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonNormal ) );
        }

    if ( iLabelSecondLine )
        {
        TCharFormat charFormat;
        TInt styleId( KFsDefaultFontStyle );

        if ( iTextStyleManager )
            {
            ResolveCharFormat( charFormat, iLabelSecondLine );
            charFormat.iFontPresentation.iUnderline = EUnderlineOff;
            TRAPD( leaveErr,
                styleId = iTextStyleManager->GetStyleIDL( charFormat ) );
            // Use modified style if possible, otherwise use the default
            // style.
            if ( KErrNone == leaveErr )
                {
                iLabelSecondLine->SetTextStyle( styleId );
                }
            }
        iLabelSecondLine->SetColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonNormal ) );
        }

    if ( iIconA )
        {
        iIconA->SetColorMode( CAlfImageVisual::EColorModulate );
        }

    if ( iIconB )
        {
        iIconB->SetColorMode( CAlfImageVisual::EColorModulate );
        }
    }


// ---------------------------------------------------------------------------
// Changes color of text when button's state is dimmed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::PrepareDrawDimmed()
    {
    FUNC_LOG;
    TCharFormat charFormat;

    if ( iLabelFirstLine )
        {
        TCharFormat charFormat;
        TInt styleId( KFsDefaultFontStyle );

        if ( iTextStyleManager )
            {
            ResolveCharFormat( charFormat, iLabelFirstLine );
            charFormat.iFontPresentation.iUnderline = EUnderlineOn;
            TRAPD( leaveErr,
                styleId = iTextStyleManager->GetStyleIDL( charFormat ) );
            // Use modified style if possible, otherwise use the default
            // style.
            if ( KErrNone == leaveErr )
                {
                iLabelFirstLine->SetTextStyle( styleId );
                }
            }
        iLabelFirstLine->SetColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonDimmed ) );
        }

    if ( iLabelSecondLine )
        {
        TCharFormat charFormat;
        TInt styleId( KFsDefaultFontStyle );

        if ( iTextStyleManager )
            {
            ResolveCharFormat( charFormat, iLabelSecondLine );
            charFormat.iFontPresentation.iUnderline = EUnderlineOn;
            TRAPD( leaveErr,
                styleId = iTextStyleManager->GetStyleIDL( charFormat ) );
            // Use modified style if possible, otherwise use the default
            // style.
            if ( KErrNone == leaveErr )
                {
                iLabelSecondLine->SetTextStyle( styleId );
                }
            }
        iLabelSecondLine->SetColor( iButtonModel->TextColor(
            CFsControlButtonModel::EButtonDimmed ) );
        }

    if ( iIconA )
        {
        iIconA->SetColorMode( CAlfImageVisual::EColorDimmed );
        }

    if ( iIconB )
        {
        iIconB->SetColorMode( CAlfImageVisual::EColorDimmed );
        }
    }


// ---------------------------------------------------------------------------
// Creates needed number of rows for layout for specified button type.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::CreateButtonVisualsL()
    {
    FUNC_LOG;
    // create icon A visuals
    if ( iButtonModel->ContainsElement( ECBElemIconA ) )
        {
        iIconAContainer =
            CAlfLayout::AddNewL( *iParent, iButtonContentLayout );
        iIconA = CAlfImageVisual::AddNewL( *iParent, iIconAContainer );
        iIconA->SetScaleMode( CAlfImageVisual::EScaleFitInside );
        iIconAContainer->SetClipping( ETrue );
        }

    // create visual for first line of text
    if ( iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
        {
        iLabelFirstLine =
            CAlfTextVisual::AddNewL( *iParent, iButtonContentLayout );
        iLabelFirstLine->SetAlign( EAlfAlignHLeft, EAlfAlignVCenter );
        iLabelFirstLine->SetPadding(
            TPoint( iButtonModel->LabelHPadding( iButtonModel->Type() ),
                iButtonModel->LabelVPadding( iButtonModel->Type() ) ) );
        }

    // create visual for second line of text
    if ( iButtonModel->ContainsElement( ECBElemLabelSndLine ) )
        {
        iLabelSecondLine =
            CAlfTextVisual::AddNewL( *iParent, iButtonContentLayout );
        iLabelSecondLine->SetAlign( EAlfAlignHLeft, EAlfAlignVCenter );
        iLabelSecondLine->SetPadding(
            TPoint( iButtonModel->LabelHPadding( iButtonModel->Type() ),
                iButtonModel->LabelVPadding( iButtonModel->Type() ) ) );
        }

    // create icon B visuals
    if ( iButtonModel->ContainsElement( ECBElemIconB ) )
        {
        iIconBContainer =
            CAlfLayout::AddNewL( *iParent, iButtonContentLayout );
        iIconB = CAlfImageVisual::AddNewL( *iParent, iIconBContainer );
        iIconB->SetScaleMode( CAlfImageVisual::EScaleNormal );
        iIconBContainer->SetClipping( ETrue );
        }

    // update text styles if needed (at least one text line)
    if ( iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
        {
        UpdateTextStyles();
        }
    }


// ---------------------------------------------------------------------------
// Calculates button size according to content.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsControlButtonVisualiser::CalculateButtonSize()
    {
    FUNC_LOG;

    if ( iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
        {
        // to get correct value from ExpandRectWithContent method
        iLabelFirstLine->SetClipping( EFalse );

        iLabelFirstLine->SetWrapping( CAlfTextVisual::ELineWrapManual );

        // Update the text visual to get correct rect size.
        TSize size( iLabelFirstLine->TextExtents() );
        TAlfRealRect padding( iLabelFirstLine->PaddingInPixels() );
        TRect textRect( 0, 0,
            padding.iTl.iX + padding.iBr.iX + size.iWidth,
            padding.iTl.iY + padding.iBr.iY + size.iHeight );
        
        if ( iButtonModel->ContainsElement( ECBElemLabelSndLine ) )
            {
            // to get correct value from ExpandRectWithContent method
            iLabelSecondLine->SetClipping( EFalse );

            iLabelSecondLine->SetWrapping( CAlfTextVisual::ELineWrapManual );

            // Update the text visual to get correct rect size.
            TSize size2( iLabelFirstLine->TextExtents() );
            TAlfRealRect padding( iLabelSecondLine->PaddingInPixels() );

            TRect secondLineRect( 0, 0,
                padding.iTl.iX + padding.iBr.iX + size2.iWidth,
                padding.iTl.iY + padding.iBr.iY + size2.iHeight );

            if ( secondLineRect.Width() > textRect.Width() )
                {
                //text size
                textRect = secondLineRect;
                }
            }

        // Calculate the difference compared to layout defined object.
        TBool supportedType( ETrue );
        TRect parentRect = CFsControlBar::GetLayoutRect( 2 );
        CFsLayoutManager::TFsText text;
        switch ( iButtonModel->Type() )
            {
            case ECBTypeOneLineLabelIconB:
                CFsLayoutManager::LayoutMetricsText( parentRect,
                    CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1, text, 1 );
                break;
            case ECBTypeOneLineLabelOnly:
                CFsLayoutManager::LayoutMetricsText( parentRect,
                    CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1, text, 0 );
                break;
            case ECBTypeOneLineLabelIconA:
                CFsLayoutManager::LayoutMetricsText( parentRect,
                    CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1, text, 2 );
                break;
            case ECBTypeOneLineLabelTwoIcons:
                CFsLayoutManager::LayoutMetricsText( parentRect,
                    CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1, text, 3 );
                break;
            //case ECBTypeIconOnly: // no text item
            case ECBTypeTwoLinesLabelOnly:
            case ECBTypeTwoLinesLabelIconA:
            case ECBTypeTwoLinesLabelIconB:
            case ECBTypeTwoLinesLabelTwoIcons:
            default:
                supportedType = EFalse;
                break;
            }

        if ( supportedType )
            {
            const TInt newWidth(
                parentRect.Width() + textRect.Width() - text.iTextRect.Width()
                );
            return newWidth;
            }
        }

    if ( iButtonModel->ContainsElement( ECBElemIconA ) )
        {
        return iIconAContainer->Size().Target().iX;
        }

    if ( iButtonModel->ContainsElement( ECBElemIconB ) )
        {
        return iIconBContainer->Size().Target().iX;
        }

    return iButtonContentLayout->Size().Target().iX;
    }


// ---------------------------------------------------------------------------
// Updates position of button.
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::UpdateButtonPos()
    {
    FUNC_LOG;
    // Set position for button background and for the content.
    iButtonLayout->SetPos( iButtonModel->TopLeftPoint() );
    iButtonContentLayout->SetPos( iButtonModel->TopLeftPoint() );
    }


// ---------------------------------------------------------------------------
// Updates size of columns in grid layouter for button's elements.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::UpdateElementsSizeL(
    TFsControlButtonType aButtonType )
    {
    FUNC_LOG;
    TRect parentRect( TPoint( 0, 0 ), iButtonModel->Size() );
    TRect layoutRect( 0, 0, 0, 0 );
    TRect iconA;
    TRect iconB;
    TRect text1;
    TRect text2;
    CFsLayoutManager::TFsText text;
    switch ( aButtonType )
        {
        case ECBTypeIconOnly:
            iconA = iButtonModel->GetLayoutSize( parentRect );
            // Need to move the rect as icon doesn't have a separate button
            // parent. It's position is related to controlbar's rect.
            iconA.Move( -iconA.iTl.iX, -iconA.iTl.iY );
            break;
        case ECBTypeOneLineLabelOnly:
            CFsLayoutManager::LayoutMetricsText( parentRect,
                CFsLayoutManager::ECmailDdmenuBtn01PaneT1, text, 1 );
            text1 = text.iTextRect;
            break;
        case ECBTypeOneLineLabelIconA:
            if ( Layout_Meta_Data::IsLandscapeOrientation() )
                {
                CFsLayoutManager::LayoutMetricsRect( parentRect,
                    CFsLayoutManager::ECmailDdmenuBtn02PaneG2,
                    iconA, 0 );
                CFsLayoutManager::LayoutMetricsText( parentRect,
                    CFsLayoutManager::ECmailDdmenuBtn02PaneT2, 
                    text, 0 );
                }
            else                
                {
                CFsLayoutManager::LayoutMetricsRect( parentRect,
                    CFsLayoutManager::ECmailDdmenuBtn01PaneG1,
                    iconA, 1 );
                CFsLayoutManager::LayoutMetricsText( parentRect,
                    CFsLayoutManager::ECmailDdmenuBtn01PaneT1, 
                    text, 0 );
                }
            text1 = text.iTextRect; 
            break;
        case ECBTypeOneLineLabelIconB:
            CFsLayoutManager::LayoutMetricsText( parentRect,
                CFsLayoutManager::ECmailDdmenuBtn01PaneT1, text, 1 );
            text1 = text.iTextRect;
            CFsLayoutManager::LayoutMetricsRect( parentRect,
                CFsLayoutManager::ECmailDdmenuBtn01PaneG2,
                iconB, 2 );
            break;
        case ECBTypeOneLineLabelTwoIcons:
            CFsLayoutManager::LayoutMetricsRect( parentRect,
                CFsLayoutManager::ECmailDdmenuBtn01PaneG1,
                iconA, 1 );
            CFsLayoutManager::LayoutMetricsText( parentRect,
                CFsLayoutManager::ECmailDdmenuBtn01PaneT1, text, 0 );
            text1 = text.iTextRect;
            CFsLayoutManager::LayoutMetricsRect( parentRect,
                CFsLayoutManager::ECmailDdmenuBtn01PaneG2,
                iconB, 1 );
            break;
        default:
            break;
        }
    
    // Buttons have different sizes so adapt the used rectangle dynamically
    TSize buttonSize = iButtonModel->Size();
    buttonSize -= TPoint( KButtonBorderSize, KButtonBorderSize ); // shrink frame border size
    
    // If there is no text in the button, center the icon to the button
    if( !iLabelFirstLine || iLabelFirstLine->Text().Length() == 0 )
        {
        TSize oldIconSize = iconA.Size();
        iconA.SetRect( TPoint( 
                            ( buttonSize.iWidth - oldIconSize.iWidth ) / 2, 
                            ( buttonSize.iHeight - oldIconSize.iHeight ) / 2 ),
                            oldIconSize );
        }
    
    if ( iButtonModel->ContainsElement( ECBElemIconA ) )
        {
        const TSize& size( iconA.Size() );
        iIconAContainer->SetSize( size );
        iIconAContainer->SetPos( iconA.iTl );
        iIconA->SetSize( TAlfRealSize( size ) );
        }
    if ( iButtonModel->ContainsElement( ECBElemIconB ) )
        {
        const TSize& size( iconB.Size() );
        iIconBContainer->SetSize( iconB.Size() );
        iIconBContainer->SetPos( iconB.iTl );
        iIconB->SetSize( TAlfRealSize( size ) );
        }
    if ( iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
        {
		if ( iLabelFirstLine )
			{
			iLabelFirstLine->SetSize( text1.Size() );
			iLabelFirstLine->SetPos( text1.iTl );
			if ( Layout_Meta_Data::IsLandscapeOrientation() )
			    {
                iLabelFirstLine->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
			    }
			else
			    {
                iLabelFirstLine->SetAlign( EAlfAlignHLocale, EAlfAlignVCenter );			
			    }
			}
        }
    if ( iButtonModel->ContainsElement( ECBElemLabelSndLine ) )
        {
        iLabelSecondLine->SetSize( text2.Size() );
        iLabelSecondLine->SetPos( text2.iTl );
        }
    // </cmail> Platform layout changes
    }


// ---------------------------------------------------------------------------
// Sets text styles for specified button type.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::UpdateTextStyles()
    {
    FUNC_LOG;
    TInt style( KFsDefaultFontStyle );
    TBool sizeChanged( EFalse );

    TCharFormat charFormat;
    ResolveCharFormat( charFormat, iLabelFirstLine );

    if ( !iTextHeightSet )
        {
        TRect parentRect(
            TPoint( 0, 0 ), iButtonContentLayout->Size().Target() );
        CFsLayoutManager::TFsText text;
        CFsLayoutManager::LayoutMetricsText( parentRect,
            CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1, text, 1 );

        charFormat.iFontSpec = text.iFont->FontSpecInTwips();
        }
    else
        {
        CWsScreenDevice* screenDev( CCoeEnv::Static()->ScreenDevice() );

        // Modify the height of the font.
        TInt twips( screenDev->VerticalPixelsToTwips( iTextHeight ) );
        if ( 0 < iTextHeight && twips != charFormat.iFontSpec.iHeight )
            {
            charFormat.iFontSpec.iHeight = twips;
            sizeChanged = ETrue;
            }
        }

    if ( iTextStyleManager )
        {
        // Use default style in case of a leave.
        TRAP_IGNORE( style = iTextStyleManager->GetStyleIDL( charFormat ) );
        }

    if ( iLabelFirstLine )
        {
        iLabelFirstLine->SetTextStyle( style );
        }

    if ( iLabelSecondLine )
        {
        iLabelSecondLine->SetTextStyle( style );
        }

    if ( sizeChanged )
        {
        UpdateButtonSize();
        }
    }


// ---------------------------------------------------------------------------
// Sets alignement of specified image of the button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetImageAlign(
    CAlfImageVisual* aImage,
    TAlfAlignHorizontal aHAlign,
    TAlfAlignVertical aVAlign )
    {
    FUNC_LOG;
    TInt iconWidth( aImage->Size().Target().iX );
    TInt iconHeight( aImage->Size().Target().iY );

    TAlfTimedPoint startPoint( 0, 0 );
    TInt layoutWidth;
    TInt layoutHeight;

    if ( aImage == iIconB )
        {
        layoutWidth = iIconBContainer->Size().Target().iX;
        layoutHeight = iIconBContainer->Size().Target().iY;
        }
    else if ( aImage == iIconA )
        {
        layoutWidth = iIconAContainer->Size().Target().iX;
        layoutHeight = iIconAContainer->Size().Target().iY;
        }
    else
        {
        return;
        }

    switch ( aHAlign )
        {
        case EAlfAlignHLeft:
            {
            startPoint.iX = 0;
            break;
            }

        case EAlfAlignHCenter:
            {
            startPoint.iX = layoutWidth / 2 - iconWidth / 2;
            break;
            }

        case EAlfAlignHRight:
            {
            startPoint.iX = layoutWidth - iconWidth;
            break;
            }

        default:
            {
            FsGenericPanic( EFsControlButtonEnumValueNotSupported );
            break;
            }
        }

    switch ( aVAlign )
        {
        case EAlfAlignVTop:
            {
            startPoint.iY = 0;
            break;
            }

        case EAlfAlignVCenter:
            {
            startPoint.iY = layoutHeight / 2 - iconHeight / 2;
            break;
            }

        case EAlfAlignVBottom:
            {
            startPoint.iY = layoutHeight - iconHeight;
            break;
            }

        default:
            {
            FsGenericPanic( EFsControlButtonEnumValueNotSupported );
            break;
            }
        }

    aImage->SetPos( startPoint );
    }


// ---------------------------------------------------------------------------
// Sets alignment of specified text of the button.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetTextAlign(
    CAlfTextVisual* aText,
    TAlfAlignHorizontal aHAlign,
    TAlfAlignVertical aVAlign )
    {
    FUNC_LOG;
    aText->SetAlign( aHAlign, aVAlign );
    }


// ---------------------------------------------------------------------------
// Set new height for the button text.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetTextHeight(
    const TInt aTextHeight )
    {
    FUNC_LOG;
    iTextHeight = aTextHeight;
    // Use default text height if the value is zero or negative.
    iTextHeightSet = ( 0 < iTextHeight );

    // Update text styles if needed (at least one text line)
    if ( iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
        {
        UpdateTextStyles();
        UpdateButtonSize();
        }
    }


// ---------------------------------------------------------------------------
// Change the current font.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlButtonVisualiser::SetTextFontL(
    const TFontSpec& aFontSpec )
    {
    FUNC_LOG;
    if ( !iButtonModel->ContainsElement( ECBElemLabelFirstLine ) )
        {
        User::Leave( KErrNotSupported );
        }

    if ( !iTextFontSpec )
        {
        iTextFontSpec = new( ELeave )TFontSpec();
        }

    *iTextFontSpec = aFontSpec;

    // Update text styles.
    Refresh();
    }


// ---------------------------------------------------------------------------
// Sets parent layout.
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::SetParentLayoutL(
    CAlfDeckLayout* aParentLayout )
    {
    FUNC_LOG;
    iParentLayout = aParentLayout;

    if ( iButtonLayout
        && KErrNotFound == iParentLayout->FindVisual( iButtonLayout ) )
        {
        User::LeaveIfError( iParentLayout->Append( iButtonLayout ) );
        }
    if ( iButtonContentLayout
        && KErrNotFound == iParentLayout->FindVisual( iButtonContentLayout ) )
        {
        User::LeaveIfError( iParentLayout->Append( iButtonContentLayout ) );
        }
    }


// ---------------------------------------------------------------------------
// Sets parent control.
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::SetParentControlL(
    CAlfControl* aParentControl )
    {
    FUNC_LOG;
    // There could be an existing observer available on previously used
    // visualiser.
    if ( iParent )
        {
        iParent->Env().TextureManager().RemoveLoadObserver( this );
        }

    iParent = aParentControl;
    ConstructL();
    }


// ---------------------------------------------------------------------------
// Set text style manager object.
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::SetTextStyleManager(
    CFsTextStyleManager* aTextStyleManager )
    {
    FUNC_LOG;
    iTextStyleManager = aTextStyleManager;
    }


// ---------------------------------------------------------------------------
// Updates bar layout.
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::UpdateBarLayout()
    {
    FUNC_LOG;
    if ( iParentLayout )
        {
        // This flag is cleared when the layout is updated.
        iParentLayout->UpdateChildrenLayout();
        }
    }


// ---------------------------------------------------------------------------
// Resolve the character format from current text visual.
// ---------------------------------------------------------------------------
//
void CFsControlButtonVisualiser::ResolveCharFormat(
    TCharFormat& aCharFormat,
    CAlfTextVisual* aTextVisual )
    {
    FUNC_LOG;
    // Get the current text style by text style id.
    CAlfTextStyle* textStyle(
        aTextVisual->Env().TextStyleManager().TextStyle(
            aTextVisual->Style() ) );

    // Copy the used format.

    if ( iTextFontSpec )
        {
        aCharFormat.iFontSpec = *iTextFontSpec;
        }
    else
        {
        TRect parentRect(
            TPoint( 0, 0 ), iButtonContentLayout->Size().Target() );
        CFsLayoutManager::TFsText text;

        // Specify button's type to get related font.
        TInt variety( 0 );
        switch ( iButtonModel->Type() )
            {
            case ECBTypeOneLineLabelOnly:
                variety = 0;
                break;
            case ECBTypeOneLineLabelIconB:
                variety = 1;
                break;
            case ECBTypeOneLineLabelIconA:
                variety = 2;
                break;
            case ECBTypeOneLineLabelTwoIcons:
                variety = 3;
                break;
            default:
                break;
            }
        CFsLayoutManager::LayoutMetricsText( parentRect,
            CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1, text, variety );

        aCharFormat.iFontSpec = text.iFont->FontSpecInTwips();
        //alf migration - Might need iFontStyle to be set.
        }
    aCharFormat.iFontPresentation.iStrikethrough =
        textStyle->IsStrikeThrough() ? EStrikethroughOn : EStrikethroughOff;
    aCharFormat.iFontPresentation.iUnderline =
        textStyle->IsUnderline() ? EUnderlineOn : EUnderlineOff;
    }
