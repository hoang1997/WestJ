#include "dataforwarding.h"
#include "ui_dataforwarding.h"

DataForwarding::DataForwarding(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataForwarding)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList() << "DFB Preview" << "Value");

    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *action = new QAction("Remove DFB", this);
    connect(action, &QAction::triggered, this, &DataForwarding::onRemoveButtonClicked);
    ui->treeWidget->addAction(action);

}

DataForwarding::~DataForwarding()
{
    delete ui;
}

//Updates preview of the DFB created
void DataForwarding::updatePreview_DFB() {

    for(int i = 0; i < dfbArray.size(); i++) {
        QTreeWidgetItem *dfb = new QTreeWidgetItem(ui->treeWidget);
        dfb->setText(0, "DFB");
        dfb->setText(1, QString::number(i+1));
        QTreeWidgetItem *ts = new QTreeWidgetItem(dfb);
        ts->setText(0, "Track Section ID");
        ts->setText(1, dfbArray[i]->trackSection_id);
        QTreeWidgetItem *sender = new QTreeWidgetItem(dfb);
        sender->setText(0, "Sender AEB ID");
        sender->setText(1,  QString::number(dfbArray[i]->aeb_sender_id));
        QTreeWidgetItem *receiver = new QTreeWidgetItem(dfb);
        receiver->setText(0, "Receiver AEB ID");
        receiver->setText(1, QString::number(dfbArray[i]->aeb_receiver_id));
    }
}

//Removes DFB's
void DataForwarding::onRemoveButtonClicked()
{
    if(ui->treeWidget->currentItem()->parent()) {
        dfbArray.remove(ui->treeWidget->currentItem()->parent()->text(1).toInt() - 1);
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());
    } else {
        dfbArray.remove(ui->treeWidget->currentItem()->text(1).toInt() - 1);
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());
    }
}

//Emits the DFB array to the newproject class
void DataForwarding::on_buttonBox_accepted()
{
    emit dataForwardingBlockSignal(dfbArray);
    this->close();
}

//closes the window
void DataForwarding::on_buttonBox_rejected()
{
    this->close();
}

//Add data forwarding block button
void DataForwarding::on_pushButton_clicked()
{
    DFB *dfb = new DFB;
    QRegExp re("\\d*");
    QString dir;
    if (re.exactMatch(ui->lineEdit->text()) && ui->lineEdit->text().size() == 4 ) {
        dfb->aeb_sender_id = ui->lineEdit->text().toInt();
    } else {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"ERROR:", "Incorrect input: AEB ID must contain only a 4 digit number");
    }

    if (re.exactMatch(ui->lineEdit_2->text()) && ui->lineEdit_2->text().size() == 4 ) {
        dfb->aeb_receiver_id = ui->lineEdit_2->text().toInt();
    } else {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"ERROR:", "Incorrect input: AEB ID must contain only a 4 digit number");
    }


    if (ui->lineEdit_2->text().size() == 4) {
        dfb->trackSection_id = ui->lineEdit_3->text();
    } else {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"ERROR:", "Incorrect input: Track Section Must Be 4 Characters Long");
    }

    dfbArray.push_back(dfb);

    ui->treeWidget->clear();
    updatePreview_DFB();
}
