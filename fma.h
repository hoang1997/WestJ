#ifndef FMA_H
#define FMA_H

#include <QDialog>
#include <QTreeWidget>
#include <QAction>
struct axleHeadData {
    QString axleHead_ID;
    int directionInversion = 0;
};


namespace Ui {
class FMA;
}

class FMA : public QDialog
{
    Q_OBJECT
signals:
    void fmaSignal(FMA*);
public:
    explicit FMA(QWidget *parent = nullptr);
    ~FMA();

    QVector<axleHeadData*> returnAxleHeads();
    QString returnTrackSectionId();

private slots:

    void on_pushButton_clicked();

    void updatePreview(axleHeadData*);

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void onRemoveButtonClicked();

    //void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::FMA *ui;

    QVector<axleHeadData*> axleHeadArr;

    QString trackSection_Id;

    QAction *action;
};

#endif // FMA_H
