#include "notebook_app.hpp"
void NotebookApp::inputSet(QString inputLine)
{
	line = inputLine;
	Interpreter interp;
	std::ifstream ifs(STARTUP_FILE);
	interp.parseStream(ifs);
	Expression exp = interp.evaluate();

	std::istringstream expression(line.toStdString());
	if (!interp.parseStream(expression)) {
		emit wasSet("Invalid Program. Could not parse.");
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
	auto layout = new QGridLayout();
	layout->addWidget(input, 0, 0);
	QObject::connect(input, QOverload<QString>::of(&InputWidget::textInputCompleted), this, &NotebookApp::inputSet);
	layout->addWidget(output, 1, 0);
	QObject::connect(this, QOverload<QString>::of(&NotebookApp::wasSet), output, &OutputWidget::recievedData);
	QObject::connect(this, &NotebookApp::expSet, output, &OutputWidget::recievedExp);

	this->setLayout(layout);
}