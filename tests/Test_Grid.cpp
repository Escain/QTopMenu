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
#include <QBoxLayout>

#include "QTopMenuButton.hpp"
#include "QTopMenuGrid.hpp"

#include <ui_Test_QTopMenuGrid.h>

int main (int argn, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app( argn, argv);

	QWidget window;
	Ui::Test_QTopMenuGrid testGui;
	testGui.setupUi(&window);
	QBoxLayout layout(QBoxLayout::TopToBottom, testGui.Content);

	Escain::QTopMenuGrid grid(testGui.Content);
	grid.setObjectName("Grid");
	layout.addWidget(&grid);

	Escain::QTopMenuButton button;
	button.nameId("Save");
	Escain::QSvgIcon icon;
	icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
	icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
	button.icon(icon);
	QObject::connect(&button, &Escain::QTopMenuButton::triggered, []()
	{
		static size_t counter1=0;
		qDebug() << "Clicked button 1" << ++counter1;
	});


	auto getCellSize = [&testGui]()-> qreal
	{
		if (testGui.cellSide15->isChecked())
		{
			return 15.0;
		}
		else if (testGui.cellSide25->isChecked())
		{
			return 25.0;
		}
		return 35.0;
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

	auto getTransversalCellNum = [&testGui]()-> size_t
	{
		if (testGui.transversalCellNum1->isChecked())
		{
			return 1;
		}
		else if (testGui.transversalCellNum2->isChecked())
		{
			return 2;
		}
		else if (testGui.transversalCellNum3->isChecked())
		{
			return 3;
		}
		return 4;
	};

	auto getButtonLabel = [&testGui]()-> std::string
	{
		if (testGui.buttonLabelEmpty->isChecked())
		{
			return "";
		}
		else if (testGui.buttonLabelShort->isChecked())
		{
			return "Short";
		}
		else if (testGui.buttonLabelLong->isChecked())
		{
			return "Long label button";
		}
		return "Very really long label for button";
	};

	auto updateButton = [&getMargin, &getCellSize, &getTransversalCellNum, &getButtonLabel, &grid, &button]()
	{
		const qreal cellSize = getCellSize();
		const qreal margin = getMargin();
		const size_t transversalCellNum = getTransversalCellNum();
		grid.cellSize(cellSize);
		grid.margin(margin);
		grid.transversalCellNum(transversalCellNum);
		button.label(getButtonLabel());
	};

	QObject::connect(testGui.Horizontal, &QRadioButton::toggled, &grid, [&layout, &grid, &testGui]()
	{
		bool hor = testGui.Horizontal->isChecked();
		grid.direction(hor ? Escain::DisplaySide::Top:Escain::DisplaySide::Left);
		layout.setDirection(hor ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
	});

	QObject::connect(testGui.enabledTrue, &QRadioButton::toggled, &grid, [&grid, &testGui]()
	{
		grid.setEnabled(testGui.enabledTrue->isChecked());
	});

	QObject::connect(testGui.transversalCellNum1, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.transversalCellNum2, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.transversalCellNum3, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.transversalCellNum4, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});

	QObject::connect(testGui.cellSide15, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.cellSide25, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.cellSide35, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});

	QObject::connect(testGui.margin0, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.margin2, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.margin8, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});

	QObject::connect(testGui.buttonLabelEmpty, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.buttonLabelShort, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.buttonLabelLong, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});
	QObject::connect(testGui.buttonLabelVeryLong, &QRadioButton::toggled, &grid, [&updateButton]() { updateButton();});


	grid.addGroup("Files");
	grid.addGroup("Shapes");
	grid.addGroup("Other");

	{
		Escain::QTopMenuGridGroup* g = grid.getGroup("Shapes");
		if (g)
		{
			g->label(g->id());
			Escain::QSvgIcon icon;
			icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
			icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
			g->icon(icon);

			g->addItem(button.createWidget(),0, QSizeF(75, 75));
			g->addItem(button.createWidget(),1, QSizeF(25, 25));
			g->addItem(button.createWidget(),1, QSizeF(55, 55));
			g->addItem(button.createWidget(),2, QSizeF(25, 25));
			g->addItem(button.createWidget(),2, QSizeF(25, 25));
			g->addItem(button.createWidget(),2, QSizeF(25, 25));
		}
	}
	{
		Escain::QTopMenuGridGroup* g = grid.getGroup("Other");
		if (g)
		{
			g->label(g->id());
			Escain::QSvgIcon icon;
			icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
			icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
			g->icon(icon);

			g->addItem(button.createWidget(),0, QSizeF(75, 75));
		}
	}
	{
		Escain::QTopMenuGridGroup* g = grid.getGroup("Files");

		if (g)
		{
			g->label(g->id());
			Escain::QSvgIcon icon;
			icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
			icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
			g->icon(icon);

			g->addItem(button.createWidget(),0, QSizeF(75, 75));
			g->addItem(button.createWidget(),1, QSizeF(25, 25));
			g->addItem(button.createWidget(),1, QSizeF(55, 55));
			g->addItem(button.createWidget(),2, QSizeF(25, 25));
			g->addItem(button.createWidget(),2, QSizeF(25, 25));
			g->addItem(button.createWidget(),2, QSizeF(25, 25));
			g->addItem(button.createWidget(),3, QSizeF(100, 25));
			g->addItem(button.createWidget(),3, QSizeF(100, 25));
			g->addItem(button.createWidget(),3, QSizeF(100, 25));
			g->addItem(button.createWidget(),4, QSizeF(75, 75));
		}
	}

	updateButton();

	window.show();

	return app.exec();
}
