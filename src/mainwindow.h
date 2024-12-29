#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_new_game_clicked();
    void update_balance();
    void update_stats();
    void game_start();
    void game_end();
    void handleBoardClick();
    void on_cashout_clicked();
    void generateChart(const std::vector<long double>& amounts);

    void on_actionCreator_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
