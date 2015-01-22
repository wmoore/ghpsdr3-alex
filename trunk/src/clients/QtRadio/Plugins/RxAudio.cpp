#include "RxAudio.h"
#include "ui_RxAudio.h"

RxAudio::RxAudio(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RxAudio)
{
    ui->setupUi(this);
}

RxAudio::~RxAudio()
{
    delete ui;
}
