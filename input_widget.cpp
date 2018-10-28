#include "input_widget.hpp"
InputWidget::InputWidget(QPlainTextEdit * parent) 
{

}

void InputWidget::keyPressEvent(QKeyEvent * ev)
{
	auto keyp = ev->key();
	if ((keyp == Qt::Key_Return) && (QApplication::keyboardModifiers() == Qt::ShiftModifier))
	{
		lineInput = toPlainText();
		emit textInputCompleted(lineInput);
	}
	else
	{
		QPlainTextEdit::keyPressEvent(ev);
	}
}
