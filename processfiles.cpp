#include "processfiles.h"
#include <QFileDialog>
ProcessFiles::ProcessFiles(NewProject*np)
{
    aebArr = np->returnAEBarr();
    comArr = np->returnCOMarr();
    orgAEB_File = setOriginalFile(":/NR_WNC_R2_AEB_FMA1.ADC");
    orgCOM_File = setOriginalFile("NR_WNC_R2_COM_standalone.ADC");
}

QString ProcessFiles::timeCheck(QString time) {
    if (time.length() == 1) {
        return "0" + time;
    }
    else {
        return time;
    }
}

QString ProcessFiles::returnStamp() {
    time_t now = time(nullptr);
    tm ltm;
    localtime_s(&ltm, &now);

    QString year = QString::number(1900 + ltm.tm_year);
    QString month = timeCheck(QString::number(1 + ltm.tm_mon));
    QString day = timeCheck(QString::number(ltm.tm_mday));
    QString time = timeCheck(QString::number(ltm.tm_hour) + QString::number(ltm.tm_min));

    return year + month + day + time;
}

QVector<QString> ProcessFiles::setOriginalFile(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    QVector<QString> fileout;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << file.errorString();


    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        qDebug() << line;
        fileout.push_back(line);
    }

    return fileout;
}

void ProcessFiles::writeFiles(QString projNum, QString dir)
{
    QString fname = "Project_" + projNum;
    QDir directory(dir);
    directory.mkdir(fname);
    QString path = dir + "/" + fname;
    QString fileNames;
    qDebug() << "writing files";
    for(int i = 0; i < aebArr.size(); i++) {
        QVector<QString> file = returnEdited_AEB(aebArr[i], projNum);
        QFile outfile(path + "/C" + aebArr[i]->AEB_Id + "_00.ADC");
        if(!outfile.open(QFile::WriteOnly | QFile::Text))
        {
            qDebug() << " Could not open file for writing";
            return;
        }

        for(QString line : file ) {
            QTextStream out(&outfile);
            out << line + "\n";

        }
        fileNames+=QString::number(i + 1) + ". C" + aebArr[i]->AEB_Id + "_00.ADC\n";
        outfile.flush();
        outfile.close();
    }

    QMessageBox::StandardButton error;
    error = QMessageBox::information(this,"Complete:", "Successfully created: \n\n" + fileNames);
}

QVector<QString> ProcessFiles::returnEdited_AEB(AEB *aebData, QString pn)
{
    qDebug() << "Editing File";
    std::vector<QString> infile = orgAEB_File.toStdVector();

    qDebug() << "Editing General Data";
    int pos = infile[0].indexOf("Cn");
    QString stamp = returnStamp();

    infile[0].replace(pos+1, 4, aebData->AEB_Id);
    infile[1].replace(infile[1].indexOf("v") + 1, 1, "1");
    infile[2].replace(infile[2].indexOf("name"), 6, aebData->evaluatorName);
    infile[13].replace(infile[13].indexOf("nnnn"), 4, aebData->AEB_Id);
    infile[13].replace(infile[13].indexOf("nnnn"), 4, aebData->AEB_Id);

    infile[185].replace(infile[185].lastIndexOf("NUM"), 4, pn);
    qDebug() << "edited project number";
    infile[192].replace(infile[192].lastIndexOf(":") + 1, 12, "0x" + stamp);
    qDebug() << "edited generatime time";

    for(int i = 0; i < aebData->fmaArr.size(); i++) {
        qDebug() << "Adding FMA";
        for(int j = 0; j < aebData->fmaArr[i]->returnAxleHeads().size(); j++) {
            qDebug() << "Adding Axle Counter Data ";
            std::vector<QString> axleHeadInfo{ "//ZP","[CONFIG]","CFG_ZP_FMA" + QString::number(i + 1) + "            7:" + QString::number(i + 1),"TYPE_PRTCT             1 : 0      //FIXED no type protection","DIR_INV                1 : B      //0 = no inversion, 1 = inversion","SLCT_TIMEOUT           3 : 0      //FIXED timeout 0 used","2ID                    12 :      //ID of ZP required in track section \n" };

            //finding positions of where the data should be placed
            int idPos = axleHeadInfo[6].indexOf(":");
            int invPos = axleHeadInfo[4].indexOf(":");

            //configuring the axle head id for comment, id, and whether or not direction inversion is opted in
            //comment
            axleHeadInfo[0] += aebData->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID;
            //id
            axleHeadInfo[6].replace(idPos + 2, 4, aebData->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID);
            //direction inversion
            axleHeadInfo[4].replace(invPos + 2, 1, aebData->fmaArr[i]->returnAxleHeads()[j]->directionInversion ? "1":"0");

            qDebug() << "Inserting axle head block to AEB";
            infile.insert(infile.begin() + 178 ,axleHeadInfo.begin(),axleHeadInfo.end());

            qDebug() << "End of iteration";
        }
        qDebug() << "finished adding axle counters";
   }

   qDebug() << "converting std::vector to qvector";
   QVector<QString> outfile = QVector<QString>::fromStdVector(infile);

   return outfile;

}


