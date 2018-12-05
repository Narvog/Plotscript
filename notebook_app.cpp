#include "notebook_app.hpp"
void NotebookApp::inputSet(QString inputLine)
{
	line = inputLine;

	std::istringstream expression(line.toStdString());
	if (!interp.parseStream(expression)) {
		emit wasSet("Error: Invalid Expression. Could not parse.");
	}
	else {
		bool error = false;
		Expression exp;
		try {
			exp = interp.evaluate();
			//emit exp
		}
		catch (const SemanticError & ex) {
			error = true;
			emit wasSet(ex.what());
		}
		if (!error)
		{
			emit expSet(exp);

		}
	}


}

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent)
{
	input->setObjectName("input");
	output->setObjectName("output");
	auto layout = new QGridLayout();
	layout->addWidget(input, 1, 0, 1, 3);
	QObject::connect(input, QOverload<QString>::of(&InputWidget::textInputCompleted), this, &NotebookApp::inputSet);
	layout->addWidget(output, 2, 0, 1, 3);
	QObject::connect(this, QOverload<QString>::of(&NotebookApp::wasSet), output, &OutputWidget::recievedData);
	QObject::connect(this, &NotebookApp::expSet, output, &OutputWidget::recievedExp);

	start->setObjectName("start");
	start->setText("Start Kernal");
	layout->addWidget(start, 0, 0, 1, 1);

	stop->setObjectName("stop");
	stop->setText("Stop Kernal");
	layout->addWidget(stop, 0, 1, 1, 1);

	reset->setObjectName("reset");
	reset->setText("Reset Kernal");
	layout->addWidget(reset, 0, 2, 1, 1);

	std::ifstream ifs(STARTUP_FILE);
	interp.parseStream(ifs);
	Expression exp = interp.evaluate();

	this->setLayout(layout);
}

void NotebookApp::resetAPP()
{
	interp = Interpreter();
	std::ifstream ifs(STARTUP_FILE);
	interp.parseStream(ifs);
	Expression exp = interp.evaluate();
}