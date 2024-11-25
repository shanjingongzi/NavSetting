#ifndef NAVCONTROLLER_H
#define NAVCONTROLLER_H
#include <QObject>
#include <qwidget.h>

#include "navsettingmodel.h"
#include "navsettingview.h"

class NavController : public QObject {
public:
  NavController(QWidget *parent = nullptr);
  void Initialize();
  QWidget *View();

private:
  NavSettingView *view;
};

#endif // NAVCONTROLLER_H
