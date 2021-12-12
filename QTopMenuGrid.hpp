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

#ifndef QTOPMENUGRID_HPP
#define QTOPMENUGRID_HPP

#include <QWidget>

#include <QPaletteExt.hpp>
#include <QSvgIcon.hpp>
#include <QTopMenuGridGroup.hpp>
#include <QTopMenuWidget.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace Escain
{

/// This class align several QTopMenuGridGroups to form the content of a top menu tab
class QTopMenuGrid: public QWidget
{
Q_OBJECT

public:
	explicit QTopMenuGrid( QWidget* parent= nullptr );
	virtual ~QTopMenuGrid() = default;
	
	/// Return a group by id, for eventual modification.
	QTopMenuGridGroup* getGroup(const std::string_view& id);
	const QTopMenuGridGroup* getGroup(const std::string_view& id) const;
	
	/// Return the ordered (as they appear) list of group ids
	std::vector<std::string> groupIds() const;
	
	/// Add a new group, at position (or end)
	///    if the id already exists, it return false, otherwise, it create it.
	///    if the position is out of bound, it append it at the end.
	bool addGroup(const QTopMenuGridGroup::Id& id, size_t pos=std::numeric_limits<size_t>::max());

	/// remove a group/section to the given menu
	/// menuId: the tab to which to remove the group
	/// groupId: the group to remove
	/// @return true if the element was removed
	virtual bool removeGroup(const QTopMenuGridGroup::Id& groupId);
	
	/// Set the direction of the grid (horizontal or vertical)
	DisplaySide direction() const { return m_direction; }
	virtual void direction( DisplaySide d );

	// Note: cell number, cells and margin allows for coherent positioning of widgets.
	//     3 cells = 3 * m_cellSize + 2 * m_margin

	/// Number of cells (available space) for widgets, perpendicular to the direction
	///     that is, vertical space for Horizontal GridGroup, and horizontal for Vertical GridGroup.
	size_t transversalCellNum() const;
	virtual void transversalCellNum( size_t cellNum );

	/// Size in Pts for a single space. (Allows to convert cell number to size)
	qreal cellSize() const;
	virtual void cellSize( qreal cellSize );

	/// Space between cells, in Pts
	qreal margin() const;
	virtual void margin( qreal margin );

	/// See Qt sizeHint
	QSize sizeHint() const override;

signals:
	void updateGeometryEvent();

protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent* e) override;

	virtual void repositionGroups();

private: 
	/// Set the order of focus (tab order) between sub-widgets
	virtual void updateFocusOrder();

	size_t m_transversalCellNum = 3;              // Number of cells perpendicular to the direction
	qreal m_cellSize = 20.0;                // Size of one-side of the cell (square)
	qreal m_margin = 2.0;                   // margin between cells
	DisplaySide m_direction = DisplaySide::Top;// direction of the grid (Horizontal, Vertical)

	std::list<QTopMenuGridGroup> m_groupV; // The list of groups, and items/widgets

	bool m_needsRepositionGroup = true;

	QSize m_cachedSizeHint;
};


}
#endif
