#ifndef QTIMAGEVIEWER_H
#define QTIMAGEVIEWER_H

#include <QWidget>

class qtimageViewer : public QWidget
{
    Q_OBJECT

public:
    qtimageViewer(QWidget *parent = nullptr);
    ~qtimageViewer();
};
#endif // QTIMAGEVIEWER_H
