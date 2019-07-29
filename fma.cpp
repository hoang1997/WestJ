#include "fma.h"
#include "ui_fma.h"
#include "newproject.h"
FMA::FMA(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FMA)
{
    ui->setupUi(this);
    QStringList list = (QStringList() << "No"<<"Yes");
    ui->comboBox->addItems(list);
}

FMA::~FMA()
{
    delete ui;
}

QVector<axleHeadData *> FMA::returnAxleHeads()
{
    return axleHeadArr;
}

QString FMA::returnTrackSectionId()
{
    return trackSection_Id;
}

void FMA::on_pushButton_clicked()
{
    axleHeadData *ahd = new axleHeadData;
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (re.exactMatch(ui->lineEdit->text()) && ui->lineEdit->text().size() == 4 ) {
        ahd->axleHead_ID = ui->lineEdit->text();
    } else {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"ERROR:", "Incorrect Input: AEB id must only contain 4 digits");
        return;
    }
    for(axleHeadData *data : axleHeadArr) {
        if(ui->lineEdit->text() == data->axleHead_ID) {
            QMessageBox::StandardButton error;
            error = QMessageBox::information(this,"ERROR:", "Invalid: Axle Head id already exists");
            return;
        }
    }
    if(ui->comboBox->currentText() == "Yes") {
        ahd->directionInversion = 1;
    } else {
        ahd->directionInversion = 0;
    }

    axleHeadArr.push_back(ahd);


    updatePreview(ahd);
}

void FMA::updatePreview(axleHeadData* data)
{
    QTreeWidgetItem *ahd = new QTreeWidgetItem(ui->treeWidget);
    ahd->setText(0, "Axle Head: " + data->axleHead_ID);
    QTreeWidgetItem *dir_inv = new QTreeWidgetItem(ahd);
    if(data->directionInversion == 1) {
        dir_inv->setText(0, "Direction Inversion: True" );
    }
    else if(data->directionInversion == 0){
        dir_inv->setText(0, "Direction Inversion: False" );
    } else {
         dir_inv->setText(0, "Direction Inversion: Undefined" );
    }
}

void FMA::on_pushButton_4_clicked()
{
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!re.exactMatch(ui->lineEdit_2->text()) && ui->lineEdit_2->text().size() == 4) {
        this->trackSection_Id = ui->lineEdit_2->text();
        emit fmaSignal(this);
        this->close();
    } else {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"ERROR:", "Invalid Input: Track Section must be 4 characters long and not contain any numbers");
        return;
    }

}

void FMA::on_pushButton_3_clicked()
{
    this->close();
}
