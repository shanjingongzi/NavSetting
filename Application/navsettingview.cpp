#include "navsettingview.h"
#include "navsettingmodel.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
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
    layout->setAlignment(Qt::AlignVCenter);
}

void NavSettingView::Initialize()
{
    InitializeTopBar();
    // InitializeDebug();
    InitializeChannelPanel();
    layout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
    InitializeAdSettingPanel();
    layout->addSpacerItem(new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Expanding));
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
    openBtn->setText(tr("open"));
    QPushButton* closeBtn = new QPushButton(this);
    connect(closeBtn, &QPushButton::clicked, [this]() { emit Close(); });
    closeBtn->setText("close");

    QSpinBox* baudRate = new QSpinBox(this);
    baudRate->setRange(0, 1000000);
    baudRate->setValue(100000);
    QHBoxLayout* toolBarLayout = new QHBoxLayout();
    toolBarLayout->setAlignment(Qt::AlignRight);
    toolBarLayout->addWidget(checkBtn);
    toolBarLayout->addWidget(lab);
    toolBarLayout->addWidget(serialPortGroup);
    toolBarLayout->addWidget(new QLabel(tr("Baud"), this));
    toolBarLayout->addWidget(baudRate);
    toolBarLayout->addWidget(openBtn);
    toolBarLayout->addWidget(closeBtn);
    layout->addLayout(toolBarLayout);
    connect(openBtn, &QPushButton::clicked, [this, serialPortGroup,baudRate]() {
        auto name = serialPortGroup->currentText();
        if (name.length()) {
            emit Open(name,baudRate->value());
        }
    });
}

void NavSettingView::InitializeDebug()
{
    QHBoxLayout* itemLayout = new QHBoxLayout();
    itemLayout->setAlignment(Qt::AlignRight);
    layout->addLayout(itemLayout);

    QPushButton* button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestSignalSource(); });
    button->setText("request signal source");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestReverse(); });
    button->setText("request reverse");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestMinimalHelm(); });
    button->setText("request minimal helm");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestFineTune(); });
    button->setText("request  fine tune");
    itemLayout->addWidget(button);

    button = new QPushButton(this);
    connect(button, &QPushButton::clicked, [this]() { emit RequestMaximalHelm(); });
    button->setText("request maximal helm");
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
        QGridLayout* layout = new QGridLayout();
        layout->addWidget(new QLabel(tr("channel"), this), 0, 0);
        layout->addWidget(new QLabel(tr("position"), this), 0, 1);
        layout->addWidget(new QLabel(tr("helm"), this), 0, 2);
        layout->addWidget(new QLabel(tr("mapper"), this), 0, 3);

        layout->addWidget(new QLabel(tr("    "), this), 0, 4);

        layout->addWidget(new QLabel(tr("channel"), this), 0, 5);
        layout->addWidget(new QLabel(tr("position"), this), 0, 6);
        layout->addWidget(new QLabel(tr("helm"), this), 0, 7);
        layout->addWidget(new QLabel(tr("mapper"), this), 0, 8);

        for (int i = 0; i < channelNum; ++i) {
            QLabel* title     = new QLabel(QString::number(i+1), this);
            QProgressBar* bar = new QProgressBar(this);
            bar->setTextVisible(false);
            bar->setRange(0, 2047);
            bar->setValue(0);
            QSpinBox* spinBox = new QSpinBox(this);
            spinBox->setValue(0);
            spinBox->setRange(0, 2047);
            spinBox->setEnabled(false);
            spinBox->setStyleSheet("QSpinBox::up-button{width: 0px; height: 0px;}"
                "QSpinBox::up-arrow{image: none;}"
                "QSpinBox::down-button{width: 0px; height: 0px;}"
                "QSpinBox::down-arrow{image: none;}");
            QComboBox* comBox = new QComboBox(this);
            for (int i = 1; i < 35; ++i) {
                comBox->addItem(GetPhysicalSignalSourceName(i));
            }
            if (i < channelNum / 2) {
                layout->addWidget(title, i + 1, 0);
                layout->addWidget(bar, i + 1, 1);
                layout->addWidget(spinBox, i + 1, 2);
                layout->addWidget(comBox, i + 1, 3);
            }
            else {
                int offset = channelNum / 2 - 1;
                layout->addWidget(title, (i - offset), 5);
                layout->addWidget(bar, (i - offset), 6);
                layout->addWidget(spinBox, (i - offset), 7);
                layout->addWidget(comBox, (i - offset), 8);
            }
            channelItems.push_back({bar, spinBox, comBox});
        }
        widget->setLayout(layout);
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
    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setAlignment(Qt::AlignLeft);
    titleLayout->addWidget(new QLabel(tr("Advanced settings"), this));
    layout->addLayout(titleLayout);
    QWidget* adSetting     = new QWidget();
    QGridLayout* container = new QGridLayout(adSetting);
    container->addWidget(new QLabel(tr("channel"), this), 0, 0);
    container->addWidget(new QLabel(tr("reverse"), this), 0, 1);
    container->addWidget(new QLabel(tr("minimal helm"), this), 0, 2);
    container->addWidget(new QLabel(tr("midpoint calibration"), this), 0, 3);
    container->addWidget(new QLabel(tr("maximal helm"), this), 0, 4);

    container->addWidget(new QLabel(tr("     "), this), 0, 5);

    container->addWidget(new QLabel(tr("channel"), this), 0, 6);
    container->addWidget(new QLabel(tr("reverse"), this), 0, 7);
    container->addWidget(new QLabel(tr("minimal helm"), this), 0, 8);
    container->addWidget(new QLabel(tr("midpoint calibration"), this), 0, 9);
    container->addWidget(new QLabel(tr("maximal helm"), this), 0, 10);
    layout->addWidget(adSetting);

    for (int i = 0; i < channelNum; ++i) {
        QString name             = QString("c%1").arg(i);
        QLabel* lab              = new QLabel(name);
        QCheckBox* reverseChkBox = new QCheckBox();
        QSpinBox* min            = new QSpinBox(this);
        min->setRange(0, 2047);
        QSpinBox* mid = new QSpinBox(this);
        mid->setRange(-2047, 2047);
        QSpinBox* max = new QSpinBox(this);
        max->setRange(0, 2047);
        int offsetx = 1;
        int offsety = 0;
        if (i < channelNum / 2) {
            offsetx = 0;
            offsety = 1;
        }
        else {
            offsetx = 6;
            offsety = -channelNum / 2 + 1;
        }
        container->addWidget(lab, i + offsety, 0 + offsetx);
        container->addWidget(reverseChkBox, i + offsety, 1 + offsetx);
        container->addWidget(min, i + offsety, 2 + offsetx);
        container->addWidget(mid, i + offsety, 3 + offsetx);
        container->addWidget(max, i + offsety, 4 + offsetx);
        adSettingItems.push_back({reverseChkBox, min, mid, max});
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
    connect(calibration, &QPushButton::clicked, [this]() { emit Calibration(); });
}

void NavSettingView::Connect()
{
    for (int i = 0; i < channelItems.size(); ++i) {
        auto iter = channelItems[i];
        connect(iter.mapper, &QComboBox::currentIndexChanged, [i, this](int slot) {
            emit MapperChanged(i%16, slot+1);
            qDebug() << "";
        });
    }
    for (int i = 0; i < adSettingItems.size(); ++i) {
        auto iter = adSettingItems[i];
        connect(iter.reverse, &QCheckBox::stateChanged, [i, this](int state) { emit ReveseChanged(i, state == Qt::Checked ? true : false); });
        connect(iter.minHelm, &QSpinBox::valueChanged, [i, this](int val) { emit MinimalHelmChanged(i, val); });
        connect(iter.midHelm, &QSpinBox::valueChanged, [i, this](int val) { emit MiddleHeelmChanged(i, val); });
        connect(iter.maxHelm, &QSpinBox::valueChanged, [i, this](int val) { emit MaximalHelmChanged(i, val); });
    }
}

void NavSettingView::SetModel(NavSettingModel* model, uint8_t sbus)
{
    for (int i = 0; i < channelNum; ++i) {
        auto iter = channelItems[i + (sbus - 1) * 16];
        SignalLock lock(iter.mapper), lock2(iter.position);
        iter.mapper->setCurrentIndex(model->GetMapSlot(i));
        if (model->GetMaximalHelm(i) > model->GetMinimalHelm(i)) {
			iter.position->setRange(model->GetMinimalHelm(i), model->GetMaximalHelm(i));
        }
        else {
			iter.position->setRange(model->GetMaximalHelm(i), model->GetMinimalHelm(i));
        }
    }
    for (int i = 0; i < channelNum; ++i) {
        auto& iter = adSettingItems[i];
        SignalLock lock1(iter.reverse), lock2(iter.minHelm), lock3(iter.maxHelm), lock4(iter.midHelm);
        iter.reverse->setCheckState(model->GetReverse(i) ? Qt::Checked : Qt::Unchecked);
        iter.minHelm->setValue(model->GetMinimalHelm(i));
        iter.maxHelm->setValue(model->GetMaximalHelm(i));
        iter.midHelm->setValue(model->GetMiddleHelm(i));
    }
}

void NavSettingView::SetPosition(NavSettingModel* model, uint8_t sbus)
{
    for (int i = 0; i < channelNum; ++i) {
        auto iter = channelItems[i + (sbus - 1) * 16];
        if (model->GetPosition(i) != iter.position->value()) {
            iter.position->setValue(model->GetPosition(i));
            iter.helm->setValue(model->GetPosition(i));
        }
    }
}