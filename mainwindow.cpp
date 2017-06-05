#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDirModel>
#include <QDirIterator>
#include <boost/foreach.hpp>
#include <string>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QSize>
#include <sstream>
#include <fstream>
#include <iostream>
#include <QDebug>

using namespace IA;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    scrollArea = new ScrollAreaNoWheel(this);
    imageViewWidget = new ImageViewWidget(scrollArea, scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(imageViewWidget);
    setCentralWidget(scrollArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::currentDirPath() const
{
    QTreeWidgetItem *current = ui->fileTreeWidget->currentItem();
    if (!current || !current->parent())
        return "";

    return current->parent()->text(0);
}

QString MainWindow::currentFilePath() const
{
    QTreeWidgetItem *current = ui->fileTreeWidget->currentItem();
    if (NULL == current || NULL == current->parent())
        return "";

    return currentDirPath() + "/" + current->text(0);
}

IA::File* MainWindow::currentFile() const
{
    return annotations.getFile(currentFilePath().toStdString());
}

/**********
 * File Tree View
 */
void MainWindow::refreshFileView()
{
    // clear and block;
    ui->fileTreeWidget->blockSignals(true);
    ui->fileTreeWidget->setUpdatesEnabled(false);
    ui->fileTreeWidget->clear();
    // add all dirs and files to the QTreeWidget
    QList<QTreeWidgetItem*> newItems;
    BOOST_FOREACH(Dir *dir, annotations.getDirs()) {
        // construct a new directory entry
        QTreeWidgetItem *dirItem = new QTreeWidgetItem();
        ui->fileTreeWidget->setItemExpanded(dirItem, true);
        dirItem->setText(0, QString::fromStdString(dir->getDirPath()));

        BOOST_FOREACH(File* file, dir->getFiles()) {
            // construct a new file entry
            string fileName = annotations.fileName(file->getFilePath());
            AnnotationFlag flag = file->getFlag();
            QTreeWidgetItem *fileEntry = new QTreeWidgetItem(dirItem);
            fileEntry->setText(0, QString::fromStdString(fileName));
            switch(flag)
            {
            case YES:
                fileEntry->setText(1, "YES");
                break;
            case NO:
                fileEntry->setText(1, "NO");
                break;
            case UNKOWN:
            default:
                break;
            }
        }

        // collect item in list
        newItems.append(dirItem);
    }
    // add all items at once to the tree widget
    ui->fileTreeWidget->addTopLevelItems(newItems);
    // sort the files + directories
    ui->fileTreeWidget->sortItems(0, Qt::AscendingOrder);
    // unblock signals
    ui->fileTreeWidget->setUpdatesEnabled(true);
    ui->fileTreeWidget->blockSignals(false);
    ui->fileTreeWidget->expandAll();
}

void MainWindow::on_fileTreeWidget_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    // check wether dir/file/object have been selected
    QString filePath = currentFilePath();
    qDebug() << filePath;
    if (filePath.isEmpty())
        return;

    // check wether we have a relative or absolute path
    if (!QDir::isAbsolutePath(filePath)) filePath = lastImageFolderPath + "/" + filePath;

    QPixmap tmpPixmap(filePath);
    imageViewWidget->setPixmap(tmpPixmap);
}

void MainWindow::on_prevButton_clicked()
{
    QTreeWidgetItem * curItem = ui->fileTreeWidget->currentItem();
    if (curItem == 0)
        return;
    QTreeWidgetItemIterator it(curItem);
    it --;
    if ((*it) == 0)
        return;
    ui->fileTreeWidget->setCurrentItem(*(it));
}

void MainWindow::on_nextButton_clicked()
{
    QTreeWidgetItem * curItem = ui->fileTreeWidget->currentItem();
    if (curItem == 0)
        return;
    QTreeWidgetItemIterator it(curItem);
    it ++;
    if ((*it) == 0)
        return;
    ui->fileTreeWidget->setCurrentItem(*(it));
}

void MainWindow::on_yesButton_clicked()
{
    File* curFile = currentFile();
    if (curFile == 0)
        return;
    curFile->setFlag(IA::AnnotationFlag::YES);
    QTreeWidgetItem* curItem = ui->fileTreeWidget->currentItem();
    curItem->setText(1, "YES");

}

void MainWindow::on_noButton_clicked()
{
    File* curFile = currentFile();
    if (curFile == 0)
        return;
    curFile->setFlag(IA::AnnotationFlag::NO);
    QTreeWidgetItem* curItem = ui->fileTreeWidget->currentItem();
    curItem->setText(1, "NO");
}

/**********
 *  Menu Bar
 */
void MainWindow::on_actionOpenImages_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Open Images",
                lastImageFolderPath,
                "Images (*.jpg *.png *.bmp)");
    if (files.empty())
        return;

    QStringListIterator it(files);
    while(it.hasNext())
    {
        QString name = it.next();
        QFileInfo info(name);
        annotations.addFile(info.absoluteFilePath().toStdString());
    }

    refreshFileView();
}

void MainWindow::on_actionOpenDatabase_triggered()
{
    // clear the status bar and set the normal mode for the pixmapWidget
    //pixmapWidget->setMouseMode(AnnotationsPixmapWidget::Normal);
    //statusBar()->clearMessage();

    // ask the user to add files
    QString file = QFileDialog::getOpenFileName(
            this,
            "Open an Existing Database",
            lastDatabasePath,
            "Annotation Database (*.adb)");

    if (file.isEmpty())
        return;

    annotations.loadFromFile(file.toStdString());
    refreshFileView();
}

void MainWindow::on_actionSaveDatabase_triggered()
{
    QString file = QFileDialog::getSaveFileName(
            this,
            "Save to Database",
            lastDatabasePath,
            "Annotation Database (*.adb)");

    if (file.isEmpty())
        return;

    annotations.saveToFile(file.toStdString());
    refreshFileView();
}

void MainWindow::on_actionOpenFolder_triggered()
{
    QString folder = QFileDialog::getExistingDirectory(
            this,
            "Open Images from Folder",
            lastImageFolderPath);

    if (folder.isEmpty())
        return;

    lastImageFolderPath = folder;
    QDirIterator it(folder, QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    while (it.hasNext())
    {
        QString name = it.next();
        QFileInfo info(name);
        if (!info.isDir())
        {
            if (info.suffix() == "jpg" || info.suffix() == "bmp" || info.suffix() == "png")
            {
                annotations.addFile(info.absoluteFilePath().toStdString());
            }
        }
    }

    refreshFileView();
}

void MainWindow::on_actionExportImages_triggered()
{
    QString folder = QFileDialog::getExistingDirectory(
            this,
            "Save Images to Folder",
            lastOutputFolderPath);

    if (folder.isEmpty())
        return;

    lastOutputFolderPath = folder;

    int errNum = 0, fileNum = 0;
    int cover = 0; // 0 : no, 1 : cover all, 2 : skip all;
    BOOST_FOREACH(Dir *dir, annotations.getDirs()) {
        BOOST_FOREACH(File* file, dir->getFiles())
        {
            QString filePath = QString::fromStdString(file->getFilePath());
            QString fileName = QString::fromStdString(annotations.fileName(file->getFilePath()));
            QString outPath = folder + "/" + fileName;
            AnnotationFlag flag = file->getFlag();
            bool skip = false;

            ui->statusBar->showMessage(tr("Copy file : \"") + fileName + "\"...");
            if (flag == AnnotationFlag::YES)
            {
                if (!QFile::exists(filePath))
                {
                    qDebug() << "inputNotExist";
                }
                if (QFile::exists(outPath))
                {
                    if (cover == 0)
                    {
                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, tr("File Exist"),
                            "file \"" + fileName + "\" exist, cover of not?",
                            QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll);
                        switch(reply)
                        {
                        case QMessageBox::YesToAll:
                            cover = 1;
                        case QMessageBox::Yes:
                            QFile::remove(outPath);
                            break;
                        case QMessageBox::NoToAll:
                            cover = 2;
                        case QMessageBox::No:
                            skip = true;
                        default:
                            break;
                        }
                    }
                    else if (cover == 1)
                    {
                        QFile::remove(outPath);
                    }
                    else if (cover == 2)
                        skip = true;
                }
                if (!skip && !QFile::copy(filePath, outPath))
                {
                    QMessageBox msgBox(QMessageBox::Warning, tr("Error"),
                                       tr("Error copy file \"") + fileName + "\"", 0, this);
                    msgBox.exec();
                    errNum ++;
                }
                fileNum ++;
            }
        }
    }
    ui->statusBar->showMessage(tr("Finish copy, ") + QString::number(fileNum) + tr(" files copied, ") + QString::number(errNum) + tr(" errors happened."));
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionShortcuts_triggered()
{
    QString shortcuts = tr("");
    shortcuts += tr("Y：将当前图片标记为YES \n");
    shortcuts += tr("N：将当前图片标记为NO \n");
    shortcuts += tr("Up 上方向键：上一张图 \n");
    shortcuts += tr("Down 下方向键：下一张图 \n");
    QMessageBox msgBox(QMessageBox::Information, tr("Short Cuts"),
                       shortcuts, 0, this);
    msgBox.exec();
}

void MainWindow::on_actionSplitImage_triggered()
{
//    QString folder = QFileDialog::getExistingDirectory(
//            this,
//            "Open Images from Folder",
//            lastImageFolderPath);

//    if (folder.isEmpty())
//        return;

//    lastImageFolderPath = folder;
//    QDirIterator it(folder, QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
//    while (it.hasNext())
//    {
//        QString name = it.next();
//        QFileInfo info(name);
//        if (!info.isDir())
//        {
//            if (info.suffix() == "jpg" || info.suffix() == "bmp" || info.suffix() == "png")
//            {
//                QImage image(info.absoluteFilePath());
//                if (image.size() != QSize(8192, 4096)) continue;
//                QImage crop0 = image.copy(1024, 1024, 3072, 3072);
//                QImage crop1 = image.copy(1024 + 2048, 1024, 3072 + 2048, 3072);
//                QImage crop2 = image.copy(1024 + 4096, 1024, 3072 + 4096, 3072);
//                QImage crop3 = image.copy(8192 - 1024, 1024, 8192, 3072);
//                QImage crop4 = image.copy(0, 1024, 1024, 3072);

//            }
//        }
//    }
}

void MainWindow::on_actionStatistic_triggered()
{
    int lc = 0, yc = 0, nc = 0, tc = 0;
    BOOST_FOREACH(Dir *dir, annotations.getDirs()) {
        // construct a new directory entry
        BOOST_FOREACH(File* file, dir->getFiles()) {
            // construct a new file entry
            //string fileName = annotations.fileName(file->getFilePath());
            tc ++;
            AnnotationFlag flag = file->getFlag();
            switch(flag)
            {
            case YES:
                yc ++;
                lc ++;
                break;
            case NO:
                nc ++;
                lc ++;
                break;
            case UNKOWN:
            default:
                break;
            }
        }
    }
    QString stat = tr("共有 ") + QString::number(tc) + tr(" 张图 \n");
    stat += tr("已标 : ") + QString::number(lc) + tr("\n");
    stat += tr("YES : ") + QString::number(yc) + tr("\n");
    stat += tr("NO : ") + QString::number(nc) + tr("\n");
    QMessageBox msgBox(QMessageBox::Information, tr("Statistic"),
                       stat, 0, this);
    msgBox.exec();
}

// Export Image List File;
void MainWindow::on_actionExportList_triggered()
{

    QString listPath = QFileDialog::getSaveFileName(
            this,
            "Save to Database",
            lastDatabasePath,
            "Text(*.txt)");

    if (listPath.isEmpty())
        return;

    IA::ImageAnnotations tmpAnnos;
    // remove 0 1 2 3 in image suffix;
    BOOST_FOREACH(Dir *dir, annotations.getDirs()) {
        // construct a new directory entry
        BOOST_FOREACH(File* file, dir->getFiles()) {
            // construct a new file entry
            //string fileName = annotations.fileName(file->getFilePath());
            AnnotationFlag flag = file->getFlag();
            string filePath;
            switch(flag)
            {
            case YES:
                filePath = file->getFilePath();
                filePath.replace(filePath.length() - 6, 2, "");
                qDebug() << "file : " << QString::fromStdString(filePath);
                tmpAnnos.addFile(filePath);
                break;
            case NO:
            case UNKOWN:
            default:
                break;
            }
        }
    }

    std::ofstream out(listPath.toStdString().c_str(), std::ios::trunc);
    if (!out) {
        cerr << "export image list" << endl;
        cerr << "\tError opening the file: " << listPath.toStdString() << endl;
        return;
    }

    BOOST_FOREACH(Dir *dir, tmpAnnos.getDirs()) {
        // construct a new directory entry
        BOOST_FOREACH(File* file, dir->getFiles()) {
            // construct a new file entry
            //string fileName = annotations.fileName(file->getFilePath());
            string name = IA::ImageAnnotations::fileName(file->getFilePath());
            qDebug() << QString::fromStdString(name);


            out << name << "\n";
        }
    }
    out.close();

}

/***********
 * Event
 */
void MainWindow::keyPressEvent(QKeyEvent * event)
{
    switch(event->key())
    {
    case Qt::Key_Up:
        on_prevButton_clicked();
        break;
    case Qt::Key_Down:
        on_nextButton_clicked();
        break;
    case Qt::Key_Y:
        on_yesButton_clicked();
        break;
    case Qt::Key_N:
        on_noButton_clicked();
        break;
    default:
        break;
    }
}
