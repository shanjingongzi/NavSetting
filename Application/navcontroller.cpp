#include "navcontroller.h"
#include "navsettingview.h"

NavController::NavController(QWidget *parent) {
  view = new NavSettingView(parent);
}
void NavController::Initialize() { view->Initialize(); }

QWidget *NavController::View() { return view; }