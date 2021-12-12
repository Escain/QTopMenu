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
#include <QPainter>
#include <QPaintEvent>

#include "QPaletteExt.hpp"

class QPaletteWidgetTest: public QWidget
{

public:
	explicit QPaletteWidgetTest( QWidget* parent=nullptr)
		: QWidget(parent)
	{
	}
	
	QString colorRoleToString(const QPalette::ColorRole& role) const
	{
		switch( role)
		{
		case QPalette::ColorRole::WindowText: return "WindowText";
		case QPalette::ColorRole::Button: return "Button";
		case QPalette::ColorRole::Light: return "Light";
		case QPalette::ColorRole::Midlight: return "Midlight";
		case QPalette::ColorRole::Dark: return "Dark";
		case QPalette::ColorRole::Mid: return "Mid";
		case QPalette::ColorRole::Text: return "Text";
		case QPalette::ColorRole::BrightText: return "BrightText";
		case QPalette::ColorRole::ButtonText: return "ButtonText";
		case QPalette::ColorRole::Base: return "Base";
		case QPalette::ColorRole::Window: return "Window";
		case QPalette::ColorRole::Shadow: return "Shadow";
		case QPalette::ColorRole::Highlight: return "Highlight";
		case QPalette::ColorRole::HighlightedText: return "HighlightedText";
		case QPalette::ColorRole::Link: return "Link";
		case QPalette::ColorRole::LinkVisited: return "LinkVisited";
		case QPalette::ColorRole::AlternateBase: return "AlternateBase";
		case QPalette::ColorRole::NoRole: return "NoRole";
		case QPalette::ColorRole::ToolTipBase: return "ToolTipBase";
		case QPalette::ColorRole::ToolTipText: return "ToolTipText";
		case QPalette::ColorRole::NColorRoles: return "NColorRoles";
        case QPalette::ColorRole::PlaceholderText: return "PlaceholderText";
		}
		
		return "";
	}
	
	void paintEvent(QPaintEvent* e) override
	{
		QPainter p(this);
		p.setClipRect(e->rect());
		
		Escain::QPaletteExt palette;
		
		static std::vector<QPalette::ColorRole> roles
		{
			QPalette::ColorRole::Window,
			QPalette::ColorRole::WindowText,
			QPalette::ColorRole::Base,
			QPalette::ColorRole::AlternateBase,
			QPalette::ColorRole::ToolTipBase,
			QPalette::ColorRole::ToolTipText,
			QPalette::ColorRole::Text,
			QPalette::ColorRole::Button,
			QPalette::ColorRole::ButtonText,
			QPalette::ColorRole::BrightText,
			
			QPalette::ColorRole::Light,
			QPalette::ColorRole::Midlight,
			QPalette::ColorRole::Dark,
			QPalette::ColorRole::Mid,
			QPalette::ColorRole::Shadow,
			
			QPalette::ColorRole::Highlight,
			QPalette::ColorRole::HighlightedText,
			QPalette::ColorRole::Link,
			QPalette::ColorRole::LinkVisited,
			QPalette::ColorRole::NoRole,
			//QPalette::ColorRole::PlaceholderText
		};
		
		QPalette pal;
		QRect r(20, 20, 50, 20);
		Escain::QPaletteExt palExt;
		
		// Print labels of each role type
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.drawText(r.bottomLeft(), colorRoleToString(roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		r=QRect(r.left()+50, 20, r.width(), r.height());
		
		/*auto drawColorList = [](const QPalette& p)
		{
			
		};*/
		
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		pal.setCurrentColorGroup(QPalette::ColorGroup::Normal);
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, pal.brush(roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		pal.setCurrentColorGroup(QPalette::ColorGroup::Active);
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, pal.brush(roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		pal.setCurrentColorGroup(QPalette::ColorGroup::Inactive);
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, pal.brush(roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		pal.setCurrentColorGroup(QPalette::ColorGroup::Disabled);
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, pal.brush(roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		/*qDebug() << static_cast<int>(QWidget::palette().currentColorGroup());
		
		pal = QWidget::palette();
		qDebug() << palExt.isDarkTheme(pal);
		r=QRect(r.left()+4, 20, r.width(), r.height());
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, palExt.normal(pal,roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, palExt.hover(pal,roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}
		
		r=QRect(r.left()+52, 20, r.width(), r.height());
		p.fillRect(QRect(r.left(), r.top()-1, r.width()+1, roles.size()*(r.height()+2)), QColor(0,0,0,64));
		for (size_t i=0; i < roles.size(); ++i)
		{
			p.fillRect(r, palExt.press(pal,roles[i]));
			r=QRect(r.left(), r.top()+22, r.width(), r.height());
		}*/
		
	}
};



int main (int argn, char *argv[])
{
	QApplication app( argn, argv);
	
	QPaletteWidgetTest window;		Escain::QPaletteExt palExt;
	
	window.resize(800,600);
	window.show();
	
	return app.exec();

}
