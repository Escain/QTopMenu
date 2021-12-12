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

#include "QTopMenuGrid.hpp"

#include <QDebug> //TODO remove

using namespace Escain;

QTopMenuGrid::QTopMenuGrid( QWidget* parent )
	: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	setFocusPolicy( Qt::FocusPolicy::StrongFocus);
}

QTopMenuGridGroup* QTopMenuGrid::getGroup(const std::string_view& id)
{
	for (auto& g: m_groupV)
	{
		if (g.id() == id)
		{
			return &g;
		}
	}
	return nullptr;
}

const QTopMenuGridGroup* QTopMenuGrid::getGroup(const std::string_view& id) const
{
	for (auto& g: m_groupV)
	{
		if (g.id() == id)
		{
			return &g;
		}
	}
	return nullptr;
}


std::vector<std::string> QTopMenuGrid::groupIds() const
{
	std::vector<std::string> ids;
	ids.reserve(m_groupV.size());
	
	for (const auto& g: m_groupV)
	{
		ids.push_back(g.id());
	}
	return ids;
}

bool QTopMenuGrid::addGroup(const QTopMenuGridGroup::Id& id, size_t pos)
{
	if(getGroup(id))
	{
		return false;
	}
	
	auto checkedPos = std::min(pos, m_groupV.size());
	
	size_t counter=0;
	auto it=m_groupV.begin();
	for (; it!=m_groupV.end(); ++it, ++counter)
	{
		if (counter==checkedPos)
		{
			break;
		}
	}

	auto insertedIt = m_groupV.emplace(it, this);
	insertedIt->id(id);
	insertedIt->direction(m_direction);
	insertedIt->transversalCellNum(m_transversalCellNum);
	insertedIt->margin(m_margin);
	insertedIt->cellSize(m_cellSize);
	insertedIt->setVisible(true);

	connect(&(*insertedIt), &QTopMenuGridGroup::updateGeometryEvent, this, [this]()
	{
		m_needsRepositionGroup=true;
		emit updateGeometryEvent();
		update();
	});

	updateFocusOrder();
	m_needsRepositionGroup = true;
	
	return true;
}

bool QTopMenuGrid::removeGroup(const QTopMenuGridGroup::Id& groupId)
{
	auto isIdEqual = [&groupId](const QTopMenuGridGroup& v){ return v.id()==groupId; };
	auto it = std::find_if(m_groupV.begin(), m_groupV.end(), isIdEqual);
	if (it == m_groupV.cend())
	{
		return false;
	}

	it->setParent(nullptr);
	m_groupV.erase(it);

	updateFocusOrder();
	m_needsRepositionGroup = true;
	emit updateGeometryEvent();
	return true;
}

void QTopMenuGrid::direction( DisplaySide d )
{
	if (m_direction != d)
	{
		m_direction = d;
		for (auto& g: m_groupV)
		{
			g.direction(d);
		}

		if (m_direction == DisplaySide::Top)
		{
			setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		}
		else
		{
			setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		}

		m_needsRepositionGroup = true;
		update();
	}
}

size_t QTopMenuGrid::transversalCellNum() const
{
	return m_transversalCellNum;
}

void QTopMenuGrid::transversalCellNum( size_t cellNum )
{
	if (m_transversalCellNum != cellNum)
	{
		m_transversalCellNum = cellNum;
		for (auto& g: m_groupV)
		{
			g.transversalCellNum(cellNum);
		}
		m_needsRepositionGroup = true;
		update();
	}
}

qreal QTopMenuGrid::cellSize() const
{
	 return m_cellSize;
}

void QTopMenuGrid::cellSize( qreal cellSize )
{
	if (m_cellSize != cellSize)
	{
		m_cellSize = cellSize;
		for (auto& g: m_groupV)
		{
			g.cellSize(cellSize);
		}
		m_needsRepositionGroup = true;
		update();
	}
}

qreal QTopMenuGrid::margin() const
{
	return m_margin;
}

void QTopMenuGrid::margin( qreal margin )
{
	if (m_margin != margin)
	{
		m_margin = margin;
		for (auto& g: m_groupV)
		{
			g.margin(margin);
		}
		m_needsRepositionGroup = true;
		update();
	}
}

void QTopMenuGrid::paintEvent(QPaintEvent* e)
{
	if (m_needsRepositionGroup)
	{
		repositionGroups();
	}

	QWidget::paintEvent(e);
}

void QTopMenuGrid::repositionGroups()
{
	QSize minSize(0,0);
	QSize maxSize(0,0);

	std::function<qreal(std::list<QTopMenuGridGroup>::iterator& it, qreal prevPos)> positionGroup;

	bool focusOrderNeedsUpdate=false;

	positionGroup = [this, &positionGroup, &minSize, &maxSize, &focusOrderNeedsUpdate]
	(std::list<QTopMenuGridGroup>::iterator it, qreal prevPos) -> qreal
	{
		if (it == m_groupV.cend())
		{
			return prevPos;
		}

		auto dir = direction();

		QSize collapsed = it->collapsedSize();
		QSize uncollapsed = it->uncollapsedSize();

		qreal dirUncollapsedSize;
		qreal dirCollapsedSize;
		if (direction() == DisplaySide::Top)
		{
			dirUncollapsedSize = uncollapsed.width();
			dirCollapsedSize = collapsed.width();
			maxSize = QSize(maxSize.width() + dirUncollapsedSize, std::max(maxSize.height(), uncollapsed.height()));
			minSize = QSize(minSize.width() + dirCollapsedSize, std::max(minSize.height(), collapsed.height()));
		}
		else
		{
			dirUncollapsedSize = uncollapsed.height();
			dirCollapsedSize = collapsed.height();
			maxSize = QSize(std::max(maxSize.width(), uncollapsed.width()), maxSize.height() + dirUncollapsedSize);
			minSize = QSize(std::max(minSize.width(), collapsed.width()), minSize.height() + dirCollapsedSize);
		}

		auto nextIt = it;
		nextIt++;
		qreal sumSize = positionGroup(nextIt, prevPos + dirUncollapsedSize);

		if (sumSize > (dir==DisplaySide::Top ? width() : height()))
		{
			if (!it->collapsed())
			{
				it->collapsed(true);
				focusOrderNeedsUpdate = true;
			}
			sumSize -= (dir==DisplaySide::Top ? uncollapsed.width()-collapsed.width() : uncollapsed.height() - collapsed.height());
			return sumSize;
		}

		if (it->collapsed())
		{
			it->collapsed(false);
			focusOrderNeedsUpdate = true;
		}

		return sumSize;
	};

	auto it = m_groupV.begin();
	positionGroup( it, 0);

	qreal pos = 0;
	for (auto gIt = m_groupV.begin(); gIt != m_groupV.end(); ++gIt)
	{
		auto newPos = direction()==DisplaySide::Top ? QPoint(pos, 0) : QPoint(0, pos);
		QSize groupSize = gIt->collapsedSize();
		if (!gIt->collapsed())
		{
			groupSize = gIt->uncollapsedSize();
		}
		gIt->move(newPos);
		gIt->divisionBar(std::next(gIt)!=m_groupV.end());
		pos += (direction()==DisplaySide::Top ? groupSize.width() : groupSize.height());

	}

	maxSize = maxSize.expandedTo(QSize(1,1)); // Max size can't be set to 0,0, avoid issues
	if (minSize != minimumSize() || maxSize != maximumSize())
	{
		setMinimumSize(minSize);
	}
	if (maxSize != m_cachedSizeHint)
	{
		m_cachedSizeHint = maxSize;
		updateGeometry();
	}

	if (focusOrderNeedsUpdate)
	{
		updateFocusOrder(); //TODO check if needed after tabulation order is fixed
	}

	m_needsRepositionGroup = false;
}

QSize QTopMenuGrid::sizeHint() const
{
	if (m_needsRepositionGroup)
	{
		const_cast<QTopMenuGrid*>(this)->repositionGroups();

	}
	return m_cachedSizeHint;
}

//For debugging TAB order, TODO remove when fixed
/*void showOrder2(QWidget* s)
{
	auto w = s;
	const auto init=w;
	QString accum;
	while (w)
	{
		accum += (accum.isEmpty() ? "'":"; '") + w->objectName() + "'";
		w =w->nextInFocusChain();
		if (w == init) break;
	}

	qDebug() << "Order:" << accum;
}*/


void QTopMenuGrid::updateFocusOrder()
{
	//TODO a possible alternative for tabulation order, remove when fixed.
	/*setFocusProxy(m_groupV.empty() ? nullptr : &(*m_groupV.begin()));
	qDebug() << "entering group focus order"; //TODO remove
	QWidget* w=this;
	for (auto& gridGroup: m_groupV)
	{
		w = gridGroup.setTabulationOrder( w );
	}
	//setTabOrder(w, this);
	qDebug() << "exiting group focus order"; //TODO remove

	showOrder2(this);*/


	QTopMenuGridGroup* prev = nullptr;
	for (auto& gridGroup: m_groupV)
	{
		if (prev)
		{
			setTabOrder(prev, &gridGroup);
		}
		prev = &gridGroup;
	}
}

void QTopMenuGrid::resizeEvent(QResizeEvent*)
{
	m_needsRepositionGroup = true;
}


