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

#include "QTopMenu.hpp"
#include "QTopMenuButton.hpp"

#include <ui_Test_QTopMenu.h>

int main (int argn, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app( argn, argv);

	QWidget window;
	Ui::Test_QTopMenu testGui;
	testGui.setupUi(&window);


	Escain::QTopMenuButton button1;
	button1.nameId("Save");
	button1.label("Save");
	Escain::QSvgIcon icon;
	icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
	icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
	button1.icon(icon);
	QObject::connect(&button1, &Escain::QTopMenuButton::triggered, []()
	{
		static size_t counter1=0;
		qDebug() << "Clicked button 1" << ++counter1;
	});

	Escain::QTopMenu& menu = *testGui.Content;

	menu.direction(Escain::DisplaySide::Top);

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

	auto getCellSize = [&testGui]()-> size_t
	{
		if (testGui.cellSide15->isChecked())
		{
			return 15;
		}
		else if (testGui.cellSide25->isChecked())
		{
			return 25;
		}
		return 35;
	};

	auto getTabMargin = [&testGui]()-> size_t
	{
		if (testGui.tabMargin0->isChecked())
		{
			return 0;
		}
		else if (testGui.tabMargin10->isChecked())
		{
			return 10;
		}
		return 20;
	};

	auto tabMinPercentil = [&testGui]() -> qreal
	{
		if (testGui.tabMinPercentil0->isChecked())
		{
			return 0;
		}
		else if (testGui.tabMinPercentil60->isChecked())
		{
			return 0.6;
		}
		return 1.0;
	};

	auto buttonLabel = [&testGui, &button1]()
	{
		std::string butLabel;
		if (testGui.buttonLabelEmpty->isChecked())
		{
			butLabel = "";
		}
		else if (testGui.buttonLabelShort->isChecked())
		{
			butLabel = "Short";
		}
		else
		{
			butLabel = "Very long text label here";
		}
		button1.label(butLabel);
	};

	auto tabGroupLabel = [&testGui, &menu]()
	{
		std::string butLabel;
		if (testGui.tabGroupLabelEmpty->isChecked())
		{
			butLabel = "";
		}
		else if (testGui.tabGroupLabelShort->isChecked())
		{
			butLabel = "Short";
		}
		else
		{
			butLabel = "Very long text label here";
		}

		const auto& tabIds = menu.tabIds();
		for (const auto& menuId: tabIds)
		{
			const auto& groupIds = menu.groupIds(menuId);
			for (const auto& groupId: groupIds)
			{
				menu.grouplabel(menuId, groupId, butLabel);
			}
		}
	};

	auto tabLabel = [&testGui, &menu]()
	{
		std::string butLabel;
		if (testGui.tabLabelEmpty->isChecked())
		{
			butLabel = "";
		}
		else if (testGui.tabLabelShort->isChecked())
		{
			butLabel = "File";
		}
		else
		{
			butLabel = "Very long text label here";
		}

		menu.tabLabel("File", butLabel);
	};

	auto updateValues = [&menu, &getCellSize, &getTransversalCellNum, &getTabMargin, &tabMinPercentil]()
	{
		const auto transversalNum = getTransversalCellNum();
		const auto cellSize = getCellSize();
		const auto tabMargin = getTabMargin();
		const auto tabMinPerc = tabMinPercentil();
		menu.transversalCellNum(transversalNum);
		menu.cellSize(cellSize);
		menu.tabMargin(tabMargin);
		menu.tabMinSizePercentil(tabMinPerc);
	};

	QObject::connect(testGui.transversalCellNum1, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.transversalCellNum2, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.transversalCellNum3, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.transversalCellNum4, &QRadioButton::toggled, &menu, updateValues);

	QObject::connect(testGui.cellSide15, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.cellSide25, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.cellSide35, &QRadioButton::toggled, &menu, updateValues);

	QObject::connect(testGui.tabMargin0, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.tabMargin10, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.tabMargin20, &QRadioButton::toggled, &menu, updateValues);

	QObject::connect(testGui.tabMinPercentil0, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.tabMinPercentil60, &QRadioButton::toggled, &menu, updateValues);
	QObject::connect(testGui.tabMinPercentil100, &QRadioButton::toggled, &menu, updateValues);

	QObject::connect(testGui.buttonLabelEmpty, &QRadioButton::toggled, &menu, buttonLabel);
	QObject::connect(testGui.buttonLabelShort, &QRadioButton::toggled, &menu, buttonLabel);
	QObject::connect(testGui.buttonLabelLong, &QRadioButton::toggled, &menu, buttonLabel);

	QObject::connect(testGui.tabGroupLabelEmpty, &QRadioButton::toggled, &menu, tabGroupLabel);
	QObject::connect(testGui.tabGroupLabelShort, &QRadioButton::toggled, &menu, tabGroupLabel);
	QObject::connect(testGui.tabGroupLabelLong, &QRadioButton::toggled, &menu, tabGroupLabel);

	QObject::connect(testGui.tabLabelEmpty, &QRadioButton::toggled, &menu, tabLabel);
	QObject::connect(testGui.tabLabelShort, &QRadioButton::toggled, &menu, tabLabel);
	QObject::connect(testGui.tabLabelLong, &QRadioButton::toggled, &menu, tabLabel);

	QObject::connect(testGui.Horizontal, &QRadioButton::toggled, &menu, [&menu, &testGui]()
	{
		menu.direction(testGui.Horizontal->isChecked() ? Escain::DisplaySide::Top:Escain::DisplaySide::Left);
	});

	QObject::connect(testGui.enabledTrue, &QRadioButton::toggled, &menu, [&menu, &testGui]()
	{
		menu.setEnabled(testGui.enabledTrue->isChecked());
	});

	QObject::connect(testGui.generalGroupShow, &QRadioButton::toggled, &menu, [&menu, &testGui]()
	{
		menu.showGenericGroup(testGui.generalGroupShow->isChecked());
	});



	menu.insertTab("File", "&File");
	menu.insertTab("Shape", "Long &Shape");
	menu.insertTab("Solid", "Solid");
	menu.insertTab("CAM", "CAM");
	menu.addGroup("File", "File");
	menu.addGroup("File", "Export");
	menu.addGroup("File", "Import");
	menu.addGroup("Shape", "Shape");

	menu.tabShortcut("Shape", QKeySequence(Qt::ALT + Qt::Key_S));
	menu.tabShortcut("File", QKeySequence(Qt::ALT + Qt::Key_F));
	menu.groupIcon("File", "File", icon);
	menu.groupIcon("File", "Export", icon);
	menu.groupIcon("File", "Import", icon);
	menu.groupIcon("Shape", "Shape", icon);


	menu.addItem("File", "File", button1.createWidget(),0, QSizeF(75, 75));
	menu.addItem("File", "File", button1.createWidget(),1, QSizeF(25, 25));
	menu.addItem("File", "File", button1.createWidget(),1, QSizeF(55, 55));
	menu.addItem("File", "File", button1.createWidget(),2, QSizeF(25, 25));
	menu.addItem("File", "File", button1.createWidget(),2, QSizeF(25, 25));
	menu.addItem("File", "File", button1.createWidget(),2, QSizeF(25, 25));
	menu.addItem("File", "File", button1.createWidget(),3, QSizeF(100, 25));
	menu.addItem("File", "File", button1.createWidget(),3, QSizeF(100, 25));
	menu.addItem("File", "File", button1.createWidget(),3, QSizeF(100, 25));
	menu.addItem("File", "Export", button1.createWidget(),0, QSizeF(75, 75));
	menu.addItem("File", "Export", button1.createWidget(),1, QSizeF(50, 50));
	menu.addItem("File", "Export", button1.createWidget(),2, QSizeF(50, 50));
	menu.addItem("File", "Export", button1.createWidget(),3, QSizeF(75, 75));
	menu.addItem("File", "Import", button1.createWidget(),0, QSizeF(75, 75));

	menu.addItem("Shape", "Shape", button1.createWidget(),0, QSizeF(75, 75));
	menu.addItem("Shape", "Shape", button1.createWidget(),1, QSizeF(55, 55));
	menu.addItem("Shape", "Shape", button1.createWidget(),2, QSizeF(75, 75));

	menu.addGenericItem(button1.createWidget(),0, QSizeF(120, 25));
	menu.addGenericItem(button1.createWidget(),1, QSizeF(75, 75));

	QObject::connect(testGui.generalGroupAdditionalItemAdd, &QRadioButton::toggled, &menu, [&button1, &menu, &testGui]()
	{
		if (testGui.generalGroupAdditionalItemAdd->isChecked())
		{
			menu.addGenericItem(button1.createWidget(), 0, true, 0, QSizeF(100, 25));
			menu.addGenericItem(button1.createWidget(), 0, false, 0, QSizeF(100, 25));
		}
		else
		{
			menu.removeGenericItem(0, 1);
			menu.removeGenericItem(0, 0);
		}
	});

	QObject::connect(testGui.additionalTabItemAdd, &QRadioButton::toggled, &menu, [&button1, &menu, &testGui]()
	{
		if (testGui.additionalTabItemAdd->isChecked())
		{
			menu.addItem("File", "Import", button1.createWidget(), 0, true, 0, QSizeF(100, 25));
			menu.addItem("File", "Import", button1.createWidget(), 0, false, 0, QSizeF(100, 25));
		}
		else
		{
			menu.removeItem("File", "Import", 0, 1);
			menu.removeItem("File", "Import", 0, 0);
		}
	});

	QObject::connect(testGui.additionalTabAdd, &QRadioButton::toggled, &menu, [&menu, &testGui]()
	{
		if (testGui.additionalTabAdd->isChecked())
		{
			menu.insertTab("Added", "A", 1);
		}
		else
		{
			menu.removeTab("Added");
		}
	});

	QObject::connect(testGui.additionalTabGroupAdd, &QRadioButton::toggled, &menu, [&icon, &button1, &menu, &testGui]()
	{
		if (testGui.additionalTabGroupAdd->isChecked())
		{
			menu.addGroup( "File", "OtherGroup", 1);
			menu.groupIcon("File", "OtherGroup", icon);
			menu.addItem("File", "OtherGroup", button1.createWidget(),0, QSizeF(75, 75));
		}
		else
		{
			menu.removeGroup("File", "OtherGroup");
		}
	});

	updateValues();
	
	window.show();
	return app.exec();
}
