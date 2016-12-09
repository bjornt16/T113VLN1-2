#include "gui.h"
#include "ui_gui.h"

#include <string>
#include <iostream>
#include <QMessageBox>

Gui::Gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gui)
{
    ui->setupUi(this);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    personModel = domain.getPersonModel();
    computerModel = domain.getComputerModel();

    loadTopTable(personModel);

    connect(
      ui->tableView->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      SLOT(onSelectionChange(const QItemSelection &, const QItemSelection &))
     );
}

Gui::~Gui()
{
    delete ui;
}

void Gui::checkStatus(){
    bool hasChanged = 0;
    if(currentMode == Person){
        hasChanged = personModel->isDirty();
    }
    else if(currentMode == Computer)
    {
        hasChanged = computerModel->isDirty();
    }

    if(hasChanged){
        ui->saveButton->setEnabled(true);
        ui->revertButton->setEnabled(true);
    }
    else
    {
        ui->saveButton->setEnabled(false);
        ui->revertButton->setEnabled(false);
    }
}

void Gui::loadTopTable(QSqlRelationalTableModel * model){
    ui->tableView-> setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->tableView->setColumnHidden(0,true);
}

void Gui::loadBottomTable(QSqlRelationalTableModel * model){
    ui->tableView_2-> setModel(model);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->tableView_2->setColumnHidden(0,true);
}


void Gui::on_addEditButton_clicked()
{
    QModelIndexList selectedList = ui->tableView->selectionModel()->selectedRows();

    if(currentMode == Person){

        if(selectedList.size()){ //if user has selection
            onEditPersonButton();
        }else{
            onAddPersonButton();
        }

    }else if(currentMode == Computer){

        if(selectedList.size()){ //if user has selection
            onEditComputerButton();
        }else{
            onAddComputerButton();
        }

    }
}

void Gui::onAddPersonButton(){

    QMap<QString, int> gList = domain.getAcceptedGenderName();
    personDialogWindow = new PersonDialog(this,gList);

    QObject::connect(personDialogWindow, SIGNAL(personRejected()), this, SLOT(onPersonRejected()));
    QObject::connect(personDialogWindow,
                     SIGNAL(newPersonAccepted(const QString &, const int &, const QString &, const int &, const int &)),
                     this,
                     SLOT(onNewPersonAccepted(const QString &, const int &, const QString &, const int &, const int &)));

    this->setEnabled(false);
    personDialogWindow->setEnabled(true);
    personDialogWindow->show();

}

void Gui::onEditPersonButton(){
    QMap<QString, int> gList = domain.getAcceptedGenderName();

    personDialogWindow = new PersonDialog(this,gList
                          ,ui->tableView->model()->index(lastSelectedRow,1).data().toString()
                          ,ui->tableView->model()->index(lastSelectedRow,2).data().toString()
                          ,ui->tableView->model()->index(lastSelectedRow,3).data().toString()
                          ,ui->tableView->model()->index(lastSelectedRow,4).data().toInt()
                          ,ui->tableView->model()->index(lastSelectedRow,5).data().toInt()
                          ,ui->tableView->model()->index(lastSelectedRow,0).data().toInt());

    QObject::connect(personDialogWindow, SIGNAL(personRejected()), this, SLOT(onPersonRejected()));
    QObject::connect(personDialogWindow,
                     SIGNAL(editPersonAccepted(const int &,const QString &, const int &, const QString &, const int &, const int &)),
                     this,
                     SLOT(onEditPersonAccepted(const int &,const QString &, const int &, const QString &, const int &, const int &)));

    this->setEnabled(false);
    personDialogWindow->setEnabled(true);
    personDialogWindow->show();
}

void Gui::onAddComputerButton(){
    QMap<int, QString> tList = domain.getAcceptedComputerTypeName();
    computerDialogWindow = new ComputerDialog(this,tList);

    QObject::connect(computerDialogWindow, SIGNAL(computerRejected()), this, SLOT(onComputerRejected()));
    QObject::connect(computerDialogWindow,
                     SIGNAL(newComputerAccepted(const QString &, const int &, const int &, const int &)),
                     this,
                     SLOT(onComputerAccepted(const QString &, const int &, const int &, const int &)));

    this->setEnabled(false);
    computerDialogWindow->setEnabled(true);
    computerDialogWindow->show();

}

void Gui::onEditComputerButton(){
    QMap<int, QString> tList = domain.getAcceptedComputerTypeName();

    computerDialogWindow = new ComputerDialog(this,tList
                          ,ui->tableView->model()->index(lastSelectedRow,1).data().toString()
                          ,ui->tableView->model()->index(lastSelectedRow,2).data().toString()
                          ,ui->tableView->model()->index(lastSelectedRow,3).data().toInt()
                          ,ui->tableView->model()->index(lastSelectedRow,4).data().toInt()
                          ,ui->tableView->model()->index(lastSelectedRow,0).data().toInt());

    QObject::connect(computerDialogWindow, SIGNAL(computerRejected()), this, SLOT(onComputerRejected()));
    QObject::connect(computerDialogWindow,
                     SIGNAL(editComputerAccepted(const int &,const QString &, const int &, const int &, const int &)),
                     this,
                     SLOT(onEditComputerAccepted(const int &,const QString &, const int &, const int &, const int &)));

    this->setEnabled(false);
    computerDialogWindow->setEnabled(true);
    computerDialogWindow->show();
}

void Gui::onPersonRejected(){
    this->setEnabled(true);
}


void Gui::onNewPersonAccepted(const QString &n, const int &g, const QString &nat, const int &b, const int &d)
{
    this->setEnabled(true);

    QSqlRecord record = personModel->record();
    record.setValue(1,n);
    record.setValue(2,g);
    record.setValue(3,nat);
    record.setValue(4,b);
    record.setValue(5,d);
    personModel->insertRecord(-1,record);
    ui->tableView->setModel(personModel);

    checkStatus();
}

void Gui::onEditPersonAccepted(const int &id, const QString &n, const int &g, const QString &nat, const int &b, const int &d){

    this->setEnabled(true);
    QSqlRecord record = personModel->record();
    record.setValue(0,id);
    record.setValue(1,n);
    record.setValue(2,g);
    record.setValue(3,nat);
    record.setValue(4,b);
    record.setValue(5,d);
    personModel->setRecord(lastSelectedRow,record);
    ui->tableView->setModel(personModel);

    checkStatus();
};

void Gui::onComputerAccepted(const QString &n, const int &t, const int &d, const int &b){

    this->setEnabled(true);

    QSqlRecord record = computerModel->record();
    record.setValue(1,n);
    record.setValue(2,t);
    record.setValue(3,d);
    record.setValue(4,b);
    computerModel->insertRecord(-1,record);
    ui->tableView->setModel(computerModel);

    checkStatus();

}

void Gui::onEditComputerAccepted(const int &id, const QString &n, const int &t, const int &d, const int &b){

    this->setEnabled(true);
    QSqlRecord record = computerModel->record();
    record.setValue(0,id);
    record.setValue(1,n);
    record.setValue(2,t);
    record.setValue(3,d);
    record.setValue(4,b);
    computerModel->setRecord(lastSelectedRow,record);
    ui->tableView->setModel(computerModel);

    checkStatus();
};


void Gui::onComputerRejected(){
    this->setEnabled(true);
}

void Gui::on_tableView_clicked(const QModelIndex &index)
{

    if(lastSelection == index.row() ){

        overrideOnSelectionChange = true;
        std::cout << "0";

        ui->tableView->selectionModel()->clearSelection();
        ui->deleteButton->setEnabled(false);
        ui->addEditButton->setText("Add");
        lastSelection = -1;

        overrideOnSelectionChange = false;

    }else{
        lastSelection = index.row();
        ui->deleteButton->setEnabled(true);
        ui->addEditButton->setText("Edit");
    }

}


void Gui::onSelectionChange(const QItemSelection &a, const QItemSelection &b)
{
    if(!overrideOnSelectionChange)
    {
        lastSelection = -1;
        ui->deleteButton->setEnabled(true);
        ui->addEditButton->setText("Edit");

        if(ui->tableView->selectionModel()->selectedRows().size() > 1){
            ui->addEditButton->setEnabled(false);
        }else{
            ui->addEditButton->setEnabled(true);
        }
    }
}

void Gui::on_saveButton_released()
{
    if(currentMode == Person)
    {
        saveModel(personModel);
    }
    else if(currentMode == Computer)
    {
        saveModel(computerModel);
    }

    for(int i = 0; i < ui->tableView->model()->rowCount(); i++){
        ui->tableView->showRow(i);
    }

    checkStatus();
}

void Gui::saveModel(QSqlRelationalTableModel * model){
    domain.submitDatabaseChanges(model);
}

void Gui::on_deleteButton_released()
{
    QModelIndexList selList = ui->tableView->selectionModel()->selectedRows();

    if(currentMode == Person)
    {
        for(int i = 0; i < selList.size(); i++){
            ui->tableView->hideRow(selList[i].row());
            personModel->removeRow(selList[i].row());
        }
    }
    else if(currentMode == Computer)
    {
        for(int i = 0; i < selList.size(); i++){
            ui->tableView->hideRow(selList[i].row());
            computerModel->removeRow(selList[i].row());
        }
    }

    checkStatus();
}

void Gui::on_revertButton_released()
{
    if(currentMode == Person)
    {
        revertModel(personModel);
    }
    else if(currentMode == Computer)
    {
        revertModel(computerModel);
    }

    checkStatus();
}

void Gui::revertModel(QSqlRelationalTableModel * model){

    model->revertAll();

    for(int i = 0; i < model->rowCount(); i++){
        ui->tableView->showRow(i);
    }

    ui->tableView->setModel(model);
}

void Gui::on_comboBox_currentIndexChanged(int index)
{

    if(index == 0){ //person

        switchToPerson();

    }else if(index == 1){ //computer

        switchToComputer();
    }

    connect(
      ui->tableView->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      SLOT(onSelectionChange(const QItemSelection &, const QItemSelection &))
     );
}

void Gui::switchToPerson(){ //happens on switch to person

    if(computerModel->isDirty()){
        QString promptTitle = "Save";
        QString promptQuestion = "Would you like to save Computer before changing?";
        QMessageBox::StandardButton prompt = QMessageBox::question(this,promptTitle, promptQuestion ,
                                                                   QMessageBox::Yes|QMessageBox::No);

        if (prompt == QMessageBox::Yes)
        {
            saveModel(computerModel);
        }
        else if(prompt == QMessageBox::No)
        {
            revertModel(computerModel);
        }
    }

    currentMode = Person;
    ui->tableView->selectionModel()->clearSelection();
    ui->deleteButton->setEnabled(false);
    ui->addEditButton->setText("Add");
    loadTopTable(domain.getPersonModel());
    //todo load bottom
}

void Gui::switchToComputer(){ //happens on switch to computer

    if(personModel->isDirty()){
        QString promptTitle = "Save";
        QString promptQuestion = "Would you like to save Computer before changing?";
        QMessageBox::StandardButton prompt = QMessageBox::question(this, promptTitle, promptQuestion ,
                                                                   QMessageBox::Yes|QMessageBox::No);

        if (prompt == QMessageBox::Yes)
        {
            saveModel(personModel);
        }
        else if(prompt == QMessageBox::No)
        {
            revertModel(personModel);
        }
    }

     currentMode = Computer;
     ui->tableView->selectionModel()->clearSelection();
     ui->deleteButton->setEnabled(false);
     ui->addEditButton->setText("Add");
     loadTopTable(domain.getComputerModel());
     //todo load bottom

}
