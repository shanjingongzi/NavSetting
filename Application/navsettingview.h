#ifndef NAVSETTINGVIEW_H
#define NAVSETTINGVIEW_H
#include <QVBoxLayout>
#include <QWidget>
#include <qtmetamacros.h>
#include <qwidget.h>

class NavSettingView : public QWidget
{
    Q_OBJECT
public:
    NavSettingView(QWidget* parent = nullptr);
    void Initialize();
    void InitializeTopBar();
    void InitializeChannelPanel();
    void InitializeBottomBar();
    void InitializeAdSettingPanel();
signals:
    void Open(const QString& name);
    void Close();

private:
    QVBoxLayout* layout;
};

#endif   // NAVSETTINGVIEW_H
