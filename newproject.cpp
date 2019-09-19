#include "newproject.h"
#include "ui_newproject.h"
#include "processfiles.h"
NewProject::NewProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProject)
{
    ui->setupUi(this);
    QStringList YesNo = (QStringList() << "Yes"<<"No");

    ipHeaderLabels = (QStringList() << "BYTE 1" << "BYTE 2" <<"BYTE 3" << "BYTE 4");
    ipRemoteSystemLabels = (QStringList() << "Primary Network A:" << "Primary Network B:" << "Secondary Network A:" << "Secondary Network B");
    ipRemoteSystemLabels = (QStringList() << "FDS Network A:" << "FDS Network B:" << "TF Network A:" << "TF Network B");
    ipBasicLabels = (QStringList() << "Network A: IP Address" << "Network A: Default Gateway" << "Network A: Subnet Mask" <<"Network B: IP Address" << "Network B: Default Gateway" << "Network B: Subnet Mask");

    ui->comboBox->addItems(QStringList() << "Network A"<<"Network B");
    ui->comboBox_2->addItems(QStringList() << "FDS" << "TF" << "Evaluator");
    ui->comboBox_3->addItems(QStringList() << "Primary Network"<<"Secondary Network");
    ui->comboBox_4->addItems(YesNo);
    ui->comboBox_5->addItems(YesNo);
    ui->comboBox_6->addItems(YesNo);

    ui->treeWidget_3->setHeaderLabel("AEB Files");
    ui->treeWidget_2->setHeaderLabels(QStringList() << "COM File Preview" << "Value");
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList() << "AEB File Preview"<<"Value");
    ui->treeWidget_3->setColumnCount(2);
    ui->treeWidget_3->setHeaderLabels(QStringList() << "AEB File Preview"<<"Value");
    ui->treeWidget_4->setColumnCount(2);
    ui->treeWidget_4->setHeaderLabels(QStringList() << "WNC Messaging Preview"<<"Value");
    ui->treeWidget_5->setColumnCount(2);
    ui->treeWidget_5->setHeaderLabels(QStringList() << "Evaluator" << "Data");
    ui->treeWidget_6->setColumnCount(2);
    ui->treeWidget_6->setHeaderLabels((QStringList() << "AEB Data Link" << "Value"));
    connect(ui->treeWidget_3, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onPreviewTreeWidgetClicked_aeb(QTreeWidgetItem*, int)));

    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidget_2->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidget_3->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidget_5->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidget_6->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *action = new QAction("Remove Section", this);
    connect(action, &QAction::triggered, this, &NewProject::onRemoveButtonClicked);
    QAction *action2 = new QAction("Remove Evaluator ", this);
    connect(action2, &QAction::triggered, this, &NewProject::onRemoveEvaluatorClicked);
    QAction *action3 = new QAction("Remove AEB Data Link", this);
    connect(action3, &QAction::triggered, this, &NewProject::onRemoveDataLinkClicked);
    QAction *action4 = new QAction("Remove AEB File", this);
    connect(action4, &QAction::triggered, this, &NewProject::onRemoveButtonClicked);
    QAction *action5 = new QAction("Remove COM File", this);
    connect(action5, &QAction::triggered, this, &NewProject::onRemoveButtonClicked);
    ui->treeWidget->addAction(action);
    ui->treeWidget_2->addAction(action4);
    ui->treeWidget_3->addAction(action5);
    ui->treeWidget_5->addAction(action2);
    ui->treeWidget_6->addAction(action3);

    ui->tableWidget->setRowCount(6);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(ipHeaderLabels);
    ui->tableWidget->setVerticalHeaderLabels(ipBasicLabels);
    ui->tableWidget_2->setRowCount(4);
    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setHorizontalHeaderLabels(ipHeaderLabels);
    ui->tableWidget_2->setVerticalHeaderLabels(ipRemoteSystemLabels);
    ui->tableWidget_3->setRowCount(4);
    ui->tableWidget_3->setColumnCount(4);
    ui->tableWidget_3->setHorizontalHeaderLabels(ipHeaderLabels);
    ui->tableWidget_3->setVerticalHeaderLabels(ipRemoteSystemLabels);

    ui->treeWidget_5->header()->resizeSection(0 /*column index*/, 120 /*width*/);
    ui->treeWidget_5->expandAll();
}

NewProject::~NewProject()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------//
//---------------------------------------AEB FILE AREA-----------------------------------------//
//--This area is where all the functions to create an AEB file is located, this also includes--//
//--the insertion of the FMA blocks, the function slot for when the signal is emitted, and-----//
//--the function to delete the FMA block.------------------------------------------------------//
//---------------------------------------------------------------------------------------------//

//-------------------------------------//
//FMA Slot for when FMA button is added//
//-------------------------------------//
void NewProject::fmaSignalReceived(FMA * fma)
{
    fmaArr.push_back(fma);
    updateFMApreview(fma);
}

//-----------------------------------------//
//SLOT for right click remove button signal//
//-----------------------------------------//
void NewProject::onRemoveButtonClicked()
{
    for(int i = 0; i < fmaArr.size(); i++) {
        if(ui->treeWidget->currentItem()->text(1) == fmaArr[i]->returnTrackSectionId()) {
            fmaArr.remove(i);
            ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());
            return;
        }
    }
}

//------------------//
//Update FMA Preview//
//------------------//
void NewProject::updateFMApreview(FMA* fma) {
    QTreeWidgetItem *FMA = new QTreeWidgetItem(ui->treeWidget);
    FMA->setText(0, "FMA ID:");
    FMA->setText(1,fma->returnTrackSectionId());
    for(int i = 0; i < fma->returnAxleHeads().size(); i++){
        QTreeWidgetItem *ah = new QTreeWidgetItem(FMA);
        ah->setText(0, "Axle Head:");
        ah->setText(1, fma->returnAxleHeads()[i]->axleHead_ID);
        QTreeWidgetItem *dir_inv = new QTreeWidgetItem(ah);
        dir_inv->setText(0, "Direction Inversion:");
        dir_inv->setText(1, (fma->returnAxleHeads()[i]->directionInversion == 1) ? "Yes" : " No");
    }
}

//----------//
//Insert FMA//
//----------//
void NewProject::on_pushButton_clicked()
{

    fma = new FMA();
    connect(fma, SIGNAL(fmaSignal(FMA*)), this, SLOT(fmaSignalReceived(FMA*)), Qt::UniqueConnection);
    fma->exec();
}

//------------------//
//Add AEB to Project//
//------------------//
void NewProject::on_pushButton_2_clicked()
{

    aeb = new AEB;

    QRegExp re("\\d*");
    QString dir;
    if (re.exactMatch(ui->lineEdit_2->text()) && ui->lineEdit_2->text().size() == 4 ) {
        aeb->AEB_Id = ui->lineEdit_2->text();

    } else {
        displayError("Incorrect input: AEB ID must contain only a 4 digit number");
        return;
    }

    if (ui->lineEdit_4->text().size() != 0) {
        aeb->evaluatorName = ui->lineEdit_4->text();

    } else {
        displayError("Incorrect input: Evaluator name MUST be included");
        return;
    }

    aeb->fmaArr = fmaArr;

    aeb->generationTime = returnGenTime();

    aebArr.push_back(aeb);
    fmaArr.clear();
    qDebug() << "AEB file stored";
    updatePPpreview(aeb);

    ui->treeWidget->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_4->clear();

    QMessageBox::StandardButton error;
    error = QMessageBox::information(this,"Success:", "Added AEB: " + aeb->AEB_Id + " Successfully" );

}


//Remove File from File Preview//
void NewProject::onRemoveFile_AEB()
{
    for(int i = 0; i < aebArr.size(); i++) {
        if(ui->treeWidget_2->currentItem()->text(1) == aebArr[i]->AEB_Id) {
            aebArr.remove(i);
            ui->treeWidget_2->takeTopLevelItem(ui->treeWidget_2->currentIndex().row());
            return;
        }
    }
}

//---------------------------------------------------------------------------------------------//
//--------------------------------------END AEB FILE AREA--------------------------------------//
//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//---------------------------------------COM FILE AREA-----------------------------------------//
//--This area is where all the functions to help fill the COM instance with data and also------//
//--to delete clear the interface as well as a signal to tell the user the file has been-------//
//--created.-----------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------//

//Basic Configuration
void NewProject::on_pushButton_7_clicked()
{
    if(com == nullptr) {
        com = new COM;
        qDebug() << "instantiated com";
    }

    if(ui->comboBox->currentText() == "Network A") {
        com->IP_A_BASIC.clear();
        com->IP_A_BASIC.push_back(ui->spinBox_7->text().toInt());
        QTableWidgetItem *item1 = new QTableWidgetItem(ui->spinBox_7->text());
        ui->tableWidget->setItem(0,3, item1);
        com->IP_A_BASIC.push_back(ui->spinBox_6->text().toInt());
        QTableWidgetItem *item2 = new QTableWidgetItem(ui->spinBox_6->text());
        ui->tableWidget->setItem(0,2, item2);
        com->IP_A_BASIC.push_back(ui->spinBox_5->text().toInt());
        QTableWidgetItem *item3 = new QTableWidgetItem(ui->spinBox_5->text());
        ui->tableWidget->setItem(0,1, item3);
        com->IP_A_BASIC.push_back(ui->spinBox_4->text().toInt());
        QTableWidgetItem *item4 = new QTableWidgetItem(ui->spinBox_4->text());
        ui->tableWidget->setItem(0,0, item4);

        com->IP_A_BASIC_DG.clear();
        com->IP_A_BASIC_DG.push_back(ui->spinBox_11->text().toInt());
        QTableWidgetItem *item5 = new QTableWidgetItem(ui->spinBox_11->text());
        ui->tableWidget->setItem(1,3, item5);
        com->IP_A_BASIC_DG.push_back(ui->spinBox_10->text().toInt());
        QTableWidgetItem *item6 = new QTableWidgetItem(ui->spinBox_10->text());
        ui->tableWidget->setItem(1,2, item6);
        com->IP_A_BASIC_DG.push_back(ui->spinBox_9->text().toInt());
        QTableWidgetItem *item7 = new QTableWidgetItem(ui->spinBox_9->text());
        ui->tableWidget->setItem(1,1, item7);
        com->IP_A_BASIC_DG.push_back(ui->spinBox_8->text().toInt());
        QTableWidgetItem *item8 = new QTableWidgetItem(ui->spinBox_8->text());
        ui->tableWidget->setItem(1,0, item8);

        com->IP_A_BASIC_SM = (ui->spinBox_3->text().toInt());
        QTableWidgetItem *item9 = new QTableWidgetItem(ui->spinBox_3->text());
        ui->tableWidget->setItem(2,0, item9);
    }
    else if(ui->comboBox->currentText() == "Network B") {
        com->IP_B_BASIC.clear();
        com->IP_B_BASIC.push_back(ui->spinBox_7->text().toInt());
        QTableWidgetItem *item1 = new QTableWidgetItem(ui->spinBox_7->text());
        ui->tableWidget->setItem(3,3, item1);
        com->IP_B_BASIC.push_back(ui->spinBox_6->text().toInt());
        QTableWidgetItem *item2 = new QTableWidgetItem(ui->spinBox_6->text());
        ui->tableWidget->setItem(3,2, item2);
        com->IP_B_BASIC.push_back(ui->spinBox_5->text().toInt());
        QTableWidgetItem *item3 = new QTableWidgetItem(ui->spinBox_5->text());
        ui->tableWidget->setItem(3,1, item3);
        com->IP_B_BASIC.push_back(ui->spinBox_4->text().toInt());
        QTableWidgetItem *item4 = new QTableWidgetItem(ui->spinBox_4->text());
        ui->tableWidget->setItem(3,0, item4);

        com->IP_B_BASIC_DG.clear();
        com->IP_B_BASIC_DG.push_back(ui->spinBox_11->text().toInt());
        QTableWidgetItem *item5 = new QTableWidgetItem(ui->spinBox_11->text());
        ui->tableWidget->setItem(4,3, item5);
        com->IP_B_BASIC_DG.push_back(ui->spinBox_10->text().toInt());
        QTableWidgetItem *item6 = new QTableWidgetItem(ui->spinBox_10->text());
        ui->tableWidget->setItem(4,2, item6);
        com->IP_B_BASIC_DG.push_back(ui->spinBox_9->text().toInt());
        QTableWidgetItem *item7 = new QTableWidgetItem(ui->spinBox_9->text());
        ui->tableWidget->setItem(4,1, item7);
        com->IP_B_BASIC_DG.push_back(ui->spinBox_8->text().toInt());
        QTableWidgetItem *item8 = new QTableWidgetItem(ui->spinBox_8->text());
        ui->tableWidget->setItem(4,0, item8);

        com->IP_B_BASIC_SM = (ui->spinBox_3->text().toInt());
        QTableWidgetItem *item9 = new QTableWidgetItem(ui->spinBox_3->text());
        ui->tableWidget->setItem(5,0, item9);
    }
}

//WNC Data Link
void NewProject::on_pushButton_10_clicked()
{
    if(com == nullptr) {
        com = new COM;
        qDebug() << "instantiated com";
    }

    WNC_AEB_DATA *wnc_aeb = new WNC_AEB_DATA;
    QRegExp re("\\d*");
    QString dir;
    if (re.exactMatch(ui->lineEdit_10->text()) && ui->lineEdit_10->text().size() == 4 ) {
        wnc_aeb->aeb_id = ui->lineEdit_10->text().toInt();
    }
    QTreeWidgetItem *aebid = new QTreeWidgetItem(ui->treeWidget_6);
    aebid->setText(0, "AEB ID");
    aebid->setText(1, QString::number(wnc_aeb->aeb_id));

    QTreeWidgetItem *ttt = new QTreeWidgetItem(aebid);
    ttt->setText(0, "Transfer To Treadle");

    QTreeWidgetItem *tf1 = new QTreeWidgetItem(aebid);
    tf1->setText(0, "Transfer FMA1 to IXL");
    QTreeWidgetItem *tf2 = new QTreeWidgetItem(aebid);
    tf2->setText(0, "Transfer FMA2 to IXL");

    if(ui->comboBox_4->currentText() == "Yes") {
        wnc_aeb->transfer_to_treadle = true;
        ttt->setText(1, "True");
    } else {
        wnc_aeb->transfer_to_treadle = false;
        ttt->setText(1, "False");
    }

    if(ui->comboBox_5->currentText() == "Yes") {
        wnc_aeb->transfer_FMA1_to_IXL = true;
        tf1->setText(1,"True");
    } else {
        wnc_aeb->transfer_to_treadle = false;
        tf1->setText(1,"False");
    }

    if(ui->comboBox_6->currentText() == "Yes") {
        wnc_aeb->transfer_FMA2_to_IXL = true;
        tf2->setText(1,"True");
    } else {
        wnc_aeb->transfer_to_treadle = false;
        tf2->setText(1,"False");
    }

    com->WNC_AEB_DATA_Arr.push_back(wnc_aeb);
}

//WNC Messaging Address
void NewProject::on_pushButton_8_clicked()
{
    if(com == nullptr) {
        com = new COM;
        qDebug() << "instantiated com";
    }

    ui->treeWidget_4->clear();

    com->wnc_NetTraceID = ui->lineEdit_6->text().toInt();
    QTreeWidgetItem *ntid = new QTreeWidgetItem(ui->treeWidget_4);
    ntid->setText(0, "WESTRACE ID");
    ntid->setText(1, QString::number(com->wnc_NetTraceID));
    qDebug() << com->wnc_NetTraceID;
    com->wnc_WestTraceAddress = ui->lineEdit_7->text().toInt();
    QTreeWidgetItem *wta = new QTreeWidgetItem(ui->treeWidget_4);
    wta->setText(0, "WESTRACE Address");
    wta->setText(1, QString::number(com->wnc_WestTraceAddress));

    com->wnc_pdv = ui->lineEdit_8->text().toInt();
    QTreeWidgetItem *pdv = new QTreeWidgetItem(ui->treeWidget_4);
    pdv->setText(0, "Product Data Version (PDV)");
    pdv->setText(1, QString::number(com->wnc_pdv));

    com->wnc_DestPort = ui->lineEdit_9->text().toInt();
    QTreeWidgetItem *dport= new QTreeWidgetItem(ui->treeWidget_4);
    dport->setText(0, "Destination Port");
    dport->setText(1, QString::number(com->wnc_DestPort));
}

//Remote System
void NewProject::on_pushButton_6_clicked()
{
    if(com == nullptr) {
        com = new COM;
        qDebug() << "instantiated com";
    }

    if(ui->comboBox_3->currentText() == "Primary Network") {
        com->ACP_Primary_IP_A.clear();
        com->ACP_Primary_IP_A.push_back(ui->spinBox_20->text().toInt());
        QTableWidgetItem *item1 = new QTableWidgetItem(ui->spinBox_20->text());
        ui->tableWidget_2->setItem(0,0, item1);
        com->ACP_Primary_IP_A.push_back(ui->spinBox_21->text().toInt());
        QTableWidgetItem *item2 = new QTableWidgetItem(ui->spinBox_21->text());
        ui->tableWidget_2->setItem(0,1, item2);
        com->ACP_Primary_IP_A.push_back(ui->spinBox_22->text().toInt());
        QTableWidgetItem *item3 = new QTableWidgetItem(ui->spinBox_22->text());
        ui->tableWidget_2->setItem(0,2, item3);
        com->ACP_Primary_IP_A.push_back(ui->spinBox_23->text().toInt());
        QTableWidgetItem *item4 = new QTableWidgetItem(ui->spinBox_23->text());
        ui->tableWidget_2->setItem(0,3, item4);

        com->ACP_Primary_IP_B.clear();
        com->ACP_Primary_IP_B.push_back(ui->spinBox_28->text().toInt());
        QTableWidgetItem *item5 = new QTableWidgetItem(ui->spinBox_28->text());
        ui->tableWidget_2->setItem(1,0, item5);
        com->ACP_Primary_IP_B.push_back(ui->spinBox_29->text().toInt());
        QTableWidgetItem *item6 = new QTableWidgetItem(ui->spinBox_29->text());
        ui->tableWidget_2->setItem(1,1, item6);
        com->ACP_Primary_IP_B.push_back(ui->spinBox_30->text().toInt());
        QTableWidgetItem *item7 = new QTableWidgetItem(ui->spinBox_30->text());
        ui->tableWidget_2->setItem(1,2, item7);
        com->ACP_Primary_IP_B.push_back(ui->spinBox_31->text().toInt());
        QTableWidgetItem *item8 = new QTableWidgetItem(ui->spinBox_31->text());
        ui->tableWidget_2->setItem(1,3, item8);

    }
    else {
        com->ACP_Secondary_IP_A.clear();
        com->ACP_Secondary_IP_A.push_back(ui->spinBox_20->text().toInt());
        QTableWidgetItem *item1 = new QTableWidgetItem(ui->spinBox_20->text());
        ui->tableWidget_2->setItem(2,0, item1);
        com->ACP_Secondary_IP_A.push_back(ui->spinBox_21->text().toInt());
        QTableWidgetItem *item2 = new QTableWidgetItem(ui->spinBox_21->text());
        ui->tableWidget_2->setItem(2,1, item2);
        com->ACP_Secondary_IP_A.push_back(ui->spinBox_22->text().toInt());
        QTableWidgetItem *item3 = new QTableWidgetItem(ui->spinBox_22->text());
        ui->tableWidget_2->setItem(2,2, item3);
        com->ACP_Secondary_IP_A.push_back(ui->spinBox_23->text().toInt());
        QTableWidgetItem *item4 = new QTableWidgetItem(ui->spinBox_23->text());
        ui->tableWidget_2->setItem(2,3, item4);

        com->ACP_Secondary_IP_B.clear();
        com->ACP_Secondary_IP_B.push_back(ui->spinBox_28->text().toInt());
        QTableWidgetItem *item5 = new QTableWidgetItem(ui->spinBox_28->text());
        ui->tableWidget_2->setItem(3,0, item5);
        com->ACP_Secondary_IP_B.push_back(ui->spinBox_29->text().toInt());
        QTableWidgetItem *item6 = new QTableWidgetItem(ui->spinBox_29->text());
        ui->tableWidget_2->setItem(3,1, item6);
        com->ACP_Secondary_IP_B.push_back(ui->spinBox_30->text().toInt());
        QTableWidgetItem *item7 = new QTableWidgetItem(ui->spinBox_30->text());
        ui->tableWidget_2->setItem(3,2, item7);
        com->ACP_Secondary_IP_B.push_back(ui->spinBox_31->text().toInt());
        QTableWidgetItem *item8 = new QTableWidgetItem(ui->spinBox_31->text());
        ui->tableWidget_2->setItem(3,3, item8);
    }
}

//Add Data Forwarding Block
void NewProject::on_pushButton_5_clicked()
{
    if(ui->comboBox_2->currentText() == "FDS") {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error", "Forward Blocks cannot be added to FDS" );
    } else if(ui->comboBox_2->currentText() == "TF") {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error", "Forward Blocks cannot be added to TF" );
    } else {
        df = new DataForwarding();
        connect(df, SIGNAL(dataForwardingBlockSignal(QVector<DFB*>)), this, SLOT(dfbSignalReceived(QVector<DFB*>)));
        df->exec();
    }
}

//Remote Destination
void NewProject::on_pushButton_4_clicked()
{
    if(com == nullptr) {
        com = new COM;
        qDebug() << "instantiated com";
    }
    if(ui->comboBox_2->currentText() == "FDS") {
        com->IP_RD_FDS_A.clear();
        com->IP_RD_FDS_A.push_back(ui->spinBox_12->text().toInt());
        QTableWidgetItem *item1 = new QTableWidgetItem(ui->spinBox_12->text());
        ui->tableWidget_3->setItem(0,0, item1);
        com->IP_RD_FDS_A.push_back(ui->spinBox_13->text().toInt());
        QTableWidgetItem *item2 = new QTableWidgetItem(ui->spinBox_13->text());
        ui->tableWidget_3->setItem(0,1, item2);
        com->IP_RD_FDS_A.push_back(ui->spinBox_14->text().toInt());
        QTableWidgetItem *item3 = new QTableWidgetItem(ui->spinBox_14->text());
        ui->tableWidget_3->setItem(0,2, item3);
        com->IP_RD_FDS_A.push_back(ui->spinBox_15->text().toInt());
        QTableWidgetItem *item4 = new QTableWidgetItem(ui->spinBox_15->text());
        ui->tableWidget_3->setItem(0,3, item4);

        com->IP_RD_FDS_B.clear();
        com->IP_RD_FDS_B.push_back(ui->spinBox_16->text().toInt());
        QTableWidgetItem *item5 = new QTableWidgetItem(ui->spinBox_16->text());
        ui->tableWidget_3->setItem(1,0, item5);
        com->IP_RD_FDS_B.push_back(ui->spinBox_17->text().toInt());
        QTableWidgetItem *item6 = new QTableWidgetItem(ui->spinBox_17->text());
        ui->tableWidget_3->setItem(1,1, item6);
        com->IP_RD_FDS_B.push_back(ui->spinBox_18->text().toInt());
        QTableWidgetItem *item7 = new QTableWidgetItem(ui->spinBox_18->text());
        ui->tableWidget_3->setItem(1,2, item7);
        com->IP_RD_FDS_B.push_back(ui->spinBox_19->text().toInt());
        QTableWidgetItem *item8 = new QTableWidgetItem(ui->spinBox_19->text());
        ui->tableWidget_3->setItem(1,3, item8);
    } else if(ui->comboBox_2->currentText() == "TF") {
        com->IP_RD_TF_A.clear();
        com->IP_RD_TF_A.push_back(ui->spinBox_12->text().toInt());
        QTableWidgetItem *item1 = new QTableWidgetItem(ui->spinBox_12->text());
        ui->tableWidget_3->setItem(2,0, item1);
        com->IP_RD_TF_A.push_back(ui->spinBox_13->text().toInt());
        QTableWidgetItem *item2 = new QTableWidgetItem(ui->spinBox_13->text());
        ui->tableWidget_3->setItem(2,1, item2);
        com->IP_RD_TF_A.push_back(ui->spinBox_14->text().toInt());
        QTableWidgetItem *item3 = new QTableWidgetItem(ui->spinBox_14->text());
        ui->tableWidget_3->setItem(2,2, item3);
        com->IP_RD_TF_A.push_back(ui->spinBox_15->text().toInt());
        QTableWidgetItem *item4 = new QTableWidgetItem(ui->spinBox_15->text());
        ui->tableWidget_3->setItem(2,3, item4);

        com->IP_RD_TF_B.clear();
        com->IP_RD_TF_B.push_back(ui->spinBox_16->text().toInt());
        QTableWidgetItem *item5 = new QTableWidgetItem(ui->spinBox_16->text());
        ui->tableWidget_3->setItem(3,0, item5);
        com->IP_RD_TF_B.push_back(ui->spinBox_17->text().toInt());
        QTableWidgetItem *item6 = new QTableWidgetItem(ui->spinBox_17->text());
        ui->tableWidget_3->setItem(3,1, item6);
        com->IP_RD_TF_B.push_back(ui->spinBox_18->text().toInt());
        QTableWidgetItem *item7 = new QTableWidgetItem(ui->spinBox_18->text());
        ui->tableWidget_3->setItem(3,2, item7);
        com->IP_RD_TF_B.push_back(ui->spinBox_19->text().toInt());
        QTableWidgetItem *item8 = new QTableWidgetItem(ui->spinBox_19->text());
        ui->tableWidget_3->setItem(3,3, item8);
    }
    else {
        Evaluator_RD *eval = new Evaluator_RD;
        eval->dataForwardingArr = dfbArr;
        eval->IP_A.push_back(ui->spinBox_12->text().toInt());
        eval->IP_A.push_back(ui->spinBox_13->text().toInt());
        eval->IP_A.push_back(ui->spinBox_14->text().toInt());
        eval->IP_A.push_back(ui->spinBox_15->text().toInt());
        eval->IP_B.push_back(ui->spinBox_16->text().toInt());
        eval->IP_B.push_back(ui->spinBox_17->text().toInt());
        eval->IP_B.push_back(ui->spinBox_18->text().toInt());
        eval->IP_B.push_back(ui->spinBox_19->text().toInt());
        com->IP_RD_EVAL_ARR.push_back(eval);

        ui->treeWidget_4->clear();

        for(int i = 0; i < com->IP_RD_EVAL_ARR.size();  i++) {
            QTreeWidgetItem *eval = new QTreeWidgetItem(ui->treeWidget_5);
            eval->setText(0, "Evaluator ");
            eval->setText(1, QString::number(i+1));

            QTreeWidgetItem *ipa = new QTreeWidgetItem(eval);
            ipa->setText(0, "IP A");
            for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->IP_A.size(); j++) {
                QTreeWidgetItem *byte = new QTreeWidgetItem(ipa);
                byte->setText(0, "Byte " + QString::number(j+1));
                byte->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->IP_A[j]));
            }

            QTreeWidgetItem *ipb = new QTreeWidgetItem(eval);
            ipb->setText(0, "IP B");
            for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->IP_B.size(); j++) {
                QTreeWidgetItem *byte = new QTreeWidgetItem(ipb);
                byte->setText(0, "Byte " + QString::number(j+1));
                byte->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->IP_B[j]));
            }

            QTreeWidgetItem *dfb = new QTreeWidgetItem(eval);
            dfb->setText(0, "Data Forwarding");

            for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->dataForwardingArr.size(); j++) {

                QTreeWidgetItem *ts = new QTreeWidgetItem(dfb);
                ts->setText(0, "Track Section ID");
                ts->setText(1, com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->trackSection_id);

                QTreeWidgetItem *sender = new QTreeWidgetItem(dfb);
                sender->setText(0, "Sender AEB ID");
                sender->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_sender_id));

                QTreeWidgetItem *receiver = new QTreeWidgetItem(dfb);
                receiver->setText(0, "Receiver AEB ID");
                receiver->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_receiver_id));


            }
        }
    }
}

//Add COM to Project
void NewProject::on_pushButton_9_clicked()
{
    QRegExp re("\\d*");
    if (re.exactMatch(ui->lineEdit_3->text()) && ui->lineEdit_3->text().size() == 4 ) {
        com->COM_ID = ui->lineEdit_3->text();

    }
    else {
        displayError("Incorrect input: COM ID must contain only a 4 digit number");
        return;
    }

    if (ui->lineEdit_5->text().size() != 0) {
        com->Evaluator_Name = ui->lineEdit_5->text();

    } else {
        displayError("Incorrect input: Evaluator name MUST be included");
        return;
    }

    if(com->IP_A_BASIC.empty() || com->IP_B_BASIC.empty()) {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error EC201", "COM IP Address: IP Address A or B is missing" );
        return;
    }
    else if(com->IP_RD_FDS_A.empty()) {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error EC202:", "COM Remote Destination: FDS Missing");
        return;
    }
    else if(com->IP_RD_TF_A.empty()) {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error EC203:", "COM Remote Destination: TF Missing");
        return;
    }
    else if(com->ACP_Primary_IP_A.empty()) {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error EC204:", "COM ACP Remote System: Primary Address(s) Missing");
        return;
    }
    else if(com->ACP_Secondary_IP_A.empty()) {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error EC205:", "COM ACP Remote System: Secondary Address(s) Missing");
        return;
    }
    else if(com->WNC_AEB_DATA_Arr.empty()) {
        QMessageBox::StandardButton error;
        error = QMessageBox::information(this,"Error EC206:", "COM WNC Data Links: No Data Links Added");
        return;
    }


    if (ui->lineEdit_5->text().size() != 0) {
        com->Evaluator_Name = ui->lineEdit_5->text();

    } else {
        displayError("Incorrect input: Evaluator name MUST be included");
        return;
    }

    com->projectNum = returnProjectNumber();
    com->generationTime = returnGenTime();
    updatePPpreview(com);
    comArr.push_back(com);

    clearComInterface();

    QMessageBox::StandardButton error;
    error = QMessageBox::information(this,"Success:", "Added COM: " + com->COM_ID + " Successfully" );

    com = new COM;
}

//Clears Com Interface for new COM file to be created
void NewProject::clearComInterface()
{
    ui->lineEdit_3->clear();
    ui->lineEdit_5->clear();

    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->lineEdit_9->clear();
    ui->lineEdit_10->clear();

    ui->spinBox_4->clear();
    ui->spinBox_5->clear();
    ui->spinBox_6->clear();
    ui->spinBox_7->clear();
    ui->spinBox_8->clear();
    ui->spinBox_9->clear();
    ui->spinBox_10->clear();
    ui->spinBox_11->clear();

    ui->spinBox_12->clear();
    ui->spinBox_13->clear();
    ui->spinBox_14->clear();
    ui->spinBox_15->clear();
    ui->spinBox_16->clear();
    ui->spinBox_17->clear();
    ui->spinBox_18->clear();
    ui->spinBox_19->clear();

    ui->spinBox_20->clear();
    ui->spinBox_21->clear();
    ui->spinBox_22->clear();
    ui->spinBox_23->clear();
    ui->spinBox_28->clear();
    ui->spinBox_29->clear();
    ui->spinBox_30->clear();
    ui->spinBox_31->clear();

    ui->treeWidget_4->clear();
    ui->treeWidget_5->clear();
    ui->treeWidget_6->clear();

    ui->tableWidget->clearContents();
    ui->tableWidget_2->clearContents();
    ui->tableWidget_3->clearContents();
}

//Function for when the "Remove Evaluator" is triggered
void NewProject::onRemoveEvaluatorClicked()
{
    if(ui->treeWidget_5->currentItem()->parent()) {
        com->IP_RD_EVAL_ARR.remove(ui->treeWidget_5->currentItem()->parent()->text(1).toInt() - 1);
        qDebug() << "removed evaluator";
    } else {
        com->IP_RD_EVAL_ARR.remove(ui->treeWidget_5->currentItem()->text(1).toInt() - 1);
        qDebug() << "removed evaluator";
    }
    ui->treeWidget_5->takeTopLevelItem(ui->treeWidget_5->currentIndex().row());
}

//Funcition for when the "Remove Data Link" is triggered
void NewProject::onRemoveDataLinkClicked()
{
    for(int i = 0; i < com->WNC_AEB_DATA_Arr.size(); i++) {
        if( ui->treeWidget_6->currentItem()->text(1).toInt() == com->WNC_AEB_DATA_Arr[i]->aeb_id){
            com->WNC_AEB_DATA_Arr.remove(i);
            ui->treeWidget_6->takeTopLevelItem(ui->treeWidget_6->currentIndex().row());
            return;
        }
    }
}

//Data Forwarding Block Slot for when Signal is emitted
void NewProject::dfbSignalReceived(QVector<DFB*> dfbArray)
{
    dfbArr = dfbArray;
}

void NewProject::onRemoveFile_COM()
{
    for(int i = 0; i < comArr.size(); i++) {
        if(ui->treeWidget_3->currentItem()->text(1) == comArr[i]->COM_ID) {
            comArr.remove(i);
            ui->treeWidget_3->takeTopLevelItem(ui->treeWidget_3->currentIndex().row());
            return;
        }
    }
}

//---------------------------------------------------------------------------------------------//
//--------------------------------------END COM FILE AREA--------------------------------------//
//---------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------//
//-----------------------------------GENERAL FUNCTIONS AREA------------------------------------//
//--The functions that are included in this area are for actios such as updating the project---//
//--preview area, exporting files, displaying errors, and creating the time stamp.-------------//
//---------------------------------------------------------------------------------------------//


//Export Files
void NewProject::on_pushButton_3_clicked()
{
    ProcessFiles *pf = new ProcessFiles(this);

    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    QString dir;
    if (re.exactMatch(ui->lineEdit->text()) && ui->lineEdit->text().size() == 3 ) {
        dir = QFileDialog::getExistingDirectory(this,  tr("Open Directory"),"/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    else {
        displayError("Incorrect Input: Project Number must not include letters and only contain 3 digits");
        return;
    }

    if(comArr.empty() && aebArr.empty()) {
        displayError("Error: No Files Created");
        return;
    } else {
        pf->writeFiles(returnProjectNumber(), dir);
        this->close();
    }
}

//Update Project Preview for AEB
void NewProject::updatePPpreview(AEB *aeb)
{
    QTreeWidgetItem *AEB = new QTreeWidgetItem(ui->treeWidget_3);
    AEB->setText(0, "File Name:");
    AEB->setText(1,"C" + aeb->AEB_Id + "_00.ADC");

    QTreeWidgetItem *version = new QTreeWidgetItem(AEB);
    version->setText(0, "Version:");
    version->setText(1, "1");

    QTreeWidgetItem *eval = new QTreeWidgetItem(AEB);
    eval->setText(0, "Evaluator Name:");
    eval->setText(1, aeb->evaluatorName);

    QTreeWidgetItem *aebid = new QTreeWidgetItem(AEB);
    aebid->setText(0, "AEB ID:");
    aebid->setText(1, aeb->AEB_Id);

    if(aeb->fmaArr.size() > 0 ){
        for(int i = 0; i < aeb->fmaArr.size(); i++){
            QTreeWidgetItem *fma = new QTreeWidgetItem(AEB);
            fma->setText(0, "FMA:" );
            fma->setText(0, QString::number(i+1));

            QTreeWidgetItem *tsn = new QTreeWidgetItem(fma);
            tsn->setText(0, "Track Section ID:");
            tsn->setText(1, aeb->fmaArr[i]->returnTrackSectionId());
            for(int j = 0; j < aeb->fmaArr[i]->returnAxleHeads().size(); j++) {
                QTreeWidgetItem *fmaAeb = new QTreeWidgetItem(fma);
                QTreeWidgetItem *dir = new QTreeWidgetItem(fmaAeb);

                fmaAeb->setText(0, aeb->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID);
                fmaAeb->setText(0, aeb->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID);
                dir->setText(0, "Direction Inversion:");
                dir->setText(1, aeb->fmaArr[i]->returnAxleHeads()[j]->directionInversion== 1 ? "Yes" : "No");
            }
        }
    }
}

//Update Project Preview for COM
void NewProject::updatePPpreview(COM *com)
{
    //Parent COM Item
    QTreeWidgetItem *comItem = new QTreeWidgetItem(ui->treeWidget_2);
    comItem->setText(0, "File Name:");
    comItem->setText(1,"C" + com->COM_ID + "_00.ADC");

    //Basic Configuration
    QTreeWidgetItem *basicConf = new QTreeWidgetItem(comItem);
    basicConf->setText(0, "Basic Configuration");

    QTreeWidgetItem *ip_basic_a = new QTreeWidgetItem(basicConf);
    ip_basic_a->setText(0, "IP A");
    for(int i = 0; i < com->IP_A_BASIC.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(ip_basic_a);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_A_BASIC[i]));
    }

    QTreeWidgetItem *ip_basic_b = new QTreeWidgetItem(basicConf);
    ip_basic_b->setText(0, "IP B");
    for(int i = 0; i < com->IP_B_BASIC.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(ip_basic_b);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_B_BASIC[i]));
    }

    QTreeWidgetItem *ip_dg_a = new QTreeWidgetItem(basicConf);
    ip_dg_a->setText(0, "Default Gateway A");
    for(int i = 0; i < com->IP_A_BASIC_DG.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(ip_dg_a);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_A_BASIC_DG[i]));
    }

    QTreeWidgetItem *ip_dg_b = new QTreeWidgetItem(basicConf);
    ip_dg_b->setText(0, "Default Gateway B");
    for(int i = 0; i < com->IP_B_BASIC_DG.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(ip_dg_b);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_B_BASIC_DG[i]));
    }

    QTreeWidgetItem *ip_sa= new QTreeWidgetItem(basicConf);
    ip_sa->setText(0, "Subnet Mask A");

    QTreeWidgetItem *ip_sb = new QTreeWidgetItem(basicConf);
    ip_sb->setText(0, "Subnet Mask B");


    //Remote Destination
    QTreeWidgetItem *rd = new QTreeWidgetItem(comItem);
    rd->setText(0, "Remote Destination");
    QTreeWidgetItem *fds = new QTreeWidgetItem(rd);
    fds->setText(0, "FDS");
    QTreeWidgetItem *fds_ipa = new QTreeWidgetItem(fds);
    fds_ipa->setText(0, "IP A");
    for(int i = 0; i < com->IP_RD_FDS_A.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(fds_ipa);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_RD_FDS_A[i]));
    }
    QTreeWidgetItem *fds_ipb = new QTreeWidgetItem(fds);
    fds_ipb->setText(0, "IP A");
    for(int i = 0; i < com->IP_RD_FDS_B.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(fds_ipb);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_RD_FDS_B[i]));
    }

    QTreeWidgetItem *tf = new QTreeWidgetItem(rd);
    tf->setText(0, "TF");
    QTreeWidgetItem *tf_ipa = new QTreeWidgetItem(tf);
    tf_ipa->setText(0, "IP A");
    for(int i = 0; i < com->IP_RD_TF_A.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(tf_ipa);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_RD_TF_A[i]));
    }
    QTreeWidgetItem *tf_ipb = new QTreeWidgetItem(tf);
    tf_ipb->setText(0, "IP A");
    for(int i = 0; i < com->IP_RD_TF_B.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(tf_ipa);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->IP_RD_TF_B[i]));
    }

    QTreeWidgetItem *eval = new QTreeWidgetItem(rd);
    eval->setText(0, "Evaluators");
    for(int i = 0; i < com->IP_RD_EVAL_ARR.size(); i++) {
        QTreeWidgetItem *evalObj = new QTreeWidgetItem(eval);
        evalObj->setText(0, "Evaluator");
        evalObj->setText(1, QString::number(i+1));
        QTreeWidgetItem *eval_ipa = new QTreeWidgetItem(evalObj);
        eval_ipa->setText(0, "IP A");
        for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->IP_A.size(); j++) {
            QTreeWidgetItem *byte = new QTreeWidgetItem(eval_ipa);
            byte->setText(0, "Byte " + QString::number(i+1));
            byte->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->IP_A[j]));
        }
        QTreeWidgetItem *eval_ipb = new QTreeWidgetItem(evalObj);
        eval_ipb->setText(0, "IP B");
        for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->IP_B.size(); j++) {
            QTreeWidgetItem *byte = new QTreeWidgetItem(eval_ipa);
            byte->setText(0, "Byte " + QString::number(i+1));
            byte->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->IP_B[j]));
        }
        QTreeWidgetItem *dataForwarding = new QTreeWidgetItem(evalObj);
        dataForwarding->setText(0, "Data Forwarding");
        for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->dataForwardingArr.size(); j++) {
            QTreeWidgetItem *dfObj = new QTreeWidgetItem(dataForwarding);

            QTreeWidgetItem *sender = new QTreeWidgetItem(dfObj);
            QTreeWidgetItem *receiver = new QTreeWidgetItem(dfObj);
            QTreeWidgetItem *trackSection = new QTreeWidgetItem(dfObj);

            trackSection->setText(0,"Track Section");
            trackSection->setText(1, com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->trackSection_id);
            sender->setText(0,"Sender AEB ID");
            sender->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_sender_id));
            receiver->setText(0,"Receiver AEB ID");
            receiver->setText(1, QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_receiver_id));
        }
    }

    //Remote System
    QTreeWidgetItem *rs = new QTreeWidgetItem(comItem);
    rs->setText(0, "Remote System (IXL)");

    QTreeWidgetItem *primary = new QTreeWidgetItem(rs);
    primary->setText(0, "Primary Network");
    QTreeWidgetItem *primary_ipa = new QTreeWidgetItem(primary);
    primary_ipa->setText(0, "IP A");
    for(int i = 0; i < com->ACP_Primary_IP_A.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(primary_ipa);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->ACP_Primary_IP_A[i]));
    }
    QTreeWidgetItem *primary_ipb = new QTreeWidgetItem(primary);
    primary_ipb->setText(0, "IP B");
    for(int i = 0; i < com->ACP_Primary_IP_B.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(primary_ipb);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->ACP_Primary_IP_B[i]));
    }

    QTreeWidgetItem *secondary = new QTreeWidgetItem(rs);
    secondary->setText(0, "Secondary Network");
    QTreeWidgetItem *secondary_ipa = new QTreeWidgetItem(secondary);
    secondary_ipa->setText(0, "IP A");
    for(int i = 0; i < com->ACP_Primary_IP_B.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(secondary_ipa);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->ACP_Primary_IP_B[i]));
    }

    QTreeWidgetItem *secondary_ipb = new QTreeWidgetItem(secondary);
    secondary_ipb->setText(0, "IP B");
    for(int i = 0; i < com->ACP_Primary_IP_B.size(); i++) {
        QTreeWidgetItem *byte = new QTreeWidgetItem(secondary_ipb);
        byte->setText(0, "Byte " + QString::number(i+1));
        byte->setText(1, QString::number(com->ACP_Primary_IP_B[i]));
    }



    //WNC Message Addressing
    QTreeWidgetItem *wma = new QTreeWidgetItem(comItem);
    wma->setText(0, "WNC Message Addressing");

    QTreeWidgetItem *ntid = new QTreeWidgetItem(wma);
    ntid->setText(0, "WESTRACE ID");
    ntid->setText(1, QString::number(com->wnc_NetTraceID));

    com->wnc_WestTraceAddress = ui->lineEdit_7->text().toInt();
    QTreeWidgetItem *wta = new QTreeWidgetItem(wma);
    wta->setText(0, "WESTRACE Address");
    wta->setText(1, QString::number(com->wnc_WestTraceAddress));

    com->wnc_pdv = ui->lineEdit_8->text().toInt();
    QTreeWidgetItem *pdv = new QTreeWidgetItem(wma);
    pdv->setText(0, "Product Data Version (PDV)");
    pdv->setText(1, QString::number(com->wnc_pdv));

    com->wnc_DestPort = ui->lineEdit_9->text().toInt();
    QTreeWidgetItem *dport= new QTreeWidgetItem(wma);
    dport->setText(0, "Destination Port");
    dport->setText(1, QString::number(com->wnc_DestPort));

    //WNC Data Link
    QTreeWidgetItem *wdl = new QTreeWidgetItem(comItem);
    wdl->setText(0, "WNC Data Links");

    for(int i = 0; i < com->WNC_AEB_DATA_Arr.size(); i++) {
        QTreeWidgetItem *aebid = new QTreeWidgetItem(wdl);
        aebid->setText(0, "AEB ID");
        aebid->setText(1, QString::number(com->WNC_AEB_DATA_Arr[i]->aeb_id));

        QTreeWidgetItem *ttt = new QTreeWidgetItem(aebid);
        ttt->setText(0, "Transfer To Treadle");
        ttt->setText(1, com->WNC_AEB_DATA_Arr[i]->transfer_to_treadle ? "True" : "False");
        QTreeWidgetItem *tf1 = new QTreeWidgetItem(aebid);
        tf1->setText(0, "Transfer FMA1 to IXL");
        tf1->setText(1, com->WNC_AEB_DATA_Arr[i]->transfer_FMA1_to_IXL ? "True" : "False");
        QTreeWidgetItem *tf2 = new QTreeWidgetItem(aebid);
        tf2->setText(0, "Transfer FMA2 to IXL");
        tf2->setText(1, com->WNC_AEB_DATA_Arr[i]->transfer_FMA2_to_IXL ? "True" : "False");
    }
}

//Displays a dialog box with the error message
void NewProject::displayError(QString errorMessage)
{
    QMessageBox::StandardButton error;
    error = QMessageBox::information(this,"ERROR:", errorMessage);
}

//Checks if the value is a single digit and adds a 0 infront of the string
QString NewProject::timeCheck(QString time) {
    if (time.length() == 1) {
        return "0" + time;
    }
    else {
        return time;
    }
}

//Retuns a time stamp for when the file was created
QString NewProject::returnGenTime()
{
    time_t now = time(nullptr);
    tm ltm;
    localtime_s(&ltm, &now);

    QString year = QString::number(1900 + ltm.tm_year);
    QString month = timeCheck(QString::number(1 + ltm.tm_mon));
    QString day = timeCheck(QString::number(ltm.tm_mday));
    QString time = timeCheck(QString::number(ltm.tm_hour) + QString::number(ltm.tm_min));

    return year + month + day + time;
}

//Returns the AEB Files for exporting
QVector<AEB *> NewProject::returnAEBarr()
{
    return aebArr;
}

//Returns the COM Files for exporting
QVector<COM *> NewProject::returnCOMarr()
{
    return comArr;
}

//Returns the project number (used for when exporting)
QString NewProject::returnProjectNumber()
{
    return ui->lineEdit->text();
}

//---------------------------------------------------------------------------------------------//
//---------------------------------END GENERAL FUNCTION AREA--------------------------------------//
//---------------------------------------------------------------------------------------------//
