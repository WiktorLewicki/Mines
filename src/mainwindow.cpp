#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "silnik.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
Game gra;
bool cl[5][5];
std::vector<long double> history;
int showMoneyDialog() {
    QDialog dialog;
    dialog.setWindowTitle("Choose an amount of money");

    QLabel *label = new QLabel("Enter the amount of money you want to deposit:", &dialog);

    QLineEdit *lineEdit = new QLineEdit(&dialog);
    QIntValidator *validator = new QIntValidator(1, 100000, lineEdit);
    lineEdit->setValidator(validator);

    QLabel *errorLabel = new QLabel(&dialog);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, [&dialog, &lineEdit, &errorLabel]() {
        if (lineEdit->text().isEmpty()) {
            errorLabel->setText("You must enter an amount!");
            errorLabel->setVisible(true);
        } else {
            dialog.accept();
        }
    });

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(errorLabel);
    layout->addWidget(buttonBox);

    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        return lineEdit->text().toInt();
    } else {
        return -1;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMenuBar *menuBar = this->menuBar();
    menuBar->setStyleSheet("background-color: white;");
    int money = showMoneyDialog();
    while (money == -1) {
        money = showMoneyDialog();
    }
    gra.set_money(money);
    history.push_back(money);
    update_balance();
    generateChart(history);
    QRegularExpression betRegExp("^[1-9][0-9]{0,5}$");
    QRegularExpressionValidator *betValidator = new QRegularExpressionValidator(betRegExp, this);
    ui->bet_amount->setValidator(betValidator);

    QRegularExpression bombRegExp("^[1-9]$|^1[0-9]$|^2[0-4]$");
    QRegularExpressionValidator *bombValidator = new QRegularExpressionValidator(bombRegExp, this);
    ui->bomb_amount->setValidator(bombValidator);
    ui->cashout->setVisible(false);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            cl[i][j]=true;
            QString buttonName = QString("pole%1%2").arg(i).arg(j);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            if (button) {
                connect(button, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::update_balance(){
    long double money = gra.get_money();
    QString qpm = QString::number(money, 'f', 2);
    QString result = "Balance: " + qpm + "EUR";
    ui->Balance->setText(result);
}
void MainWindow::update_stats() {
    long double multiplier = gra.get_multiplier();
    QString qpm = QString::number(multiplier, 'f', 2);
    QString result = "Multiplier: x" + qpm;
    ui->multiplier->setText(result);
    long double potential = gra.get_potential();
    qpm = QString::number(potential, 'f', 2);
    result = "Potential Win: " + qpm;
    ui->potential->setText(result);
}
void MainWindow::game_start(){
    update_balance();
    update_stats();
    ui->cashout->setVisible(true);
    ui->cashout->setEnabled(true);
    ui->new_game->setVisible(false);
    ui->new_game->setEnabled(false);
    ui->bet_amount->setEnabled(false);
    ui->bomb_amount->setEnabled(false);
    for(int i = 0; i < 5; ++i) {
        for(int j = 0; j < 5; ++j) {
            cl[i][j]=false;
            QString buttonName = QString("pole%1%2").arg(i).arg(j);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            QIcon icon(":/img/empty.png");
            button->setIcon(icon);
            button->setIconSize(QSize(150, 150));
            button->setCursor(Qt::PointingHandCursor);
        }
    }
}
void MainWindow::game_end(){
    gra.game_end();
    history.push_back(gra.get_money());
    generateChart(history);
    ui->cashout->setVisible(false);
    ui->cashout->setEnabled(false);
    update_balance();
    ui->new_game->setVisible(true);
    ui->new_game->setEnabled(true);
    ui->bet_amount->setEnabled(true);
    ui->bomb_amount->setEnabled(true);
    for(int i = 0; i < 5; ++i) {
        for(int j = 0; j < 5; ++j) {
            if(cl[i][j]) continue;
            cl[i][j]=true;
            QString buttonName = QString("pole%1%2").arg(i).arg(j);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            QString pl;
            if(gra.get_p(i, j)) pl=":/img/red_after.png";
            else pl=":/img/green_after.png";
            QIcon icon(pl);
            button->setIcon(icon);
            button->setIconSize(QSize(150, 150));
            button->setCursor(Qt::PointingHandCursor);
            button->setCursor(Qt::ArrowCursor);
        }
    }
}

void MainWindow::on_new_game_clicked()
{
    QString betText = ui->bet_amount->text();
    QString bombText = ui->bomb_amount->text();
    if (betText.isEmpty() || bombText.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("You must provide a value for both the bet and the number of bombs.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    int money = betText.toInt();
    int bombs = bombText.toInt();
    if(money>gra.get_money()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("You don't have enough money.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    gra.new_game(bombs, money);
    game_start();
}
void MainWindow::handleBoardClick()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;
    QString buttonName = clickedButton->objectName();
    if (buttonName.startsWith("pole")) {
        int x = buttonName.mid(4, 1).toInt();
        int y = buttonName.mid(5, 1).toInt();
        if(cl[x][y]) return;
        cl[x][y]=true;
        int res = gra.click(x, y);
        if(res){
            QString buttonName = QString("pole%1%2").arg(x).arg(y);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            QIcon icon(":/img/green.png");
            button->setIcon(icon);
            button->setIconSize(QSize(150, 150));
            button->setCursor(Qt::ArrowCursor);
        }
        else{
            QString buttonName = QString("pole%1%2").arg(x).arg(y);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            QIcon icon(":/img/red.png");
            button->setIcon(icon);
            button->setIconSize(QSize(150, 150));
            button->setCursor(Qt::ArrowCursor);
            game_end();
        }
        update_stats();
        if(res==2) game_end();
    }
}


void MainWindow::on_cashout_clicked()
{
    game_end();
}
void MainWindow::generateChart(const std::vector<long double>& accountHistory) {
    QLineSeries *series = new QLineSeries();
    for (size_t i = 0; i < accountHistory.size(); ++i) {
        series->append(static_cast<qreal>(i), static_cast<qreal>(accountHistory[i]));
    }
    series->setPointsVisible(false);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Balance History");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Balance");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    if (ui->chartarea->layout() != nullptr) {
        QLayout *oldLayout = ui->chartarea->layout();
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView);
    ui->chartarea->setLayout(layout);
}






void MainWindow::on_actionCreator_triggered()
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Creator");
    msgBox->setText("Wiktor Lewicki, viksolve@gmail.com");

    msgBox->setStyleSheet("QMessageBox { color: white; }"
                          "QMessageBox QLabel { color: white; }"
                          "QMessageBox QPushButton { color: white; }");

    msgBox->exec();
}





