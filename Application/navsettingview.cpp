#include "navsettingview.h"
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSpinBox>
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qserialport.h>
#include <qserialportinfo.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>

NavSettingView::NavSettingView(QWidget* parent)
    : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
}

void NavSettingView::Initialize()
{
    InitializeTopBar();
    InitializeChannelPanel();
    InitializeAdSettingPanel();
    InitializeBottomBar();
}
void NavSettingView::InitializeTopBar()
{
    QComboBox* serialPortGroup = new QComboBox(this);
    QPushButton* checkBtn      = new QPushButton(this);
    auto EnumerateFunc         = [=]() {
        serialPortGroup->clear();
        auto info = QSerialPortInfo::availablePorts();
        for (const auto& iter : info) {
            serialPortGroup->addItem(iter.portName());
        }
    };
    EnumerateFunc();
    connect(checkBtn, &QPushButton::clicked, [=]() { EnumerateFunc(); });
    checkBtn->setText(tr("enumerate"));
    QLabel* lab = new QLabel();
    lab->setText(tr("device"));
    QPushButton* openBtn = new QPushButton(this);
    connect(openBtn, &QPushButton::clicked, [this, serialPortGroup]() {
        auto name = serialPortGroup->currentText();
        if (name.length()) {
            emit Open(name);
        }
    });
    openBtn->setText(tr("open"));
    QPushButton* closeBtn = new QPushButton(this);
    connect(closeBtn, &QPushButton::clicked, [this]() { emit Close(); });
    closeBtn->setText("close");

    QHBoxLayout* toolBarLayout = new QHBoxLayout();
    toolBarLayout->setAlignment(Qt::AlignRight);
    toolBarLayout->addWidget(checkBtn);
    toolBarLayout->addWidget(lab);
    toolBarLayout->addWidget(serialPortGroup);
    toolBarLayout->addWidget(openBtn);
    toolBarLayout->addWidget(closeBtn);
    layout->addLayout(toolBarLayout);
}

void NavSettingView::InitializeChannelPanel()
{
    QTabWidget* container = new QTabWidget(this);
    layout->addWidget(container);

    QWidget* sbusWidget1 = new QWidget(this);
    QWidget* sbusWidget2 = new QWidget(this);

    container->addTab(sbusWidget1, tr("sbus1"));
    container->addTab(sbusWidget2, tr("sbus2"));
}

void NavSettingView::InitializeAdSettingPanel()
{
    QWidget* adSetting           = new QWidget();
    QHBoxLayout* containerLayout = new QHBoxLayout(adSetting);
    adSetting->setWindowTitle(tr("advance setting"));
    layout->addWidget(adSetting);
    auto InitFunc = [this](int index, QVBoxLayout* pageLayout, const QString& name) {
        QHBoxLayout* itemLayout = new QHBoxLayout();
        pageLayout->addLayout(itemLayout);
        QLabel* lab = new QLabel(name);
        itemLayout->addWidget(lab);
        QCheckBox* reverseChkBox = new QCheckBox();
        itemLayout->addWidget(reverseChkBox);
        QSpinBox* min = new QSpinBox(this);
        itemLayout->addWidget(min);
        QSpinBox* mid = new QSpinBox(this);
        itemLayout->addWidget(mid);
        QSpinBox* max = new QSpinBox(this);
        itemLayout->addWidget(max);
    };
    QVBoxLayout* page1Layout = new QVBoxLayout();
    QVBoxLayout* page2Layout = new QVBoxLayout();
    {
        auto InitTitleFunc = [=]() {
            QHBoxLayout* labelLayout = new QHBoxLayout();
            QLabel* lab              = new QLabel(tr("channel"), this);
            labelLayout->addWidget(lab);
            lab = new QLabel(tr("reverse"), this);
            labelLayout->addWidget(lab);
            lab = new QLabel(tr("min"), this);
            labelLayout->addWidget(lab);
            lab = new QLabel(tr("mid"), this);
            labelLayout->addWidget(lab);
            lab = new QLabel(tr("max"), this);
            labelLayout->addWidget(lab);
            return labelLayout;
        };
        page1Layout->addLayout(InitTitleFunc());
        page2Layout->addLayout(InitTitleFunc());
    }
    containerLayout->addLayout(page1Layout);
    containerLayout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
    containerLayout->addLayout(page2Layout);
    for (int i = 0; i < 16; ++i) {
        if (i < 8) {
            InitFunc(i, page1Layout, QString("C%1").arg(i));
        }
        else {
            InitFunc(i, page2Layout, QString("C%1").arg(i));
        }
    }
}

void NavSettingView::InitializeBottomBar()
{
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    layout->addLayout(bottomLayout);
    QPushButton* readConfig = new QPushButton(this);
    readConfig->setText(tr("read config"));
    bottomLayout->addWidget(readConfig);

    QPushButton* saveConfig = new QPushButton(this);
    saveConfig->setText(tr("save config"));
    bottomLayout->addWidget(saveConfig);
    layout->addLayout(bottomLayout);

    bottomLayout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QPushButton* calibration = new QPushButton(this);
    calibration->setText(tr("calibration"));
    bottomLayout->addWidget(calibration);
    layout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
}
