#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include "input_widget.hpp"
#include "output_widget.hpp"
#include "interpreter.hpp"
#include "startup_config.hpp"
#include "semantic_error.hpp"


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <QLayout>

class NotebookApp : public QWidget
{
	Q_OBJECT

public:

	NotebookApp(QWidget * parent = nullptr);
	void reset();
signals:
	void setOut();
private:
	
	InputWidget * input = new InputWidget();
	OutputWidget * output = new OutputWidget();
	Interpreter interp;
	QString line;
signals:
	void wasSet(QString inputLine);
	void expSet(Expression exp);
private slots:
	void inputSet(QString inputLine);
};


#endif