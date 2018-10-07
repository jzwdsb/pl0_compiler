
#include "mainwindow.h"

#include <QWidget>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QDesktopWidget>
#include <QTextStream>

#include "Scanner.h"
#include "Lexer.h"
#include "pl0.h"

#include "rundialog.h"

const QString err_msg [] =
    {
        /*  0*/      "",
        /*  1*/      "Found ':=' when expecting '='",
        /*  2*/      "There must be a number to follow",
        /*  3*/      "There must be an '=' to follow the identifier",
        /*  4*/      "There must be an identifier to follow 'const', 'var' or 'procedure'",
        /*  5*/      "Missing ',' or ';'",
        /*  6*/      "Incorrect procedure name",
        /*  7*/      "Statement expected",
        /*  8*/      "Follow the statement is an incorrect symbol",
        /*  9*/      "'.' expected",
        /* 10*/      "',' expected",
        /* 11*/      "Undeclared identifier",
        /* 12*/      "Illegal assignment",
        /* 13*/      "':=' expected",
        /* 14*/      "There must be an identifier to follow the call",
        /* 15*/      "A constant or variable can not be called",
        /* 16*/      "'then' expected.",
        /* 17*/      "';' or 'end' expected",
        /* 18*/      "'do' expected.",
        /* 19*/      "Incorrect symbol.",
        /* 20*/      "Relative operators expected",
        /* 21*/      "Procedure identifier can not be in an expression",
        /* 22*/      "Missing ')'.",
        /* 23*/      "The symbol can not be followed by an factor",
        /* 24*/      "The symbol can not be as the beginning of an expression",
        /* 25*/      "Invalid Instruction",
        /* 26*/      "",
        /* 27*/      "",
        /* 28*/      "",
        /* 29*/      "",
        /* 30*/      "",
        /* 31*/      "The number is too great",
        /* 32*/      "There are too many levels",
    };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    textEdit(new QPlainTextEdit)
{
    this->parser = new pl0_parser();

    this->setCentralWidget(textEdit);

    this->createActions();
    this->createStatusBar();

    this->readSettings();

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    connect(this->parser, SIGNAL(error(int, int)), this, SLOT(errorHandle(int, int)));
#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)),
            this, SLOT(commitData(QSessionManager&)));
#endif
    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
    delete this->parser;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        writeSettings();
        event->accept();
    }else
    {
        event->ignore();
    }

}

void MainWindow::newfile()
{
    if(maybeSave())
    {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("选择源文件"),
                                                        tr("/home/manout/pl0_compiler"),
                                                        tr("text *pl0 *cpp *java *py"));
        if (not fileName.isEmpty())
        {
            loadFile(fileName);
        }
    }
}

bool MainWindow::save()
{
    if(currFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(currFile);
    }

}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() not_eq QDialog::Accepted)
    {
        return false;
    }
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::about()
{
    QMessageBox::about(this,
                       tr("about this Application"),
                       tr("The <b>Application</b> example demoonstrates how to "
                          "write modern GUI applicaion using Qt, with a menu bar,"
                          "toolbars, and a status bar"));

}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::generateCode(bool)
{
    this->maybeSave();
    this->parser->setCodeSource(this->currFile);
    this->isSucessed = true;
    this->parser->parserCode();
    const QList<QString>* parserStr = this->parser->codeToStr();
    QString doc;
    for (const QString& line : *parserStr)
    {
        doc.append(line);
    }
    QString filename = tr("/home/manout/parseResult.txt");
    QFile parserFile(filename);
    if ( not parserFile.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("警告"),
                             tr("不能写入文件 %1:\n%2").
                             arg(QDir::toNativeSeparators(filename)).
                             arg(parserFile.errorString()));
        return ;
    }
    QTextStream out(&parserFile);
#ifndef QT_NOT_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << doc;
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    if (isSucessed)
    {
        QMessageBox::about(this, tr("完成"), tr("编译完成"));
    }
    else
    {
        QMessageBox::about(this, tr("出错"), tr("编译出错"));
    }
    return ;
}

void MainWindow::showCode(bool)
{
    const QList<QString>* codeStr = this->parser->codeToStr();
    QString content;
    std::for_each(codeStr->begin(), codeStr->end(), [&](const QString& str){content.append(str);});
    this->setWindowFilePath(tr("中间代码"));
    this->textEdit->setPlainText(content);
}

void MainWindow::runCode(bool)
{
    RunDialog rundialog(this,  this->parser->codeToStr(), this->parser->getCode());
    if (rundialog.exec() == QDialog::Accepted)
        return ;
}

void MainWindow::errorHandle(int row, int err_code)
{
    this->isSucessed = false;
    QMessageBox::warning(this, tr("编译出错"),
                         tr("%1 行, 错误信息:\n\t%3").arg(row).arg(err_msg[err_code]),
                         QMessageBox::Cancel, QMessageBox::Accepted);
}

void MainWindow::commitData(QSessionManager &)
{

}

void MainWindow::createActions()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&文件"));
    QMenu* buildMenu = menuBar()->addMenu(tr("&构建"));
    QToolBar* fileToolBar = addToolBar(tr("文件"));
    QToolBar* buildToolBar = addToolBar(tr("构建"));

    const QIcon newIcon = QIcon::fromTheme(tr("document-new"), QIcon(tr(":/icon/new")));
    QAction *newAct = new QAction(newIcon, tr("&新建"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("创建新文件"));
    connect(newAct, SIGNAL(triggered(bool)), this, SLOT(newfile()));
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme(tr("document-open"), QIcon(tr(":/icon/open")));
    QAction *openAct = new QAction(openIcon, tr("&打开"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("打开现有文件"));
    connect(openAct, SIGNAL(triggered(bool)), this, SLOT(open()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme(tr("document-save"), QIcon(tr(":/icon/save")));
    QAction *saveAct = new QAction(saveIcon, tr("&保存"),this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("保存"));
    connect(saveAct, SIGNAL(triggered(bool)), this, SLOT(save()));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme(tr("document-saveAs"), QIcon(tr(":/icon/saveAs")));
    QAction *saveAsAct = new QAction(saveAsIcon, tr("&另存为"), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("另存为"));
    connect(saveAsAct, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    fileMenu->addAction(saveAsAct);
    fileToolBar->addAction(saveAsAct);

    const QIcon complieIcon = QIcon::fromTheme(tr("code-compile"), QIcon(tr(":/icon/compiler")));
    QAction *compilerAct = new QAction(complieIcon, tr("&编译"), this);
    compilerAct->setShortcut(QKeySequence(tr("F5")));
    compilerAct->setStatusTip(tr("编译"));
    connect(compilerAct, SIGNAL(triggered(bool)), this, SLOT(generateCode(bool)));
    buildMenu->addAction(compilerAct);
    buildToolBar->addAction(compilerAct);

    const QIcon showcodeIcon = QIcon::fromTheme(tr("document-show"), QIcon(tr(":/icon/show")));
    QAction* showAct = new QAction(showcodeIcon, tr("&显示中间代码"), this);
    showAct->setShortcut(QKeySequence("F6"));
    showAct->setStatusTip(tr("显示中间代码"));
    connect(showAct, SIGNAL(triggered(bool)), this, SLOT(showCode(bool)));
    buildMenu->addAction(showAct);
    buildToolBar->addAction(showAct);

    const QIcon runIcon = QIcon::fromTheme(tr("run-code"), QIcon(tr(":/icon/run")));
    QAction* runAct = new QAction (runIcon, tr("&运行"), this);
    runAct->setShortcut(QKeySequence(tr("F7")));
    runAct->setStatusTip(tr("运行"));
    connect(runAct, SIGNAL(triggered(bool)), this, SLOT(runCode(bool)));
    buildMenu->addAction(runAct);
    buildToolBar->addAction(runAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::readSettings()
{
    QSettings setting(QCoreApplication::organizationName(),
                      QCoreApplication::applicationName());
    const QByteArray geometry = setting.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        this->resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        this->move((availableGeometry.width() - this->width()) / 2,
                    (availableGeometry.height() - this->height()) / 2);
    }
    else
    {
        this->restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings setting (QCoreApplication::organizationName(), QCoreApplication::applicationName());
    setting.setValue("geometry", this->saveGeometry());
}

bool MainWindow::maybeSave()
{
    if (not this->textEdit->document()->isModified())
    {
        return true;
    }
    const QMessageBox::StandardButton ret =
        QMessageBox::warning(this,
                             tr("是否保存"),
                             tr("这份文件已经过编辑，是否保存？"),
                             QMessageBox::Save |
                             QMessageBox::Discard |
                             QMessageBox::Cancel);
    switch (ret)
    {
    case QMessageBox::Save:
        return this->save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &filename)
{
    QFile file(filename);

    if (not file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("警告"),
                             tr("不能读取文件　%1:\n%2")
                             .arg(QDir::toNativeSeparators(filename), file.errorString()));
        return ;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    this->textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    this->setCurrentFile(filename);
    this->statusBar()->showMessage(tr("已加载文件"), 2000);
}

bool MainWindow::saveFile(const QString &filename)
{
    QFile file(filename);
    if (not file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("警告"),
                             tr("不能写入文件 %1:\n%2").
                             arg(QDir::toNativeSeparators(filename)).
                             arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
#ifndef QT_NOT_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << this->textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    this->setCurrentFile(filename);
    this->statusBar()->showMessage(tr("文件已保存"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &filename)
{
    this->currFile = filename;
    textEdit->document()->setModified(false);

    QString shownName = this->currFile;
    if (currFile.isEmpty())
    {
        shownName = tr("untitled.txt");
    }
    this->setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::closeParser()
{
    this->parser->close();
}
