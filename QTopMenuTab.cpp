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

#include "QTopMenuTab.hpp"

#include <optional>

#include <QCoreApplication>	// Get current path for relative paths
#include <QDir>				// Manage relative paths for loading
#include <QPainter>			// Required to render the svg

#include <QPaletteExt.hpp>

using namespace Escain;

QTopMenuTab::QTopMenuTab( QWidget* parent )
	: QWidget(parent)
	, m_underlineAnimator(this)
{
	m_clickManager.enableHover(*this);

	connect( &m_clickManager, &QClickManager::hovered, this, [this]
	(const QPointF&, bool hovered, const size_t clickId, const QRectF& rect)
	{
		for (auto& [id, tab]: m_tabs)
		{
			if (tab.m_clickableRectangleId == clickId)
			{
				tab.m_hovered = hovered;
			}
		}
		update(rect.toRect());
	});

	connect( &m_clickManager, &QClickManager::pressed, this, [this]
	(const QPointF&, bool pressed, const std::unordered_set<size_t>& clickableRectangleIds)
	{
		size_t clickId = 0;
		if (pressed)
		{
			if (clickableRectangleIds.size() != 1)
			{
				assert(false);
				throw std::runtime_error("Pressed on QTabMenu tab without/too_many tab ID");
			}
			clickId = *clickableRectangleIds.begin();
		}

		for (auto& [id, tab]: m_tabs)
		{
			if (pressed && tab.m_clickableRectangleId == clickId)
			{
				tab.m_pressed = pressed;
				update(tab.tabRect().toRect());
			}
			else if (tab.m_pressed)
			{
				tab.m_pressed = false;
				update(tab.tabRect().toRect());
			}
		}
	});


	connect( &m_clickManager, &QClickManager::clicked, this, [this]
	(const QPointF& , const std::unordered_set<size_t>& clickableRectangleIds)
	{
		if (isEnabled())
		{
			if (clickableRectangleIds.size() != 1)
			{
				assert(false);
				throw std::runtime_error("Click to QTabMenu without/too_many tab ID");
			}
			size_t clickId = *clickableRectangleIds.begin();

			for (auto& [id, tab]: m_tabs)
			{
				if (tab.m_clickableRectangleId==clickId)
				{
					switchToTabById(id);
					break;
				}
			}
		}
	});

	m_underlineAnimator.setEasingCurve(QEasingCurve::OutCubic);
	connect( &m_underlineAnimator, &QVariantAnimation::valueChanged, this, [this](const QVariant& val)
	{
		QRect toUpdate = m_underlineAnimated.united(val.toRect());
		m_underlineAnimated = val.toRect();
		update(toUpdate);
	});

	setMouseTracking(true);
}

bool QTopMenuTab::tabShortcut( const Id& id, const QKeySequence& seq)
{
	auto tabIt = m_tabs.find(id);
	if (tabIt != m_tabs.cend())
	{
		if (!tabIt->second.m_shortcut)
		{
			tabIt->second.m_shortcut = std::make_unique<QShortcut>(this);
			connect( tabIt->second.m_shortcut.get(), &QShortcut::activated, this, [this, id]()
			{
				switchToTabById(id);
			});
		}

		tabIt->second.m_shortcut->setKey(seq);
		return true;
	}
	return false;
}

const QKeySequence QTopMenuTab::tabShortcut( const Id& id ) const
{
	auto tabIt = m_tabs.find(id);
	if (tabIt != m_tabs.cend() && tabIt->second.m_shortcut)
	{
		return tabIt->second.m_shortcut->key();
	}
	return {};
}

const std::optional<std::string> QTopMenuTab::tabLabel( const Id& menuId ) const
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt != m_tabs.cend())
	{
		return {tabIt->second.m_label.toStdString()};
	}
	return std::nullopt;
}

bool QTopMenuTab::tabLabel( const Id& menuId, const std::string& label)
{
	auto tabIt = m_tabs.find(menuId);
	if (tabIt != m_tabs.cend())
	{
		QString newLabel = QString::fromStdString(label);
		if (newLabel != tabIt->second.m_label)
		{
			tabIt->second.m_label = newLabel;
			m_underlineAnimated = QRect();
			m_needUpdateTabLabelSizes = true;
			m_needUpdateTabLabelPos = true;
			m_needUpdateMinMaxSizes = true;
			update();
		}
		return true;
	}
	return false;
}

qreal QTopMenuTab::tabMargin() const
{
	return m_margin;
}

void QTopMenuTab::tabMargin( qreal margin )
{
	if (m_margin != margin)
	{
		m_margin = margin;
		m_underlineAnimated = QRect();
		m_needUpdateTabLabelSizes = true;
		m_needUpdateTabLabelPos = true;
		m_needUpdateMinMaxSizes = true;
		update();
	}
}

qreal QTopMenuTab::tabMinSizePercentil() const
{
	return m_tabSizePercentil;
}

void QTopMenuTab::tabMinSizePercentil( qreal percentil )
{
	if (m_tabSizePercentil != percentil)
	{
		m_tabSizePercentil = percentil;
		m_underlineAnimated = QRect();
		m_needUpdateTabLabelSizes = true;
		m_needUpdateTabLabelPos = true;
		m_needUpdateMinMaxSizes = true;
		update();
	}
}

void QTopMenuTab::updateMinMaxSizes()
{
	bool isAtTop = direction()==DisplaySide::Top;

	auto transpIfVert = [isAtTop]( const QSize& orig) -> QSize
	{
		return isAtTop ? orig : orig.transposed();
	};

	// Working with transposed size (as it was all Horizontal/TopSide)

	// All Tab labels
	if (m_needUpdateTabLabelSizes)
	{
		updateTabLabelSizes(); // needed to compute label required space.
	}
	QSize labelCoSumT(0,0);
	for (auto& [id, tab]: m_tabs)
	{

		const auto& labelCoSizeT = transpIfVert(tab.tabRect().size().toSize());

		labelCoSumT = QSize(labelCoSumT.width()+labelCoSizeT.width(),
			std::max(labelCoSumT.height(), labelCoSizeT.height()));
	}

	const auto minSize = transpIfVert(labelCoSumT);
	m_cachedSizeHint = minSize;

	if (minSize != minimumSize())
	{
		setMinimumSize( minSize );
		setMaximumSize( minSize );
		updateGeometry();
		emit updateGeometryEvent();
	}

	m_needUpdateMinMaxSizes = false;
}

void QTopMenuTab::updateTabLabelSizes()
{
	const bool isAtTop = (DisplaySide::Top==m_direction);
	auto transpIfVert = [isAtTop]( const QSizeF& orig) -> QSizeF
	{
		return isAtTop ? orig : orig.transposed();
	};

	// Get the list of sizes for each tab text
	std::vector<qreal> tabWidths(m_tabOrder.size());
	for ( size_t i=0; i< m_tabOrder.size(); ++i)
	{
		Id id= m_tabOrder[i];
		TopMenuTabItem& tab = m_tabs[id];

		QFont font;
		setupFontForLabel(font);
		QFontMetrics metric(font);
		tab.m_cacheLabelWidth = metric.horizontalAdvance(tab.m_label);
		tabWidths[i]=tab.m_cacheLabelWidth;
	}

	// Calculate the percentil given by mTabSizePercentil
	qreal minTabSize = 0.0;

	if (!tabWidths.empty())
	{
		assert(m_tabSizePercentil >= 0.0 && m_tabSizePercentil <= 1.0);
		std::sort(tabWidths.begin(), tabWidths.end());
		qreal pos = m_tabSizePercentil*(tabWidths.size()-1.0);
		size_t posInt = static_cast<size_t>(pos);
		qreal prop = pos-static_cast<qreal>(posInt);
		minTabSize = tabWidths[posInt];
		// Ideally, posInt should never be the last element, but to allows 100% percentil, we check it.
		if (prop>0.0)
		{
			minTabSize += prop*(tabWidths[posInt+1]-tabWidths[posInt]);
		}
	}

	// Set the tab rect size
	for ( size_t i=0; i< m_tabOrder.size(); i++)
	{
		Id id= m_tabOrder[i];
		TopMenuTabItem& tab = m_tabs[id];

		qreal thisWidth = std::max(minTabSize, tab.m_cacheLabelWidth)+2.0*m_margin;

		QRectF r = tab.tabRect();
		r.setSize(transpIfVert(QSizeF(thisWidth, m_tabHeight)));

		tab.tabRect(r);
	}

	m_needUpdateTabLabelSizes = false;
}

void QTopMenuTab::updateTabLabelPos()
{
	const bool isAtTop = (DisplaySide::Top==m_direction);

	auto transpIfVert = [isAtTop]( const auto& orig)
	{
		return isAtTop ? orig : orig.transposed();
	};

	// Get the space available for tabs
	const qreal globalWidth = 0.0;

	// Make sure the label sizes are already computed
	if (m_needUpdateTabLabelSizes)
	{
		updateTabLabelSizes();
	}

	// Set the tab rect size
	qreal sum=globalWidth;
	for ( size_t i=0; i< m_tabOrder.size(); i++)
	{
		Id id= m_tabOrder[i];
		TopMenuTabItem& tab = m_tabs[id];

		const auto thisSize = tab.tabRect().size();
		qreal thisWidth = transpIfVert(thisSize).width();

		QRectF r = tab.tabRect();
		r.moveTopLeft(transpIfVert(QPointF(sum, 0.0)));
		tab.tabRect(r);

		sum += thisWidth;

		m_clickManager.clickableRectangleById( tab.m_clickableRectangleId, tab.tabRect().toRect());
	}
	m_needUpdateTabLabelPos = false;
}

void QTopMenuTab::switchToTabById( const Id& idToSelect)
{
	const auto prevId = m_selectedTab;
	auto currentlySelected = m_tabs.find(idToSelect);
	if (currentlySelected != m_tabs.cend())
	{
		if (idToSelect != m_selectedTab)
		{
			m_selectedTab = idToSelect;

			QRect selectedRect = transposeIfVert(m_direction, currentlySelected->second.tabRect().toRect()).toRect();
			selectedRect.setTop(UNDERLINE_HEIGHT);
			selectedRect.setBottom(UNDERLINE_HEIGHT+UNDERLINE_WIDTH);
			selectedRect.setLeft(selectedRect.left()+m_margin);
			selectedRect.setRight(selectedRect.right()-m_margin);
			if (m_underlineAnimated.isValid())
			{
				m_underlineAnimator.stop();
				m_underlineAnimator.setDuration(UNDERLINE_ANIMATION_TIME_S*1000.0);
				m_underlineAnimator.setStartValue(m_underlineAnimated);
				m_underlineAnimator.setEndValue(transposeIfVert(m_direction, selectedRect).toRect());
				m_underlineAnimator.start();
			}

			emit tabChanged(prevId, m_selectedTab);
			const auto widgetSizeT = transposeIfVert(m_direction, size());
			const auto updateSizeT = QSize(widgetSizeT.width(), m_tabHeight);
			update(QRect(QPoint(0,0), transposeIfVert(m_direction, updateSizeT).toSize()));
		}
	}
	else
	{
		assert(false);
		throw std::runtime_error("Switching to an non-existing tab Id");
	}
}

const QTopMenuTab::Id& QTopMenuTab::selectedId() const
{
	return m_selectedTab;
}

void QTopMenuTab::resizeEvent(QResizeEvent *)
{
	m_needUpdateMinMaxSizes = true;
}


void QTopMenuTab::paintEvent(QPaintEvent* e)
{
	if (m_needUpdateTabLabelSizes)
	{
		updateTabLabelSizes();
	}

	if (m_needUpdateTabLabelPos)
	{
		updateTabLabelPos();
	}

	if (m_needUpdateMinMaxSizes)
	{
		updateMinMaxSizes();
	}

	QWidget::paintEvent(e);
	QPainter p(this);
	p.setClipRect(e->rect());
	p.setRenderHint(QPainter::Antialiasing );
	QPaletteExt pal = QWidget::palette();

	QFont font = p.font();
	setupFontForLabel(font);
	p.setFont(font);

	// Paint tabs
	QRect selectedRect;
	for (size_t i=0; i< m_tabOrder.size(); i++)
	{
		Id id = m_tabOrder[i];
		TopMenuTabItem& tab = m_tabs.at(id);

		paintTab( p, tab, tab.m_hovered, tab.m_pressed, id == m_selectedTab);

		if (id == m_selectedTab)// And get selected tab rect at the same time
		{
			selectedRect = tab.tabRect().toRect();
		}
	}

	ColorRoleExt underlineRole = ColorRoleExt::Highlight_Normal;
	QColor underlineColor = pal.color(underlineRole);

	if (!m_underlineAnimated.isValid()) // If underline is empty (e.g. first drawing)
	{
		QRect selectedRect;
		const auto selIt = m_tabs.find(m_selectedTab);
		assert(selIt != m_tabs.cend());

		selectedRect = transposeIfVert(m_direction, selIt->second.tabRect()).toRect();

		selectedRect.setTop(UNDERLINE_HEIGHT);
		selectedRect.setBottom(UNDERLINE_HEIGHT+UNDERLINE_WIDTH);
		selectedRect.setLeft(selectedRect.left()+m_margin);
		selectedRect.setRight(selectedRect.right()-m_margin);
		m_underlineAnimated = transposeIfVert(m_direction, selectedRect).toRect();
	}

	p.fillRect(m_underlineAnimated, underlineColor);
}

void QTopMenuTab::paintTab( QPainter& p, TopMenuTabItem& tab, bool hover, bool press, bool selected )
{
	p.save();
	QPaletteExt pal= QWidget::palette();

	QRectF tabRectR;
	if (m_direction==DisplaySide::Left)
	{
		tabRectR = QRectF(0.0, 0.0, tab.m_cacheTabTextRect.height(), tab.m_cacheTabTextRect.width());

		p.translate(tab.m_cacheTabTextRect.left(), tab.m_cacheTabTextRect.top()+tab.m_cacheTabTextRect.height());
		p.rotate(-90.0);
	}
	else
	{
		tabRectR = tab.m_cacheTabTextRect;
	}

	ColorRoleExt role = selected ? ColorRoleExt::Background_Normal : ColorRoleExt::BackgroundMid_Normal;
	ColorRoleExt roleText = ColorRoleExt::TextOverBackground_Normal;
	p.setPen(Qt::NoPen);

	if (isEnabled() && !selected)
	{
		if (press)
		{
			role = ColorRoleExt::Background_Pressed;
			roleText = ColorRoleExt::TextOverBackground_Pressed;
		}
		else if (hover)
		{
			role = ColorRoleExt::Background_Hover;
			roleText = ColorRoleExt::TextOverBackground_Hover;
		}
	}
	QColor bgColor = pal.color(role);
	p.setBrush(bgColor);
	p.drawRoundedRect(tabRectR, 0, 0);

	p.setBrush(bgColor);
	if (isEnabled())
	{
		p.drawRoundedRect(tabRectR, 0, 0);
	}

	QColor textColor = pal.color(roleText);
	p.setPen(textColor);

	tabRectR.moveTop(tabRectR.top()+4);
	paintText( p, tab, tabRectR);

	p.restore();
}

void QTopMenuTab::paintText( QPainter& p, const TopMenuTabItem& tab, const QRectF& rotatedRect)
{
	p.drawText(rotatedRect, Qt::TextShowMnemonic | Qt::AlignHCenter | Qt::AlignBaseline, tab.m_label);
}

void QTopMenuTab::direction(DisplaySide dir)
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

		m_underlineAnimated = QRect(); // invalidate underline rect, so it is reset.

		m_needUpdateTabLabelSizes = true;
		m_needUpdateTabLabelPos = true;
		m_needUpdateMinMaxSizes = true;
		update();
	}
}

DisplaySide QTopMenuTab::direction() const
{
	return m_direction;
}

bool QTopMenuTab::insertTab(const Id& id, const QString& name, int pos)
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
	auto [newIt, insertedBool] = m_tabs.emplace(id, TopMenuTabItem{});
	auto& newTabObj = newIt->second;

	//QFont font;
	//setupFontForLabel(font);

	//QTextOption textOption;
	//textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	//textOption.setAlignment(Qt::AlignCenter | Qt::AlignBaseline);
	//newTabObj.m_labelStaticText.setTextFormat(Qt::TextFormat::AutoText);
	//newTabObj.m_labelStaticText.setTextOption(textOption);

	//newTabObj.m_labelStaticText.setText(name);
	newTabObj.m_label = name;
	//newTabObj.m_labelStaticText.prepare(QTransform(), font);
	m_tabOrder.insert(m_tabOrder.begin()+checkedPos,id);

	if (1==m_tabs.size())
	{
		switchToTabById(id);
	}

	// Manage clickable zones for hover/pressed/click
	static size_t lastId=0;
	newTabObj.m_clickableRectangleId = ++lastId;
	m_clickManager.addClickableRectangle(newTabObj.m_clickableRectangleId, QRect());

	m_needUpdateTabLabelSizes = true;
	m_needUpdateTabLabelPos = true;
	m_needUpdateMinMaxSizes = true;
	update();

	return true;
}

bool QTopMenuTab::removeTab(const Id& id)
{
	// Avoid tabs with id empty
	if (id.empty())
	{
		return false;
	}

	auto it = m_tabs.find(id);
	if (it == m_tabs.end())
	{
		return false;
	}

	bool needSetSelected = (m_selectedTab == id);
	m_clickManager.removeClickableRectangle(it->second.m_clickableRectangleId);
	m_tabs.erase(it);
	auto orderIt = std::find(m_tabOrder.begin(), m_tabOrder.end(), id);
	if (orderIt != m_tabOrder.cend())
	{
		m_tabOrder.erase(orderIt);
	}

	if (needSetSelected && !m_tabOrder.empty())
	{
		switchToTabById(*m_tabOrder.begin());
	}
	else if (needSetSelected)
	{
		m_selectedTab.clear();
	}

	m_needUpdateTabLabelSizes = true;
	m_needUpdateTabLabelPos = true;
	m_needUpdateMinMaxSizes = true;
	update();

	return true;
}

void QTopMenuTab::setupFontForLabel( QFont& f) const
{
	f.setWeight(QFont::DemiBold);
	f.setPointSizeF(f.pointSizeF());
}

bool QTopMenuTab::event(QEvent* e)
{
	auto ret = m_clickManager.eventHandler( e );
	if (ret)
	{
		return true;
	}

	return QWidget::event(e);
}

QSize QTopMenuTab::sizeHint() const
{
	if (m_needUpdateMinMaxSizes)
	{
		// Cache needs to be updated, but sizeHint is still const
		const_cast<QTopMenuTab*>(this)->updateMinMaxSizes();
	}

	return m_cachedSizeHint;

}

QPointF QTopMenuTab::transposeIfVert(DisplaySide dir, const QPointF& p)
{
	const bool isAtTop = (DisplaySide::Top==dir);
	return isAtTop ? p : p.transposed();
}

QSizeF QTopMenuTab::transposeIfVert(DisplaySide dir, const QSizeF& s)
{
	const bool isAtTop = (DisplaySide::Top==dir);
	return isAtTop ? s : s.transposed();
}

QRectF QTopMenuTab::transposeIfVert(DisplaySide dir, const QRectF& s)
{
	const bool isAtTop = (DisplaySide::Top==dir);
	return isAtTop ? s : QRectF(s.topLeft().transposed(), s.size().transposed());
}

