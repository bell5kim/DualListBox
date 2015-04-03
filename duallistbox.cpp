#include "duallistbox.h"
#include "ui_duallistbox.h"

#include <QDomElement>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>

#ifndef XML
#define XML on
#endif

using namespace std;
#include <iostream>

#ifdef XML
class Status {
  public:
     QString value, date;
};

QDomElement StatusToNode (QDomDocument &d, const Status &s, QString e){
   QDomElement elm = d.createElement(e);
   elm.setAttribute("value", s.value);
   elm.setAttribute("date", s.date);
   return elm;
};
#endif


DualListBox::DualListBox(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DualListBox)
{
    ui->setupUi(this);

    init();
}

DualListBox::~DualListBox()
{
    delete ui;
}


void DualListBox::init()
{
    QString PWD = getenv("PWD");
    QString mName = PWD.section('/',-2,-2);
    ui->textLabelMachine->setText(mName);
    setWindowTitle("Calculation Control for " + mName);
  //cout << "XVMC_HOME = " << getenv("XVMC_HOME") << endl;
  //cout << "XVMC_WORK = " << getenv("XVMC_WORK") << endl;

    ui->listBoxLeft->setSelectionMode(QAbstractItemView::MultiSelection );
    ui->listBoxRight->setSelectionMode(QAbstractItemView::MultiSelection );
}

void DualListBox::setUsrItems(QStringList items)
{
    ui->listBoxLeft->clear();
    // ui->listBoxLeft->insertStringList(-1,items);
    ui->listBoxLeft->addItems(items);

    QString PWD = getenv("PWD");
    QString mName = PWD.section('/',-1);
    QString XVMC_WORK = getenv("XVMC_WORK");
    QString XVMC_HOME = getenv("XVMC_HOME");
    QString CMD = "";
    // setenv("MACHINE",mName,1);

    QDir *calcDir = new QDir;
    calcDir->setPath(XVMC_WORK+"/"+mName+"/calc");
    if (calcDir->exists()) {
       calcDir->setNameFilters(QStringList("*.log"));
       QStringList lList = calcDir->entryList(QDir::Files, QDir::Name);
       for (QStringList::Iterator it = lList.begin(); it != lList.end(); ++it){
          QString logName = *it;
          QString pName = logName.section(".log",0,0);
          for (int i=ui->listBoxLeft->count()-1; i >= 0 ; i--) {
             QString leftText = ui->listBoxLeft->item(i)->text();
             // QTextStream (stdout) << "setUsrItems: " << leftText <<endl;
             if (leftText.contains(pName)) {
                ui->listBoxRight->addItem(leftText+" (Done)");
                ui->listBoxLeft->takeItem(i);
                // ui->listBoxRight->sort(true);
             }
          }
       }
    }
    sortRightBox();
    sortLeftBox();
    copyCalcToTmp();
}

void DualListBox::setUsrOpt(char *opt) {
    QString qOpt = opt;
#ifdef XVMC
    ui->checkBoxOffAxis->setEnabled(true);
    if (qOpt.contains("OffAxis"))
        ui->checkBoxOffAxis->setChecked(true);
    else
        ui->checkBoxOffAxis->setChecked(false);
#endif
}

void DualListBox::sortRightBox() {
     // Sort
     int iRight = ui->listBoxRight->count();
     for (int i=0; i<iRight; i++) {
         QString tRight = ui->listBoxRight->item(i)->text();
         bool ok;
         QString FS = tRight.section("x",1,1).section(".",0,0);
         float minFS = FS.toFloat(&ok);
         for (int j=i; j<iRight; j++) {
             QString rText = ui->listBoxRight->item(j)->text();
             FS = rText.section("x",1,1).section(".",0,0);
             float fs = FS.toFloat(&ok);
             // cout << fs << " vs " << minFS << endl;
             if (fs < minFS) {
                 ui->listBoxRight->takeItem(j);
                 ui->listBoxRight->insertItem(i,rText);
                 minFS = fs;
             }
         }
         // cout << i << "= " << ui->listBoxRight->item(i)->text() << endl;
     }
}

void DualListBox::sortLeftBox() {
     // Sort
     int iLeft = ui->listBoxLeft->count();
     for (int i=0; i<iLeft; i++) {
         QString tLeft = ui->listBoxLeft->item(i)->text();
         bool ok;
         QString FS = tLeft.section("x",1,1).section(".",0,0);
         float minFS = FS.toFloat(&ok);
         for (int j=i; j<iLeft; j++) {
             QString lText = ui->listBoxLeft->item(j)->text();
             FS = lText.section("x",1,1).section(".",0,0);
             float fs = FS.toFloat(&ok);
             // cout << fs << " vs " << minFS << endl;
             if (fs < minFS) {
                 ui->listBoxLeft->takeItem(j);
                 ui->listBoxLeft->insertItem(i,lText);
                 minFS = fs;
             }
         }
         // cout << i << "= " << ui->listBoxLeft->item(i)->text() << endl;
     }
}

void DualListBox::moveToRight() {
  for ( int i = ui->listBoxLeft->count()-1; i >= 0 ; i-- ) {
     QListWidgetItem *item = ui->listBoxLeft->item( i );
     // if the item is selected...
     if ( item->isSelected() ) {
       ui->listBoxRight->addItem(item->text());
       ui->listBoxLeft->takeItem(i);
       // ui->listBoxRight->sort(true);
     }
  }
  sortRightBox();
}

void DualListBox::moveToLeft() {
  for (int i = ui->listBoxRight->count()-1; i >= 0 ; i-- ) {
     QListWidgetItem *item = ui->listBoxRight->item( i );
     // if the item is selected...
   if ( item->isSelected() ) {
     ui->listBoxLeft->addItem(item->text());
     ui->listBoxRight->takeItem(i);
     // ui->listBoxLeft->sort(true);
   }
  }
  sortLeftBox();
}

void DualListBox::selectAllLeft() {
  //  ui->listBoxLeft->setSelectionMode(QAbstractItemView::MultiSelection );
    ui->listBoxLeft->selectAll();
  //for ( unsigned int i = 0; i < ui->listBoxLeft->count(); i++ ) {
  //  ui->listBoxLeft->item(i)->setSelected(true);
  //}
}

void DualListBox::deselectAllLeft() {
  for ( unsigned int i = 0; i < ui->listBoxLeft->count(); i++ ) {
    ui->listBoxLeft->item(i)->setSelected(false);
  }
}

void DualListBox::selectAllRight() {
  //  ui->listBoxRight->setSelectionMode(QAbstractItemView::MultiSelection );
    ui->listBoxRight->selectAll();
  //for ( unsigned int i = 0; i < ui->listBoxRight->count(); i++ ) {
  //  ui->listBoxRight->item(i)->setSelected(true);
  //}
}

void DualListBox::deselectAllRight() {
  for ( unsigned int i = 0; i < ui->listBoxRight->count(); i++ ) {
    ui->listBoxRight->item(i)->setSelected(false);
  }
}

void DualListBox::Done() {
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");
   QString CMD = "";

   QDir *tmpDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/tmp");
   QDir *calcDir = new QDir;
   calcDir->setPath(XVMC_WORK+"/"+mName+"/calc");
   if(tmpDir->exists()) {
      if(calcDir->exists()) {
         CMD = "rm -rf " + XVMC_WORK + "/"+mName+"/calc";
         if (system(CMD.toStdString().c_str()) != 0) {
            cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
         }
      }
      CMD = "mv " + XVMC_WORK + "/"+mName+"/tmp "+ XVMC_WORK + "/"+mName+"/calc";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
    if (ui->checkBoxOffAxis->isChecked())
       writeSettingString("OffAxis", "1");
    else
       writeSettingString("OffAxis", "0");

   writeSettingString("Verification", "Done");
   close();
}


void DualListBox::Cancel() {
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");
   QString CMD = "";

   QDir *tmpDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/tmp");
   if(tmpDir->exists()) {
      CMD = "rm -rf " + XVMC_WORK + "/"+mName+"/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   writeSettingString("Verification", "NotDone");
   close();
}


void DualListBox::runXVMC() {
   // Set Local XVMC_WORK and XVMC_HOME
   // Always it starts in Patient/Patient
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");
   QString CMD = "";
   // Remove existing files in calc temp directory

   QDir *tmpDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/tmp");
   if(tmpDir->exists()) {
      CMD = "rm -rf " + XVMC_WORK + "/"+mName+"/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   CMD = "mkdir " + XVMC_WORK + "/"+mName+"/tmp";
   if (system(CMD.toStdString().c_str()) != 0) {
      cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
   }

   int iProgress = 0;
   int nProgress = 0;
   for (unsigned int i = 0; i < ui->listBoxRight->count(); i++ ) {
     QListWidgetItem *item = ui->listBoxRight->item(i);
     if (item->isSelected()) nProgress++;
   }
   for (unsigned int i = 0; i < ui->listBoxRight->count(); i++ ) {
     QListWidgetItem *item = ui->listBoxRight->item(i);
     if (item->isSelected()) {
        QString planName = item->text().section("(Done)",0,0);
#ifdef XVMC
        runOneXVMC(planName.section(' ',0,0));
#else
        runOneVerify(planName.section(' ',0,0));
#endif
        iProgress += ui->progressBarRunMC->maximum()/nProgress;
        int p = ui->progressBarRunMC->value();
        while ( p <= iProgress) ui->progressBarRunMC->setValue(p++);
        ui->listBoxRight->item(i)->setSelected(false);
        ui->listBoxRight->takeItem(i);
        ui->listBoxRight->insertItem(i,planName + "(Done)");
     }
   }
 int p = ui->progressBarRunMC->value();
   while ( p <= 10000) ui->progressBarRunMC->setValue(p++);
}

void DualListBox::copyCalcToTmp() {
   // Set Local XVMC_WORK and XVMC_HOME
   // Always it starts in Patient/Patient
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");
   QString CMD = "";
   // Remove existing files in calc temp directory

   QDir *tmpDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/tmp");
   QDir *calcDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/calc");
   if(tmpDir->exists()) {
      CMD = "rm -rf " + XVMC_WORK + "/"+mName+"/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   if(calcDir->exists()) {
      CMD = "cp -R " + XVMC_WORK + "/"+mName+"/calc " + XVMC_WORK + "/"
          + mName + "/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong in copyCalcToTmp: " << CMD.toStdString() << endl;
      }
   } else {
      CMD = "mkdir " + XVMC_WORK + "/"+mName+"/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong in copyCalcToTmp: " << CMD.toStdString() << endl;
      }
   }
}

void DualListBox::addXVMC() {
   // Set Local XVMC_WORK and XVMC_HOME
   // Always it starts in Patient/Patient
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");
   QString CMD = "";
   // Remove existing files in calc temp directory
   copyCalcToTmp();

   QDir *tmpDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/tmp");
   QDir *calcDir = new QDir;
   tmpDir->setPath(XVMC_WORK+"/"+mName+"/calc");
   if(tmpDir->exists()) {
      CMD = "rm -rf " + XVMC_WORK + "/"+mName+"/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   if(calcDir->exists()) {
      CMD = "cp -R " + XVMC_WORK + "/"+mName+"/calc " + XVMC_WORK + "/"
          + mName + "/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   } else {
      CMD = "mkdir " + XVMC_WORK + "/"+mName+"/tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }

   ui->progressBarRunMC->setMaximum(10000);
   ui->progressBarRunMC->reset();
   int iProgress = 0;
   int nProgress = 0;
   for (unsigned int i = 0; i < ui->listBoxRight->count(); i++ ) {
     QListWidgetItem *item = ui->listBoxRight->item(i);
     if (item->isSelected()) nProgress++;
   }
   for (unsigned int i = 0; i < ui->listBoxRight->count(); i++ ) {
     QListWidgetItem *item = ui->listBoxRight->item(i);
     if (item->isSelected()) {
        QString planName = item->text().section("(Done)",0,0);
#ifdef XVMC
        runOneXVMC(planName.section(' ',0,0));
#else
        runOneVerify(planName.section(' ',0,0));
#endif
        iProgress += ui->progressBarRunMC->maximum()/nProgress;
        int p = ui->progressBarRunMC->value();
        while ( p <= iProgress) ui->progressBarRunMC->setValue(p++);
        ui->listBoxRight->item(i)->setSelected(false);
        ui->listBoxRight->takeItem(i);
        ui->listBoxRight->insertItem(i,planName + "(Done)");
     }
   }
   int p = ui->progressBarRunMC->value();
   while ( p <= 10000) ui->progressBarRunMC->setValue(p++);
}

void DualListBox::plotComp() {
   selectAllRight();
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   // cout << "runXVMC.csh " << item->currentItem()->text() << endl;
   QString CMD = "";
   QString LBIN = getenv("XVMC_HOME");
#ifdef XVMC
   CMD = LBIN + "/plotComp_xvmc.exe -d \"tmp\" -f \"1.000\"";
#else
   CMD = LBIN + "/plotComp.exe -d \"tmp\" -f \"1.000\"";
#endif
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
}

void DualListBox::writeSettingString(QString keyWord, QString keyValue){
   QDateTime currentDateTime = QDateTime::currentDateTime();
   QString DT = currentDateTime.toString();
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString group = XVMC_WORK.section('/',-1);
#ifdef XML
   QString mName = group;
   QFile xmlFile(XVMC_WORK+"/status.xml");
   // cout << XVMC_WORK+"/status.xml" << endl;
   if (!xmlFile.open(QIODevice::ReadOnly)){
      cout << "No " << XVMC_WORK.toStdString()+"/status.xml is found" << endl;
      exit(-1);
   }

   QDomDocument doc(mName);
   doc.setContent(&xmlFile);
   xmlFile.close();

   QDomElement root = doc.documentElement();
   if (root.tagName() != mName) {
      // cout << "Tag Name ("<<eRoot.tagName()<<") is different from " << mName << endl;
      exit(-1);
   }

   QDomNode n = root.firstChild();

   // Set attribute for existing tag
   bool isDone = false;
   while (!n.isNull()){
      QDomElement e = n.toElement();
      if(!e.isNull()) {
         if(e.tagName() == keyWord) {
        e.setAttribute("value", keyValue);
        isDone = true;
         }
      }
      n = n.nextSibling();
   }

   if(!isDone) {
      Status s;  // Status class

      QString eName = keyWord;
      s.value = keyValue;  s.date = DT;
      root.appendChild(StatusToNode(doc, s, eName));
   }

   xmlFile.open(QIODevice::WriteOnly);
   QTextStream txtStrm(&xmlFile);
   txtStrm << doc.toString();
   xmlFile.close();



#else
   QString Entry = "/"+group+"/" + keyWord;
   QSettings settings;
   settings.insertSearchPath(QSettings::Unix, XVMC_WORK);
   settings.beginGroup("/"+group);
   settings.writeEntry(Entry, keyValue);
   settings.endGroup();
   // cout << "End of writeSetting" << endl;
#endif
}

void DualListBox::runOneXVMC(QString planName){
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");

   QString planFile = XVMC_WORK+"/"+mName+"/INPUT/"+planName+".vmc";
   QString vmcFile = XVMC_WORK+"/"+mName+"/"+planName+".vmc";
   QString logFile = XVMC_WORK+"/"+mName+"/"+planName+".log";
   QString d3dFile = XVMC_WORK+"/"+mName+"/"+planName+"00.d3d";
   QString e3dFile = XVMC_WORK+"/"+mName+"/"+planName+"00.e3d";
 //cout << "runOneXVMC: planFile = " << planFile << endl;
 //cout << "runOneXVMC: vmcFile = " << vmcFile << endl;
 //cout << "runOneXVMC: logFile = " << logFile << endl;
   QFile dFile(d3dFile);
   if (dFile.exists()) {
      QString CMD = "rm " + d3dFile;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile eFile(e3dFile);
   if (eFile.exists()) {
      QString CMD = "rm " + e3dFile;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile pFile(planFile);
   if (pFile.exists()) {
      QString CMD = "mv " + planFile + " " + vmcFile;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }

   QString qExe = "xvmc";
    if (ui->checkBoxOffAxis->isChecked())	qExe = "xvmc_offaxis";
   QFile vFile(vmcFile);
   if (vFile.exists()) {
      QString CMD = XVMC_HOME+"/" + qExe + " " + mName + " " + planName + " > " + logFile;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile lFile(logFile);
   if (lFile.exists()) {
      QString CMD = "mv " + planName + ".* " + XVMC_WORK+"/"+mName+"/tmp/";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
}

void DualListBox::runOneVerify(QString planName){
   QString PWD = getenv("PWD");
   QString mName = PWD.section('/',-1);
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString XVMC_HOME = getenv("XVMC_HOME");
   QString BDT_FILE = XVMC_WORK + "/dat/" + mName + ".bdt";
   QString BDT_BASE_FILE = XVMC_HOME + "/dat/basedata/" + mName + ".bdt";
    if (mName.contains("PB_")) {
        QString pName = XVMC_WORK.section('/',-1);
        BDT_FILE = XVMC_WORK + "/dat/" + pName.section("_",0,0) + ".RPB";
        BDT_BASE_FILE = XVMC_HOME + "/dat/basedata/" + pName.section("_",0,0) + ".RPB";
        // cout << BDT_BASE_FILE << endl;
    }
   QString planFile = XVMC_WORK+"/"+mName+"/INPUT/"+planName;
   QString planFileVER = planFile+".VER";
   QString planFileCLN = planFile+".CLN";
   QString verFile = XVMC_WORK+"/"+mName+"/"+planName+".VER";
   QString clnFile = XVMC_WORK+"/"+mName+"/"+planName+".CLN";
   QString logFile = XVMC_WORK+"/"+mName+"/"+planName+".log";
   QFile pFileVER(planFileVER);
   if (pFileVER.exists()) {
      QString CMD = "mv " + planFileVER + " " + verFile;
      // cout << CMD << endl;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile pFileCLN(planFileCLN);
   if (pFileCLN.exists()) {
      QString CMD = "mv " + planFileCLN + " " + clnFile;
      // cout << CMD << endl;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile bdtFile(BDT_FILE);
   if (bdtFile.exists()) {
      QString CMD = "cp " + BDT_FILE + " " + BDT_BASE_FILE;
      // cout << CMD << endl;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile vFile(verFile);
   QFile cFile(clnFile);
   QFile bFile(BDT_BASE_FILE);
   if (vFile.exists() && cFile.exists() && bFile.exists()) {
#ifdef CMS_SPECT
      QString CMD = "cd " + XVMC_WORK+"/"+mName + ";"
                    + XVMC_HOME+"/verify_cms " + planName
                    + " > " + logFile;
#else
      QString CMD = "cd " + XVMC_WORK+"/"+mName + ";"
                    + XVMC_HOME+"/verify " + planName
                    + " > " + logFile;
#endif
      // cout << CMD << endl;
      if (system(CMD.toStdString().c_str()) < 0) {
        cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
   QFile lFile(logFile);
   if (lFile.exists()) {
      QString CMD = "mv " + planName + ".* " + XVMC_WORK+"/"+mName+"/tmp/";
      // cout << CMD << endl;
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl;
      }
   }
}


void DualListBox::on_pushButtonToRight_clicked()
{
    moveToRight();
}

void DualListBox::on_pushButtonToLeft_clicked()
{
    moveToLeft();
}

void DualListBox::on_pushButtonSelectAllLeft_clicked()
{
    selectAllLeft();
}

void DualListBox::on_pushButtonDeselectAllLeft_clicked()
{
    deselectAllLeft();
}

void DualListBox::on_pushButtonSelectAllRight_clicked()
{
    selectAllRight();
}

void DualListBox::on_pushButtonDeselectAllRight_clicked()
{
    deselectAllRight();
}

void DualListBox::on_pushButtonDone_clicked()
{
    Done();
}

void DualListBox::on_pushButtonRun_clicked()
{
    runXVMC();
}

void DualListBox::on_pushButtonPlot_clicked()
{
    plotComp();
}

void DualListBox::on_pushButtonCancel_clicked()
{
    Cancel();
}

void DualListBox::on_pushButtonAdd_clicked()
{
    addXVMC();
}
