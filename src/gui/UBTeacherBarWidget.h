#ifndef UBTEACHERBARWIDGET_H
#define UBTEACHERBARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QTabWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>

#include "UBDockPaletteWidget.h"
#include "customWidgets/UBWidgetList.h"
#include "interfaces/IDropable.h"
#include "UBTeacherBarDataMgr.h"
#include "UBTBDocumentPreviewWidget.h"
#include "UBTBPageEditWidget.h"
#include "UBTeacherBarPreviewWidget.h"
#include "UBTBDocumentEditWidget.h"

#define LABEL_MINWIDHT      80

class UBTeacherBarWidget : public UBDockPaletteWidget
{
    Q_OBJECT
public:
    UBTeacherBarWidget(QWidget* parent=0, const char* name="UBTeacherBarWidget");
    ~UBTeacherBarWidget();

private slots:
    void saveContent();
    void loadContentInfos();
    void onValueChanged();
    void onShowEditMode();
    void onTBStateChanged(eTeacherBarState state);
    void onActiveDocumentChanged();

private:
    void loadContent(bool docChanged = false);
    bool isEmpty();

    QVBoxLayout mLayout;
    QStackedWidget* mpStackWidget;
    eTeacherBarState mState;

    UBTBPageEditWidget* mpPageEditWidget;
    UBTeacherBarPreviewWidget* mpPreview;
    UBTBDocumentPreviewWidget* mpDocPreviewWidget;
    UBTBDocumentEditWidget* mpDocEditWidget;

    UBTeacherBarDataMgr mData;
};

#endif // UBTEACHERBARWIDGET_H
