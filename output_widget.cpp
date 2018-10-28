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
	QGraphicsTextItem *text = new QGraphicsTextItem;
	text->setPos(0, 0);
	text->setPlainText(passed);
	gScene->addItem(text);
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
					Atom checkerSize("\"size\"");
					checkerSize.setString();
					double size = 0;

					double xloc;
					double yloc;
					if (exp.head().is_prop(checkerSize))
					{
						Atom sizeA(exp.head().get_prop(checkerSize));
						if (sizeA.isNumber())
						{
							size = sizeA.asNumber();
						}
					}
					if (exp.rTail()[0].head().isNumber())
					{
						xloc = exp.rTail()[0].head().asNumber();
					}

					if (exp.rTail()[1].head().isNumber())
					{
						yloc = exp.rTail()[1].head().asNumber();
					}
					if (size >= 0)
					{
						qreal qXloc(xloc);
						qreal qYloc(yloc);
						qreal qSize(size);
						QGraphicsEllipseItem * dot = new QGraphicsEllipseItem(xloc, yloc, size, size);
						if (xloc > 0)
						{
							if (yloc > 0)
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate((-xloc - size / 2), -yloc));
							}
							else if (yloc < 0)
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate(-xloc - size / 2, (-yloc - size / 2)));
							}
							else
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate(-xloc, (-yloc - size / 2)));
							}
						}
						else if(xloc < 0)
						{
							if (yloc > 0)
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate((-xloc - size / 2), -yloc));
							}
							else if (yloc < 0)
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate(-xloc - size / 2, (-yloc - size / 2)));
							}
							else
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate(-xloc - size / 2, (-yloc - size / 2)));
							}
						}
						else
						{
							if (yloc > 0)
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate((-xloc - size / 2), -yloc));
							}
							else if (yloc < 0)
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate(xloc - size/2, (-yloc - size / 2)));
							}
							else
							{
								dot->setTransform(QTransform().translate(xloc, yloc).rotate(0).translate(-xloc, (-yloc - size / 2)));
							}
						}

						
						dot->setBrush(QBrush(QColor(0,0,0), Qt::BrushStyle(Qt::SolidPattern)));
						gScene->addItem(dot);
					}
					else
					{
						QString fin = "Error: size is a negative number";
						QGraphicsTextItem *text = new QGraphicsTextItem;
						text->setPos(0, 0);
						text->setPlainText(fin);
						gScene->addItem(text);
					}
					
				}
				else if (type == checkerL)
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
			QGraphicsTextItem *text = new QGraphicsTextItem;
			text->setPos(0, 0);
			text->setPlainText(fin);
			gScene->addItem(text);

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
				QGraphicsTextItem *text = new QGraphicsTextItem;
				text->setPos(0, 0);
				text->setPlainText(fin);
				gScene->addItem(text);
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
			QGraphicsTextItem *text = new QGraphicsTextItem;
			text->setPos(0, 0);
			text->setPlainText(fin);
			gScene->addItem(text);

		}
	}
}
