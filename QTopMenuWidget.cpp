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

#include "QTopMenuWidget.hpp"

namespace Escain
{

QTopMenuWidget::QTopMenuWidget(  const std::string& name, size_t id, QWidget* parent)
	: QWidget(parent)
	, m_name(name)
	, m_id(id)
{
	setObjectName(QString::fromStdString(nameId()) + "_" + QString::number(id)); //TODO remove
}

std::optional<QSizeF> QTopMenuWidget::bestSizeBetweenPossibles(
const std::vector<QSizeF>& possibles, const QSizeF& hint, const QSizeF& maxSize)
{
	double minDiff = std::numeric_limits<double>::max();
	std::optional<QSizeF> bestSize;
	
	for (const auto& s: possibles)
	{
		double diff = std::abs(s.width()-hint.width())+std::abs(s.height()-hint.height());
		if (diff < minDiff && s.width()<=maxSize.width() && s.height()<=maxSize.height())
		{
			minDiff = diff;
			bestSize = s;
		}
	}
	
	return bestSize;
}

void QTopMenuWidget::direction( const DisplaySide d )
{
	if (m_direction != d)
	{
		m_direction = d;
		update();
	}
}

void QTopMenuWidget::nameId( const std::string& id )
{
	m_name = id;
	setObjectName(QString::fromStdString(m_name) + "_" + QString::number(m_id)); //TODO remove
}
	
}
