/*
 * This file is part of Escain QTopMenu library
 *
 * QTopMenu library is free software: you can redistribute it and/or modify
 * it under ther terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Escain Documentor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 *
 * Author: Adrian Maire escain (at) gmail.com
 */

#include <iostream>
#include <QDebug>
#include <QApplication>

#include "QTopMenuButton.hpp"
#include "QTopMenuButtonWidget.hpp"

#include <ui_Test_QTopMenuButton.h>

int main (int argn, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app( argn, argv);

	QWidget window;
	Ui::Test_QTopMenuButton testGui;
	testGui.setupUi(&window);

	Escain::QTopMenuButton button;

	auto wid = std::static_pointer_cast<Escain::QTopMenuButtonWidget>( button.createWidget(testGui.Content));

	auto getCellSize = [&testGui]()-> qreal
	{
		if (testGui.cellSide10->isChecked())
		{
			return 10.0;
		}
		else if (testGui.cellSide20->isChecked())
		{
			return 20.0;
		}
		return 30.0;
	};

	auto getMargin = [&testGui]()-> qreal
	{
		if (testGui.margin0->isChecked())
		{
			return 0.0;
		}
		else if (testGui.margin2->isChecked())
		{
			return 2.0;
		}
		return 8.0;
	};

	auto getSizeHintCell = [&testGui]()-> QSizeF
	{
		if (testGui.sizeHint1x1->isChecked())
		{
			return {20.0, 20.0};
		}
		else if (testGui.sizeHint2x2->isChecked())
		{
			return {45.0, 45.0};
		}
		else if (testGui.sizeHint3x3->isChecked())
		{
			return {70.0, 70.0};
		}
		else if (testGui.sizeHint5x1->isChecked())
		{
			return {120, 20.0};
		}
		return {240.0, 20.0};
	};

	auto getLabel = [&testGui]()-> std::string
	{
		if (testGui.labelEmpty->isChecked())
		{
			return "";
		}
		else if (testGui.labelShort->isChecked())
		{
			return "Short";
		}
		else if (testGui.labelLong->isChecked())
		{
			return "Long label button";
		}
		return "Very really long label for button";
	};

	auto updateButton = [&getMargin, &getCellSize, &getSizeHintCell, &wid]()
	{
		const auto maxSize = QSizeF(300,300);
		const qreal cellSize = getCellSize();
		const qreal margin = getMargin();
		const QSizeF hint = getSizeHintCell();
		const auto newSize = wid->bestSize(wid->direction(), Escain::CellInfo{cellSize, margin, 3},
			hint, maxSize);
		wid->margin(margin);
		wid->resize(newSize->toSize());
	};

	auto updateLabel = [&getLabel, &wid]()
	{
		wid->label(getLabel());
	};

	QObject::connect(testGui.Horizontal, &QRadioButton::toggled, [&wid, &testGui]()
	{
		wid->direction(testGui.Horizontal->isChecked() ? Escain::DisplaySide::Top:Escain::DisplaySide::Left);
	});

	QObject::connect(testGui.enabledTrue, &QRadioButton::toggled, [&wid, &testGui]()
	{
		wid->setEnabled(testGui.enabledTrue->isChecked());
	});

	QObject::connect(&button, &Escain::QTopMenuButton::bestSizeChanged, updateButton);

	QObject::connect(testGui.sizeHint1x1, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.sizeHint2x2, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.sizeHint3x3, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.sizeHint5x1, &QRadioButton::toggled, updateButton);

	QObject::connect(testGui.cellSide10, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.cellSide20, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.cellSide30, &QRadioButton::toggled, updateButton);

	QObject::connect(testGui.margin0, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.margin2, &QRadioButton::toggled, updateButton);
	QObject::connect(testGui.margin8, &QRadioButton::toggled, updateButton);

	QObject::connect(testGui.labelEmpty, &QRadioButton::toggled, updateLabel);
	QObject::connect(testGui.labelShort, &QRadioButton::toggled, updateLabel);
	QObject::connect(testGui.labelLong, &QRadioButton::toggled, updateLabel);
	QObject::connect(testGui.labelVeryLong, &QRadioButton::toggled, updateLabel);

	QObject::connect(&button, &Escain::QTopMenuButton::triggered, []()
	{
		static size_t counter1=0;
		qDebug() << "Clicked button 1" << ++counter1;
	});

	updateButton();
	updateLabel();

	Escain::QSvgIcon icon;
	icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
	icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
	button.icon(icon);

	window.show();


	return app.exec();
}
