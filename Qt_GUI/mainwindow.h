#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QApplication>
#include <QMainWindow>
#include <QSessionManager>
#include <QPlainTextEdit>

#include "pl0_parser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

    void loadFile(const QString& fileName);
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void newfile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void generateCode(bool);
    void showCode(bool);
    void runCode(bool);
    void errorHandle(int row, int err_code);
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager& );
#endif
private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString& filename);
    void setCurrentFile(const QString& filename);
    QString strippedName(const QString& fullFileName);

    void closeParser();

    QPlainTextEdit* textEdit;
    QString currFile;

    bool isSucessed;
    pl0_parser* parser;
};

#endif // MAINWINDOW_H
