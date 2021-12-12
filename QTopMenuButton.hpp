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

#ifndef QTOPMENUBUTTON_HPP
#define QTOPMENUBUTTON_HPP

#include <memory>

#include <QAction>
#include <QObject>

#include <QSvgIcon.hpp>
#include "QTopMenuAction.hpp"

#include "QTopMenuButtonWidget.hpp" //TODO remove

namespace Escain
{

/// TODO
/// Keyboard shortcut
/// Tooltip and WhatisThis
/// Documentation
/// bi-state

class QTopMenuButtonWidget;

class QTopMenuButton: public QObject, public QTopMenuAction
{
	Q_OBJECT
public:
	explicit QTopMenuButton();
	virtual ~QTopMenuButton() override;

	std::shared_ptr<QTopMenuWidget> createWidget( QWidget* parent=nullptr ) override;
	
	virtual void icon(const QSvgIcon& ic);
	virtual const QSvgIcon& icon() const;

	virtual void label( const std::string& label);
	virtual const std::string& label() const;

	virtual void margin( const qreal margin);
	virtual const qreal margin() const;

	virtual void enable( bool enabled);
	virtual bool enable() const;
signals:
	void triggered(QTopMenuButtonWidget* me); //TODO remove me

	/// Emitted when the size of the widget requires to be requested and applied.
	void bestSizeChanged();
protected:
	//QAction m_action;
	QSvgIcon m_icon;
	std::string m_label;
	bool m_enabled=true;
	qreal m_margin = 2.0;
	
	// Auto counter to represent each widget. Helping managing ids for QSvgIcon cache.
	size_t m_idAutocounter=0;
};

}

#endif
