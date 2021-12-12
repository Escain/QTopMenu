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

#ifndef QTOPMENUTAB_HPP
#define QTOPMENUTAB_HPP

#include <string>
#include <unordered_map>

#include <QShortcut>
#include <QStaticText>
#include <QVariantAnimation>
#include <QWidget>

#include <QClickManager.hpp>
#include "QTopMenuWidgetTypes.hpp"

class QPainter;

namespace Escain
{

/**
 * @brief QTopMenuTab
 *
 * Tab (label) component of the QTopMenu.
 * 
 */
class QTopMenuTab: public QWidget
{
Q_OBJECT
public:

	using Id = std::string;

	explicit QTopMenuTab( QWidget* parent=nullptr );
	virtual ~QTopMenuTab() = default;

	virtual void direction(DisplaySide dir);
	virtual DisplaySide direction() const;
	
	class TopMenuTabItem
	{
		friend QTopMenuTab;
		
		QRectF m_cacheTabTextRect;
		QString m_label; //QStaticText is not able to manage mnemonic, use standard QPainter drawText
		qreal m_cacheLabelWidth; //cache value for the text width from mStaticText
		size_t m_clickableRectangleId;
		bool m_pressed = false; //Cache if the cursor is pressed on that tab
		bool m_hovered = false; //Cache if the cursor is hovering that tab
		std::unique_ptr<QShortcut> m_shortcut;
	public:
		void tabRect( const QRectF& r) { m_cacheTabTextRect = r; }
		const QRectF& tabRect() const { return m_cacheTabTextRect; }
	};
	
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

	/// Returns the Id of the currently selected tab
	const Id& selectedId() const;

	/// Space around text in tab
	qreal tabMargin() const;
	virtual void tabMargin( qreal margin );

	/// Percentil of the tabs (label) widths, used as minimal tab size:
	///     0%: all tabs takes exactly their label width + tabMargin*2
	///     100%: all tabs adjust to the same size as the largest + tabMargin*2
	qreal tabMinSizePercentil() const;
	virtual void tabMinSizePercentil( qreal percentil);

	/// Set or retrieve the shortcut to trigger a tab. Underline is applied if one of the
	///     characters of the lable match.
	const QKeySequence tabShortcut( const Id& id ) const;
	virtual bool tabShortcut( const Id& id, const QKeySequence& seq);

	/// Label for the given tab
	const std::optional<std::string> tabLabel( const Id& menuId ) const;
	/// Set the label for the given tab
	/// @return true if set properly (false if the menu does not exist)
	virtual bool tabLabel( const Id& menuId, const std::string& label);

	/// See Qt sizeHint
	QSize sizeHint() const override;

signals:
	void tabChanged( const Id& prevSelected, const Id& newSelected);

	void updateGeometryEvent();
	
protected:

	void resizeEvent(QResizeEvent * event) override;
	void paintEvent(QPaintEvent* e) override;
	bool event(QEvent* e) override;

	virtual void paintTab( QPainter& p, TopMenuTabItem& tab, bool hover, bool press, bool selected ) ;
	virtual void paintText( QPainter& p, const TopMenuTabItem& tab, const QRectF& rotatedRect);

	/// Set/update the minimum/maximum size
	virtual void updateMinMaxSizes();
	/// Update cached position of tabs (just the title bar)
	virtual void updateTabLabelPos();
	/// Update cached size of tabs (just the title bar)
	virtual void updateTabLabelSizes();

	/// Setup the font to the proper size, weight, etc...
	virtual void setupFontForLabel( QFont& f) const;

	/// Convert a coordinate/size to it transposed if the widget is at left side (vertical)
	static QPointF transposeIfVert(DisplaySide dir, const QPointF& p);
	static QSizeF transposeIfVert(DisplaySide dir, const QSizeF& s);
	static QRectF transposeIfVert(DisplaySide dir, const QRectF& s);

	std::unordered_map<Id, TopMenuTabItem> m_tabs; // Assume all Ids are there and valid.
	std::vector<Id> m_tabOrder;
	
	///@brief In which side of the window the tab is shown
	DisplaySide m_direction = DisplaySide::Top;
	
	/* The tabs size is calculated in the following way:
	 * Text is used to get a per-tab required size + margin for aesthetic.
	 * The mTabSizePercentil set the percentil of tab sizes to which all smaller tabs is 
	 * adjusted: e.g. if percentil 0, all tabs take custom sizes, if percentil 80, 80%
	 * smaller part of tabs will take the same size, and only 20% will have custom size.*/
	
	///@brief mTabSizePercentil
	qreal m_tabSizePercentil = 0.6;
	
	///@brief relative height of tabs, equivalent to an arbitrary font size/dpi
	qreal m_tabHeight=40.0;
	
	///@brief margin around texts, or half the minimum width for a tab, and between/around widgets
	qreal m_margin=15.0;

	///@brief currently selected tab ID
	Id m_selectedTab = "";

	QClickManager m_clickManager;

	///@brief update the minimum and maximum size of this widget
	bool m_needUpdateMinMaxSizes = true;

	///@brief cache the size of tab labels
	bool m_needUpdateTabLabelSizes = true;

	///@brief cache the position of tab labels
	bool m_needUpdateTabLabelPos = true;

	///@brief cache sizeHint value
	QSize m_cachedSizeHint = QSize(0,0);

	QRect m_underlineAnimated;
	QVariantAnimation m_underlineAnimator;

	static constexpr qreal UNDERLINE_HEIGHT = 30.0;
	static constexpr qreal UNDERLINE_WIDTH = 2.0;
	static constexpr qreal UNDERLINE_ANIMATION_TIME_S = 0.4;
};
}

#endif //QTOPMENUTAB_HPP
