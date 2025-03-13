#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVector>

class MultiselectComboBox : public QComboBox {
  Q_OBJECT

public:
  static const size_t BUTTONS_COUNT = 2;

  explicit MultiselectComboBox(QWidget *aParent = Q_NULLPTR);
  ~MultiselectComboBox();

  /**
   * \brief addItem Add a checkable
   * \param text the text to be displayed
   * \param checked (default = false) is the box checked by default
   **/
  void addItem(const QString &text, bool checked = false);

  void addItems(const QStringList &texts);

  void setPlaceHolderText(const QString &placeholderText);
  void hidePopup() override;

  QVector<size_t> getSelectedIds();
  void setSelectedIds(QVector<size_t> ids);

  QStringList currentText() const;

  int count() const;

  bool isTextInWidgets(QString &str);

signals:
  void selectionChanged();

public slots:
  void clear();
  void selectAll();
  void resetSelection();
  void setCurrentText(const QString &menuLabel);
  void setCurrentText(const QStringList &menuLabels);

protected:
  void wheelEvent(QWheelEvent *aWheelEvent) override;
  bool eventFilter(QObject *aObject, QEvent *aEvent) override;
  void keyPressEvent(QKeyEvent *aEvent) override;

private:
  void stateChanged(int aState);
  void itemClicked(int aIndex);

  bool _isHidden;

  QListWidget *_widgets;
  QLineEdit *_lineEdits;
};
