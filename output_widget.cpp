#include "output_widget.hpp"
#include <sstream>
#include <QLayout>
#include <QDebug>
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
	//text->setPos((0 - (text->boundingRect().width() / 2)), (0 - (text->boundingRect().height() / 2)));
	text->setPlainText(passed);
	gScene->addItem(text);
}

void OutputWidget::resizeEvent(QResizeEvent * ev)
{
	//qDebug() << "resize Event Caught!";
	gView->fitInView(gView->scene()->sceneRect(), Qt::KeepAspectRatio);
	//auto resize = ev;
	//QWidget::resizeEvent(ev);
}

void OutputWidget::recievedExp(Expression exp)
{
	gScene->clear();
	helperOut(exp);
	//gView->fitInView(gView->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void OutputWidget::helperOut(Expression exp)
{
	//gView->fitInView(gView->scene()->sceneRect(), Qt::KeepAspectRatio);
	if (exp.head().isNone())
	{
		if (exp.isLList())
		{
			Atom checker("\"object-name\"");
			checker.setString();
			if (exp.is_prop(checker))
			{
				Atom checkerP("\"point\"");
				checkerP.setString();
				Atom checkerL("\"line\"");
				checkerL.setString();
				Expression type = exp.get_prop(checker);
				if (type.head() == checkerP)
				{
					Atom checkerSize("\"size\"");
					checkerSize.setString();
					double size = 0;

					double xloc;
					double yloc;
					if (exp.is_prop(checkerSize))
					{
						Expression sizeA(exp.get_prop(checkerSize));
						if (sizeA.head().isNumber())
						{
							size = sizeA.head().asNumber();
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
						QGraphicsEllipseItem * dot = new QGraphicsEllipseItem((xloc -(size/2)), (yloc - (size / 2)), size, size);
						dot->setBrush(QBrush(QColor(0,0,0), Qt::BrushStyle(Qt::SolidPattern)));
						gScene->addItem(dot);
					}
					else
					{
						QString fin = "Error: size is a negative number";
						QGraphicsTextItem *text = new QGraphicsTextItem;
						text->setPos(0, 0);
						//text->setPos((0 - (text->boundingRect().width() / 2)), (0 - (text->boundingRect().height() / 2)));
						text->setPlainText(fin);
						gScene->addItem(text);
					}
					
				}
				else if (type == checkerL)
				{
					Atom checkerThick("\"thickness\"");
					checkerThick.setString();
					double thickness = 1;
					if (exp.is_prop(checkerThick))
					{
						Expression thickA(exp.get_prop(checkerThick));
						if (thickA.head().isNumber())
						{
							thickness = thickA.head().asNumber();
						}
					}
						double x1 = 0;
						double y1 = 0;
						double x2 = 0;
						double y2 = 0;
						if (exp.rTail()[0].is_prop(checker))
						{
							Expression type = exp.rTail()[0].get_prop(checker);
							if (type.head() == checkerP)
							{
								if (exp.rTail()[0].rTail()[0].isHeadNumber())
								{
									x1 = exp.rTail()[0].rTail()[0].head().asNumber();
								}
								else
								{
									//error
								}

								if (exp.rTail()[0].rTail()[1].isHeadNumber())
								{
									y1 = exp.rTail()[0].rTail()[1].head().asNumber();
								}
								else
								{
									//error
								}
							}
							else
							{
								//error argument 1 to line isn't a point
							}

					}
						if (exp.rTail()[1].is_prop(checker))
						{
							Expression type = exp.rTail()[1].get_prop(checker);
							if (type.head() == checkerP)
							{
								if (exp.rTail()[1].rTail()[0].isHeadNumber())
								{
									x2 = exp.rTail()[1].rTail()[0].head().asNumber();
								}
								else
								{
									//error
								}

								if (exp.rTail()[1].rTail()[1].isHeadNumber())
								{
									y2 = exp.rTail()[1].rTail()[1].head().asNumber();
								}
								else
								{
									//error
								}
							}
							else
							{
								//error argument 1 to line isn't a point
							}

						}

						QGraphicsLineItem * line = new QGraphicsLineItem(x1, y1, x2, y2);
						QPen * pen = new QPen(QColor(0, 0, 0));
						pen->setWidth(thickness);
						line->setPen(*pen);
						gScene->addItem(line);
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
			//text->setPos((0 - (text->boundingRect().width() / 2)), (0 - (text->boundingRect().height() / 2)));
			text->setPlainText(fin);
			gScene->addItem(text);

		}
	}
	else if (exp.head().isSymbol() || exp.head().isString() || exp.head().isNumber() || exp.head().isComplex())
	{
		Atom checker("\"object-name\"");
		checker.setString();
		if (exp.is_prop(checker))
		{
			Atom checkerT("\"text\"");
			checkerT.setString();
			Expression type = exp.get_prop(checker);
			if (type.head() == checkerT)
			{
				Atom checkerP("\"position\"");
				checkerP.setString();
				double x1 = 0;
				double y1 = 0;
					if (exp.is_prop(checkerP))
					{
						Expression type = exp.get_prop(checkerP);
						Atom checkerPo("\"point\"");
						checkerPo.setString();
						if (type.is_prop(checker))
						{
							Expression pt = type.get_prop(checker);
							if (pt.head() == checkerPo)
							{
								if (type.rTail()[0].isHeadNumber())
								{
									x1 = type.rTail()[0].head().asNumber();
								}
								else
								{
									//error
								}

								if (type.rTail()[0].isHeadNumber())
								{
									y1 = type.rTail()[1].head().asNumber();
								}
								else
								{
									//error
								}
							}
						
						}
					}
				Atom checkerS("\"text-scale\"");
				checkerS.setString();
				double scale = 1;
				if (exp.is_prop(checkerS))
				{
					Expression scalen = exp.get_prop(checkerS);
					if (scalen.isHeadNumber())
					{
						if (scalen.head().asNumber() > 0)
						{
							scale = scalen.head().asNumber();
						}
						else
						{
							//throw error
						}
					}
					else
					{
						//throw error
					}
				}

				Atom checkerR("\"text-rotation\"");
				checkerR.setString();
				double rad = 0;
				if (exp.is_prop(checkerR))
				{
					Expression rotation = exp.get_prop(checkerR);
					if (rotation.isHeadNumber())
					{
						rad = rotation.head().asNumber() * (180.0 / std::atan2(0, -1));
					}
				} 


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
				text->setPlainText(fin);
				auto font = QFont("Monospace");
				font.setStyleHint(QFont::TypeWriter);
				font.setPointSize(1);

				text->setFont(font);
				text->setScale(scale);
				
				text->setPos((x1 - (text->boundingRect().width() / 2)), (y1 - (text->boundingRect().height() / 2)));
				text->setTransformOriginPoint(QPointF(((text->boundingRect().width() / 2)), ((text->boundingRect().height() / 2))));
				text->setRotation(rad);
				
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
			//text->setPos((0 - (text->boundingRect().width() / 2)), (0 - (text->boundingRect().height() / 2)));
			text->setPlainText(fin);
			gScene->addItem(text);

		}
	}
}
