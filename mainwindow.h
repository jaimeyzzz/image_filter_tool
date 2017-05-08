#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "imageannotations.h"
#include "imageviewwidget.h"
#include "scrollareanowheel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString currentFilePath() const;
    QString currentDirPath() const;
    IA::File *currentFile() const;

    // Image Viewer
    void setImage();
    // File Viewer
    void refreshFileView();
public slots:
    void on_actionOpenImages_triggered();
    void on_actionOpenFolder_triggered();
    void on_actionOpenDatabase_triggered();
    void on_actionSaveDatabase_triggered();
    void on_actionExportImages_triggered();
    void on_actionQuit_triggered();
    void on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void on_yesButton_clicked();
    void on_noButton_clicked();
    void keyPressEvent(QKeyEvent *);
private:
    // Image Viewer;
    ImageViewWidget *imageViewWidget;
    ScrollAreaNoWheel *scrollArea;
    // File Viewer;
    IA::ImageAnnotations annotations;
    QString lastDatabasePath, lastImageFolderPath, lastOutputFolderPath;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
