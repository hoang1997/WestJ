#include "newproject.h"
#include "ui_newproject.h"
#include "processfiles.h"
NewProject::NewProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProject)
{
    ui->setupUi(this);
}

NewProject::~NewProject()
{
    delete ui;
}

void NewProject::displayError(QString errorMessage)
{
    QMessageBox::StandardButton error;
    error = QMessageBox::information(this,"ERROR:", errorMessage);
}

QString NewProject::timeCheck(QString time) {
    if (time.length() == 1) {
        return "0" + time;
    }
    else {
        return time;
    }
}

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

QVector<AEB *> NewProject::returnAEBarr()
{
    return aebArr;
}

QVector<COM *> NewProject::returnCOMarr()
{
    return comArr;
}

QString NewProject::returnProjectNumber()
{
    return ui->lineEdit->text();
}

void NewProject::fmaSignalReceived(FMA * fma)
{
    aeb = new AEB;
    aeb->fmaArr.push_back(fma);
    updateFMApreview(fma);
}

void NewProject::updateFMApreview(FMA* fma) {
    QTreeWidgetItem *FMA = new QTreeWidgetItem(ui->treeWidget);
    FMA->setText(0, "FMA ID: " + fma->returnTrackSectionId());

    for(int i = 0; i < fma->returnAxleHeads().size(); i++){
        QTreeWidgetItem *ah = new QTreeWidgetItem(FMA);
        ah->setText(0, "Axle Head: " + fma->returnAxleHeads()[i]->axleHead_ID);
        QTreeWidgetItem *dir_inv = new QTreeWidgetItem(ah);
        dir_inv->setText(0, (fma->returnAxleHeads()[i]->directionInversion == 1) ? "Direction Inversion: Yes" : "Direction Inversion: No");
    }
}

void NewProject::updatePPpreview(AEB *aeb)
{
    ui->treeWidget_3->setHeaderLabel("AEB Files");

    QTreeWidgetItem *AEB = new QTreeWidgetItem(ui->treeWidget_3);
    AEB->setText(0, "File Name: C" + aeb->AEB_Id + "_00.ADC");

    QTreeWidgetItem *version = new QTreeWidgetItem(AEB);
    version->setText(0, "Version: 1");

    QTreeWidgetItem *eval = new QTreeWidgetItem(AEB);
    eval->setText(0, "Evaluator Name: "  + aeb->evaluatorName);

    QTreeWidgetItem *aebid = new QTreeWidgetItem(AEB);
    aebid->setText(0, "AEB ID: "  + aeb->AEB_Id);

    for(int i = 0; i < aeb->fmaArr.size(); i++){
        QTreeWidgetItem *fma = new QTreeWidgetItem(AEB);
        fma->setText(0, "FMA " + QString::number(i+1));

        QTreeWidgetItem *tsn = new QTreeWidgetItem(fma);
        tsn->setText(0, "Track Section ID: " + aeb->fmaArr[i]->returnTrackSectionId());
        for(int j = 0; j < aeb->fmaArr[i]->returnAxleHeads().size(); j++) {
            QTreeWidgetItem *fmaAeb = new QTreeWidgetItem(fma);
            QTreeWidgetItem *dir = new QTreeWidgetItem(fmaAeb);

            fmaAeb->setText(0, aeb->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID);
            dir->setText(0, aeb->fmaArr[i]->returnAxleHeads()[j]->directionInversion== 1 ? "Direction Inversion: Yes" : "Direction Inversion: No");
        }
    }
}

void NewProject::on_pushButton_4_clicked()
{
    this->close();
}

void NewProject::on_pushButton_clicked()
{
    if(fmaCounter > 2) {
        displayError("Cannot Exceed More than 2 FMA's");
        return;
    }
    else {
        fma = new FMA();
        connect(fma, SIGNAL(fmaSignal(FMA*)), this, SLOT(fmaSignalReceived(FMA*)), Qt::UniqueConnection);
        fma->exec();
        fmaCounter++;
        ui->pushButton->setText("Insert FMA " + QString::number(fmaCounter));
    }
}

void NewProject::on_pushButton_2_clicked()
{
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
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

    aeb->generationTime = returnGenTime();

    aebArr.push_back(aeb);
    updatePPpreview(aeb);
    ui->treeWidget->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_4->clear();


}

void NewProject::on_pushButton_3_clicked()
{
    ProcessFiles *pf = new ProcessFiles(this);

    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    QString dir;
    if (re.exactMatch(ui->lineEdit->text()) && ui->lineEdit->text().size() == 3 ) {
        dir = QFileDialog::getExistingDirectory(this,  tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    }
    else {
        displayError("Incorrect Input: Project Number must not include letters and only contain 3 digits");
        return;
    }
    qDebug() << dir;
    pf->writeFiles(returnProjectNumber(), dir);
}
