#include "fma.h"
#include "ui_fma.h"
#include "newproject.h"
FMA::FMA(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FMA)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(2);
    QStringList list = (QStringList() << "No"<<"Yes");
    ui->comboBox->addItems(list);

    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QStringList labels = (QStringList() << "Type" << "Value");
    ui->treeWidget->setHeaderLabels(labels);



    action = new QAction("Remove Axle Head", this);
    connect(action, &QAction::triggered, this, &FMA::onRemoveButtonClicked);
    ui->treeWidget->addAction(action);
}

FMA::~FMA()
{
    delete ui;
}

//public function to return axle heads
QVector<axleHeadData *> FMA::returnAxleHeads()
{
    return axleHeadArr;
}

//retrives the track section id
QString FMA::returnTrackSectionId()
{
    return trackSection_Id;
}

//creates and adds axle head to axle head array
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

//updates the preview for axle head data
void FMA::updatePreview(axleHeadData* data)
{
    QTreeWidgetItem *ahd = new QTreeWidgetItem(ui->treeWidget);
    ahd->setText(0, "Axle Head:");
    ahd->setText(1, data->axleHead_ID);
    ahd->setFlags(ahd->flags() | Qt::ItemIsEditable);
    QTreeWidgetItem *dir_inv = new QTreeWidgetItem(ahd);
    if(data->directionInversion == 1) {
        dir_inv->setText(0, "Direction Inversion:" );
        dir_inv->setText(1, "True" );
        dir_inv->setFlags(dir_inv->flags() | Qt::ItemIsEditable);
    }
    else if(data->directionInversion == 0){
        dir_inv->setText(0, "Direction Inversion:");
        dir_inv->setText(1, "False");
    } else {
         dir_inv->setText(0, "Direction Inversion:" );
         dir_inv->setText(0, "Undefined" );
    }
}

//finished creating axle head, and emits signal which sends itself to a function
void FMA::on_pushButton_4_clicked()
{
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!re.exactMatch(ui->lineEdit_2->text()) && ui->lineEdit_2->text().size() == 4 && !axleHeadArr.empty()) {
        this->trackSection_Id = ui->lineEdit_2->text();
        emit fmaSignal(this);
        this->close();
    } else {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"ERROR:", "Invalid: Track Section must be 4 characters long, not contain any numbers and must contain Axle Head Data");
        return;
    }

}

//closes window
void FMA::on_pushButton_3_clicked()
{
    this->close();
}

//removes axle head if it was wrong
void FMA::onRemoveButtonClicked()
{
    for(int i = 0; i < axleHeadArr.size(); i++) {
        if(ui->treeWidget->currentItem()->text(1) == axleHeadArr[i]->axleHead_ID) {
            axleHeadArr.remove(i);
            ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());
            return;
        } else if(ui->treeWidget->currentItem()->parent()->text(1) == axleHeadArr[i]->axleHead_ID) {
            axleHeadArr.remove(i);
            ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem()->parent()));
        }
    }
}
