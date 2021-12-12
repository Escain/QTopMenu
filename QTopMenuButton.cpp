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

#include "QTopMenuButton.hpp"

#include <QWidget>
#include <QPainter>

#include <QTopMenuButtonWidget.hpp>

using namespace Escain;

QTopMenuButton::QTopMenuButton(): QObject(), QTopMenuAction()
{
	m_icon.forgetId(0); // by default 0 is added.
}

QTopMenuButton::~QTopMenuButton()
{
	// Free icons, so there is no double deletion.
	for (auto& butPtr: m_widgetVector)
	{
		auto ptr = std::static_pointer_cast<QTopMenuButtonWidget>(butPtr);
		assert(ptr);
		ptr->icon(nullptr);
	}
}

std::shared_ptr<QTopMenuWidget> QTopMenuButton::createWidget( QWidget* parent )
{
	auto id=m_idAutocounter++;

	auto but = std::make_shared<QTopMenuButtonWidget>(nameId(), id, parent);

	//but->connect(but.get(), &QTopMenuButtonWidget::clicked, this, &QTopMenuButton::triggered); //TODO recover and remove next
	but->connect(but.get(), &QTopMenuButtonWidget::clicked, this, [this](const QPointF& , const std::unordered_set<size_t>&, QTopMenuButtonWidget* me){ emit triggered(me);});
	but->connect(but.get(), &QTopMenuButtonWidget::bestSizeChanged, this, &QTopMenuButton::bestSizeChanged);
	
	m_widgetVector.push_back(but);
	but->label(m_label);
	but->margin(m_margin);
	but->icon(&m_icon);
	
	return std::static_pointer_cast<QTopMenuWidget>(but);
}

void QTopMenuButton::icon(const QSvgIcon& ic)
{
	m_icon = ic;
	for (auto& butPtr: m_widgetVector)
	{
		auto ptr = std::static_pointer_cast<QTopMenuButtonWidget>(butPtr);
		assert(ptr);
		ptr->icon(&m_icon);
	}
}

const QSvgIcon& QTopMenuButton::icon() const
{
	return m_icon;
}

void QTopMenuButton::label( const std::string& label)
{
	if( m_label != label)
	{
		m_label = label;
		for (auto& butPtr: m_widgetVector)
		{
			auto ptr = std::static_pointer_cast<QTopMenuButtonWidget>(butPtr);
			assert(ptr);
			ptr->label(m_label);
		}
	}
}

const std::string& QTopMenuButton::label() const
{
	return m_label;
}

void QTopMenuButton::margin( const qreal margin)
{
	if( m_margin != margin)
	{
		m_margin = margin;
		for (auto& butPtr: m_widgetVector)
		{
			auto ptr = std::static_pointer_cast<QTopMenuButtonWidget>(butPtr);
			assert(ptr);
			ptr->margin(m_margin);
		}
	}
}

const qreal QTopMenuButton::margin() const
{
	return m_margin;
}


void QTopMenuButton::enable( bool enabled)
{
	if (m_enabled!=enabled)
	{
		m_enabled=enabled;
		for (auto& butPtr: m_widgetVector)
		{
			butPtr->setEnabled(enabled);
		}
	}
}

bool QTopMenuButton::enable() const
{
	return m_enabled;
}

