#include "processfiles.h"
#include <QFileDialog>
ProcessFiles::ProcessFiles(NewProject*np)
{
    aebArr = np->returnAEBarr();
    comArr = np->returnCOMarr();
    orgAEB_File = setOriginalFile(":/NR_WNC_R2_AEB_FMA1.ADC");
    orgCOM_File = setOriginalFile(":/NR_WNC_R2_COM_standalone.ADC");
}

//checks if there is a single digit, and if there is then it will append with a 0
QString ProcessFiles::timeCheck(QString time) {
    if (time.length() == 1)
        return "0" + time;
    else
        return time;
}

//Returns time stamp
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

//sets template file
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

        fileout.push_back(line);
    }
    return fileout;
}

//creates a project folder, and creates the aeb and com files if there are any
void ProcessFiles::writeFiles(QString projNum, QString dir)
{
    //Create project folder name
    QString fname = "Project_" + projNum;
    //Gets Directory
    QDir directory(dir);
    //Creates the project folder in the directory
    directory.mkdir(fname);
    //Creates path in the newly created folder
    QString path = dir + "/" + fname;
    //File names for dialog box at the end
    QString fileNames;
    qDebug() << "writing files";

    //check if AEB data is empty or not
    if(!aebArr.empty()) {
        //loop through each AEB data created
        for(int i = 0; i < aebArr.size(); i++) {
            //create AEB File
            QVector<QString> file = returnEdited_AEB(aebArr[i], projNum);
            //Create File and File Name
            QFile outfile(path + "/C" + aebArr[i]->AEB_Id + "_00.ADC");
            //Check to see if the file can be opened, return an error if not
            if(!outfile.open(QFile::WriteOnly | QFile::Text)){
                qDebug() << " Could not open file for writing";
                return;
            }

            //loop through each line of the newly created file and
            //stream through the lines into the text file
            for(QString line : file ) {
                QTextStream out(&outfile);
                out << line + "\n";
            }
            //append the file name
            fileNames+=QString::number(i + 1) + ". C" + aebArr[i]->AEB_Id + "_00.ADC\n";

            //flush and close the file, so that the stream can be used again in the next iteration
            outfile.flush();
            outfile.close();
        }
    }
    else {
        qDebug() << "No AEB Files Detected";
    }

    //Same concept with previous AEB data
    if(!comArr.empty()){
        for(int i = 0; i < comArr.size(); i++) {
            QVector<QString> file = returnEdited_COM(comArr[i], projNum);
            QFile outfile(path + "/C" + comArr[i]->COM_ID + "_00.ADC");
            if(!outfile.open(QFile::WriteOnly | QFile::Text))
            {
                qDebug() << " Could not open file for writing";
                return;
            }
            for(QString line : file ) {
                QTextStream out(&outfile);
                out << line + "\n";

                qDebug() << line;

            }
            fileNames+=QString::number(i + 1) + ". C" + comArr[i]->COM_ID + "_00.ADC\n";
            outfile.flush();
            outfile.close();
        }
    }
    else {
        qDebug()<<"No COM File Detected";
    }

    QMessageBox::StandardButton error;
    error = QMessageBox::information(this,"Complete:", "Successfully created: \n\n" + fileNames);
}

//Takes in AEB data and edits the template to return an array of strings which is the file
QVector<QString> ProcessFiles::returnEdited_AEB(AEB *aebData, QString pn)
{
    qDebug() << "Editing File";
    std::vector<QString> infile = orgAEB_File.toStdVector();
    qDebug() << aebData->AEB_Id;
    qDebug() << "Editing General Data";
    int pos = infile[0].indexOf("Cn");
    QString stamp = returnStamp();

    //Editing Header
    infile[0].replace(pos+1, 4, aebData->AEB_Id);
    qDebug() << aebData->AEB_Id;
    infile[1].replace(infile[1].indexOf("v") + 1, 1, "1");
    infile[2].replace(infile[2].indexOf("name"), 6, aebData->evaluatorName);
    infile[13].replace(infile[13].indexOf("nnnn"), 4, aebData->AEB_Id);

    infile[185].replace(infile[185].lastIndexOf("NUM"), 4, pn);
    qDebug() << "edited project number";
    infile[192].replace(infile[192].lastIndexOf(":") + 1, 12, "0x" + stamp);
    qDebug() << "edited generatime time";

    for(int i = 0; i < aebData->fmaArr.size(); i++) {
        qDebug() << "Adding FMA";
        for(int j = 0; j < aebData->fmaArr[i]->returnAxleHeads().size(); j++) {
            qDebug() << "Adding Axle Counter Data ";
            std::vector<QString> axleHeadInfo{ "//ZP","[CONFIG]","CFG_ZP_FMA" + QString::number(i + 1) + "            7:" + QString::number(i + 1),"TYPE_PRTCT             1:0      //FIXED no type protection","DIR_INV                1:B      //0 = no inversion, 1 = inversion","SLCT_TIMEOUT           3:0      //FIXED timeout 0 used","ID                    12:      //ID of ZP required in track section CONFIGURABLE\n" };

            //finding positions of where the data should be placed
            int idPos = axleHeadInfo[6].indexOf(":");
            int invPos = axleHeadInfo[4].indexOf(":");

            //configuring the axle head id for comment, id, and whether or not direction inversion is opted in
            //comment
            axleHeadInfo[0] += aebData->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID;
            //id
            axleHeadInfo[6].replace(idPos + 1, 4, aebData->fmaArr[i]->returnAxleHeads()[j]->axleHead_ID);
            //direction inversion
            if(aebData->fmaArr[i]->returnAxleHeads()[j]->directionInversion == 0) {
                 axleHeadInfo[4].replace(invPos + 1, 1, "0");
            } else {
                 axleHeadInfo[4].replace(invPos + 1, 1, "1");
            }

            qDebug() << "Inserting axle head block to AEB";
            infile.insert(infile.begin() + 178 ,axleHeadInfo.begin(),axleHeadInfo.end());

            qDebug() << "End of iteration";
        }
        //Insert the track section info
        infile.insert(infile.begin() + 178 , "//Configure FMA"+QString::number(i+1)+" - Track Section " + aebData->fmaArr[i]->returnTrackSectionId());
        qDebug() << "finished adding axle counters";
   }

   qDebug() << "converting std::vector to qvector";
   QVector<QString> outfile = QVector<QString>::fromStdVector(infile);
   return outfile;

}

//Takes in the file in the format of an array of strings, takes the array of IP values and takes the starting position
//purpose is to quickly edit the areas with IP addresses
std::vector<QString> ProcessFiles::returnEditedBlock_IP(std::vector<QString> infile, QVector<int> IP_Arr, size_t starting_pos)
{
    for(int i = 0; i < IP_Arr.size(); i++){
        infile[starting_pos].replace(infile[starting_pos].indexOf(":") + 1, 8, QString::number(IP_Arr[i]));
        qDebug() << infile[starting_pos];
        starting_pos++;
    }

    return infile;
}

//Edits the template with COM data and then returns an array of strings
//which can be used to write to a text file.
QVector<QString> ProcessFiles::returnEdited_COM(COM * com, QString projectNum)
{
    std::vector<QString> infile = orgCOM_File.toStdVector();
    //Header -- WORKING
    infile[0].replace(infile[0].indexOf("Cn") + 1, 4, com->COM_ID);
    infile[1].replace(infile[1].indexOf("v") + 1, 4, QString::number(1));
    infile[2].replace(infile[2].indexOf("name"), 4, com->Evaluator_Name);
    infile[15].replace(infile[15].indexOf("nnnn"), 4, com->COM_ID);
    qDebug() << "Finished Header";

    //Footer - WORKING
    infile[280].replace(infile[280].lastIndexOf("NUM"), 4, projectNum);
    infile[287].replace(infile[287].lastIndexOf(":") + 1, 12, "0x" + returnStamp());
    qDebug() << "Finished Footer";

    //Basic Configuration - WORKING
    infile = returnEditedBlock_IP(infile, com->IP_A_BASIC, 24);
    infile = returnEditedBlock_IP(infile, com->IP_B_BASIC, 32);
    infile[41].replace(infile[41].indexOf(":") + 1, 3, QString::number(com->IP_A_BASIC_SM));
    infile[43].replace(infile[43].indexOf(":") + 1, 3, QString::number(com->IP_B_BASIC_SM));
    infile = returnEditedBlock_IP(infile, com->IP_A_BASIC_DG, 48);

    infile = returnEditedBlock_IP(infile, com->IP_B_BASIC_DG, 56);
    qDebug() << "Finished Basic Configuration";

    //Remote System - Working
    infile[149].replace(infile[149].indexOf("iiii"), 5, com->COM_ID);
    infile = returnEditedBlock_IP(infile, com->ACP_Primary_IP_A, 152);

    infile[157].replace(infile[157].indexOf("iiii"), 5, com->COM_ID);
    infile = returnEditedBlock_IP(infile, com->ACP_Primary_IP_B, 160);

    infile[165].replace(infile[165].indexOf("iiii"), 5, com->COM_ID);
    infile = returnEditedBlock_IP(infile, com->ACP_Secondary_IP_A, 168);

    infile[173].replace(infile[173].indexOf("iiii"), 5, com->COM_ID);
    infile = returnEditedBlock_IP(infile, com->ACP_Secondary_IP_B, 176);
    qDebug() << "Finished Remote System";


    //WNC Addressing - Working
    infile[198].replace(infile[198].indexOf("iiii"), 4, QString::number(com->wnc_NetTraceID));
    infile[203].replace(infile[203].indexOf(":")+1, 4, QString::number(com->wnc_WestTraceAddress));
    infile[210].replace(infile[210].indexOf(":xx")+1, 4, QString::number(com->wnc_pdv));
    infile[215].replace(infile[215].indexOf(":xx")+1, 4, QString::number(com->wnc_DestPort));
    qDebug() << "Finished WNC Addressing";

    //WNC Link - Working
    for(int i = 0; i < com->WNC_AEB_DATA_Arr.size(); i++) {
        std::vector<QString> aebDataTemplate { "//TRACK SECTION     tttt (FMA1) / tttt (FMA2) / *treadlename*","[CONFIG]"
            ,"CFG_APPDATA_FADC       8:64"
            ,(com->WNC_AEB_DATA_Arr[i]->transfer_to_treadle) ? "ZP                     1:1 //Transfer Treadle to IXL    CONFIGURABLE" : "ZP                     1:0 //Transfer Treadle to IXL   CONFIGURABLE"
            ,"SPEED                  1:0          //FIXED do not send (prohibited by Frauscher)"
            ,"WHEEL_DIAMETER         1:0          //FIXED do not send (prohibited by Frauscher)"
            ,"IO_DATA                1:0          //FIXED do not send (prohibited by Frauscher"
            ,"RESERVED               4:0"
            ,(com->WNC_AEB_DATA_Arr[i]->transfer_FMA1_to_IXL) ? "FMA1                   1:1          //Do(1) or do not(0) transfer FMA1 to IXL  CONFIGURABLE" : "FMA1                   1:0          //Do(1) or do not(0) transfer FMA1 to IXL  CONFIGURABLE"
            ,"FMA1_COUNT             1:0          //FIXED do not send (prohibited by Frauscher)"
            ,"FMA1_TRAIN_LENGTH      1:0          //FIXED do not send (prohibited by Frauscher)"
            ,(com->WNC_AEB_DATA_Arr[i]->transfer_FMA2_to_IXL) ? "FMA2                   1:1          //Do(1) or do not(0) transfer FMA2 to IXL CONFIGURABLE" : "FMA2                   1:0          //Do(1) or do not(0) transfer FMA2 to IXL    CONFIGURABLE"
            ,"FMA2_COUNT             1:0          //FIXED do not send (prohibited by Frauscher)"
            ,"FMA2_TRAIN_LENGTH      1:0          //FIXED do not send (prohibited by Frauscher)"
            ,"RESERVED               3:0"
            ,"SLCT_TIMEOUT           3:0          //FIXED Timeout 0 used"
            ,"CAN_TX_ID             12:"+QString::number(com->WNC_AEB_DATA_Arr[i]->aeb_id)+"       //CAN sender ID (ID of the AEB to be output) CONFIGURABLE \n"};

         infile.insert(infile.begin() + 244 , aebDataTemplate.begin(), aebDataTemplate.end());
    }
    qDebug() << "Finished WNC Link";


    //Remote Destination - Working
    infile = returnEditedBlock_IP(infile, com->IP_RD_FDS_A, 75);
    infile = returnEditedBlock_IP(infile, com->IP_RD_FDS_B, 83);
    infile = returnEditedBlock_IP(infile, com->IP_RD_TF_A, 105);
    infile = returnEditedBlock_IP(infile, com->IP_RD_TF_B, 113);

    for(int i =0 ; i < com->IP_RD_EVAL_ARR.size(); i++) {
        int num = 3;
        std::vector<QString> evalTemplate {"//-------------------------------------------------------------------------------------",
            "//Configure remote destination (INT_ID_DEST=" +QString::number(num+i)+") CONFIGURABLE\n",
            "//Destination IP address Network A",
            "[CONFIG]",
            "CFG_INT_ID_DEST_NW1    8:"+QString::number(34+i)+ "         //FIXED Net A IP address for INT_ID_DEST=2",
            "DEST_IP_INT_ID_NW1_B1  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_A[0])+"        //Destination IP address Net A, byte 1    CONFIGURABLE",
            "DEST_IP_INT_ID_NW1_B2  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_A[1])+"        //Destination IP address Net A, byte 2    CONFIGURABLE",
            "DEST_IP_INT_ID_NW1_B3  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_A[2])+"        //Destination IP address Net A, byte 3    CONFIGURABLE",
            "DEST_IP_INT_ID_NW1_B4  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_A[3])+"        //Destination IP address Net A, byte 4    CONFIGURABLE\n",
            "//Destination IP address Network B",
            "[CONFIG]",
            "CFG_INT_ID_DEST_NW2    8:"+QString::number(50+i)+ "         //FIXED Net B IP address for INT_ID_DEST=2",
            "DEST_IP_INT_ID_NW2_B1  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_B[0])+"        //Destination IP address Net B, byte 1    CONFIGURABLE",
            "DEST_IP_INT_ID_NW2_B2  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_B[1])+"        //Destination IP address Net B, byte 2    CONFIGURABLE",
            "DEST_IP_INT_ID_NW2_B3  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_B[2])+"        //Destination IP address Net B, byte 3    CONFIGURABLE",
            "DEST_IP_INT_ID_NW2_B4  8:"+QString::number(com->IP_RD_EVAL_ARR[i]->IP_B[3])+"        //Destination IP address Net B, byte 4    CONFIGURABLE"};

        for(int j = 0; j < com->IP_RD_EVAL_ARR[i]->dataForwardingArr.size(); j++) {
            std::vector<QString> dfTemplate {"//Data from ZP"+ QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_sender_id) + " required by ZP"+QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_receiver_id)+"for track section " + com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->trackSection_id,
                        "[CONFIG]",
                        "CFG_FWRD_ACD           8:9          //FIXED forwarding of axle counter data",
                        "INT_ID_DEST            4:2          //Internal destination to send data to",
                        "CAN_TX_ID             12:"+QString::number(com->IP_RD_EVAL_ARR[i]->dataForwardingArr[j]->aeb_sender_id)+"       //ID of AEB which transmits the count  CONFIGURABLE"};
            evalTemplate.insert(evalTemplate.end() ,dfTemplate.begin(),dfTemplate.end());
        }
        infile.insert(infile.begin() + 130, evalTemplate.begin(), evalTemplate.end());
    }
    qDebug() <<"Finished Remote Destination";

    QVector<QString> outfile = QVector<QString>::fromStdVector(infile);
    return outfile;
}


