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

#ifndef QTOPMENUGRIDGROUP_HPP
#define QTOPMENUGRIDGROUP_HPP

#include <memory>
#include <vector>

#include <QStaticText>
#include <QWidget>

#include <QClickManager.hpp>
#include <QSvgIcon.hpp>

#include "QTopMenuGridGroupPopup.hpp"
#include "QTopMenuWidgetTypes.hpp"

namespace Escain
{

class QTopMenuWidget;

/// information for statically drawing QTopMenuGridGroup
struct QTopMenuGridGroupStyleOptions
{
	bool isCollapsed=false;
	bool isHover = false;
	bool isEnabled = true;
	bool isPressed = false;
	bool isFocussed = false;
	QPaletteExt palette;
	const QSvgIcon* icon=nullptr;
	const QSvgPixmapCache* arrow=nullptr;
	const QStaticText* staticText=nullptr;
	QRect widgetRect;
	DisplaySide direction;
	CellInfo cellInfo;
	size_t transversalCellNum;
	bool showDivisionBar;
};

// TODO
/// - Fix: popup may appear partially out of the screen. + ADD UT (text)
/// - Documentation


/// A group represents a set of Widgets with a common goal.
/// When space is missing, all widgets in the group are replaced by a
///     drop-down taking much less space.
/// Usually, the widgets of the group are arranged either horizontally or vertically. In
///     the first case, the vertical space is fixed and widgets are placed in row.
class QTopMenuGridGroup: public QWidget
{
Q_OBJECT
	/// Save each widgets and related data in a grid group.
	class Item
	{
	public:
		Item (std::weak_ptr<QTopMenuWidget> but, const QSizeF& sizeHint, const CellInfo& cellInfo);
		std::weak_ptr<QTopMenuWidget> widget();
		const std::weak_ptr<QTopMenuWidget> widget() const;
		const QSizeF& originalSizeHint() const;
		const CellInfo& originalCellInfo() const;
	private:
		std::weak_ptr<QTopMenuWidget> m_widget;

		// Due to the size requirements of the m_widget, the desired sizeHint can actually not
		//     be used as it is. But we want to keep that value for later uses.
		QSizeF m_originalSize;
		CellInfo m_originalCellInfo;
	};

public:

	using Id = std::string;

	struct GroupItemInfo
	{
		size_t column=0; // aligment position (column for Top/Horizontal direction, row otherwise)
		size_t heightPos=0; // transversal position (row for top/horizontal direction, column otherwise)
		std::string nameId; // nameId of the widget at that position.(may be empty)
	};

	explicit QTopMenuGridGroup( QWidget* parent=nullptr);
	virtual ~QTopMenuGridGroup();

	/// Identifier to manage groups
	const Id& id() const;
	virtual void id( const Id& );

	/// Label
	const std::string& label() const;
	virtual void label( const std::string_view& label );

	///Direction: Set if the widget are arranged horizontally or vertically
	DisplaySide direction() const;
	virtual void direction( const DisplaySide d );

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

	/// collapsed or normal
	bool collapsed() const;
	virtual void collapsed( bool col );

	/// Show/hide a division bar after the grid-group
	bool divisionBar() const;
	virtual void divisionBar( bool show );

	/// Add a widget at a given position (column index for Horizontal, row index for Vertical)
	/// @param widget: the widget to be added
	/// @param column: where to add the widget (column for horizontal/top, and row for vertical/left)
	/// @param newColumn: if true, a new column is inserted instead of using the existing one.
	/// @param heightPos: where to insert the widget, between 0 and the number of existing widgets on
	///                   that row
	/// @param SizeHint is the desired size for the widget: can be adjusted to widgets size requirements
	/// @throws if the widget cold not be inserted (e.g. invalid values)
	virtual void addItem( std::shared_ptr<QTopMenuWidget> widget, size_t column, bool newColumn,
	    size_t heightPos, const QSizeF& sizeHint );
	/// Add a widget at a given position (column index for Horizontal, row index for Vertical)
	/// The widget is added at the last position on the given column. The column is only
	///     added if it happens to be the very next one to the last existing.
	/// @param widget: the widget to be added
	/// @param column: where to add the widget (column for horizontal/top, and row for vertical/left)
	/// @param SizeHint is the desired size for the widget: can be adjusted to widgets size requirements
	/// @throws if the widget cold not be inserted (e.g. invalid values)
	virtual void addItem( std::shared_ptr<QTopMenuWidget> widget, size_t column, const QSizeF& sizeHint );

	/// Retrieve the configuration of widgets in that group
	/// @return the list of all widgets and their column/heightPos. Empty if the tab/group does not exists.
	virtual const std::vector<GroupItemInfo> itemsInfo() const;

	/// Following the same logic than itemAt, remove the widget from the group
	/// If the column is empty, it is removed.
	/// returns true if the widget was removed.
	virtual bool removeItem( size_t column, size_t heightPos);

	/// return the upper cell-aligned size of an arbitrary size
	static qreal upperBound( qreal cellSize, qreal margin, qreal size );
	/// return the number of cells occupied by a size
	static size_t overlappedCells( qreal cellSize, qreal margin, qreal size );
	/// return the size of a certain number of cells
	static qreal sizeForCells( qreal cellSize, qreal margin, size_t cellNum );

	/// Set the collapsed icon
	virtual void icon( const QSvgIcon& ico );

	/// See Qt sizeHint
	QSize sizeHint() const override;

	/// overridable call to the static equivalent
	virtual void drawControl( const QTopMenuGridGroupStyleOptions& opt, QPainter& p) const { staticDrawControl(opt, p); }
	/// Draw a QTopMenuGridGroup widget. Static so it can be reused externally or overrided.
	static void staticDrawControl( const QTopMenuGridGroupStyleOptions&, QPainter& p);

	/// Return the required size of the widget when collapsed.
	virtual QSize collapsedSize() const;
	/// Return the required size of the widget when uncollapsed
	virtual QSize uncollapsedSize() const;

	QWidget* setTabulationOrder( QWidget* first);
signals:
	void updateGeometryEvent();
protected:
	/// Set the widget to be recalculated for sub-widgets position
	virtual void triggerRepositionWidgets() { m_needRepositionWidgets = true; }
	/// Set the widget to be recalculated for sub-widgets position AND size
	virtual void triggerResizeWidgets() { triggerRepositionWidgets(); m_needResizeWidgets = true; }

	/// Override of qt events
	void paintEvent(QPaintEvent* e) override;
	void resizeEvent(QResizeEvent*) override;
	bool event(QEvent* e) override;
	void keyPressEvent(QKeyEvent *e) override;

	/// Show/Hide the popup when in collapsed mode
	virtual void togglePopup();

	/// Initialize the information structure for statically drawing (allowing to re-use the drawing)
	virtual void initStyleOption(QTopMenuGridGroupStyleOptions&) const;

	/// Setup the font to the proper size, weight, etc...
	virtual void setupFontForLabel( QFont& f) const { staticSetupFontForLabel(f); }
	static void staticSetupFontForLabel( QFont& );

	/// Called by widgets to hide the pop-up.
	virtual void fadePopup();

	/// Reposition all the widgets of the group, accordingly to current properties
	virtual void repositionSubWidgets();

	/// Return the rect required for the collapsed icon
	static QRect collapsedIconRect(const QRect& widgetRect, const CellInfo& cellInfo,
	    const qreal divSpace, DisplaySide dir);
	virtual QRect collapsedIconRect() const;

	/// Returns the rect where to draw the Group Label. In vertical mode, the rect
	///     expects the QPainter to be rotated CW 90º (the rect is rotated CCW 90º)
	///     expects the staticText to be prepared with appropiate text and size.
	static QRect textRect(const QRect& widgetRect, qreal margin, qreal divSpace,
	    const QStaticText& staticText, DisplaySide dir);
	virtual QRect textRect() const;

	/// Returns the rect where to draw the Group arrow (collapsed mode). In vertical mode, the rect
	///     expects the QPainter to be rotated CW 90º (the rect is rotated CCW 90º)
	static QRect arrowRect(const QRect& widgetRect, qreal margin, qreal divSpace,
	    const QSvgPixmapCache& arrow, DisplaySide dir);
	virtual QRect arrowRect() const;

	/// Update the order of TAB/shift+Tab for all sub-widgets
	virtual void updateFocusOrder();

	/// Convert a coordinate/size to it transposed if the widget is at left side (vertical)
	static QPointF transposeIfVert(DisplaySide dir, const QPointF& p);
	static QSizeF transposeIfVert(DisplaySide dir, const QSizeF& s);

	std::string m_id;
	std::string m_label="Group label";
	std::vector<std::vector<Item>> m_content; // horizontal<vertical<>>
	QTopMenuGridGroupPopup m_frame;

	size_t m_transversalCellNum = 3;              // Number of cells perpendicular to the direction
	qreal m_cellSize = 25.0;                      // Size of one-side of the cell (square)
	qreal m_margin = 2.0;                         // margin between cells
	bool m_showDivisionBar = false;
	DisplaySide m_direction = DisplaySide::Top;// direction of the grid (Horizontal, Vertical)
	bool m_needRepositionWidgets = true; // If the grid needs to re-compute widgets position before paint.
	bool m_needResizeWidgets = true; // If the grid needs to re-compute the size of widgets.
	bool m_cacheHovered = false; // Save if the widget is hovered (for collapsed)

	QSvgIcon m_icon;
	QSvgPixmapCache m_arrow;
	QClickManager m_clickManager;
	bool m_isCollapsed = false;
	bool m_clickPressed = false;

	QStaticText m_staticText;


	constexpr static qreal borderWidth = 1.0; // Line width for hover highlight
	constexpr static qreal cornerRadius=0.0;  // Hover highlight corner radius
	constexpr static qreal divisionLineWidth = 2.0; // division bar line width
	constexpr static qreal divisionSpace = 5.0; // reserved space for the division bar
	constexpr static qreal MAX_WIDTH = 250.0; // maximum acceptable widget size
};

}
#endif
