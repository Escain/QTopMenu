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

#include "QTopMenu.hpp"

#include <QCoreApplication>	// Get current path for relative paths
#include <QDir>				// Manage relative paths for loading
#include <QImage>			// Required to render the svg
#include <QPainter>			// Required to render the svg

using namespace Escain;

QTopMenu::QTopMenu( QWidget* parent )
	: QWidget(parent)
	, m_genericGroup(this)
	, m_tabWidget(this)
{
	m_genericGroup.divisionBar(true);
	m_genericGroup.label("");

	connect( &m_tabWidget, &QTopMenuTab::tabChanged, this, [this]
	(const Id& prev, const Id& next)
	{
		if (!prev.empty()) // first time it's empty
		{
			auto tabItPrev = m_tabs.find(prev);
			assert(tabItPrev!=m_tabs.end());
			tabItPrev->second.setVisible(false);
		}

		auto tabItNext = m_tabs.find(next);
		assert(tabItNext!=m_tabs.end());
		tabItNext->second.setVisible(true);

		// Set focus to the first group in the selected tab
		const auto groupIds = tabItNext->second.groupIds();
		if (!groupIds.empty())
		{
			auto* focussedGroup = tabItNext->second.getGroup(groupIds[0]);
			if (focussedGroup)
			{
				focussedGroup->setFocus();
			}
		}
	});

	connect(&m_tabWidget, &QTopMenuTab::updateGeometryEvent, this, [this]()
	{
		m_needUpdateMinMaxSizes = true;
	});

	connect(&m_genericGroup, &QTopMenuGridGroup::updateGeometryEvent, this, [this]()
	{
		m_needRecalculateGridsGeometry = true;
		m_needUpdateMinMaxSizes = true;
	});

	m_genericGroup.transversalCellNum(m_transversalCellNum);
	m_genericGroup.cellSize(m_cellSize);
}

size_t QTopMenu::transversalCellNum() const
{
	return m_transversalCellNum;
}

void QTopMenu::transversalCellNum( size_t cellNum )
{
	if (m_transversalCellNum != cellNum)
	{
		m_transversalCellNum = cellNum;
		for (auto& [id, grid]: m_tabs)
		{
			grid.transversalCellNum(cellNum);
		}
		m_genericGroup.transversalCellNum(cellNum);
		m_needRecalculateGridsGeometry = true;
		update();
	}
}

qreal QTopMenu::cellSize() const
{
	 return m_cellSize;
}

void QTopMenu::cellSize( qreal cellSize )
{
	if (m_cellSize != cellSize)
	{
		m_cellSize = cellSize;
		for (auto& [id, grid]: m_tabs)
		{
			grid.cellSize(cellSize);
		}
		m_genericGroup.cellSize(cellSize);
		m_needRecalculateGridsGeometry = true;
		update();
	}
}

qreal QTopMenu::tabMargin() const
{
	return m_tabWidget.tabMargin();
}

void QTopMenu::tabMargin( qreal margin )
{
	if (m_tabWidget.tabMargin() != margin)
	{
		m_tabWidget.tabMargin(margin);
		m_needUpdateMinMaxSizes = true;
		update();
	}
}

qreal QTopMenu::tabMinSizePercentil() const
{
	return m_tabWidget.tabMinSizePercentil();
}

void QTopMenu::tabMinSizePercentil( qreal percentil)
{
	if (m_tabWidget.tabMinSizePercentil() != percentil)
	{
		m_tabWidget.tabMinSizePercentil(percentil);
		m_needUpdateMinMaxSizes = true;
		update();
	}
}

bool QTopMenu::showGenericGroup() const
{
	return m_showGenericGroup;
}

void QTopMenu::showGenericGroup( bool visible )
{
	if (m_showGenericGroup != visible)
	{
		m_showGenericGroup = visible;
		m_needUpdateMinMaxSizes = true;
		m_needRecalculateGridsGeometry = true;
		update();
	}
}

void QTopMenu::recalculateGridsGeometry()
{
	const bool isAtTop = (DisplaySide::Top==m_direction);

	auto transpIfVert = [isAtTop]( const auto& orig)
	{
		return isAtTop ? orig : orig.transposed();
	};

	const auto tabWidgetSizeT = transpIfVert(m_tabWidget.size());

	const auto& genericGroupPos = transpIfVert(QPoint(0, tabWidgetSizeT.height()));
	if (m_genericGroup.pos() != genericGroupPos)
	{
		m_genericGroup.move(genericGroupPos);
	}
	if (m_showGenericGroup != m_genericGroup.isVisible())
	{
		m_genericGroup.setVisible(m_showGenericGroup);
	}

	// Get the space available for tabs
	const qreal globalWidth = m_showGenericGroup ? transpIfVert(m_genericGroup.uncollapsedSize()).width() : 0.0;
	QPointF tabPos  = transpIfVert(QPointF(globalWidth, 0.0));
	if (tabPos != m_tabWidget.pos())
	{
		m_tabWidget.move(tabPos.toPoint());
	}

	QSize thisSizeT = transpIfVert(size());

	// Set the tab rect size
	for ( size_t i=0; i< m_tabOrder.size(); i++)
	{
		Id id= m_tabOrder[i];
		QTopMenuGrid& grid = m_tabs.at(id);
		const QPoint gridPos = transpIfVert(QPoint(globalWidth, tabWidgetSizeT.height()));
		const QSize gridSize = transpIfVert(QSize(thisSizeT.width()-globalWidth,
		    thisSizeT.height()- tabWidgetSizeT.height()));

		if (grid.pos() != gridPos)
		{
			grid.move(gridPos);
		}
		if (grid.size() != gridSize)
		{
			grid.resize(gridSize);
		}
	}

	m_needRecalculateGridsGeometry = false;
}

void QTopMenu::updateMinMaxSizes()
{

	bool isAtTop = direction()==DisplaySide::Top;

	auto transpIfVert = [isAtTop]( const QSize& orig) -> QSize
	{
		return isAtTop ? orig : orig.transposed();
	};

	const auto tabHeight = m_tabWidget.height();

	// Working with transposed size (as it was all Horizontal/TopSide)

	// GeneralGroup
	auto genSizeT = QSize(0,0);
	if (m_showGenericGroup)
	{
		genSizeT = transpIfVert(m_genericGroup.uncollapsedSize());
	}

	// All Tab labels
	QSize labelCoSumT = transpIfVert(m_tabWidget.minimumSize());

	// All Tab groups
	QSize tabCoMaxT(0,0);
	for (auto& [id, tab]: m_tabs)
	{
		const auto& gridCoSizeT = transpIfVert(tab.minimumSize());

		tabCoMaxT = QSize(std::max(tabCoMaxT.width(), gridCoSizeT.width()),
		    std::max(tabCoMaxT.height(), gridCoSizeT.height()));
	}

	QSize intermediateT = QSize(0, tabHeight);
	intermediateT = QSize(std::max(intermediateT.width(), genSizeT.width()),
	    std::max(intermediateT.height(), genSizeT.height()));
	intermediateT = QSize( std::max(intermediateT.width(), genSizeT.width()+labelCoSumT.width()),
	    std::max(intermediateT.height(), labelCoSumT.height()));

	// Calculate sizeHint
	QSize tabSizeHintCoMaxT(0,0);
	for (auto& [id, tab]: m_tabs)
	{
		const auto& gridCoSizeT = transpIfVert(tab.sizeHint());

		tabSizeHintCoMaxT = QSize(std::max(tabSizeHintCoMaxT.width(), gridCoSizeT.width()),
		    std::max(tabSizeHintCoMaxT.height(), gridCoSizeT.height()));
	}
	QSize sizeHintT = intermediateT;
	sizeHintT = QSize( std::max(sizeHintT.width(), genSizeT.width()+tabSizeHintCoMaxT.width()),
	    std::max(sizeHintT.height(), tabSizeHintCoMaxT.height()+static_cast<int>(tabHeight)));
	m_cachedSizeHint = transpIfVert(sizeHintT);


	// Set the result
	QSize minSizeT = intermediateT;
	minSizeT = QSize( std::max(minSizeT.width(), genSizeT.width()+tabCoMaxT.width()),
	    std::max(minSizeT.height(), tabCoMaxT.height()+static_cast<int>(tabHeight)));

	const auto minSize = transpIfVert(minSizeT);
	if (minSize != minimumSize())
	{
		setMinimumSize( transpIfVert(minSizeT));
		updateGeometry();
	}

	m_needUpdateMinMaxSizes = false;
}

void QTopMenu::switchToTabById( const Id& idToSelect)
{
	m_tabWidget.switchToTabById(idToSelect);
}

const QTopMenu::Id& QTopMenu::selectedId() const
{
	return m_tabWidget.selectedId();
}

const QKeySequence QTopMenu::tabShortcut( const Id& menuId ) const
{
	return m_tabWidget.tabShortcut(menuId);
}

bool QTopMenu::tabShortcut( const Id& menuId, const QKeySequence& seq)
{
	return m_tabWidget.tabShortcut( menuId, seq);
}

const std::vector<QTopMenu::Id>& QTopMenu::tabIds() const
{
	return m_tabOrder;
}

const std::optional<std::string> QTopMenu::tabLabel( const Id& menuId) const
{
	return m_tabWidget.tabLabel(menuId);
}

bool QTopMenu::tabLabel( const Id& menuId, const std::string& label)
{
	return m_tabWidget.tabLabel(menuId, label);
}

void QTopMenu::resizeEvent(QResizeEvent*)
{
	m_needRecalculateGridsGeometry = true;
	m_needUpdateMinMaxSizes = true;
}

void QTopMenu::paintEvent(QPaintEvent* e)
{
	if (m_needRecalculateGridsGeometry)
	{
		recalculateGridsGeometry();
		recalculateGridsGeometry();
	}

	if (m_needUpdateMinMaxSizes)
	{
		updateMinMaxSizes();
	}

	bool isAtTop = direction()==DisplaySide::Top;

	auto transposeIfVert = [isAtTop]( const auto& orig)
	{
		return isAtTop ? orig : orig.transposed();
	};

	QWidget::paintEvent(e);
	QPainter p(this);
	p.setClipRect(e->rect());
	p.setRenderHint(QPainter::Antialiasing );
	QPaletteExt pal = QWidget::palette();

	if (!isEnabled())
	{
		pal.setCurrentColorGroup(QPalette::Disabled);
	}

	QSize widgetSizeT = transposeIfVert(size());
	QSize tabSizeT = transposeIfVert(m_tabWidget.size());
	QColor bgColor = pal.color(ColorRoleExt::BackgroundMid_Normal);
	const auto darkBg = transposeIfVert(QSize(widgetSizeT.width(), tabSizeT.height()));
	p.fillRect(QRect(QPoint(0,0), darkBg), bgColor);
}

void QTopMenu::direction(DisplaySide dir)
{
	if (dir != m_direction)
	{
		m_direction = dir;
		if (m_direction == DisplaySide::Top)
		{
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		}
		else
		{
			setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		}
		m_genericGroup.direction(dir);
		m_tabWidget.direction(dir);
		for (auto& [id, tab]: m_tabs)
		{
			tab.direction(dir);
		}

		m_needUpdateMinMaxSizes = true;
		m_needRecalculateGridsGeometry = true;
		updateGeometry();
		update();
	}
}

DisplaySide QTopMenu::direction() const
{
	return m_direction;
}

bool QTopMenu::groupIcon( const Id& menuId, const QTopMenuGridGroup::Id& groupId, const QSvgIcon& ico )
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return false;
	}

	group->icon(ico);

	return true;
}

const std::vector<QTopMenu::Id> QTopMenu::groupIds(const Id& menuId) const
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return {};
	}
	return tabIt->second.groupIds();
}

const std::optional<std::string> QTopMenu::groupLabel(
    const Id& menuId, const QTopMenuGridGroup::Id& groupId) const
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return {};
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return {};
	}

	return group->label();
}

bool QTopMenu::grouplabel( const Id& menuId, const QTopMenuGridGroup::Id& groupId,
    const std::string& label)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return false;
	}

	group->label(label);
	return true;
}

bool QTopMenu::addGroup(const Id& menuId, const QTopMenuGridGroup::Id& groupId, size_t pos)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	const auto ret = tabIt->second.addGroup(groupId, pos);

	if (ret)
	{
		auto* group = tabIt->second.getGroup(groupId);
		if (group)
		{
			group->label(groupId);
		}
		m_needUpdateMinMaxSizes = true;
		update();
	}

	return ret;
}

bool QTopMenu::removeGroup(const Id& menuId, const QTopMenuGridGroup::Id& groupId)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	const auto ret = tabIt->second.removeGroup(groupId);

	if (ret)
	{
		m_needUpdateMinMaxSizes = true;
		update();
	}

	return ret;
}

bool QTopMenu::addItem(const Id& menuId, const QTopMenuGridGroup::Id& groupId,
	std::shared_ptr<QTopMenuWidget> widget, size_t column, const QSizeF& sizeHint)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return false;
	}

	group->addItem(widget, column, sizeHint);

	m_needUpdateMinMaxSizes = true;
	update();

	return true;
}

bool QTopMenu::addItem(const Id& menuId, const QTopMenuGridGroup::Id& groupId,
    std::shared_ptr<QTopMenuWidget> widget, size_t column, bool newColumn, size_t heightPos, const QSizeF& sizeHint)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return false;
	}

	group->addItem(widget, column, newColumn, heightPos, sizeHint);

	m_needUpdateMinMaxSizes = true;
	update();

	return true;
}

void QTopMenu::addGenericItem(std::shared_ptr<QTopMenuWidget> widget,
    size_t column, const QSizeF& sizeHint)
{
	m_genericGroup.addItem(widget, column, sizeHint);
	m_needUpdateMinMaxSizes = true;
	m_needRecalculateGridsGeometry = true;
	update();
}

void QTopMenu::addGenericItem(std::shared_ptr<QTopMenuWidget> widget,
    size_t column, bool newColumn, size_t heightPos, const QSizeF& sizeHint)
{
	m_genericGroup.addItem(widget, column, newColumn, heightPos, sizeHint);
	m_needUpdateMinMaxSizes = true;
	m_needRecalculateGridsGeometry = true;
	update();
}

const std::vector<QTopMenuGridGroup::GroupItemInfo> QTopMenu::genericGroupItemsInfo() const
{
	return m_genericGroup.itemsInfo();
}

const std::vector<QTopMenuGridGroup::GroupItemInfo> QTopMenu::groupItemsInfo(
    const Id& menuId, const QTopMenuGridGroup::Id& groupId) const
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return {};
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return {};
	}

	return group->itemsInfo();
}

bool QTopMenu::removeItem( const Id& menuId, const QTopMenuGridGroup::Id& groupId,
	size_t column, size_t heightPos)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt==m_tabs.end())
	{
		return false;
	}

	auto* group = tabIt->second.getGroup(groupId);
	if (!group)
	{
		return false;
	}

	const auto ret = group->removeItem(column, heightPos);
	m_needUpdateMinMaxSizes = true;
	update();
	return ret;
}

bool QTopMenu::removeGenericItem( size_t column, size_t heightPos)
{
	const auto ret = m_genericGroup.removeItem(column, heightPos);
	m_needUpdateMinMaxSizes = true;
	m_needRecalculateGridsGeometry = true;
	update();
	return ret;
}

bool QTopMenu::insertTab(const Id& id, const QString& name, int pos)
{
	// Avoid tabs with id empty
	if (id.empty())
	{
		return false;
	}
	auto checkedPos = std::min(static_cast<size_t>(pos), m_tabs.size());

	auto it = m_tabs.find(id);
	if (it!=m_tabs.end())
	{
		return false;
	}

	// Insert new element
	auto [newIt, insertedBool] = m_tabs.emplace(id, this);

	auto& newTabObj = newIt->second;
	m_tabOrder.insert(m_tabOrder.begin()+checkedPos,id);
	newTabObj.transversalCellNum(m_transversalCellNum);
	newTabObj.cellSize(m_cellSize);
	newTabObj.setVisible(false);
	m_tabWidget.insertTab(id, name, pos);

	m_needUpdateMinMaxSizes = true;
	m_needRecalculateGridsGeometry = true;

	updateFocusOrder();
	return true;
}

bool QTopMenu::removeTab(const Id& tabId)
{
	// Avoid tabs with id empty
	if (tabId.empty())
	{
		return false;
	}

	auto it = m_tabs.find(tabId);
	if (it==m_tabs.end())
	{
		return false;
	}

	m_tabs.erase(it);
	auto orderIt = std::find(m_tabOrder.begin(), m_tabOrder.end(), tabId);
	if (orderIt != m_tabOrder.cend())
	{
		m_tabOrder.erase(orderIt);
	}
	m_tabWidget.removeTab(tabId);

	m_needUpdateMinMaxSizes = true;
	m_needRecalculateGridsGeometry = true;
	updateFocusOrder();

	return true;
}

void QTopMenu::updateFocusOrder()
{
	for (auto& [id, tabGrid]: m_tabs)
	{
		setTabOrder(&m_genericGroup, &tabGrid);
	}
}

QSize QTopMenu::sizeHint() const
{
	if (m_needUpdateMinMaxSizes)
	{
		// Cache needs to be updated, but sizeHint is still const
		const_cast<QTopMenu*>(this)->updateMinMaxSizes();
	}
	return m_cachedSizeHint;

}

