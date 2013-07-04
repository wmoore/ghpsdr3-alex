#include "WebBrowser.h"
#include "ui_WebBrowser.h"

WebBrowser::WebBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebBrowser)
{
    ui->setupUi(this);
}

WebBrowser::~WebBrowser()
{
    delete ui;
}
