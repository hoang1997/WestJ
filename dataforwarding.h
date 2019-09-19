#ifndef DATAFORWARDING_H
#define DATAFORWARDING_H

#include <QDialog>
#include <QAction>
#include <QMessageBox>
namespace Ui {
class DataForwarding;
}

struct DFB {
    QString trackSection_id;
    int aeb_sender_id;
    int aeb_receiver_id;
};

class DataForwarding : public QDialog
{
    Q_OBJECT
signals:
    void dataForwardingBlockSignal(QVector<DFB*>);

public:
    explicit DataForwarding(QWidget *parent = nullptr);
    ~DataForwarding();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_clicked();

    void onRemoveButtonClicked();

private:
    void updatePreview_DFB();

    Ui::DataForwarding *ui;

    QVector<DFB*> dfbArray;
};

#endif // DATAFORWARDING_H
