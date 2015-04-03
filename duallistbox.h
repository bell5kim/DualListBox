#ifndef DUALLISTBOX_H
#define DUALLISTBOX_H

#include <QMainWindow>

namespace Ui {
class DualListBox;
}

class DualListBox : public QMainWindow
{
    Q_OBJECT

public:
    explicit DualListBox(QWidget *parent = 0);
    ~DualListBox();

public slots:
    virtual void setUsrItems( QStringList items );
    virtual void setUsrOpt( char * opt );
    virtual void sortRightBox();
    virtual void sortLeftBox();
    virtual void moveToRight();
    virtual void moveToLeft();
    virtual void selectAllLeft();
    virtual void deselectAllLeft();
    virtual void selectAllRight();
    virtual void deselectAllRight();
    virtual void Done();
    virtual void Cancel();
    virtual void runXVMC();
    virtual void copyCalcToTmp();
    virtual void addXVMC();
    virtual void plotComp();
    virtual void writeSettingString( QString keyWord, QString keyValue );
    virtual void runOneXVMC( QString planName );
    virtual void runOneVerify( QString planName );

private slots:
    void on_pushButtonToRight_clicked();

    void on_pushButtonToLeft_clicked();

    void on_pushButtonSelectAllLeft_clicked();

    void on_pushButtonDeselectAllLeft_clicked();

    void on_pushButtonSelectAllRight_clicked();

    void on_pushButtonDeselectAllRight_clicked();

    void on_pushButtonDone_clicked();

    void on_pushButtonRun_clicked();

    void on_pushButtonPlot_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonAdd_clicked();

private:
    Ui::DualListBox *ui;

    void init();
};

#endif // DUALLISTBOX_H
