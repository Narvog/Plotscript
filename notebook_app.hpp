#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include "input_widget.hpp"
#include "output_widget.hpp"
#include "interpreter.hpp"
#include "startup_config.hpp"
#include "semantic_error.hpp"
#include "consumer.hpp"


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <QLayout>
#include <QPushButton>

class NotebookApp : public QWidget
{
	Q_OBJECT

public:

	NotebookApp(QWidget * parent = nullptr);
	~NotebookApp();
	void resetAPP();
signals:
	void setOut();
private:
	
	InputWidget * input = new InputWidget();
	OutputWidget * output = new OutputWidget();
	QPushButton * start = new QPushButton();
	QPushButton * stop = new QPushButton();
	QPushButton * reset = new QPushButton();

	QString line;

	ThreadSafeQueue<std::string> inputQ;
	ThreadSafeQueue<Expression> outputQ;

	Consumer cons;
	std::thread t1;

	enum State { RUNNING, STOPPED };
	State currentS = RUNNING;
signals:
	void wasSet(QString inputLine);
	void expSet(Expression exp);

private slots:
	void inputSet(QString inputLine);
	void stopRepl();
	void startRepl();
	void resetRepl();
};


#endif