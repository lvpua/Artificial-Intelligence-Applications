
#include "qdlgprogress.h"
#include "ui_qdlgprogress.h"

CQDlgProgress::CQDlgProgress(QWidget *parent) : QDialog(parent,Qt::SplashScreen), ui(new Ui::CQDlgProgress)
{
    ui->setupUi(this);
    ui->qqBar->setMinimum(0);
    ui->qqBar->setMaximum(100);
    ui->qqBar->setValue(0);
}

CQDlgProgress::~CQDlgProgress()
{
    delete ui;
}

void CQDlgProgress::updateProgress(float p,const char* txt)
{
    if (p<0.0f)
        ui->qqBar->setMaximum(0);
    else
        ui->qqBar->setValue(int(p*100.0f));
    ui->qqText->setText(txt);
}


