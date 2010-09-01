/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Field list box model.
 *
*/


#ifndef CFSCFIELDLISTBOXMODEL_H
#define CFSCFIELDLISTBOXMODEL_H

//  INCLUDES
#include <e32base.h>
#include "TFscAppIconId.h"
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CPbk2IconArray;
class CDesC16Array;
class MFscClipListBoxText;
class MFscFieldAnalyzer;
class CFscFieldFormatter;
class CVPbkContactManager;
class CFscPresentationContactField;
class CFscPresentationContactFieldCollection;
class MPbk2FieldPropertyArray;
class MVPbkFieldType;
class CFscContactFieldDynamicProperties;

// CLASS DECLARATION

/**
 * Phonebook 2 list box model row.
 *
 * @internal Only Phonebook 2 internal use supported!
 */
NONSHARABLE_CLASS(CFscFieldListBoxRow) : public CBase
    {
public: // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CFscFieldListBoxRow* NewL();

    /**
     * Destructor.
     */
    ~CFscFieldListBoxRow();

public: // Interface

    /**
     * Returns the number of columns in this row.
     *
     * @return  Number of columns in this row.
     */
    TInt ColumnCount() const;

    /**
     * Returns the descriptor at the given position.
     *
     * @param aColumnIndex  Column index.
     * @return  The descriptor at the given index.
     */
    TPtrC At(
            TInt aColumnIndex ) const;

    /**
     * Appends a given column to the row.
     *
     * @param aColumnText   Column to append.
     */
    void AppendColumnL(
            const TDesC& aColumnText );

    /**
     * Returns the total descriptor length of all the columns.
     *
     * @return  Total row length.
     */
    TInt TotalLength() const;

    /**
     * Returns the maximum column length of this row.
     *
     * @return  Maximum column length.
     */
    TInt MaxColumnLength() const;

private: // Implementation
    CFscFieldListBoxRow();

private: // Data
    /// Own: Column array
    RPointerArray<HBufC> iColumns;
    };

/**
 * Phonebook 2 field list box model.
 *
 * @internal Only Phonebook 2 internal use supported!
 */
NONSHARABLE_CLASS(CFscFieldListBoxModel) : public CBase,
public MDesCArray
    {
public: // Types

    /**
     * Column indexes.
     */
    enum TColumnIndex
        {
        /// Icon column
        EIconColumn = 0,
        /// Label column
        ELabelColumn,
        /// Content column
        EContentColumn
        };

public: // Data structures
    /**
     * Construction parameters for CFscFieldListBoxModel.
     */
    class TParams
        {
    public: // Construction
        /**
         * Constructor.
         *
         * @param aFieldCollection      Array of fields to display.
         * @param aContactManager       Virtual Phonebook
         *                              contact manager.
         * @param aFieldProperties      Field properties.
         * @param aTimeFormat           Time formatting string
         *                              to use.
         * @param aIconArray            Icon array to use.
         * @param aFieldAnalyzer        Interface for
         *                              analyzing fields.
         * @param aDynamicProperties    Dynamic field properties.
         */
        TParams (
                const CFscPresentationContactFieldCollection&
                aFieldCollection,
                const CVPbkContactManager& aContactManager,
                const MPbk2FieldPropertyArray& aFieldProperties,
                const TDesC& aTimeFormat,
                const CPbk2IconArray& aIconArray,
                const MFscFieldAnalyzer* aFieldAnalyzer,
                const CFscContactFieldDynamicProperties* aDynamicProperties ):
        iFields( aFieldCollection ),
        iContactManager( aContactManager ),
        iFieldProperties( aFieldProperties ),
        iTimeFormat( aTimeFormat ),
        iIconArray( aIconArray ),
        iDefaultIconId( EPbk2qgn_prop_nrtyp_empty ),
        iFieldAnalyzer( aFieldAnalyzer ),
        iDynamicProperties( aDynamicProperties )
            {
            }

    public: // Data
        /// Ref: Array of fields to display
        const CFscPresentationContactFieldCollection& iFields;
        /// Ref: Virtual Phonebook contact manager
        const CVPbkContactManager& iContactManager;
        /// Ref: Field properties
        const MPbk2FieldPropertyArray& iFieldProperties;
        /// Ref: Time formatting string
        const TDesC& iTimeFormat;
        /// Ref: Icon array
        const CPbk2IconArray& iIconArray;
        /// Own: Default icon id
        TFscAppIconId iDefaultIconId;
        /// Ref: Interface for analyzing field
        const MFscFieldAnalyzer* iFieldAnalyzer;
        /// Own: Dynamic properties for contact fields
        const CFscContactFieldDynamicProperties* iDynamicProperties;
        };

public: // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @param aParams   List box model parameters.
     * @return  A new instance of this class.
     */
    static CFscFieldListBoxModel* NewL(
            TParams& aParams );

    /**
     * Destructor.
     */
    ~CFscFieldListBoxModel();

public: // Interface

    /**
     * Appends a given row to the end of the model.
     *
     * @param aLine     The row to append.
     */
    void AppendRowL(
            CFscFieldListBoxRow* aLine );

    /**
     * Sets the text clipper to given clipper.
     *
     * @param aTextClipper  The clipper to set.
     */
    void SetClipper(
            MFscClipListBoxText& aTextClipper );

    /**
     * Formats fields. Needs to be call after construction.
     */
    void FormatFieldsL();

public: // From MDesCArray
    TInt MdcaCount() const;
    TPtrC16 MdcaPoint(
            TInt aIndex ) const;

private: // Implementation
    CFscFieldListBoxModel(
            TParams& aParams );
    void ConstructL(
            TParams& aParams );
    void FormatFieldL(
            const CFscPresentationContactField& aField,
            TParams& aParams );
    void ExpandBuffersL(
            const CFscFieldListBoxRow& aRow );
    TPtr ExpandColumnBufferL(
            TInt aRequiredLength );
    void ClipTextL(
            const CFscPresentationContactField& aField,
            TInt aIndex,
            TPtr& aColumnText,
            TInt aColumnIndex ) const;
    void AppendFieldLabelL(
            const CFscPresentationContactField& aField,
            CFscFieldListBoxRow& aRow,
            TPtr& aColumnBuf );
    void AppendFieldContentL(
            const CFscPresentationContactField& aField,
            const MVPbkFieldType& aFieldType,
            CFscFieldListBoxRow& aRow,
            TPtr& aColumnBuf );
    void AppendIconsToBeginningL(
            const CFscPresentationContactField& aField,
            const MVPbkFieldType& aFieldType,
            CFscFieldListBoxRow& aRow,
            TParams& aParams );
    void AppendIconsToEndL(
            const CFscPresentationContactField& aField,
            CFscFieldListBoxRow& aRow,
            TParams& aParams );

private: // Data
    /// Own: Text row array
    CArrayPtrFlat<CFscFieldListBoxRow> iRows;
    /// Own: Buffer storing one formatted listbox row
    HBufC* iLineBuf;
    /// Own: Buffer for storing one formatted listbox column
    HBufC* iColumnBuf;
    /// Own: Field content formatter
    CFscFieldFormatter* iFieldFormatter;
    /// Ref: Object for clipping text
    MFscClipListBoxText* iTextClipper;
    /// Own: Dynamic field properties
    const CFscContactFieldDynamicProperties* iDynamicProperties;
    /// Own: Parameters
    TParams iParams;
    };

#endif // CFscFieldListBoxModel_H
// End of File
