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

#ifndef QTOPMENU_HPP
#define QTOPMENU_HPP

#include <string>
#include <unordered_map>

#include <QStaticText>
#include <QWidget>

#include <QClickManager.hpp>
#include "QTopMenuGrid.hpp"
#include "QTopMenuTab.hpp"
#include "QTopMenuWidgetTypes.hpp"

namespace Escain
{

///TODO
/// - Fix: Setting tab group label to larger text making minimumSize to increase, the menu is not updated and part of the groups is hidden.
///      & (related) adding runtime a group to a grid is not updating the minimumSize of the QTopMenu
/// - Fix: In vertical mode, the text is elided way too short?
/// - Clean todos and code
/// - UT Texts
/// - Document
/// - Fix: tabulation in both collapsed and uncollapsed state -> wait for a solution
///
/// IMPROVE
/// - Fix: ButtonWidget: in big and medium size, the label maximum can be increased.
/// - Generic group can be configured by the user, with the widgets from the library.

/**
 * @brief QTopMenu
 *
 * Widget for creating a user-friendly top menu, with tabs, customization, etc.
 * 
 * Use QAction to add capabilities to the top menu
 * 
 */
class QTopMenu: public QWidget
{
Q_OBJECT
public:

	using Id = std::string;

	explicit QTopMenu( QWidget* parent=nullptr );
	virtual ~QTopMenu() = default;

	//*//////////// GENERAL PROPERTIES //////////
	virtual void direction(DisplaySide dir);
	virtual DisplaySide direction() const;

	// Note: cell number, cells and margin allows for coherent positioning of widgets.
	//     3 cells = 3 * m_cellSize + 2 * m_margin

	/// Number of cells (available space) for widgets, perpendicular to the direction
	///     that is, vertical space for Horizontal GridGroup, and horizontal for Vertical GridGroup.
	size_t transversalCellNum() const;
	virtual void transversalCellNum( size_t cellNum );

	/// Size in Pts for a single space. (Allows to convert cell number to size)
	qreal cellSize() const;
	virtual void cellSize( qreal cellSize );
	

	//*//////////// TAB MANAGEMENT //////////////
	/// @brief Create a new tab in the widget
	/// @param tabId an id to identify the tab, must be unused for this widget.
	/// @param name any label
	/// @param pos: where to insert the tab, -1 indicate at the end
	/// @return if the insert were successful, (not already existing)
	virtual bool insertTab(const Id& tabId, const QString& name, int pos = -1);

	/// @brief Remove an existing tab and all it content.
	/// @param tabId an id to identify the tab.
	/// @return if the removal were successful.
	virtual bool removeTab(const Id& tabId);

	/// Make the given tab to be selected
	virtual void switchToTabById( const Id& idToSelect);

	/// Return the id of the currenctly selected tab
	const Id& selectedId() const;

	/// Shortcut for a given tab
	const QKeySequence tabShortcut( const Id& menuId ) const;
	virtual bool tabShortcut( const Id& menuId, const QKeySequence& seq);

	/// Space around text in tab
	qreal tabMargin() const;
	virtual void tabMargin( qreal margin );

	/// Percentil of the tabs (label) widths, used as minimal tab size:
	///     0%: all tabs takes exactly their label width + tabMargin*2
	///     100%: all tabs adjust to the same size as the largest + tabMargin*2
	qreal tabMinSizePercentil() const;
	virtual void tabMinSizePercentil( qreal percentil);

	/// Return a vector with all the tab Ids
	const std::vector<Id>& tabIds() const;

	/// Label for the given tab
	const std::optional<std::string> tabLabel( const Id& menuId) const;
	/// Set the label for the given tab
	/// @return true if set properly (false if the menu does not exist)
	virtual bool tabLabel( const Id& menuId, const std::string& label);

	//*//////////// GROUP MANAGEMENT //////////////
	/// Add a group/section to the given menu at that position
	/// @param menuId: the tab to which to add the group
	/// @param groupId: the new group ID to tadd
	/// @param pos: the column(horizontal) or row(vertical) where to add it. Default: at the end.
	/// @return true if the element was added (false if already existing or tab not yet existing)
	virtual bool addGroup(const Id& menuId, const QTopMenuGridGroup::Id& groupId,
		size_t pos=std::numeric_limits<size_t>::max());

	/// remove a group/section to the given menu
	/// @param menuId: the tab to which to remove the group
	/// @param groupId: the group to remove
	/// @return true if the element was removed
	virtual bool removeGroup(const Id& menuId, const QTopMenuGridGroup::Id& groupId);

	/// Set the collapsed icon for a tab and group
	/// @return true if the icon was set (not set if the menuId is not found).
	virtual bool groupIcon( const Id& menuId, const QTopMenuGridGroup::Id& groupId, const QSvgIcon& ico );

	/// Return the list of group Ids for a given tab.
	/// Note: no setter for this: delete the group and recreate
	/// @param menuId: the tab for which to list groups
	/// @return the list of groups, or an empty vector if the tab is not found.
	const std::vector<Id> groupIds(const Id& menuId) const;

	/// Get the label for the given group
	/// @param menuId: id of the tab
	/// @param groupId: id of the group
	/// @return the given label, or nullopt if the group does not exist.
	const std::optional<std::string> groupLabel( const Id& menuId, const QTopMenuGridGroup::Id& groupId) const;

	/// Set the label for the given group
	/// @param menuId: id of the tab
	/// @param groupId: id of the group
	/// @param label: the new label to set
	/// @return true if the label was set properly, false otherwise (e.g. group does not exist)
	virtual bool grouplabel( const Id& menuId, const QTopMenuGridGroup::Id& groupId, const std::string& label);

//TODO group & genericGroup margin getter/setter (4 functions)

	//*//////////// ITEMs MANAGEMENT IN GENERIC GROUP //////////////
	/// Show or hide the general group (that group visible left/top of all tabbed grid-groupos)
	bool showGenericGroup() const;
	void showGenericGroup( bool visible );

	/// Add a widget at a given position (column index for Horizontal, row index for Vertical)
	/// @param widget: the widget to be added
	/// @param column: where to add the widget (column for horizontal/top, and row for vertical/left)
	/// @param newColumn: if true, a new column is inserted instead of using the existing one.
	/// @param heightPos: where to insert the widget, between 0 and the number of existing widgets on
	///                   that row
	/// @param SizeHint is the desired size for the widget: can be adjusted to widgets size requirements
	/// @throws if the widget cold not be inserted (e.g. invalid values)
	virtual void addGenericItem( std::shared_ptr<QTopMenuWidget> widget, size_t column, bool newColumn,
	    size_t heightPos, const QSizeF& sizeHint );

	/// Add a widget at a given position (column index for Horizontal, row index for Vertical)
	/// The widget is added at the last position on the given column. The column is only
	///     added if it happens to be the very next one to the last existing.
	/// @param widget: the widget to be added
	/// @param column: where to add the widget (column for horizontal/top, and row for vertical/left)
	/// @param SizeHint is the desired size for the widget: can be adjusted to widgets size requirements
	/// @throws if the widget cold not be inserted (e.g. invalid values)
	virtual void addGenericItem( std::shared_ptr<QTopMenuWidget> widget, size_t column, const QSizeF& sizeHint );

	/// Retrieve the configuration of widgets in the generic group
	/// @return the list of all widgets and their column/heightPos.
	virtual const std::vector<QTopMenuGridGroup::GroupItemInfo> genericGroupItemsInfo() const;

	/// Following the same logic than itemAt, remove the widget from the group
	/// If the column is empty, it is removed.
	/// @param column is in the direction of the grid, heightPos is transversal to that direction:
	///    - horizontal and vertical for Direction::Horizontal
	///    - vertical and horizontal for Direction::Vertical
	/// @param heightPos: is the position of the widget in that column
	/// Warning: if no space is available, widgets may appear visual on a temporary/different column.
	///     than the one in which it really belong
	/// @param true if the widget was found and removed
	virtual bool removeGenericItem(size_t column, size_t heightPos);

	//*//////////// ITEMs MANAGEMENT IN TAB GROUPs //////////////
	/// Add a widget at a given position (column index for Horizontal, row index for Vertical)
	/// @param menuId: the id of the tab
	/// @param groupId: the id of the group in that tab
	/// @param widget: the widget to be added
	/// @param column: where to add the widget (column for horizontal/top, and row for vertical/left)
	/// @param newColumn: if true, a new column is inserted instead of using the existing one.
	/// @param heightPos: where to insert the widget, between 0 and the number of existing widgets on
	///                   that row
	/// @param SizeHint is the desired size for the widget: can be adjusted to widgets size requirements
	/// @return true if the group was found (incesion succeed)
	/// @throws if the widget cold not be inserted (e.g. invalid values)
	virtual bool addItem( const Id& menuId, const QTopMenuGridGroup::Id& groupId,
	    std::shared_ptr<QTopMenuWidget> widget, size_t column, bool newColumn,
	    size_t heightPos, const QSizeF& sizeHint );
	/// Add a widget at a given position (column index for Horizontal, row index for Vertical)
	/// The widget is added at the last position on the given column. The column is only
	///     added if it happens to be the very next one to the last existing.
	/// @param menuId: the id of the tab
	/// @param groupId: the id of the group in that tab
	/// @param widget: the widget to be added
	/// @param column: where to add the widget (column for horizontal/top, and row for vertical/left)
	/// @param SizeHint is the desired size for the widget: can be adjusted to widgets size requirements
	/// @return true if the group was found (incesion succeed)
	/// @throws if the widget cold not be inserted (e.g. invalid values)
	virtual bool addItem(const Id& menuId, const QTopMenuGridGroup::Id& groupId,
	    std::shared_ptr<QTopMenuWidget> widget, size_t column, const QSizeF& sizeHint );

	/// Retrieve the configuration of widgets in that group
	/// @return the list of all widgets and their column/heightPos. Empty if the tab/group does not exists.
	virtual const std::vector<QTopMenuGridGroup::GroupItemInfo> groupItemsInfo(
	    const Id& menuId, const QTopMenuGridGroup::Id& groupId) const;

	/// Following the same logic than itemAt, remove the widget from the group
	/// If the column is empty, it is removed.
	/// @param menuId: the id of the tab
	/// @param groupId: the id of the group in that tab
	/// @param column is in the direction of the grid, heightPos is transversal to that direction:
	///    - horizontal and vertical for Direction::Horizontal
	///    - vertical and horizontal for Direction::Vertical
	/// @param heightPos: is the position of the widget in that column
	/// Warning: if no space is available, widgets may appear visual on a temporary/different column.
	///     than the one in which it really belong
	/// @param true if the widget was found and removed
	virtual bool removeItem( const Id& menuId, const QTopMenuGridGroup::Id& groupId,
	    size_t column, size_t heightPos);

	//*//////////// OTHERS //////////////
	/// See Qt sizeHint
	QSize sizeHint() const override;
	
protected:
	void resizeEvent(QResizeEvent * event) override;
	void paintEvent(QPaintEvent* e) override;

	/// Set/update the minimum/maximum size
	virtual void updateMinMaxSizes();
	/// Update the position of all QTopMenuGrid widgets and general QTopMenuGridGroup.
	virtual void recalculateGridsGeometry();
	/// Set the order of focus (tab order) between sub-widgets
	virtual void updateFocusOrder();
	
	std::unordered_map<Id, QTopMenuGrid> m_tabs; // Assume all Ids are there and valid.
	std::vector<Id> m_tabOrder;

	QTopMenuGridGroup m_genericGroup;
	bool m_showGenericGroup=true;

	QTopMenuTab m_tabWidget;
	
	///@brief In which side of the window the tab is shown
	DisplaySide m_direction = DisplaySide::Top;

	///@brief indicate that grids position and sizes need to be recomputed
	bool m_needRecalculateGridsGeometry = true;

	///@brief update the minimum and maximum size of this widget
	bool m_needUpdateMinMaxSizes = true;

	///@brief cache sizeHint value
	QSize m_cachedSizeHint = QSize(0,0);

	///@brief Number of cells perpendicular to the direction; used for general and tab grids
	size_t m_transversalCellNum = 3;
	///@brief Size of one-side of the cell (square); used for general and tab grids
	qreal m_cellSize = 25.0;
};
}

#endif //QTOPMENU_HPP
