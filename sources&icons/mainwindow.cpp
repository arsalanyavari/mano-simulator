#include<QFileDialog>
#include<fstream>
#include<QFile>
#include<QMessageBox>
#include<sstream>
#include <ctype.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
QString emptystring = "";
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ARYA Mano Simulator");
    ui->editor->setFocus();
    on_reset_btn_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::emptyTable()
{
    for(int v=0;v<4096;v++)
    {
        QString result = QString::number( v, 16 ).toUpper();
        QTableWidgetItem *itm = new QTableWidgetItem();
        QTableWidgetItem *empty = new QTableWidgetItem();
        itm->setText(result);
        empty->setText(0000);
        ui->ram_tb->insertRow(v);
        ui->ram_tb->setItem(v,1,itm);
        ui->ram_tb->setItem(v,3,empty);

    }
}

void MainWindow::resetRam()
{
    for(int i=0;i<4096;i++)
    {
        ram[i].reset();
    }
}

void MainWindow::arithmeticUnitADD()
{
    std::bitset<1> cOut(0);
    std::bitset<16> sum(0);

    for(quint16 i = 0; i < AC.size(); i++){
        sum[i] = AC[i] ^ DR[i] ^ cOut[0];
        cOut = cOut[0] & (AC[i] ^ DR[i]) | (AC[i] & DR[i]);
    }

    E = cOut;
    AC = sum;
}

void MainWindow::printReg()
{
    ui->sc_line->setText(QString::number( SC.to_ulong(), 16 ).toUpper());
    ui->pc_line->setText(QString::number( PC.to_ulong(), 16 ).toUpper());
    ui->ar_line->setText(QString::number( AR.to_ulong(), 16 ).toUpper());
    ui->ir_line->setText(QString::number( IR.to_ulong(), 16 ).toUpper());
    ui->dr_line->setText(QString::number( DR.to_ulong(), 16 ).toUpper());
    ui->ac_line->setText(QString::number( AC.to_ulong(), 16 ).toUpper());
    ui->tr_line->setText(QString::number( TR.to_ulong(), 16 ).toUpper());
    ui->inpr_line->setText(QString::number( INPR.to_ulong(), 16 ).toUpper());
    ui->outr_line->setText(QString::number( OUTR.to_ulong(), 16 ).toUpper());
    ui->i_line->setText(QString::number( I.to_ulong(), 16 ).toUpper());
    ui->s_line->setText(QString::number( S.to_ulong(), 16 ).toUpper());
    ui->e_line->setText(QString::number( E.to_ulong(), 16 ).toUpper());
    ui->r_line->setText(QString::number( R.to_ulong(), 16 ).toUpper());
    ui->ien_line->setText(QString::number( IEN.to_ulong(), 16 ).toUpper());
    ui->fgi_line->setText(QString::number( FGI.to_ulong(), 16 ).toUpper());
    ui->fgo_line->setText(QString::number( FGO.to_ulong(), 16 ).toUpper());
}

void MainWindow::printTable()
{
    if(reseter)
    {
        ui->ram_tb->setRowCount(0);
        for(int v=0;v<4096;v++)
        {
            QString address = QString::number( v, 16 ).toUpper();
            QTableWidgetItem *itmaddr = new QTableWidgetItem();
            QTableWidgetItem *itmHex = new QTableWidgetItem();
            itmaddr->setText(address);
            itmHex->setText("0000");
            ui->ram_tb->insertRow(v);
            ui->ram_tb->setItem(v,1,itmaddr);
            ui->ram_tb->setItem(v,3,itmHex);
            reseter=0;
        }
    }
    else
    {
        for(int v=0;v<4096;v++)
        {
            QString address = QString::number( v, 16 ).toUpper();
            QTableWidgetItem *itmaddr = new QTableWidgetItem();
            QTableWidgetItem *itmHex = new QTableWidgetItem();
            itmaddr->setText(address);
            QString checkerString =QString::number( ram[v].to_ulong(), 16 ).toUpper();
            if(checkerString.size()==3){
                checkerString = "0" + checkerString;
            }
            else if(checkerString.size()==2){
                checkerString = "00" + checkerString;
            }
            else if(checkerString.size()==1){
                checkerString = "000" + checkerString;
            }
            itmHex->setText(checkerString);
            ui->ram_tb->setItem(v,1,itmaddr);
            ui->ram_tb->setItem(v,3,itmHex);
        }
    }
}

bool MainWindow::isNumber(const QString &str)
{
    string check=str.toStdString();
    for (char const &c : check)
    {
        if(c=='a' || c=='b' || c=='c' || c=='d' || c=='e' || c=='f'|| c=='A' || c=='B' || c=='C' || c=='D' || c=='E' || c=='F'|| c=='-' || c=='+')
        {
            continue;
        }
        if (isdigit(c) == 0) return false;
    }
       return true;
}

void MainWindow::arithmeticUnitCIR() {
    E[0] = AC[0];
    AC = AC >> 1;
    AC[15] = E[0];
}

void MainWindow::arithmeticUnitCIL() {
    E[0] = AC[15];
    AC = AC << 1;
    AC[0] = E[0];
}

void MainWindow::on_action_Exit_triggered()
{
    QMessageBox::warning(this, tr("ARYA Application"),
                         tr("Do you want save your changes before exit?!!"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel, QMessageBox::Save);
    this->close();
}

void MainWindow::on_actionnext_step_triggered()
{
    on_next_btn_clicked();
}

void MainWindow::on_actionreset_code_triggered()
{
    on_reset_btn_clicked();
}

void MainWindow::on_savebtn_clicked()
{
    if(issaved==emptystring)
    {
        on_actionSave_as_triggered();
    }
    else
    {
        ofstream f;
        f.open(issaved.toLocal8Bit(),ios::out);
        f<<ui->editor->toPlainText().toStdString();
        f.flush();
        f.close();
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save"), "",tr("Mytext editor file (*.txt)"));
    if (fileName.isEmpty())
         return;
     else {
        ofstream f;
        f.open(fileName.toLocal8Bit(),ios::out);
        f<<ui->editor->toPlainText().toStdString();
        f.flush();
        f.close();
        issaved=fileName;
    }

}

void MainWindow::on_action_Save_triggered()
{
    on_savebtn_clicked();
}

void MainWindow::on_openbtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Text file"), "",tr("Myeditor file (*.txt)"));
    if (fileName.isEmpty())
        return;
    else {
        ui->editor->clear();
        issaved=fileName;
        ifstream infile;
        infile.open(fileName.toLocal8Bit());
        char tmp[300];
        while(infile.eof()!=true)
        {
            infile.getline(tmp,300);
            ui->editor->insertPlainText(QString::fromStdString(tmp));
            ui->editor->insertPlainText("\n");
            compiled=0;
        }
    }
}

void MainWindow::on_action_Open_triggered()
{
    on_openbtn_clicked();
    compiled=0;
}

void MainWindow::on_action_New_triggered()
{
    ui->editor->clear();
    issaved=emptystring;
    compiled=0;
}

void MainWindow::on_newbtn_clicked()
{
    on_action_New_triggered();
    compiled=0;
    on_reset_btn_clicked();
}

void MainWindow::on_reset_btn_clicked()
{
    reseter=1;
    SC.reset();
    PC.reset();
    AR.reset();
    IR.reset();
    DR.reset();
    AC.reset();
    TR.reset();
    INPR.reset();
    OUTR.reset();
    I.reset();
    S.reset();
    E.reset();
    R.reset();
    IEN.reset();
    FGI.reset();
    FGO.reset();
    resetRam();
    printTable();
    printReg();
    ui->console->setText("");
    allDatas.clear();
    firstallDatas.clear();    
    commands.clear();
    compiled=0;
    clk=0;
    lineStep=0;
    memorystep=0;
    run=0;
    printing=1;
    ui->run_btn->setEnabled(true);
    ui->next_btn->setEnabled(true);
    ui->in_line->setText("");
    ui->operation->setText("");
    memoryToLine.clear();
}

void MainWindow::on_actioncompile_program_triggered()
{
    on_compile_btn_clicked();
}

void MainWindow::on_actionRun_Program_triggered()
{
    on_run_btn_clicked();
}

void MainWindow::on_compile_btn_clicked()
{
    on_reset_btn_clicked();
    compiled=1;
    int lc=0;
    int lc1=0;
    ui->console->setText("");
    commands = ui->editor->toPlainText().split('\n', QString::SkipEmptyParts);
    tcommmands=commands.size();
    int endp1=0;
    for(int i=0;i<tcommmands;i++)
    {
        if(lc>4096)
        {
            ui->console->setText("ERROR: OPPS RAM Overfloweded!!!\n");
            compiled=0;
            break;
        }
        QStringList riz = commands.at(i).split(' ', QString::SkipEmptyParts);
        if(riz.at(0)[riz.at(0).size()-1]==',')
        {
            QString wait=emptystring;
            for(int cw=0;cw<riz.at(0).size()-1;cw++)
            {
                wait+=riz.at(0)[cw];
            }
            firstallDatas[wait]=lc1;
        }
        else if(riz.at(0)=="ORG")
        {
            if(isNumber(riz.at(1)))
            {
                bool ok=1;
                lc1=riz.at(1).toInt(&ok,16);
                continue;

            }
            else
            {
                ui->console->setText("error in line:"+QString::number(i+1)+"\nafter ORG, hexdecimal number neaded. \n");
                compiled=0;
                break;
            }
        }
        memoryToLine[lc1]=i;
        lc1++;
    }

    for(int i=0;i<tcommmands;i++)
    {
        if(lc>4096)
        {
            ui->console->setText("ERROR: OPPS RAM Overfloweded!!!\n");
            compiled=0;
            break;
        }

        QStringList riz = commands.at(i).split(' ', QString::SkipEmptyParts);
            if(riz.at(0)=="//" || riz.at(0)[0]=='/')
            {
                continue;
            }
            else if(riz.at(0)=="ORG")
            {
                if(isNumber(riz.at(1)))
                {
                    bool ok=1;
                    lc=riz.at(1).toInt(&ok,16);
                    continue;
                }
                else
                {
                    ui->console->setText("error in line:"+QString::number(i+1)+"\nafter ORG, hexdecimal number neaded. \n");
                    compiled=0;
                    break;
                }
            }
            else if(riz.at(0)[riz.at(0).size()-1]==',')
            {
                allDatas[riz.at(0)]=i;
                QTableWidgetItem *itmlabel = new QTableWidgetItem();
                itmlabel->setText(riz.at(0));
                ui->ram_tb->setItem(lc,0,itmlabel);
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                int convertNumber;
                if(riz.at(1)=="HEX")
                {
                    if(isNumber(riz.at(2)))
                    {
                        bool ok=1;
                        convertNumber=riz.at(2).toInt(&ok,16);
                        ram[lc]=convertNumber;
                        lc++;
                        continue;
                    }
                    else
                    {
                        ui->console->setText("error in line:"+QString::number(i+1)+"\nUnvalid number after HEX flag :/ \n");
                        compiled=0;
                        break;
                    }
                }
                else if(riz.at(1)=="DEC")
                {
                    if(isNumber(riz.at(2)))
                    {
                        bool ok=1;
                        convertNumber=riz.at(2).toInt(&ok,10);
                        ram[lc]=convertNumber;
                        lc++;
                        continue;
                    }
                    else
                    {
                        ui->console->setText("error in line:"+QString::number(i+1)+"\nUnvalid number after DEC flag :/ \n");
                        compiled=0;
                        break;
                    }
                }
                else
                {

                    for(int x=0;x<(riz.length())-1;x++)
                    {
                        riz[x]=riz[x+1];
                    }
                }

            }
            if(riz.at(0)=="DEC")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(riz.length()>1 && isNumber(riz.at(1)) )
                {
                    bool ok=1;
                    int convertNumber=riz.at(1).toInt(&ok,10);
                    ram[lc]=convertNumber;
                }
                else
                {
                    ui->console->setText("error in line:"+QString::number(i+1)+"\nUnvalid number after DEC flag :/ \n");
                    compiled=0;
                    break;
                }

            }
            else if(riz.at(0)=="HEX")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(riz.length()>1 && isNumber(riz.at(1)))
                {
                    bool ok=1;
                    int convertNumber=riz.at(1).toInt(&ok,16);
                    ram[lc]=convertNumber;
                }
                else
                {
                    ui->console->setText("error in line:"+QString::number(i+1)+"\nUnvalid number after HEX flag :/ \n");
                    compiled=0;
                    break;
                }
            }

            else if(riz.at(0)=="AND")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line:"+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;

                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0x8000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x0000+firstallDatas[riz.at(1)];
                }

            }
            else if(riz.at(0)=="ADD")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line:"+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;

                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0x9000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x1000+firstallDatas[riz.at(1)];
                }

            }

            else if(riz.at(0)=="LDA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line:"+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;
                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0xA000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x2000+firstallDatas[riz.at(1)];
                }
            }

            else if(riz.at(0)=="STA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line:"+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;

                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0xB000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x3000+firstallDatas[riz.at(1)];
                }

            }

            else if(riz.at(0)=="BUN")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line:"+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;
                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0xC000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x4000+firstallDatas[riz.at(1)];
                }
            }
            else if(riz.at(0)=="BSA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line:"+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;
                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0xD000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x5000+firstallDatas[riz.at(1)];
                }
            }

            else if(riz.at(0)=="ISZ")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                if(firstallDatas[riz.at(1)]==0)
                {
                    ui->console->setText("Error in line: "+QString::number(i+1)+"The "+riz.at(1)+" lable not found!!");
                    compiled=0;
                    break;
                }
                if(riz.size()>=3 && riz.at(2)=='I')
                {
                    ram[lc]=0xE000+firstallDatas[riz.at(1)];
                }
                else
                {
                    ram[lc]=0x6000+firstallDatas[riz.at(1)];
                }
            }

            else if(riz.at(0)=="INP")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0xf800;
            }

            else if(riz.at(0)=="OUT")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0xf400;
            }
            else if(riz.at(0)=="SKI")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0xf200;
            }
            else if(riz.at(0)=="SKO")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0xf100;
            }
            else if(riz.at(0)=="ION")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0xf080;
            }
            else if(riz.at(0)=="IOF")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0xf040;
            }
            else if(riz.at(0)=="CLA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7800;
            }

            else if(riz.at(0)=="CLE")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7400;
            }
            else if(riz.at(0)=="CMA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7200;
            }
            else if(riz.at(0)=="CME")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7100;
            }
            else if(riz.at(0)=="CIR")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7080;
            }
            else if(riz.at(0)=="CIL")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7040;
            }
            else if(riz.at(0)=="INC")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7020;
            }
            else if(riz.at(0)=="SPA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7010;
            }
            else if(riz.at(0)=="SNA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7008;
            }
            else if(riz.at(0)=="SZA")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7004;
            }
            else if(riz.at(0)=="SZE")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7002;
            }
            else if(riz.at(0)=="HLT")
            {
                QTableWidgetItem *itmintraction = new QTableWidgetItem();
                itmintraction->setText(commands.at(i));
                ui->ram_tb->setItem(lc,2,itmintraction);
                ram[lc]=0x7001;
            }
            else if(riz.at(0)=="END")
            {
                endp1=1;
                printTable();
                if(compiled)
                {
                    ui->console->setText("Program successfully compiled!\n");
                }
                else
                {
                    ui->console->append("\nOPPS compile error!!\n");
                }
                break;
            }
            else
            {
                //bad command
                ui->console->setText("error in line:"+QString::number(i+1)+"\n");
                ui->console->append(commands.at(i));
                compiled=0;
                break;
            }
            lc++;
    }
    if(!endp1 && compiled)
    {
        ui->console->setText("error! put END flag please :):\n");
        compiled=0;
    }
}

void MainWindow::on_run_btn_clicked()
{
    if(!compiled)
    {
        ui->console->setText("\nAt the first please complie your code :))\n");
    }
    else
    {
        S=1;
        printing=0;
        run=1;
        while ((S.to_ulong()))
        {
            on_next_btn_clicked();
        }
        printReg();
        printTable();
    }
}
void MainWindow::on_next_btn_clicked()
{
    if(!run)
    {
        printing=1;
    }
    if(!compiled)
    {
        ui->console->setText("\nAt the first please complie your code :))\n");
    }
    else
    {
        ui->ram_tb->selectRow(memorystep-1);
        ui->console->setText("");

                SC=clk;
                    QStringList riz = commands.at(lineStep).split(' ', QString::SkipEmptyParts);
                    if(riz.at(0)=="//" || riz.at(0)[0]=='/')
                    {
                       lineStep++;
                    }
                    else if(riz.at(0)=="ORG")
                    {
                            bool ok=1;
                            memorystep=riz.at(1).toInt(&ok,16);
                            lineStep++;
                    }
                    riz = commands.at(lineStep).split(' ', QString::SkipEmptyParts);

                    if(clk==0)
                    {
                        S=1;
                        PC=memorystep;
                        AR=PC;
                        clk++;
                        SC=clk;
                        if(printing)
                        {
                            printReg();
                            printTable();
                        }

                        ui->operation->setText("T0 FETCH: AR <- PC ");
                        return;
                    }

                    else if(clk==1)
                    {
                        inrPC();
                        memorystep++;
                        IR=ram[AR.to_ulong()];
                        clk++;
                        SC=clk;
                        if(printing)
                        {
                            printReg();
                            printTable();
                        }
                        ui->operation->setText("T1 FETCH:PC <- PC+1  , IR <- M[AR] ");
                        return;
                    }

                    else if(clk==2)
                    {
                        for (int bitC=0;bitC<12;bitC++)
                        {
                            AR[bitC]=IR[bitC];

                        }
                        I[0]=IR[15];
                        clk++;
                        ui->operation->setText("T2 DECODE:AR <- IR(0-11) , I <- IR(15) ");
                        return;
                    }

                    if(riz.at(0)[riz.at(0).size()-1]==',')
                    {

                            for(int x=0;x<(riz.length())-1;x++)
                            {
                                riz[x]=riz[x+1];
                            }

                    }
                    if(riz.at(0)=="DEC")
                    {
                        clk=0;
                        lineStep++;
                    }
                    else if(riz.at(0)=="HEX")
                    {
                        clk=0;
                        lineStep++;
                    }

                    else if(riz.at(0)=="AND")
                    {
                        if(clk==3)
                        {

                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                            }
                            else
                            {}
                        }

                        else if (clk==4) {
                            ui->operation->setText("DR <- M[AR]");
                            clk++;
                            DR=ram[AR.to_ulong()];
                        }
                        else if (clk==5) {
                            clk=0;
                            SC=0;
                            lineStep++;
                            logicUnitAND();
                            ui->operation->setText("AC <- AC^DR  , SC <- 0");
                        }
                    }
                    else if(riz.at(0)=="ADD")
                    {
                        if(clk==3)
                        {
                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                            }
                            else
                            {}
                        }
                        if(clk==4)
                        {
                            clk++;
                            DR=ram[AR.to_ulong()];
                            ui->operation->setText("DR <- M[AR]");
                        }
                        if(clk==5)
                        {
                            clk=0;
                            lineStep++;
                            SC=0;
                            arithmeticUnitADD();
                            ui->operation->setText(" AC <- AC+DR , E<-COUT , SC <- 0");
                        }
                    }
                    else if(riz.at(0)=="LDA")
                    {
                        if(clk==3)
                        {
                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                            }
                            else
                            {}
                        }
                        else if(clk==4)
                        {
                            clk++;
                            DR=ram[AR.to_ulong()];
                            ui->operation->setText("DR <- M[AR]");
                        }
                        else if (clk==5) {
                            SC=0;
                            clk=0;
                            lineStep++;
                            AC=DR;
                            ui->operation->setText("AC <- DR , SC <- 0");
                        }
                    }
                    else if(riz.at(0)=="STA")
                    {
                        if(clk==3)
                        {
                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                            }
                            else
                            {}
                        }
                        else if(clk==4)
                        {
                            ui->operation->setText("M[AR] <- AC , SC <- 0");
                            SC=0;
                            lineStep++;
                            clk=0;
                            ram[AR.to_ulong()]=AC;
                            QTableWidgetItem *empty = new QTableWidgetItem();
                            empty->setText("");
                            ui->ram_tb->setItem(firstallDatas[riz.at(1)],2,empty);
                        }
                    }
                    else if(riz.at(0)=="BUN")
                    {
                        int Ichecker;
                        if(clk==3)
                        {
                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                                Ichecker=1;
                            }
                            else
                            {
                                Ichecker=0;

                            }
                        }
                        else if(clk==4)
                        {
                            ui->operation->setText("PC <- AR , SC <- 0");
                            SC=0;
                            clk=0;
                            PC=AR;
                            if(Ichecker)
                            {
                               lineStep=memoryToLine[AR.to_ulong()]-1;
                            }
                            else
                            {
                               lineStep=allDatas[riz.at(1)+","]-1;
                            }
                            lineStep++;
                            memorystep=PC.to_ulong();
                        }
                    }
                    else if(riz.at(0)=="BSA")
                    {
                        if(clk==3)
                        {
                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                            }
                            else
                            {}
                        }
                        else if(clk==4)
                        {
                            ui->operation->setText("M[AR] <- PC , AR <- AR+1");
                            clk++;
                            inrAR();
                            for (int bitC=0;bitC<12;bitC++)
                            {
                                ram[(AR.to_ulong())-1][bitC]=PC[bitC];
                            }
                        }
                        else if(clk==5)
                        {
                            clk=0;
                            PC=AR;
                            lineStep=allDatas[riz.at(1)+","];
                            lineStep++;
                            SC=0;
                            memorystep=PC.to_ulong();
                            ui->operation->setText("PC <- AR , SC <- 0");
                        }
                    }
                    else if(riz.at(0)=="ISZ")
                    {
                        if(clk==3)
                        {
                            clk++;
                            if(riz.size()>=3 && riz.at(2)=='I')
                            {
                                ui->operation->setText("AR <- M[AR] ");
                                bitset<12> tmpAR=AR;
                                for (int bitC=0;bitC<12;bitC++)
                                {
                                    AR[bitC]=ram[tmpAR.to_ulong()][bitC];
                                }
                            }
                            else
                            {}
                        }
                        else if(clk==4)
                        {
                            ui->operation->setText("DR <- M[AR]");
                            clk++;
                            DR=ram[AR.to_ulong()];
                        }
                        else if(clk==5)
                        {
                            ui->operation->setText("DR <- DR+1");
                            clk++;
                            inrDR();
                        }
                        else if(clk==6)
                        {
                            SC=0;
                            clk=0;
                            lineStep++;
                            ram[AR.to_ulong()]=DR;
                            ui->operation->setText("M[AR] <- DR, SC <- 0");
                            if(DR.to_ulong()==0)
                            {
                                inrPC();
                                lineStep++;
                                memorystep++;
                                ui->operation->setText("PC <- PC+1 , M[AR] <- DR, SC <- 0");
                            }
                        }
                    }
                    else if(riz.at(0)=="INP")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        bool ok=1;
                        INPR=ui->in_line->text().toInt(&ok,16);
                        for (int bitC=0;bitC<8;bitC++)
                        {
                            AC[bitC]=INPR[bitC];
                        }
                        FGI=0;
                        ui->operation->setText("AC(0-7) <- INPR , FGI <- 0 , SC <- 0");
                    }
                    else if(riz.at(0)=="OUT")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        for (int bitC=0;bitC<8;bitC++)
                        {
                            OUTR[bitC]=AC[bitC];
                        }
                        FGO=0;
                        ui->operation->setText("OUTR <- AC(0-7), FGO <- 0 , SC <- 0");
                    }
                    else if(riz.at(0)=="SKI")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->operation->setText("SC <- 0");
                        if(FGI==1)
                        {
                            inrPC();
                            lineStep++;
                            memorystep++;
                            ui->operation->setText("PC <- PC+1 , SC <- 0");
                        }
                    }
                    else if(riz.at(0)=="SKO")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->operation->setText("SC <- 0");
                        if(FGO==1)
                        {
                            inrPC();
                            lineStep++;
                            memorystep++;
                            ui->operation->setText("PC <- PC+1 , SC <- 0");
                        }
                    }
                    else if(riz.at(0)=="ION")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        IEN=1;
                        ui->operation->setText("IEN <- 1 , SC <- 0");
                    }
                    else if(riz.at(0)=="IOF")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        IEN=0;
                        ui->operation->setText("IEN <- 0 , SC <- 0");
                    }
                    else if(riz.at(0)=="CLA")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        AC=0;
                        ui->operation->setText("AC <- 0  , SC <- 0 ");
                    }
                    else if(riz.at(0)=="CLE")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        E=0;
                        ui->operation->setText("E <- 0  , SC <- 0 ");
                    }
                    else if(riz.at(0)=="CMA")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        AC.flip();
                        ui->operation->setText("AC <- ~AC  , SC <- 0 ");
                    }
                    else if(riz.at(0)=="CME")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        E.flip();
                        ui->operation->setText("E <- ~E  , SC <- 0 ");
                    }
                    else if(riz.at(0)=="CIR")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                       arithmeticUnitCIR();
                       ui->operation->setText("AC <- shr  , AC(15)<-E , E <- AC(0) ");
                    }
                    else if(riz.at(0)=="CIL")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        arithmeticUnitCIL();
                        ui->operation->setText("AC <- shl  , AC(0)<-E , E <- AC(15) ");
                    }
                    else if(riz.at(0)=="INC")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        inrAC();
                        ui->operation->setText("AC <- AC+1  , SC <- 0 ");
                    }
                    else if(riz.at(0)=="SPA")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->operation->setText("SC <- 0 ");
                        if(AC[15]==0)
                        {
                            ui->operation->setText("PC <- PC+1  , SC <- 0 ");
                            inrPC();
                            lineStep++;
                            memorystep++;
                        }
                    }
                    else if(riz.at(0)=="SNA")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->operation->setText("SC <- 0 ");
                        if(AC[15]==1)
                        {
                            inrPC();
                            lineStep++;
                            memorystep++;
                            ui->operation->setText("PC <- PC+1  , SC <- 0 ");
                        }
                    }
                    else if(riz.at(0)=="SZA")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->operation->setText("SC <- 0 ");
                        if(AC.to_ulong()==0)
                        {
                            inrPC();
                            lineStep++;
                            memorystep++;
                            ui->operation->setText("PC <- PC+1  , SC <- 0 ");
                        }
                    }
                    else if(riz.at(0)=="SZE")
                    {
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->operation->setText("SC <- 0 ");
                        if(E.to_ulong()==0)
                        {
                            inrPC();
                            lineStep++;
                            memorystep++;
                            ui->operation->setText("PC <- PC+1  , SC <- 0 ");
                        }
                    }
                    else if(riz.at(0)=="HLT")
                    {
                        S=0;
                        SC=0;
                        clk=0;
                        lineStep++;
                        ui->run_btn->setEnabled(false);
                        ui->next_btn->setEnabled(false);
                        ui->operation->setText("S <- 0");
                        ui->console->setText("Congratulation!! Program run successfully =)) \n");
                        if(printing)
                        {
                            printReg();
                            printTable();
                        }
                        return;
                    }
                    else if(riz.at(0)=="END")
                    {
                        clk=0;
                        lineStep++;
                        if(printing)
                        {
                            printReg();
                            printTable();
                        }
                        ui->console->setText("Congratulation!! Program run successfully =))\n");
                        ui->run_btn->setEnabled(false);
                        ui->next_btn->setEnabled(false);
                        return;
                    }
                    else
                    {
                        clk=0;
                        lineStep++;
                        if(printing)
                        {
                            printReg();
                            printTable();
                        }
                        ui->console->setText("OPPS! Error arised!! \n");
                        ui->run_btn->setEnabled(false);
                        ui->next_btn->setEnabled(false);
                        return;
                    }
                    if(printing)
                    {
                        printReg();
                        printTable();
                    }
        }
}

void MainWindow::on_actionAbout_Mano_simulator_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("ARYA's basic mano computer. This program writed using C++ & QT framework and the source in my github account:\nhttps://github.com/arsalanyavari/mano-simulator    :)");
    msgBox.exec();
}
