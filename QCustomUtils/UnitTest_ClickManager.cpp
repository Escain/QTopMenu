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
#include <vector>

#include <QDebug>
#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>

#include "QClickManager.hpp"

class ClickableWidgetTest: public QWidget
{
	Escain::QClickManager clicker;
	std::vector<QRectF> areas = 
		{
			QRectF(10.0, 25.0, 30.0, 50.0),
			QRectF(30.0, 10.0, 50.0, 30.0),
			QRectF(100.0, 10.0, 20.0, 20.0),
			QRectF(50.0, 50.0, 30.0, 2.0)
		};
public:
	explicit ClickableWidgetTest( QWidget* parent=nullptr)
		: QWidget(parent)
		, clicker(this)
	{
		using Escain::QClickManager;
		
		for (size_t i=0; i<areas.size(); i++)
		{
			clicker.addClickableRectangle( i, areas[i].toRect() );
		}
		connect(&clicker, &QClickManager::clicked, this, &ClickableWidgetTest::clicked);
	}
	
	bool event(QEvent* e) override
	{
		clicker.eventHandler(e);
		return QWidget::event(e);
	}
	
	const std::vector<QRectF>& getRect() const 
	{
		return areas;
	}

	public slots:
	
	void clicked(const QPointF& cursorPos, const std::unordered_set<size_t>& clickableRectangleIds)
	{
		QString s;
		for (size_t id: clickableRectangleIds)
		{
			s += " " + QString::number(id);
		}
		qDebug() << "Clicked: " << cursorPos << s;
	}
};



int main (int argn, char *argv[])
{
	QApplication app( argn, argv);
	
	QWidget window;
	
	ClickableWidgetTest clickerWindow;
	for (const QRectF& r:clickerWindow.getRect())
	{
		QWidget* clickerArea = new QWidget(&clickerWindow);
		clickerArea->setGeometry(r.toRect());
		clickerArea->setStyleSheet("background-color: rgba(0, 128, 255, 64);");
	}
	
	
	clickerWindow.resize(300,200);
	clickerWindow.show();
	

	
	return app.exec();

}
