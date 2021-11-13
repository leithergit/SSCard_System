#include "uc_facecapture.h"
#include "ui_uc_facecapture.h"

uc_FaceCapture::uc_FaceCapture(QLabel *pTitle,int nTimeout,QWidget *parent) :
    QStackPage(pTitle,nTimeout,parent),
    ui(new Ui::FaceCapture)
{
    ui->setupUi(this);
}

uc_FaceCapture::~uc_FaceCapture()
{
    delete ui;
}

int uc_FaceCapture::ProcessBussiness()
{
    return 0;
}

void uc_FaceCapture::OnTimeout()
{

}
