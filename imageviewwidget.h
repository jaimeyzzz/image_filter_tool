#ifndef IMAGEVIEWWIDGET_H
#define IMAGEVIEWWIDGET_H

#include <QWidget>
#include <QPixmap>

class QAbstractScrollArea;

class ImageViewWidget : public QWidget
{
    Q_OBJECT

    QPixmap *m_pm;
    double zoomFactor;
    QMatrix currentMatrixInv;
    QMatrix currentMatrix;
    QAbstractScrollArea *scrollArea;
protected:
    void paintEvent(QPaintEvent*);
//	void wheelEvent(QWheelEvent*);
    QMatrix getMatrixInv() const;
    QMatrix getMatrix() const;
public:
    ImageViewWidget( QAbstractScrollArea* scrollArea, QWidget *parent=0 );
    ~ImageViewWidget();
    double getZoomFactor();
public slots:
    void setZoomFactor(double);
    void setPixmap(const QPixmap&);
signals:
    void zoomFactorChanged(double);
    void pixmapChanged(QPixmap*);
};

#endif // IMAGEVIEWWIDGET_H
