
#include <QTableWidget>
#include <QInputDialog>
#include <QMessageBox>

#include "rundialog.h"
#include "ui_rundialog.h"

const int MAX_STACK = 128;

RunDialog::RunDialog(QWidget *parent, const QList<QString> *source, const QList<instruction>* code) :
    QDialog(parent), source(source), code(code),
    ui(new Ui::RunDialog)
{
    ui->setupUi(this);
    connect(ui->execOneStepBtn, SIGNAL(clicked(bool)), this, SLOT(runOneStep()));
    connect(ui->execBtn, SIGNAL(clicked(bool)), this, SLOT(run()));
    connect(ui->pauseBtn, SIGNAL(clicked(bool)), this, SLOT(pause()));
    connect(ui->quitBtn, SIGNAL(clicked(bool)), this, SLOT(quit()));
    connect(ui->speedValue, SIGNAL(valueChanged(int)), this, SLOT(changeSpeed(int)));
    createStackTable();

    reset();


    QString doc;
    for (int i = 0; i < code->size(); ++i)
    {
        doc.append(tr("%1 : %2").arg(i).arg(source->at(i)));
    }

    ui->codeShow->setText(doc);

}

RunDialog::~RunDialog()
{
    delete ui;
}

void RunDialog::reset()
{
    prevIR = 0;
    /**	初始化程序计数器，从翻译的 code 段起始位置计数*/
    PC = 0;

    /** 初始化栈基寄存器和栈顶寄存器*/
    ESP = 3;
    EBP = 0;

    /** 初始化顶层静态链，动态链, 返回地址RA*/
    runtime_stack[0] = 0;
    runtime_stack[1] = 0;
    runtime_stack[2] = 0;
}

void RunDialog::runOneStep()
{
    runOneCode();
    updateLabel();
}

void RunDialog::run()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(runOneStep()));
    timer->start(1000);
}

void RunDialog::pause()
{
    disconnect(timer, SIGNAL(timeout()), this, SLOT(runOneCode()));
}

void RunDialog::quit()
{
    this->done(QDialog::Accepted);
}

int RunDialog::base(int l)
{
    int b = this->EBP;
    while (l--)
    {
        b = this->runtime_stack[b];
    }
    return b;
}

void RunDialog::updateLabel()
{
    ui->IRlabel->setText(tr("%1").arg(source->at(prevIR)));
    ui->PClabel->setText(QString::number(PC));
    ui->ESPlabel->setText(QString::number(ESP));
    ui->EBPlabel->setText(QString::number(EBP));
    for (int i = 0; i < ESP; ++i)
    {
        ui->runtimeStackShow->item(i, 0)->setText(QString::number(runtime_stack[i]));
    }
}

void RunDialog::changeSpeed(int speed)
{
    if (speed == 0)
    {
        timer->start(10000);
    }
    else
    {
        timer->start(10000 / speed);
    }
}

void RunDialog::createStackTable()
{
    ui->runtimeStackShow->setRowCount(MAX_STACK);
    ui->runtimeStackShow->setColumnCount(1);
    for (int i = MAX_STACK; i >= 0; --i)
    {
        QTableWidgetItem* newitem = new QTableWidgetItem();
        newitem->setText(QString::number(0));
        ui->runtimeStackShow->setItem(i, 0, newitem);
    }
}

/** 对　PC　更新操作作如下约定
 * 		１　顺序执行， PC每取一条指令自动加１，自增过程发生在取指令操作之后
 * 		２　跳转操作， 当发生跳转指令时，约定指令所指示的位置即为新的PC值(此点由编译器保证),
 * 		   跳转之后直接取指令然后更新PC
 * */
#define stack_operate(OP)   runtime_stack[ESP - 2] = runtime_stack[ESP - 2] OP runtime_stack[ESP - 1]
void RunDialog::runOneCode()
{
    IR = code->at(PC);
    prevIR = PC;
    ++PC;
    switch (IR.OP)
    {
        case fct::lit :
            /** lit 0 M
             * 		push the constant value M onto the stack*/
            runtime_stack[ESP] = IR.M;
            ++ESP;
            break;
        case fct::opr :
            switch (IR.M)
            {
                case 0:
                    /**	opr 0 0
                     * 		return to the caller from a procedure*/
                    ESP = EBP;
                    PC = runtime_stack[ESP  + 2];
                    EBP = runtime_stack[ESP + 1];
                    break;
                case 1:
                    /** opr 0 1
                     * 		Negation, pop the stack and return the negative of the value
                     * */
                    runtime_stack[ESP] = -runtime_stack[ESP];
                    break;
                case 2:
                    /**	opr 0 2
                     * 		add
                     * */
                    stack_operate(+);
                    --ESP;
                    break;
                case 3:
                    /**	opr 0 3
                     * 		sub
                     * */
                    stack_operate(-);
                    --ESP;
                    break;
                case 4:
                    /** opr 0 4
                     * 		multiplication
                     * */
                    stack_operate(*);
                    --ESP;
                    break;
                case 5:
                    /**	opr 0 5
                     * 		divide
                     * */
                    stack_operate(/);
                    --ESP;
                    break;
                case 6:
                    /**	opr 0 6
                     * 		is it odd? pop the stack and push 1 if odd, 0 if even*/
                    runtime_stack[ESP - 1] = runtime_stack[ESP - 1] % 2;
                    break;
                case 7:
                    /**	opr 0 7
                     * 		modulus ;pop two values from the stack, divide second
                     *   and push the reminder
                     * */
                    stack_operate(%);
                    --ESP;
                    break;
                case 8:
                    /** opr 0 9
                     * 		equality; pop two values from the stack
                     * 	 and push 1 if equal, 0 if not*/
                    stack_operate(==);
                    --ESP;
                    break;
                case 9:
                    /**	opr 0 9
                     * 		inequality
                     * 	 pop two values, push 0 if is equal, 1 if not*/
                    stack_operate(!=);
                    --ESP;
                    break;
                case 10:
                    /**	opr 0 10
                     * 		less then
                     * 	 pop two values, push 1 if first is less then second, 0 if not*/
                    stack_operate(>);
                    --ESP;
                    break;
                 case 11:
                    /**	opr 0 11
                     * 		less then or equal then
                     * 	 pop two values, push 1 if first is less then  or equal second, 0 if not*/
                    stack_operate(>=);
                    --ESP;
                    break;
                 case 12:
                    /** opr 0 12
                     * 		greater then
                     * */
                    stack_operate(<);
                    --ESP;
                    break;
                 case 13:
                    /**	opr 0 13
                     * 		greater or equal to
                     * */
                    stack_operate(<=);
                    --ESP;
                    break;
                 default:
                    break;
                }
                break;
            case fct::lod :
                runtime_stack[ESP] = runtime_stack[base(IR.L) + IR.M];
                ++ESP;
                break;
            case fct::lda:
                runtime_stack[ESP - 1] = runtime_stack[base(IR.L) + IR.M + runtime_stack[ESP - 1]];
                break;
            case fct::sto :
                runtime_stack[base(IR.L) + IR.M] = runtime_stack[ESP - 1];
                break;
            case fct::sta:
                runtime_stack[base(IR.L) + IR.M + runtime_stack[ESP - 2]] = runtime_stack[ESP - 1];
                break;
            case fct::cal :
                runtime_stack[ESP] = base(IR.L);
                runtime_stack[ESP + 1] = EBP;
                runtime_stack[ESP + 2] = PC;
                EBP = ESP;
                ESP += 3;
                PC = IR.M;
                break;
            case fct::inc :
                ESP += IR.M;
                break;
            case fct::jmp :
                PC = IR.M;
                break;
            case fct::jpc :
                if (runtime_stack[ESP - 1] == 0)
                {
                    PC = IR.M;
                }
                break;
            case fct::sio :
                switch (IR.M)
                {
                    case 1:
                        QMessageBox::about(this, tr("输出"), tr("当前输出为:\n%1").arg(runtime_stack[ESP -1]));
                        --ESP;
                        break;
                    case 2:
                        runtime_stack[ESP] = QInputDialog::getInt(this, tr("输入"), tr("请输入："));
                        ++ESP;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    if (PC == 0)
    {
        reset();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(runOneStep()));
        QMessageBox::about(this, tr("完成"), tr("程序运行结束"));
    }
}
