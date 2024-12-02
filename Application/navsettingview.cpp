#include "navsettingview.h"
#include "navsettingmodel.h"
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
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
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qserialport.h>
#include <qserialportinfo.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>


QString GetPhysicalSignalSourceName(int index)
{
    if (index < 17) {
        return QString("AD%1").arg(index);
    }
    else if (index > 28) {
        return QString("SW%1").arg(index - 28);
    }
    else {
        return QString("KEY%1").arg(index - 16);
    }
}
NavSettingView::NavSettingView(int channelNum, QWidget* parent)
    : QWidget(parent)
    , channelNum{channelNum}
{
    layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
}

void NavSettingView::Initialize()
{
    InitializeTopBar();
    InitializeDebug();
    InitializeChannelPanel();
    InitializeAdSettingPanel();
    InitializeBottomBar();
    Connect();
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

void NavSettingView::InitializeDebug()
{
    QHBoxLayout* itemLayout = new QHBoxLayout();
    itemLayout->setAlignment(Qt::AlignRight);
    layout->addLayout(itemLayout);
    QPushButton* button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestReverse(); });
    button->setText("request reverse");
    itemLayout->addWidget(button);
    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestMaximalHelm(); });
    button->setText("request maximal helm");
    itemLayout->addWidget(button);
    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestSendQueue(); });
    button->setText("request send queue");
    itemLayout->addWidget(button);
    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit WriteSignalSource(); });
    button->setText("write signal source");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit WriteReverse(); });
    button->setText("write reverse");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit WriteMinimalHelm(); });
    button->setText("write minimal helm");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit WriteMaximalHelm(); });
    button->setText("write maximal helm");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit WriteMiddleHelm(); });
    button->setText("write middle helm");
    itemLayout->addWidget(button);
}
void NavSettingView::InitializeChannelPanel()
{
    QTabWidget* container = new QTabWidget(this);
    layout->addWidget(container);

    QWidget* sbusWidget1 = new QWidget(this);
    QWidget* sbusWidget2 = new QWidget(this);
    QWidget* sbusWidget3 = new QWidget(this);

    auto InitFunc = [this](QWidget* widget) {
        QVBoxLayout* page1Layout     = new QVBoxLayout();
        QVBoxLayout* page2Layout     = new QVBoxLayout();
        QHBoxLayout* containerLayout = new QHBoxLayout(widget);
        containerLayout->addLayout(page1Layout);
        containerLayout->addLayout(page2Layout);
        for (int i = 0; i < channelNum; ++i) {
            QHBoxLayout* itemLayout = new QHBoxLayout();
            QLabel* title           = new QLabel(QString::number(i), this);
            QProgressBar* bar       = new QProgressBar(this);
            bar->setTextVisible(false);
            bar->setRange(-100, 100);
            bar->setValue(0);
            QSpinBox* spinBox = new QSpinBox(this);
            spinBox->setValue(0);
            QComboBox* comBox = new QComboBox(this);
            for (int i = 1; i < 35; ++i) {
                comBox->addItem(GetPhysicalSignalSourceName(i));
            }
            itemLayout->addWidget(title);
            itemLayout->addWidget(bar);
            itemLayout->addWidget(spinBox);
            itemLayout->addWidget(comBox);
            if (i < channelNum / 2) {
                page1Layout->addLayout(itemLayout);
            }
            else {
                page2Layout->addLayout(itemLayout);
            }
            channelItems.push_back({bar, spinBox, comBox});
        }
    };
    InitFunc(sbusWidget1);
    InitFunc(sbusWidget2);
    InitFunc(sbusWidget3);
    container->addTab(sbusWidget1, tr("sbus1"));
    container->addTab(sbusWidget2, tr("sbus2"));
    container->addTab(sbusWidget3, tr("sbus3"));
    connect(container, &QTabWidget::currentChanged, [this](int index) { emit sbusChanged(index); });
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
        adSettingItems.push_back({reverseChkBox, min, mid, max});
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
    for (int i = 0; i < channelNum; ++i) {
        if (i < channelNum / 2) {
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
    connect(readConfig, &QPushButton::clicked, [this]() { emit Read(); });

    QPushButton* saveConfig = new QPushButton(this);
    saveConfig->setText(tr("save config"));
    bottomLayout->addWidget(saveConfig);
    layout->addLayout(bottomLayout);
    connect(saveConfig, &QPushButton::clicked, [this]() { emit Write(); });

    bottomLayout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QPushButton* calibration = new QPushButton(this);
    calibration->setText(tr("calibration"));
    bottomLayout->addWidget(calibration);
    layout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
    connect(calibration, &QPushButton::clicked, [this]() { emit Calibration(); });

    QPushButton* stopListen = new QPushButton(this);
    stopListen->setText("stop listen");
    bottomLayout->addWidget(stopListen);
    connect(stopListen, &QPushButton::clicked, [this]() { emit StopListen(); });
}

void NavSettingView::Connect()
{
    for (int i = 0; i < channelItems.size(); ++i) {
        auto iter = channelItems[i];
        connect(iter.mapper, &QComboBox::currentIndexChanged, [i, this](int slot) {
            emit MapperChanged(i, slot);
            qDebug() << "";
        });
    }
    for (int i = 0; i < adSettingItems.size(); ++i) {
        auto iter = adSettingItems[i];
        // connect(iter.reverse, &QCheckBox::stateChanged, [i, this](Qt::CheckState state) { emit ReveseChanged(i, state == Qt::Checked ? true : false); });
        connect(iter.minHelm, &QSpinBox::valueChanged, [i, this](int val) { emit MinimalHelmChanged(i, val); });
        connect(iter.midHelm, &QSpinBox::valueChanged, [i, this](int val) { emit MiddleHeelmChanged(i, val); });
        connect(iter.maxHelm, &QSpinBox::valueChanged, [i, this](int val) { emit MaximalHelmChanged(i, val); });
    }
}

void NavSettingView::SetModel(NavSettingModel* model)
{
    for (auto iter : channelItems) {}
    for (int i = 0; i < adSettingItems.size(); ++i) {
        auto& iter = adSettingItems[i];
        iter.reverse->setCheckState(model->GetReverse(i) ? Qt::Checked : Qt::Unchecked);
        iter.minHelm->setValue(model->GetMinimalHelm(i));
        iter.maxHelm->setValue(model->GetMaximalHelm(i));
        iter.midHelm->setValue(model->GetMiddleHelm(i));
    }
}