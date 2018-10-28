#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QApplication>

class InputWidget : public QPlainTextEdit
{
	Q_OBJECT

public:
	InputWidget(QPlainTextEdit * parent = nullptr);
	void keyPressEvent(QKeyEvent *ev);
signals:
	void textInputCompleted(QString Line);
private:
	QString lineInput;
};
#endif