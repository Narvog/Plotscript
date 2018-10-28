#include "output_widget.hpp"
#include <sstream>
#include <QLayout>
OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent)
{
	auto layout2 = new QHBoxLayout();
	layout2->addWidget(gView);
	this->setLayout(layout2);
}

void OutputWidget::recievedData(QString info)
{
	passed = info;
	gScene->clear();
	QGraphicsTextItem *text = gScene->addText(passed);
	text->setPos(0, 0);
}

void OutputWidget::recievedExp(Expression exp)
{
	gScene->clear();
	helperOut(exp);
}

void OutputWidget::helperOut(Expression exp)
{
	if (exp.head().isNone())
	{
		if (exp.isLList())
		{
			Atom checker("\"object-name\"");
			checker.setString();
			if (exp.head().is_prop(checker))
			{
				Atom checkerP("\"point\"");
				checkerP.setString();
				Atom checkerL("\"line\"");
				checkerL.setString();
				Atom type = exp.head().get_prop(checker);
				if (type == checkerP)
				{

				}
				else if (type == checkerL)
				{

				}
				else
				{

				}
			}
			else
			{
				for (std::size_t i = 0; i < exp.rTail().size(); i++)
				{
					helperOut(exp.rTail()[i]);
				}
			}

		}
		else if (exp.isLLambda())
		{

		}
		else
		{
			gScene->clear();
			std::stringstream out;
			out << exp;
			std::string output;
			output = out.str();
			QString fin = QString::fromStdString(output);
			QGraphicsTextItem *text = gScene->addText(fin);
			text->setPos(0, 0);
		}
	}
	else if (exp.head().isSymbol() || exp.head().isString() || exp.head().isNumber() || exp.head().isComplex())
	{
		Atom checker("\"object-name\"");
		checker.setString();
		if (exp.head().is_prop(checker))
		{
			Atom checkerT("\"text\"");
			checkerT.setString();
			Atom type = exp.head().get_prop(checker);
			if (type == checkerT)
			{
				std::stringstream out;
				out << exp;
				std::string output;
				output = out.str();
				if (output.size() > 4)
				{
					output = output.substr(2, (output.size() - 4));
				}
				else
				{
					output = "";
				}
				QString fin = QString::fromStdString(output);
				QGraphicsTextItem *text = gScene->addText(fin);
				text->setPos(0, 0);
			}
		}
		else
		{
			gScene->clear();
			std::stringstream out;
			out << exp;
			std::string output;
			output = out.str();
			QString fin = QString::fromStdString(output);
			//I think there is an error here 
			QGraphicsTextItem *text = gScene->addText(fin);
			text->setPos(0, 0);
		}
	}
}
