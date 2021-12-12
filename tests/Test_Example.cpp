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

#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>

#include "QTopMenu.hpp"
#include "QTopMenuButton.hpp"


int main (int argn, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app( argn, argv);

	QWidget window;
	QVBoxLayout layout(&window);
	Escain::QTopMenu menu(&window);
	layout.addWidget(&menu);
	QWidget white(&window);
	layout.addWidget(&white);
	layout.setMargin(0);
	layout.setSpacing(0);
	layout.setStretch(0,0);
	layout.setStretch(1,1);
	white.setStyleSheet("background-color: white;");

	menu.direction(Escain::DisplaySide::Top);

	menu.insertTab("File", "&File");
	menu.insertTab("Shape", "&Shape");
	menu.insertTab("Solid", "S&olid");
	menu.insertTab("CAM", "&CAM");
	menu.addGroup("File", "File");
	menu.addGroup("File", "Export");
	menu.addGroup("Shape", "Shape");

	menu.tabShortcut("Shape", QKeySequence(Qt::ALT + Qt::Key_S));
	menu.tabShortcut("File", QKeySequence(Qt::ALT + Qt::Key_F));


	Escain::QTopMenuButton buttonSave;
	buttonSave.label("Save");
	Escain::QSvgIcon iconSave;
	iconSave.iconBackground(Escain::QSvgPixmapCache(QString("save_bg.svg")));
	iconSave.iconForeground(Escain::QSvgPixmapCache(QString("save_fg.svg")));
	buttonSave.icon(iconSave);
	QObject::connect(&buttonSave, &Escain::QTopMenuButton::triggered, []()
	{
		static size_t counter1=0;
		qDebug() << "Save clicked:" << ++counter1;
	});

	Escain::QTopMenuButton buttonOpen;
	buttonOpen.label("Open");
	Escain::QSvgIcon iconOpen;
	iconOpen.iconBackground(Escain::QSvgPixmapCache(QString("open_bg.svg")));
	iconOpen.iconForeground(Escain::QSvgPixmapCache(QString("open_fg.svg")));
	buttonOpen.icon(iconOpen);
	QObject::connect(&buttonOpen, &Escain::QTopMenuButton::triggered, []()
	{
		static size_t counter1=0;
		qDebug() << "Open clicked" << ++counter1;
	});

	menu.groupIcon("File", "File", iconSave);
	menu.groupIcon("File", "Export", iconSave);
	menu.groupIcon("Shape", "Shape", iconSave);

	menu.addItem("File", "File", buttonSave.createWidget(),0, QSizeF(75, 75));
	menu.addItem("File", "File", buttonOpen.createWidget(),1, QSizeF(25, 25));
	menu.addItem("File", "File", buttonSave.createWidget(),1, QSizeF(55, 55));
	menu.addItem("File", "File", buttonSave.createWidget(),2, QSizeF(25, 25));
	menu.addItem("File", "File", buttonSave.createWidget(),2, QSizeF(25, 25));
	menu.addItem("File", "File", buttonSave.createWidget(),2, QSizeF(25, 25));
	menu.addItem("File", "File", buttonSave.createWidget(),3, QSizeF(100, 25));
	menu.addItem("File", "File", buttonSave.createWidget(),3, QSizeF(100, 25));
	menu.addItem("File", "File", buttonSave.createWidget(),3, QSizeF(100, 25));
	menu.addItem("File", "Export", buttonOpen.createWidget(),0, QSizeF(75, 75));
	menu.addItem("File", "Export", buttonSave.createWidget(),1, QSizeF(55, 55));
	menu.addItem("File", "Export", buttonSave.createWidget(),2, QSizeF(55, 55));
	menu.addItem("File", "Export", buttonSave.createWidget(),3, QSizeF(55, 55));

	menu.addItem("Shape", "Shape", buttonSave.createWidget(),0, QSizeF(75, 75));
	menu.addItem("Shape", "Shape", buttonSave.createWidget(),1, QSizeF(55, 55));
	menu.addItem("Shape", "Shape", buttonSave.createWidget(),2, QSizeF(75, 75));

	menu.addGenericItem(buttonSave.createWidget(),0, QSizeF(75, 75));
	menu.addGenericItem(buttonOpen.createWidget(),1, QSizeF(120, 25));


	window.resize(600,400);
	window.show();


	return app.exec();
}
