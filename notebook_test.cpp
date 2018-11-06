#include <QTest>
#include "notebook_app.hpp"

class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  

  // TODO: implement additional tests here
  void Test1();
  void Test2();
  void Test3();
  void Test4();
  void Test5();
  void Test6();
  void Test7();
  void Test8();
  void Test9();
  void Test10();
  void Test11();
  void Test12();
  void Test13();
  void Test14();
  void Test15();
  void Test16();
  void Test17();
  void Test18();
  void Test19();
  void Test20();
  void Test21();
  void Test22();
  void Test23();

private:
	NotebookApp Notebook;
	InputWidget * input;
	OutputWidget * output;
};

void NotebookTest::initTestCase(){
	Notebook.show();

	input = Notebook.findChild<InputWidget *>("input");
	QVERIFY2(input, "Couldn't find the InputWidget.");
	QVERIFY2(input->isVisible(), "InputWidget is visable");

	output = Notebook.findChild<OutputWidget *>("output");
	QVERIFY2(output, "Couldn't find the OutputWidget.");

	QVERIFY2(Notebook.isVisible(), "Notebook didn't show up.");
	QVERIFY2(output->isVisible(), "InputWidget is visable");
}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"

void NotebookTest::Test1()
{
	input->setPlainText("(get-property \"key\" (3))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene(); 
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "NONE", "Incorrect Text");

}

void NotebookTest::Test2()
{
	input->setPlainText("(cos pi)");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "(-1)", "Incorrect Text");

}

void NotebookTest::Test3()
{
	input->setPlainText("(^ e (- (* I pi)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "(-1,-1.22465e-16)", "Incorrect Text");

}

void NotebookTest::Test4()
{
	input->setPlainText("(begin (define title \"The Title\") (title))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "(\"The Title\")", "Incorrect Text");

}

void NotebookTest::Test5()
{
	input->setPlainText("(define inc (lambda (x) (+ x 1)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	QVERIFY2(list.size() == 0, "Error an Item was found in the scene when Lambda was called.");

}

void NotebookTest::Test6()
{
	input->setPlainText("(make-point 0 0)");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto ellipse = list.at(0);
	auto ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->pos() == QPointF(0,0), "Ellipse found at wrong coordinates");
}

void NotebookTest::Test7()
{
	input->setPlainText("(set-property \"size\" 20 (make-point 0 0))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto ellipse = list.at(0);
	auto ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 20.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0,0)), "Ellipse found at wrong coordinates");
}

void NotebookTest::Test8()
{
	input->setPlainText("(list (set-property \"size\" 1 (make-point 0 0)) (set-property \"size\" 2 (make-point 4 0)) (set-property \"size\" 4 (make-point 8 0)) (set-property \"size\" 8 (make-point 16 0)) (set-property \"size\" 16 (make-point 32 0)) (set-property \"size\" 32 (make-point 64 0)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	QVERIFY2(list.size() == 6, "To many points added");
	auto ellipse = list.at(5);
	auto ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 1.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 0)), "Ellipse found at wrong coordinates");

	ellipse = list.at(4);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 2.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(4, 0)), "Ellipse found at wrong coordinates");

	ellipse = list.at(3);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 4.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(8, 0)), "Ellipse found at wrong coordinates");

	ellipse = list.at(2);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 8.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(16, 0)), "Ellipse found at wrong coordinates");

	ellipse = list.at(1);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 16.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(32, 0)), "Ellipse found at wrong coordinates");

	ellipse = list.at(0);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 32.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(64, 0)), "Ellipse found at wrong coordinates");
}

void NotebookTest::Test9()
{
	input->setPlainText("(list (set-property \"size\" 1 (make-point 0 0)) (set-property \"size\" 2 (make-point 0 4)) (set-property \"size\" 4 (make-point 0 8)) (set-property \"size\" 8 (make-point 0 16)) (set-property \"size\" 16 (make-point 0 32)) (set-property \"size\" 32 (make-point 0 64)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	QVERIFY2(list.size() == 6, "To many points added");
	auto ellipse = list.at(5);
	auto ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 1.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 0)), "Ellipse found at wrong coordinates");

	ellipse = list.at(4);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 2.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 4)), "Ellipse found at wrong coordinates");

	ellipse = list.at(3);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 4.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 8)), "Ellipse found at wrong coordinates");

	ellipse = list.at(2);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 8.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 16)), "Ellipse found at wrong coordinates");

	ellipse = list.at(1);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 16.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 32)), "Ellipse found at wrong coordinates");

	ellipse = list.at(0);
	ellipseL = qgraphicsitem_cast<QGraphicsEllipseItem *>(ellipse);
	QVERIFY2(ellipseL->isVisible(), "Ellipse Not Visible");
	QVERIFY2(ellipseL->brush() == QBrush(QColor(0, 0, 0), Qt::BrushStyle(Qt::SolidPattern)), "Ellipse is wrong color");
	QVERIFY2((ellipseL->rect().height() == 32.0), "Ellipse has wrong dimensions");
	QVERIFY2((ellipseL->rect().center() == QPoint(0, 64)), "Ellipse found at wrong coordinates");
}

void NotebookTest::Test10()
{
	input->setPlainText("(make-line (make-point 0 0) (make-point 20 20))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto line = list.at(0);
	auto lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(0, 0)) && lineL->contains(QPoint(20, 20)), "Line isn't at the right points");
	QPen pen = QPen(QColor(0, 0, 0));
	pen.setWidth(1);
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");
}

void NotebookTest::Test11()
{
	input->setPlainText("(set-property \"thickness\" (4) (make-line (make-point 0 0) (make-point 20 20)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto line = list.at(0);
	auto lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(0, 0)) && lineL->contains(QPoint(20, 20)), "Line isn't at the right points");
	QPen pen = QPen(QColor(0, 0, 0));
	pen.setWidth(4);
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");
}

void NotebookTest::Test12()
{
	input->setPlainText("(list (make-line (make-point 0 0) (make-point 0 20)) (make-line (make-point 10 0) (make-point 10 20)) (make-line (make-point 20 0) (make-point 20 20)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();

	auto line = list.at(2);
	auto lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(0, 0)) && lineL->contains(QPoint(0, 20)), "Line isn't at the right points");
	QPen pen = QPen(QColor(0, 0, 0));
	pen.setWidth(1);
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");

	line = list.at(1);
	lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(10, 0)) && lineL->contains(QPoint(10, 20)), "Line isn't at the right points");
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");

	line = list.at(0);
	lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(20, 0)) && lineL->contains(QPoint(20, 20)), "Line isn't at the right points");
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");
}

void NotebookTest::Test13()
{
	input->setPlainText("(list (make-line (make-point 0 0) (make-point 20 0)) (make-line (make-point 0 10) (make-point 20 10)) (make-line (make-point 0 20) (make-point 20 20)))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();

	auto line = list.at(2);
	auto lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(0, 0)) && lineL->contains(QPoint(20, 0)), "Line isn't at the right points");
	QPen pen = QPen(QColor(0, 0, 0));
	pen.setWidth(1);
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");

	line = list.at(1);
	lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(0, 10)) && lineL->contains(QPoint(20, 10)), "Line isn't at the right points");
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");

	line = list.at(0);
	lineL = qgraphicsitem_cast<QGraphicsLineItem *>(line);
	QVERIFY2(lineL->isVisible(), "Line Not Visible");
	QVERIFY2(lineL->contains(QPoint(0, 20)) && lineL->contains(QPoint(20, 20)), "Line isn't at the right points");
	QVERIFY2((lineL->pen() == pen), "Incorrect pen width");
}

void NotebookTest::Test14()
{
	input->setPlainText("(make-text \"Hello World!\")");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	qDebug() << textL->boundingRect().center();
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hello World!", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 0), "Error Text in Wrong Place");

}

void NotebookTest::Test15()
{
	input->setPlainText("(begin (define xloc 0) (define yloc 0) (list (set-property \"position\" (make-point (+ xloc 20) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (+ xloc 40) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (+ xloc 60) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (+ xloc 80) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (+ xloc 100) yloc) (make-text \"Hi\"))))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(4);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(20, 0), "Error Text in Wrong Place");

	text = list.at(3);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(40, 0), "Error Text in Wrong Place");

	text = list.at(2);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(60, 0), "Error Text in Wrong Place");

	text = list.at(1);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(80, 0), "Error Text in Wrong Place");

	text = list.at(0);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(100, 0), "Error Text in Wrong Place");

}

void NotebookTest::Test16()
{
	input->setPlainText("(begin (define xloc 0) (define yloc 0) (list (set-property \"position\" (make-point xloc (+ yloc 20)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (+ yloc 40)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (+ yloc 60)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (+ yloc 80)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (+ yloc 100)) (make-text \"Hi\"))))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(4);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 20), "Error Text in Wrong Place");

	text = list.at(3);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 40), "Error Text in Wrong Place");

	text = list.at(2);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 60), "Error Text in Wrong Place");

	text = list.at(1);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 80), "Error Text in Wrong Place");

	text = list.at(0);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 100), "Error Text in Wrong Place");

}

void NotebookTest::Test17()
{
	input->setPlainText("(begin))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Error: Invalid Expression. Could not parse.", "Incorrect Text");

}

void NotebookTest::Test18()
{
	input->setPlainText("(begin (define a I) (first a))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(0);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Error: argument to first is not a list", "Incorrect Text");

}

void NotebookTest::Test19()
{
	input->setPlainText("(begin (define xloc 0) (define yloc 0) (list (set-property \"position\" (make-point xloc (- yloc 20)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (- yloc 40)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (- yloc 60)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (- yloc 80)) (make-text \"Hi\")) (set-property \"position\" (make-point xloc (- yloc 100)) (make-text \"Hi\"))))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(4);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, -20), "Error Text in Wrong Place");

	text = list.at(3);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, -40), "Error Text in Wrong Place");

	text = list.at(2);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, -60), "Error Text in Wrong Place");

	text = list.at(1);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, -80), "Error Text in Wrong Place");

	text = list.at(0);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, -100), "Error Text in Wrong Place");

}

void NotebookTest::Test20()
{
	input->setPlainText("(begin (define xloc 0) (define yloc 0) (list (set-property \"position\" (make-point (- xloc 20) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 40) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 60) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 80) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 100) yloc) (make-text \"Hi\"))))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto list = scene->items();
	auto text = list.at(4);
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-20, 0), "Error Text in Wrong Place");

	text = list.at(3);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-40, 0), "Error Text in Wrong Place");

	text = list.at(2);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-60, 0), "Error Text in Wrong Place");

	text = list.at(1);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-80, 0), "Error Text in Wrong Place");

	text = list.at(0);
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-100, 0), "Error Text in Wrong Place");

}

void NotebookTest::Test21()
{
	input->setPlainText("(make-text \"Hello World!\")");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto text = scene->itemAt(QPointF(0, 0), QTransform());
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	qDebug() << textL->boundingRect().center();
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hello World!", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 0), "Error Text in Wrong Place");

}

void NotebookTest::Test22()
{
	input->setPlainText("(begin (define xloc 0) (define yloc 0) (list (set-property \"position\" (make-point (- xloc 20) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 40) yloc) (make-text \"Pi\")) (set-property \"position\" (make-point (- xloc 60) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 80) yloc) (make-text \"Hi\")) (set-property \"position\" (make-point (- xloc 100) yloc) (make-text \"Hi\"))))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto text = scene->itemAt(QPointF(-20, 0), QTransform());
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-20, 0), "Error Text in Wrong Place");

	text = scene->itemAt(QPointF(-40, 0), QTransform());
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Pi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-40, 0), "Error Text in Wrong Place");

	text = scene->itemAt(QPointF(-60, 0), QTransform());
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-60, 0), "Error Text in Wrong Place");

	text = scene->itemAt(QPointF(-80, 0), QTransform());
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-80, 0), "Error Text in Wrong Place");

	text = scene->itemAt(QPointF(-100, 0), QTransform());
	textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hi", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(-100, 0), "Error Text in Wrong Place");

}

void NotebookTest::Test23()
{
	input->setPlainText("(set-property \"text-rotation\" (/ pi 2) (make-text \"Hello World!\"))");
	QTest::keyEvent(QTest::Press, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);
	QTest::keyEvent(QTest::Release, input, Qt::Key_Return, Qt::KeyboardModifier(Qt::ShiftModifier), 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view->isVisible(), "Can't find view.");
	auto scene = view->scene();
	auto text = scene->itemAt(QPointF(0, 0), QTransform());
	auto textL = qgraphicsitem_cast<QGraphicsTextItem *>(text);
	//qDebug() << textL->boundingRect().center();
	QVERIFY2(textL->isVisible(), "Text Not Visible");
	QVERIFY2(textL->toPlainText() == "Hello World!", "Incorrect Text");
	QVERIFY2(textL->sceneBoundingRect().center() == QPointF(0, 0), "Error Text in Wrong Place");

}
