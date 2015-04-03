#include "duallistbox.h"
#include <QApplication>
#include <QStringList>

#include <cstdio>

void usage() {
  printf("\n Usage: %s -l \"Machine List\" -m \"Message\" -d deliminator -opt options\n", "duallistbox.exe");
  printf("        Machine List and Message must be enclosed by \" and \" ");
  printf("        White space delimits machines.\n");
  printf("        -d change deliminator to :\n");
  printf("\n");
}


int main(int argc, char *argv[])
{

    char *fields = "None";
    char delim = ' ';
    char *opt = "";

    for(int iArg=0; iArg < argc; iArg++){
        if(iArg < argc-1){  // For Pair Argument
         if( strcmp(argv[iArg],"-list") == 0 || strcmp(argv[iArg],"-l") == 0) {
           iArg++;
           fields = argv[iArg];
         }
      }
       if( strcmp(argv[iArg],"-d") == 0 || strcmp(argv[iArg],"-delim") == 0) {
           delim = ':';
      }

       if( strcmp(argv[iArg],"-o") == 0 || strcmp(argv[iArg],"-opt") == 0) {
           iArg++;
           opt = argv[iArg];
      }

       if(strcmp(argv[iArg],"-help") == 0 || strcmp(argv[iArg],"-h") == 0 ) {
         usage();
         return(0);
       }
    }

    // QStringList items = QStringList::split( delim, fields );
    // printf ("%s\n", fields);
    QStringList items = QString(fields).split(delim,QString::SkipEmptyParts);

    QApplication app(argc, argv);
    DualListBox *dualListBox = new DualListBox;

    // app.setMainWidget(dualListBox);

    dualListBox->setUsrItems(items);
    dualListBox->setUsrOpt(opt);
    dualListBox->show();

    return app.exec();
}
