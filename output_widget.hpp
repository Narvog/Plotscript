#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include "interpreter.hpp"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QGraphicsTextItem>

class OutputWidget : public QWidget
{
	Q_OBJECT

public:
	OutputWidget(QWidget * parent = nullptr);
	void resizeEvent(QResizeEvent *ev);
public slots:
void recievedData(QString info);
void recievedExp(Expression exp);


private:
	void helperOut(Expression exp);
	QGraphicsScene * gScene = new QGraphicsScene();
	QGraphicsView * gView = new QGraphicsView(gScene);
	QString passed;
};

#endif