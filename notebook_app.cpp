#include "notebook_app.hpp"
void NotebookApp::inputSet(QString inputLine)
{
	line = inputLine;
	Expression exp;
	std::string trans;
	//interupt = false;
	switch (currentS)
	{
	case NotebookApp::RUNNING:
		inputQ.push(line.toStdString());
		

		outputQ.wait_and_pop(exp);

		if (exp.isError())
		{
			if (exp.head().asSymbol() == "Invalid Expression. Could not parse.")
			{
				emit wasSet("Error: Invalid Expression. Could not parse.");
			}
			else
			{
				if (exp.head().asSymbol() == "Error: interpreter kernel interrupted")
				{
					resetAPP();
				}
				emit wasSet(QString::fromStdString(exp.head().asSymbol()));
			}
			
		}
		else
		{
			emit expSet(exp);
		}
		break;
	case NotebookApp::STOPPED:
		emit wasSet("Error: interpreter kernal not running");
		break;
	default:
		break;
	}

}

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent)
{
	input->setObjectName("input");
	output->setObjectName("output");
	auto layout = new QGridLayout();
	layout->addWidget(input, 1, 0, 1, 4);
	QObject::connect(input, QOverload<QString>::of(&InputWidget::textInputCompleted), this, &NotebookApp::inputSet);
	layout->addWidget(output, 2, 0, 1, 4);
	QObject::connect(this, QOverload<QString>::of(&NotebookApp::wasSet), output, &OutputWidget::recievedData);
	QObject::connect(this, &NotebookApp::expSet, output, &OutputWidget::recievedExp);

	start->setObjectName("start");
	start->setText("Start Kernal");
	layout->addWidget(start, 0, 0, 1, 1);
	QObject::connect(start, &QPushButton::released, this, &NotebookApp::startRepl);

	stop->setObjectName("stop");
	stop->setText("Stop Kernal");
	layout->addWidget(stop, 0, 1, 1, 1);
	QObject::connect(stop, &QPushButton::released, this, &NotebookApp::stopRepl);

	reset->setObjectName("reset");
	reset->setText("Reset Kernal");
	layout->addWidget(reset, 0, 2, 1, 1);
	QObject::connect(reset, &QPushButton::released, this, &NotebookApp::resetRepl);

	interrupt->setObjectName("interrupt");
	interrupt->setText("Interrupt");
	layout->addWidget(interrupt, 0, 3, 1, 1);
	QObject::connect(interrupt, &QPushButton::released, this, &NotebookApp::interuptRepl);


	cons = Consumer(&inputQ, &outputQ);
	std::thread t2(&Consumer::run, cons);
	t1 = std::move(t2);

	this->setLayout(layout);
}

NotebookApp::~NotebookApp()
{
	std::string stop = "%stop";
	inputQ.push(stop);
	Expression exp;
	outputQ.wait_and_pop(exp);
	t1.join();
}

void NotebookApp::resetAPP()
{
	currentS = RUNNING;
	std::string res = "%reset";
	inputQ.push(res);
	Expression exp;
	outputQ.wait_and_pop(exp);
	t1.join();
	Consumer cons(&inputQ, &outputQ);
	std::thread t2(&Consumer::run, cons);
	t1 = std::move(t2);
}


void NotebookApp::stopRepl()
{
	if (currentS == RUNNING)
	{
		currentS = STOPPED;
		std::string stop = "%stop";
		inputQ.push(stop);
		Expression exp;
		outputQ.wait_and_pop(exp);
		t1.join();
	}
	else
	{

	}
}

void NotebookApp::startRepl()
{
	if (currentS == STOPPED)
	{
		currentS = RUNNING;
		Consumer cons(&inputQ, &outputQ);
		std::thread t2(&Consumer::run, cons);
		t1 = std::move(t2);
	}
	else
	{

	}
}


void NotebookApp::resetRepl()
{
	if (currentS == RUNNING)
	{
		currentS = RUNNING;
	std::string res = "%reset";
	inputQ.push(res);
	Expression exp;
	outputQ.wait_and_pop(exp);
	t1.join();
	Consumer cons(&inputQ, &outputQ);
	std::thread t2(&Consumer::run, cons);
	t1 = std::move(t2);
	}
	else
	{

	}
}


void NotebookApp::interuptRepl()
{
	if (currentS == RUNNING)
	{
		interupt = true;
	}
}

