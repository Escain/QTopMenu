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

#include <ui_Test_QTopMenuGridGroup.h>


#include "QTopMenuButton.hpp"
#include "QTopMenuGridGroup.hpp"
#include "QTopMenuWidget.hpp"

int main (int argn, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app( argn, argv);


	QWidget window;
	Ui::Test_QTopMenuGridGroup testGui;
	testGui.setupUi(&window);

	Escain::QTopMenuGridGroup group(testGui.Content);
	group.resize(64,64);
	group.label("Short");
	group.margin(2);
	group.direction(testGui.Horizontal->isChecked() ? Escain::DisplaySide::Top:Escain::DisplaySide::Left);
	group.collapsed(testGui.Collapsed->isChecked());
	group.setObjectName("QTopMenuGridGroup");

	QObject::connect(testGui.Horizontal, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		group.direction(testGui.Horizontal->isChecked() ? Escain::DisplaySide::Top:Escain::DisplaySide::Left);
	});

	QObject::connect(testGui.enabledTrue, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		group.setEnabled(testGui.enabledTrue->isChecked());
	});

	QObject::connect(testGui.showDivisionTrue, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		group.divisionBar(testGui.showDivisionTrue->isChecked());
	});

	QObject::connect(testGui.Collapsed, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		group.collapsed(testGui.Collapsed->isChecked());
	});

	QObject::connect(testGui.transversalCellNum1, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.transversalCellNum1->isChecked())
		{
			group.transversalCellNum(1);
		}
	});
	QObject::connect(testGui.transversalCellNum2, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.transversalCellNum2->isChecked())
		{
			group.transversalCellNum(2);
		}
	});
	QObject::connect(testGui.transversalCellNum3, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.transversalCellNum3->isChecked())
		{
			group.transversalCellNum(3);
		}
	});
	QObject::connect(testGui.transversalCellNum4, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.transversalCellNum4->isChecked())
		{
			group.transversalCellNum(4);
		}
	});

	QObject::connect(testGui.cellSide15, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.cellSide15->isChecked())
		{
			group.cellSize(15);
		}
	});
	QObject::connect(testGui.cellSide25, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.cellSide25->isChecked())
		{
			group.cellSize(25);
		}
	});
	QObject::connect(testGui.cellSide35, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.cellSide35->isChecked())
		{
			group.cellSize(35);
		}
	});

	QObject::connect(testGui.margin0, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.margin0->isChecked())
		{
			group.margin(0);
		}
	});
	QObject::connect(testGui.margin2, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.margin2->isChecked())
		{
			group.margin(2);
		}
	});
	QObject::connect(testGui.margin8, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.margin8->isChecked())
		{
			group.margin(8);
		}
	});

	QObject::connect(testGui.labelEmpty, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.labelEmpty->isChecked())
		{
			group.label("");
		}
	});
	QObject::connect(testGui.labelShort, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.labelShort->isChecked())
		{
			group.label("Short");
		}
	});
	QObject::connect(testGui.labelLong, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.labelLong->isChecked())
		{
			group.label("Long label button");
		}
	});
	QObject::connect(testGui.labelVeryLong, &QRadioButton::toggled, &group, [&group, &testGui]()
	{
		if (testGui.labelVeryLong->isChecked())
		{
			group.label("Very really long label for button");
		}
	});

	Escain::QSvgIcon icon;
	icon.iconBackground(Escain::QSvgPixmapCache(QString("iconBg.svg")));
	icon.iconForeground(Escain::QSvgPixmapCache(QString("iconFg.svg")));
	group.icon(icon);
	//group.direction(Escain::Direction::Vertical);

	Escain::QTopMenuButton button1;
	button1.margin(2);
	button1.label("Save");
	button1.icon(icon);
	size_t counter1=0;
	QObject::connect(&button1, &Escain::QTopMenuButton::triggered, [&counter1]()
	{
		qDebug() << "Clicked button c=" << ++counter1;
	});


	auto buttonLabel = [&testGui, &button1]()
	{
		std::string label;
		if (testGui.buttonLabelShort->isChecked())
		{
			label = "Short";
		}
		else if (testGui.buttonLabelLong->isChecked())
		{
			label="Very really long label for button";
		}
		button1.label(label);
	};
	QObject::connect(testGui.buttonLabelEmpty, &QRadioButton::toggled, &group, buttonLabel);
	QObject::connect(testGui.buttonLabelShort, &QRadioButton::toggled, &group, buttonLabel);
	QObject::connect(testGui.buttonLabelLong, &QRadioButton::toggled, &group, buttonLabel);

	group.addItem(button1.createWidget(),0, QSizeF(80, 80));
	group.addItem(button1.createWidget(),1, QSizeF(25, 25));
	/*group.addItem(button1.createWidget(),2, QSizeF(25, 25));
	group.addItem(button1.createWidget(),2, QSizeF(25, 25));
	group.addItem(button1.createWidget(),2, QSizeF(25, 25));
	group.addItem(button1.createWidget(),3, QSizeF(140, 25));
	group.addItem(button1.createWidget(),3, QSizeF(140, 25));
	group.addItem(button1.createWidget(),3, QSizeF(140, 25));
	group.addItem(button1.createWidget(),4, QSizeF(75, 75));*/
	group.addItem(button1.createWidget(),1, QSizeF(55, 55));


	/*group.itemAt(0,0)->widget().lock().get()->setObjectName("But0_0");
	group.itemAt(1,0)->widget().lock().get()->setObjectName("But1_0");
	group.itemAt(1,1)->widget().lock().get()->setObjectName("But1_1");
	group.itemAt(2,0)->widget().lock().get()->setObjectName("But2_0");
	group.itemAt(2,1)->widget().lock().get()->setObjectName("But2_1");
	group.itemAt(2,2)->widget().lock().get()->setObjectName("But2_2");
	group.itemAt(3,0)->widget().lock().get()->setObjectName("But3_0");
	group.itemAt(3,1)->widget().lock().get()->setObjectName("But3_1");
	group.itemAt(3,2)->widget().lock().get()->setObjectName("But3_2");
	group.itemAt(4,0)->widget().lock().get()->setObjectName("But4_0");*/


	window.show();


	return app.exec();
}
