#ifndef CT_GENERICCONFIGURABLEWIDGET_H
#define CT_GENERICCONFIGURABLEWIDGET_H

#include "ct_view/ct_abstractconfigurablewidget.h"

#include <QGridLayout>
#include <QToolTip>

#include "ct_view/ct_filechoicebutton.h"
#include "ct_view/ct_asciifilechoicebutton.h"

class CT_WidgetWithValueReferenceInterface;
class CT_ButtonGroup;
class CT_ComboBox;
class CT_GroupBox;
class CT_SpinBox;
class CT_DoubleSpinBox;
class CT_CheckBox;
class CT_LineEdit;
class CT_MultiLineEdit;

/**
 * @brief This is a simple and generic configurable widget. It was a widget with a gridlayout that can be configured
 *        simply. Each line contains first a label to display a short information, the second column contains the
 *        spinBox or checkbox or combobox or etc... and the third column contains a secondary label that can be per example
 *        the unit. Optionnaly a icon with a tooltip was inserted in the last column if you set a description for the parameter.
 *
 *        When the user accept the configuration, it will update members of you class directly !
 */
class CTLIBWIDGET_EXPORT CT_GenericConfigurableWidget : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:
    CT_GenericConfigurableWidget(QWidget *parent = nullptr);
    ~CT_GenericConfigurableWidget();

    /**
     * @brief Returns true if nothing was added to this widget
     */
    bool isEmpty() const;

    /**
     * @brief Create and add a new group box that can contains grouped elements
     * @param title : the title that will be set to the group box
     * @return the group box created that you must use to add other elements
     */
    virtual CT_GroupBox* addNewGroupBox(const QString& title);

    /**
     * @brief Add a new line that display three labels.
     *
     * @param firstColumnLabelText : set the text to display on the first column. If the second and third label is empty, the first label will be extended on three columns.
     * @param secondColumnLabelText : set the text to display on the second column. If the third label is empty, the second label will be extended on two columns.
     * @param thirdColumnLabelText : set the text to display on the third column.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return true if add is successful
     */
    bool addText(const QString &firstColumnLabelText,
                 const QString &secondColumnLabelText = "",
                 const QString &thirdColumnLabelText = "",
                 const QString &description = "");

    /**
     * @brief Shortcut to addText with second and third column empty
     * @param text : text that was extended on three columns
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return true if add is successful
     */
    bool addTitle(const QString &text,
                  const QString &description = "");


    /**
     * @brief Shortcut to add a separation line
     */
    void addSeparationLine();


    /**
     * @brief Add an int parameter input to a new line in the dialog
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QSpinBox is added in the second column for the user to enter the int value.\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.
     *
     * @param beforeLabelText : text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : text to show in the third column. Generally the unit of the value.
     * @param minValue : minimum allowed value
     * @param maxValue : maximum allowed value
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return nullptr if an error occured
     */
    CT_SpinBox* addInt(const QString &beforeLabelText,
                const QString &afterLabelText,
                const int &minValue,
                const int &maxValue,
                int &valueToUpdate,
                const QString &description = "");

    /**
     * @brief Add a double parameter input to a new line in the dialog
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QDoubleSpinBox is added in the second column for the user to enter the double value.\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.\n\n
     * Note : The output variable could be in one unit, and the user can give it in another one.
     * For example if the variable is in meter, and you wan't the dialog to be in centimeters, use
     * a <b>multValue</b> of 100. The conversion is automatic.
     *
     * @param beforeLabelText : Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : Text to show in the third column. Generally the unit of the value.
     * @param minValue : Minimum allowed value.
     * @param maxValue : Maximum allowed value.
     * @param nDecimals : Number of decimals allowed for the value.
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param multValue : (optionnal) Multiplicative factor to the value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return nullptr if an error occured
     */
    CT_DoubleSpinBox* addDouble(const QString &beforeLabelText,
                   const QString &afterLabelText,
                   const double &minValue,
                   const double &maxValue,
                   const int &nDecimals,
                   double &valueToUpdate,
                   const double &multValue = 1,
                   const QString &description = "");

    /**
     * @brief Add a bool parameter input to a new line in the dialog
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QCheckBox is added in the second column for the user to enter the bool value.\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.
     *
     * @param beforeLabelText Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText Text to show in the third column. Generally the unit of the value.
     * @param checkBoxText Text to show at the right of the checkBox, in the second column.
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return nullptr if an error occured
     */
    CT_CheckBox* addBool(const QString &beforeLabelText,
                 const QString &afterLabelText,
                 const QString &checkBoxText,
                 bool &valueToUpdate,
                 const QString &description = "");


    /**
     * @brief Add a string parameter input to a new line in the dialog (one line = QLineEdit)
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QLineEdit is added in the second column for the user to enter the QString value.\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.
     *
     * @param beforeLabelText : Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : Text to show in the third column. Generally the unit of the value.
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return nullptr if an error occured
     */
    CT_LineEdit* addString(const QString &beforeLabelText,
                   const QString &afterLabelText,
                   QString &valueToUpdate,
                   const QString &description = "");

    /**
     * @brief Add a string parameter input to a new line in the dialog (multi line)
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QLineEdit is added in the second column for the user to enter the QString value.\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.
     *
     * @param beforeLabelText : Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : Text to show in the third column. Generally the unit of the value.
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return nullptr if an error occured
     */
    CT_MultiLineEdit* addMultiString(const QString &beforeLabelText,
                        const QString &afterLabelText,
                        QString &valueToUpdate,
                        const QString &description = "");


    /**
     * @brief Add a list of choice for a parameter to a new line in the dialog
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QComboBox is added in the second column for the user to enter the choosen value.\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.
     *
     * @param beforeLabelText : Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : Text to show in the third column. Generally the unit of the value.
     * @param valuesList : List of QString (QStringList) allowed values for the parameter.
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return the combobox added if successful, nullptr otherwise.
     */
    CT_ComboBox* addStringChoice(const QString &beforeLabelText,
                                 const QString &afterLabelText,
                                 const QStringList &valuesList,
                                 QString &valueToUpdate,
                                 const QString &description = "");

    /**
     * @brief Create a CT_ButtonGroup associated to an index value
     *
     * This method don't add any graphic component to dialog.
     * It only manage the mutual exclusion of associated exclude Values.
     * The <b>value</b> parameter store the selected case index.
     * The index corresponding to each case is specified with corresponding addExcludeValue() call.
     *
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @return the button group added.
     */
    CT_ButtonGroup& addButtonGroup(int &valueToUpdate);

    /**
     * @brief Add an exclude value to a new line in the dialog
     *
     * This has to be used in combination with "addButtonGroup" method.
     * You should add one exclude value for each case.
     * All of these exclusives values have to be in the same CT_ButtonGroup,
     * to ensure that only one can be select at one time.
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * The <b>beforeLabelText</b> is added as a QLabel in the first column.\n
     * A QRadioButton is added in the second column for the user to choose a case\n
     * The <b>afterLabelText</b> is added as a QLabel in the third column.
     *
     * @param beforeLabelText : Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : Text to show in the third column. Generally the unit of the value.
     * @param radioButtonText : Text to show at the right of the radioButton, in the second column.
     * @param group : Group of exclusives values, it will be modified to add this radio button
     * @param id : Id of this case
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @return true if add is successful
     */
    bool addExcludeValue(const QString &beforeLabelText,
                         const QString &afterLabelText,
                         const QString &radioButtonText,
                         CT_ButtonGroup &group,
                         const int &id,
                         const QString &description = "", bool leftRadio = false);

    /**
     * @brief Add a new empty line in the dialog. For separating differents parts in configurations dialog.
     */
    void addEmpty();

    /** @brief Add a control allowing to choose a file to a new line in the dialog
     *
     * The following graphic components are automatically added to the next line of the dialog :\n
     * A QPushButton, with label <b>btLabel</b>. On click it open a choose file dialog.\n
     * A QLabel
     *
     * @param btLabel : Label of the button.
     * @param filetype : Type of FileChooseDialog needed :\n
     * - CT_FileChoiceButton::OneExistingFile : to select only one an existing file.\n
     * - CT_FileChoiceButton::OneOrMoreExistingFiles : to select one or multiple an existing file(s).\n
     * - CT_FileChoiceButton::OneNewFile : to select a new file to create.\n
     * - CT_FileChoiceButton::OneExistingFolder : to select an existing folder.\n
     *
     * @param[out] valueToUpdate : member of your class that will be updated. Note that it's also used to fix input default value.
     * @param description : (optionnal) set a description. It will insert a icon with a tooltip to show description of this parameter.
     * @param beforeLabelText : Text to show in the first column. Generally the asking text for the parameter.
     * @param afterLabelText : Text to show in the third column.
     * @return the button added if successful, nullptr otherwise.
     */
    CT_FileChoiceButton* addFileChoice(const QString &btLabel,
                                       const CT_FileChoiceButton::NeededFileType &filetype,
                                       const QString &fileFilter,
                                       QStringList &valueToUpdate,
                                       const QString &description = "",
                                       const QString &beforeLabelText = "",
                                       const QString &afterLabelText = "");


    /*! \brief Add a control allowing to choose an ASCII file to a new line in the dialog
     *
     * The following graphic composants are automatically added to the next line of the dialog :\n
     * A QPushButton, with label <b>btLabel</b>. On click it open a choose file dialog.\n
     * A QLabel
     *
     * \param btLabel Label of the button.
     * \param fileFilter File filter.
     * \param autoDetect Fields separator automatic detection.
     * \param neededFields List of needed fields from the ascii file.
     *
     * \param[out] fileName Choosen file name. Note that it's also used to fix input default value.
     * \param[out] header Does the file have a header. Note that it's also used to fix input default value.
     * \param[out] separator Fields separator. Note that it's also used to fix input default value.
     * \param[out] decimal Decimal separator. Note that it's also used to fix input default value.
     * \param[out] locale Locale corresponding to decimal separator. Note that it's also used to fix input default value.
     * \param[out] skip Number of lines to skip before fields header line. Note that it's also used to fix input default value.
     * \param[out] columns correspondance between needed fields and corresponding columns numbers in the file. Note that it's also used to fix input default value.
     * \param[out] description Description (for the script export). Note that it's also used to fix input default value.
     */
    CT_AsciiFileChoiceButton *addAsciiFileChoice(QString btlab,
                                                 QString fileFilter,
                                                 bool autoDetect,
                                                 const QStringList &neededFields,
                                                 QString &fileName,
                                                 bool &header,
                                                 QString &separator,
                                                 QString &decimal,
                                                 QLocale &locale,
                                                 int &skip,
                                                 QMap<QString, int> &columns,
                                                 QString description = "");

    CT_AsciiFileChoiceButton *addAsciiFileChoice(QString btlab,
                                                 QString fileFilter,
                                                 bool autoDetect,
                                                 QList<CT_TextFileConfigurationFields> &neededFields,
                                                 QString &fileName,
                                                 bool &header,
                                                 QString &separator,
                                                 QString &decimal,
                                                 QLocale &locale,
                                                 int &skip,
                                                 QMap<QString, int> &columns,
                                                 QString description = "");

    /**
     * @brief Save all values of this widget that must be
     */
    virtual void saveSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief Restore all values that must be
     */
    virtual bool restoreSettings(SettingsReaderInterface& reader);

    /**
     * @brief Returns true if user has modified something.
     */
    bool isSettingsModified() const;

    /**
     * @brief Returns help text for dialog.
     */
    QString helpText(bool addDescriptions);


private:
    QGridLayout                                     *m_gridLayout;
    bool                                            m_canEdit;
    int                                             m_nRow;
    QList<CT_WidgetWithValueReferenceInterface*>    m_ctWidget;
    QList<CT_GroupBox*>                             m_groupBoxCollection;
    QWidget*                                        m_centralWidget;

    QList<CT_WidgetWithValueReferenceInterface*>            m_widgetList;
    QList<QString>                                          m_paramLab;
    QList<QString>                                          m_paramDesc;


    QString formatToUpdateValue(CT_WidgetWithValueReferenceInterface* widget, QVariant value);

protected:

    class HelpButton : public QLabel
    {
    public:
        HelpButton(QWidget *parent = nullptr) : QLabel(parent) {}

    protected:
        void enterEvent(QEvent *event) {
            QLabel::enterEvent(event);
            QToolTip::showText(mapToGlobal(QPoint(0,0)), toolTip(), this, rect());
        }
    };

    CT_GenericConfigurableWidget(bool useScrollArea, QWidget *parent = nullptr);

    /**
     * @brief Return the list of widgets added to this object
     */
    QList<CT_WidgetWithValueReferenceInterface*> allWidgets() const;

    /**
     * @brief Returns the layout
     */
    QGridLayout* getLayout() const;

    /**
     * @brief Add a widget to the layout
     * @param row : the row index
     * @param column : the column index
     * @param wid : the widget
     * @param rowSpan : number of row to span
     * @param colSpan : number of column to span
     */
    void addWidget(int row, int column, QWidget *wid, int rowSpan = 1, int colSpan = 1);

    /**
     * @brief Add a label to the layout
     * @param row : the row index
     * @param column : the column index
     * @param text : the text to set to the label
     */
    void addLabel(int row, int column, const QString &text, int rowSpan = 1, int colSpan = 1);

    /**
     * @brief Add a help button to show the description
     * @param row : the row index
     * @param description : the description to show
     * @param column : (optionnal) the column index, by default = 3
     */
    void addDescription(int row, const QString &description, int column = 3);

    /**
     * @brief Returns true if edit of this widget is authorized (can add new element ?)
     */
    bool canEdit() const;

    /**
     * @brief Returns the number of rows set to the layout
     */
    int nRows() const;

    /**
     * @brief Call it after filled a line in the layout to add a new row
     */
    void addRow();

    /**
     * @brief Call it when you want to disable edit of this widget (add new elements). By default called in show event
     */
    void endEdit();

    void showEvent(QShowEvent *e) {
        endEdit();
        CT_AbstractConfigurableWidget::showEvent(e);
    }

public slots:
    /**
     * @brief Update the element to match the configuration defined by the user
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of nullptr
     * @return true if update was successfull, otherwise returns false and filled the "err" parameter if it was not nullptr
     */
    bool updateElement(QString *err = nullptr);
};

#endif // CT_GENERICCONFIGURABLEWIDGET_H
