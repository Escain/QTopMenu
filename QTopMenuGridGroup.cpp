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

#include "QTopMenuGridGroup.hpp"

#include <cmath>

#include <QPainter>
#include <QPaintEvent>

#include "QTopMenuWidget.hpp"

using namespace Escain;


//*///////////////////// Item implementation /////////////

QTopMenuGridGroup::Item::Item (std::weak_ptr<QTopMenuWidget> but, const QSizeF& sizeHint, const CellInfo& cellInfo)
: m_widget(but)
, m_originalSize(sizeHint)
, m_originalCellInfo(cellInfo)
{
}

std::weak_ptr<QTopMenuWidget> QTopMenuGridGroup::Item::widget()
{
	return m_widget;
}

const std::weak_ptr<QTopMenuWidget> QTopMenuGridGroup::Item::widget() const
{
	return m_widget;
}

const QSizeF& QTopMenuGridGroup::Item::originalSizeHint() const
{
	return m_originalSize;
}

const CellInfo& QTopMenuGridGroup::Item::originalCellInfo() const
{
	return m_originalCellInfo;
}


//*/////////////// QTopMenuGridGroup implementation //////////////

QTopMenuGridGroup::QTopMenuGridGroup(  QWidget* parent )
: QWidget(parent)
, m_frame(this)
{
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFocusPolicy( Qt::StrongFocus);
	m_frame.setFocusPolicy( Qt::StrongFocus);

	m_isCollapsed=true;
	collapsed(false); // Ensure it is different, so it applies the changes.

	QTextOption textOption;
	textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	textOption.setAlignment(Qt::AlignCenter | Qt::AlignBaseline);
	m_staticText.setTextFormat(Qt::TextFormat::PlainText);
	m_staticText.setTextOption(textOption);
	const auto prevLabel = m_label;
	m_label= prevLabel + "-";
	label(prevLabel); // Trick to make sure the new label is different and setup with initial value

	//Label is lazily created, force an initial m_staticText correct size for setting minimumSize/maximumSize
	QFont font;
	setupFontForLabel(font);
	m_staticText.setText(QString::fromStdString(m_label));
	m_staticText.prepare(QTransform(), font);

	m_icon.margin(m_margin);

	QByteArray arrowSvg =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20.0pt\" height=\"10.0pt\" "
		"viewBox=\"0 0 20.0 10.0\" version=\"1.1\">"
		"    <g transform=\"translate(0.0,0.0)\">"
		"        <path style=\"fill:#000000;stroke:none;\" d=\"m 5.0,2.5 10.0,0.0 -5.0,5.0 z\"/>"
		"    </g>"
		"</svg>";
	m_arrow = QSvgPixmapCache(arrowSvg);

	connect(&m_clickManager, &QClickManager::hovered,
	[this](const QPoint&, bool hovered, const size_t, const QRect&)
	{
		m_cacheHovered = hovered;
	});

	connect(&m_clickManager, &QClickManager::clicked,
	[this](const QPointF&, const std::unordered_set<size_t>&)
	{
		togglePopup();
	});

	connect(&m_frame, &QTopMenuGridGroupPopup::fade, this, [this]()
	{
		fadePopup();
	});

	connect(&m_clickManager, &QClickManager::pressed,
	[this](const QPointF&, bool pressed)
	{
		m_clickPressed=pressed;
		update();
	});

	triggerResizeWidgets();
}

QTopMenuGridGroup::~QTopMenuGridGroup()
{
	for (auto& itemList: m_content)
	{
		for (auto& item: itemList)
		{
			auto widgetLocked = item.widget().lock();
			if(widgetLocked)
			{
				widgetLocked->setParent(nullptr);
			}
		}
	}
}

void QTopMenuGridGroup::togglePopup()
{
	if (collapsed() && isEnabled())
	{
		m_frame.setVisible(!m_frame.isVisible());
		if (m_direction==Escain::DisplaySide::Top)
		{
			m_frame.move(this->mapToGlobal(QPoint((width()-m_frame.width())/2,height())));
		}
		else
		{
			m_frame.move(this->mapToGlobal(QPoint(width(),(height()-m_frame.height())/2)));
		}

		// Ensure the focus is in
		if (nullptr == m_frame.focusWidget())
		{
			if (!m_content.empty() && !m_content[0].empty())
			{
				const auto& item = m_content[0][0];
				const auto& lockedWidget = item.widget().lock();
				if (lockedWidget)
				{
					lockedWidget->setFocus();
				}
				else
				{
					assert(false);
					throw std::runtime_error("QWidget in QTopMenuWidgetGrid became invalid");
				}
			}
		}
	}
}

const QTopMenuGridGroup::Id& QTopMenuGridGroup::id() const
{
	return m_id;
}

void QTopMenuGridGroup::id(const Id& id )
{
	m_id = id;
}

const std::string& QTopMenuGridGroup::label() const
{
	return m_label;
}

void QTopMenuGridGroup::label( const std::string_view& label )
{
	if (m_label != label)
	{
		m_label = label;
		m_frame.label(label);
		updateGeometry();
		triggerResizeWidgets();
		update();
	}
}

void QTopMenuGridGroup::icon( const QSvgIcon& ico )
{
	m_icon = ico;
	m_icon.margin(m_margin);
}

DisplaySide QTopMenuGridGroup::direction() const
{
	return m_direction;
}

void QTopMenuGridGroup::direction( const DisplaySide d )
{
	if (m_direction != d)
	{
		m_direction = d;
		m_frame.direction(d);
		triggerResizeWidgets();
		update();
	}
}

size_t QTopMenuGridGroup::transversalCellNum() const
{
	return m_transversalCellNum;
}

void QTopMenuGridGroup::transversalCellNum( size_t cellNum )
{
	if (m_transversalCellNum != cellNum)
	{
		m_transversalCellNum = cellNum;
		triggerResizeWidgets();
		updateGeometry();
		update();
	}
}

qreal QTopMenuGridGroup::cellSize() const
{
	return m_cellSize;
}

void QTopMenuGridGroup::cellSize( qreal cellSize )
{
	if (m_cellSize != cellSize)
	{
		m_cellSize = cellSize;
		triggerResizeWidgets();
		updateGeometry();
		update();
	}
}

qreal QTopMenuGridGroup::margin() const
{
	return m_margin;
}

void QTopMenuGridGroup::margin( qreal margin )
{
	if (m_margin != margin)
	{
		m_margin = margin;
		m_frame.margin(margin);
		m_icon.margin(m_margin);
		triggerResizeWidgets();
		updateGeometry();
		update();
	}
}

bool QTopMenuGridGroup::collapsed() const
{
	return m_isCollapsed;
}

void QTopMenuGridGroup::collapsed( bool col )
{
	if (m_isCollapsed != col)
	{
		m_isCollapsed = col;

		if (m_isCollapsed)
		{
			m_frame.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
			m_frame.setVisible(false);

			m_clickManager.enableHover(*this);
			setMouseTracking(true);
		}
		else
		{
			m_frame.setWindowFlags(Qt::Widget);
			m_frame.setVisible(true);
			m_frame.move(0,0);

			m_clickManager.disableHover();
			setMouseTracking(false);
		}

		updateGeometry();
		update();
		triggerResizeWidgets();
	}
}

//For debugging tab order. TODO remove when fixed
/*void showOrder(QWidget* s)
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
}

QWidget* QTopMenuGridGroup::setTabulationOrder( QWidget* first)
{
	QWidget* prev=first;
	if (!m_isCollapsed)
	{
		if (first)
		{
			setTabOrder(first, this);
		}
		prev = this;

		for (const auto& butList: m_content)
		{
			for (const auto& but: butList)
			{
				const auto lockedWidget = but.widget().lock();
				if (lockedWidget)
				{
					setTabOrder(prev, lockedWidget.get());
					prev = lockedWidget.get();
				}
			}
		}
	}
	return prev;
}*/

bool QTopMenuGridGroup::divisionBar() const
{
	return m_showDivisionBar;
}

void QTopMenuGridGroup::divisionBar( bool show )
{
	if (show != m_showDivisionBar)
	{
		m_showDivisionBar = show;

		updateGeometry();
		update();
		triggerResizeWidgets();
	}
}

QSize QTopMenuGridGroup::collapsedSize() const
{
	auto maxSize = sizeForCells(m_cellSize, m_margin, m_transversalCellNum);

	const qreal widthT = std::max(maxSize, std::min(m_staticText.size().width(), maxSize*2.0)) +
	    2*m_margin +  (m_showDivisionBar ? divisionSpace : 0.0);
	const qreal heightT = maxSize + 3*m_margin + 2.0*m_staticText.size().height();

	return transposeIfVert(m_direction, QSizeF(widthT, heightT)).toSize();
}

QSize QTopMenuGridGroup::uncollapsedSize() const
{
	if (m_needRepositionWidgets || m_needResizeWidgets)
	{
		// Even if we need to update caches to get the proper value, this function is
		// a getter and we don't want it to be non-const.
		const_cast<QTopMenuGridGroup*>(this)->repositionSubWidgets();
	}

	const auto divWidth = (m_showDivisionBar ? divisionSpace : 0.0);
	return m_frame.size() + transposeIfVert(m_direction, QSizeF(divWidth, 0.0)).toSize();
}

QSize QTopMenuGridGroup::sizeHint() const
{
	QSize result;

	if (collapsed())
	{
		result = collapsedSize();
	}
	else
	{
		result = m_frame.size();
	}
	return result;
}

void QTopMenuGridGroup::addItem( std::shared_ptr<QTopMenuWidget> widget, size_t column,
    bool newColumn, size_t heightPos, const QSizeF& sizeHint )
{
	// Sanity check...I believe you did not on purpose
	if (!widget)
	{
		assert(false);
		throw std::runtime_error("Trying to insert nullptr widget in a QTopMenuWigetGrid");
	}

	// Sanity check: don't insert past the last position
	if (m_content.size() < column || (m_content.size()==column && !newColumn) )
	{
		assert(false);
		throw std::out_of_range("Trying to insert widget in a QTopMenuWidgetGrid at invalid column");
	}

	// create the new "column" (row for Vertical)
	if (newColumn)
	{
		m_content.emplace(m_content.begin()+column);
	}

	auto& itemList = m_content[column];

	// Sanity check: don't insert past the last hegiht
	if (itemList.size() < heightPos)
	{
		assert(false);
		throw std::out_of_range("Trying to insert widget in a QTopMenuWidgetGrid at invalid heightPos");
	}

	// Insert
	auto insertedItemIt = itemList.emplace(itemList.begin()+heightPos,
	    Item(widget, sizeHint, CellInfo{m_cellSize, m_margin, m_transversalCellNum}));

	auto& insertedItem = *insertedItemIt;

	// Set widget properties
	auto widgetLocked = insertedItem.widget().lock();
	if(widgetLocked)
	{
		widgetLocked->setParent(static_cast<QWidget*>(&m_frame));
		connect (widgetLocked.get(), &QTopMenuWidget::fadePopup, this, [this]()
		{
			if (m_isCollapsed)
			{
				fadePopup();
			}
		});
		connect(widgetLocked.get(), &QTopMenuWidget::bestSizeChanged, this, [this]()
		{
			triggerResizeWidgets();
		});
	}
	else
	{
		assert(false);
		throw std::runtime_error("Inserted QTopMenuWidget inserted in QTopMenuWidgetGrid became invalid");
	}

	// Set focusProxy to the first element
	if (1==m_content.size() && 1==m_content[0].size())
	{
		auto widgetLocked = m_content[0][0].widget().lock();
		if(widgetLocked)
		{
			m_frame.setFocusProxy(widgetLocked.get());
		}
	}

	// Update focus order for all the group
	updateFocusOrder();

	// Update widgets
	triggerResizeWidgets();
}

void QTopMenuGridGroup::addItem(  std::shared_ptr<QTopMenuWidget> widget, size_t column, const QSizeF& sizeHint )
{
	const bool newColumn = m_content.size()==column;
	const size_t heightPos = m_content.size()>column ? m_content.at(column).size() : 0;
	addItem(widget, column, newColumn, heightPos, sizeHint);
}

void QTopMenuGridGroup::updateFocusOrder()
{
	QWidget* prev = nullptr;
	for (size_t x=0; x<m_content.size(); ++x)
	{
		for (size_t y=0; y<m_content[x].size(); ++y)
		{
			auto widgetLockIt = m_content[x][y].widget().lock();
			if (!widgetLockIt)
			{
				assert(false);
				throw std::runtime_error("Invalid widget in QTopMenuWidgetGrid");
			}
			if (prev)
			{
				setTabOrder(prev, widgetLockIt.get());
			}
			prev = widgetLockIt.get();
		}
	}
}

void QTopMenuGridGroup::staticSetupFontForLabel( QFont& f)
{
	f.setWeight(QFont::ExtraBold);
	f.setPointSizeF(f.pointSizeF() *0.75);
}

void QTopMenuGridGroup::fadePopup()
{
	m_frame.hide();

	update();
}

const std::vector<QTopMenuGridGroup::GroupItemInfo> QTopMenuGridGroup::itemsInfo() const
{
	std::vector<GroupItemInfo> ret;
	for (size_t col=0; col<m_content.size(); ++col)
	{
		const auto& itemList = m_content[col];
		for (size_t h=0; h<itemList.size(); ++h)
		{
			const auto widgetLocked = itemList[h].widget().lock();
			if (widgetLocked)
			{
				ret.push_back(GroupItemInfo{col, h, widgetLocked->nameId()});
			}
			else
			{
				assert(false);
				throw std::runtime_error("Accessing a widget in QTopMenuGridGroup which has been deleted.");
			}
		}
	}
	return ret;
}

bool QTopMenuGridGroup::removeItem( size_t column, size_t heightPos)
{
	if (column >= m_content.size())
	{
		return false;
	}

	auto& itemList = m_content[column];

	if (heightPos >= itemList.size())
	{
		return false;
	}

	auto widgetLocked = itemList.at(heightPos).widget().lock();
	if(widgetLocked)
	{
		widgetLocked->setParent(nullptr);
	}

	itemList.erase(itemList.begin()+heightPos);

	if ( itemList.empty())
	{
		m_content.erase(m_content.begin()+column);
	}

	// Set focus proxy to the first element if needed
	if (0==column && 0==heightPos && !m_content.empty() && !m_content[0].empty())
	{
		auto widgetLocked = m_content[0][0].widget().lock();
		if(widgetLocked)
		{
			m_frame.setFocusProxy(widgetLocked.get());
		}
	}

	// Update focus order for all the group
	updateFocusOrder();

	// Update widgets
	triggerResizeWidgets();

	return true;
}

void QTopMenuGridGroup::repositionSubWidgets()
{
	qreal deltaX = m_margin; // Accumulated used space in this direction

	const qreal groupFrameHeight = sizeForCells(m_cellSize, m_margin, m_transversalCellNum);
	for (size_t x=0; x<m_content.size(); ++x)
	{
		qreal maxW = 0;     // Highest widget size in this direction
		qreal deltaY = m_margin; // Accumulated used space in tranversal direction

		for (size_t y=0; y<m_content[x].size(); ++y)
		{
			Item& item = m_content[x][y];

			// Resize Buttons inside
			QSizeF widgetSize;
			auto widgetLocked = item.widget().lock();
			if (widgetLocked && m_needResizeWidgets)
			{
				// What would be an adequate Maximum Widget Size?
				const QSizeF maxSize = transposeIfVert(m_direction, QSizeF(MAX_WIDTH, groupFrameHeight));

				// Get the best widget size
				const auto& cInfo = item.originalCellInfo();
				auto sizeHint = item.originalSizeHint()*(groupFrameHeight /
					sizeForCells(cInfo.cellSize, cInfo.margin, m_transversalCellNum));
				const auto bestWidgetSize = widgetLocked->bestSize(m_direction,
					CellInfo{m_cellSize, m_margin, m_transversalCellNum}, sizeHint, maxSize);
				if (!bestWidgetSize)
				{
					if (widgetLocked->isVisible())
					{
						widgetLocked->setVisible(false);
					}
					continue;
				}
				if (!widgetLocked->isVisible())
				{
					widgetLocked->setVisible(true);
				}

				// Check if the definitive size fit
				if (groupFrameHeight+m_margin < deltaY + (m_direction==DisplaySide::Top ?
				bestWidgetSize->height() : bestWidgetSize->width()))
				{
					// new "virtual" column to fit the widget, which otherwise does not fit
					deltaX += maxW + m_margin;
					maxW = 0;
					deltaY = m_margin;

				}

				if (widgetLocked->size() != bestWidgetSize)
				{
					widgetLocked->resize(bestWidgetSize->toSize());
				}
				widgetSize = *bestWidgetSize;
			}
			else if (widgetLocked)
			{
				widgetSize = widgetLocked->size();
			}
			else
			{
				assert(false);
				throw std::runtime_error("QTopMenuWidget in QTopMenuWidgetGrid is invalid. "
					"Remove it before to destroy it");
			}

			// Reposition widgets inside
			const auto widgetSizeT = transposeIfVert(m_direction, widgetSize);
			if (maxW<widgetSizeT.width())
			{
				maxW = widgetSizeT.width();
			}
			const QSizeF newWidgetSize(widgetSizeT.width(),
				upperBound(m_cellSize, m_margin, widgetSizeT.height()));
			const QPointF newWidgetPos(deltaX, deltaY);
			const QRectF newWidgetRect(transposeIfVert(m_direction, newWidgetPos),
				transposeIfVert(m_direction, newWidgetSize));

			deltaY += newWidgetSize.height() + m_margin;

			if (widgetLocked->pos() != newWidgetRect.topLeft().toPoint())
			{
				widgetLocked->move(newWidgetRect.topLeft().toPoint());
			}
		}

		deltaX += maxW + m_margin;

	}

	// Set Text size, elide, and properties
	{
		auto qLabel = QString::fromUtf8(m_label.c_str());
		auto maxSize = sizeForCells(m_cellSize, m_margin, m_transversalCellNum);
		QFont font;
		setupFontForLabel(font);
		QFontMetrics metrics(font);
		QString elidedLabel;
		elidedLabel = metrics.elidedText(qLabel, Qt::TextElideMode::ElideMiddle, maxSize*2.0);

		if (metrics.height() != m_arrow.size().height())
		{
			m_arrow.resize(QSize(metrics.height()*2, metrics.height()));
		}

		m_staticText.setText(elidedLabel); //TODO document utf8
		m_staticText.prepare(QTransform(), font);
	}

	// Resize the frame
	const QSizeF textSizeT = transposeIfVert(m_direction, m_staticText.size());

	deltaX = std::max(deltaX, textSizeT.width()+2.0*m_margin); //If no widget, allows minimum space
	const QSizeF frameSizeT = transposeIfVert( m_direction,
		QSizeF(deltaX, groupFrameHeight + 3.0*m_margin + m_staticText.size().height()*2.0));
	m_frame.resize(frameSizeT.toSize());

	// Resize the widget & Icon
	QSizeF newSize;
	if (!collapsed())
	{
		const auto divWidth = (m_showDivisionBar ? divisionSpace : 0.0);
		newSize = m_frame.size() + transposeIfVert(m_direction, QSizeF(divWidth, 0.0)).toSize();
	}
	else
	{
		newSize = collapsedSize();
	}

	if (newSize != size())
	{
		auto iconSize = collapsedIconRect().size();
		m_icon.resize(iconSize);
		resize(newSize.toSize());
		setMinimumSize(newSize.toSize());
		setMaximumSize(newSize.toSize());
		updateGeometry();
	}
	emit updateGeometryEvent(); // needed even if size did not changed -> e.g. collapsed need uncollapse

	// Set flags, so we know all is updated
	m_needResizeWidgets = false;
	m_needRepositionWidgets = false;
}

QRect QTopMenuGridGroup::collapsedIconRect() const
{
	const auto divSpace = (m_showDivisionBar ? divisionSpace : 0.0);
	const CellInfo cellInfo{m_cellSize, m_margin, m_transversalCellNum};
	return collapsedIconRect(rect(), cellInfo, divSpace, direction());
}

QRect QTopMenuGridGroup::collapsedIconRect(const QRect& widgetRect, const CellInfo& cellInfo,
    const qreal divSpace, DisplaySide dir)
{
	const auto maxSize = sizeForCells(cellInfo.cellSize, cellInfo.margin, cellInfo.transversalCellNum) + cellInfo.margin*2;

	const auto widgetSizeT = transposeIfVert(dir, widgetRect.size());

	return QRect(widgetRect.topLeft() + transposeIfVert(dir,
	    QPointF((widgetSizeT.width()-maxSize - divSpace) / 2, 0)).toPoint(), QSize(maxSize,maxSize));
}

QRect QTopMenuGridGroup::textRect() const
{
	const auto divSpace = (m_showDivisionBar ? divisionSpace : 0.0);
	return textRect(rect(), margin(), divSpace, m_staticText, direction());
}

QRect QTopMenuGridGroup::textRect(const QRect& widgetRect, qreal margin, qreal divSpace,
    const QStaticText& staticText, DisplaySide dir)
{
	QSizeF textSize = staticText.size();

	const auto widgetSizeT = transposeIfVert(dir, widgetRect.size());

	QPoint pos((widgetSizeT.width()-textSize.width()-divSpace)/2,
	    widgetSizeT.height()-margin-textSize.height()*2);

	return QRect(pos, textSize.toSize());
}

QRect QTopMenuGridGroup::arrowRect() const
{
	const auto divSpace = (m_showDivisionBar ? divisionSpace : 0.0);
	return arrowRect(rect(), margin(), divSpace, m_arrow, direction());
}

QRect QTopMenuGridGroup::arrowRect(const QRect& widgetRect, qreal margin, qreal divSpace,
    const QSvgPixmapCache& arrow, DisplaySide dir)
{
	QSizeF arrowSize = arrow.size();

	const auto widgetSizeT = transposeIfVert(dir, widgetRect.size());

	QPoint pos((widgetSizeT.width()-arrowSize.width()-divSpace)/2,
	    widgetSizeT.height()-margin-arrowSize.height());

	return QRect(pos, arrowSize.toSize());
}

qreal QTopMenuGridGroup::upperBound( qreal cellSize, qreal margin, qreal size )
{
	return sizeForCells(cellSize, margin, overlappedCells(cellSize, margin, size));
}

size_t QTopMenuGridGroup::overlappedCells( qreal cellSize, qreal margin, qreal size )
{
	if (size<=0.0)
	{
		return 0.0;
	}

	return static_cast<size_t>(0.1+std::ceil((size - cellSize) / (cellSize+margin))+1.0);
}

qreal QTopMenuGridGroup::sizeForCells( qreal cellSize, qreal margin, size_t cellNum )
{
	const qreal cellNumR = static_cast<qreal>(cellNum);
	return cellNumR*cellSize + std::max(cellNumR-1.0, 0.0)*margin;
}

void QTopMenuGridGroup::resizeEvent(QResizeEvent*)
{
	triggerRepositionWidgets();
}

void QTopMenuGridGroup::staticDrawControl( const QTopMenuGridGroupStyleOptions& opt, QPainter& p)
{
	// Gather color for state/role
	QPaletteExt pal = opt.palette;
	if (!opt.isEnabled)
	{
		pal.setCurrentColorGroup(QPalette::Disabled);
	}

	if (opt.isCollapsed)
	{
		const auto divSpace = (opt.showDivisionBar ? divisionSpace : 0.0);

		ColorRoleExt role = ColorRoleExt::Background_Normal;
		ColorRoleExt roleText = ColorRoleExt::TextOverBackground_Normal;
		ColorRoleExt roleDrawing = ColorRoleExt::LinesOverBackground_Normal;

		if (opt.isEnabled)
		{
			if (opt.isPressed)
			{
				role = ColorRoleExt::Background_Pressed;
				roleText = ColorRoleExt::TextOverBackground_Pressed;
				roleDrawing = ColorRoleExt::LinesOverBackground_Pressed;
			}
			else if (opt.isHover)
			{
				role = ColorRoleExt::Background_Hover;
				roleText = ColorRoleExt::TextOverBackground_Hover;
				roleDrawing = ColorRoleExt::LinesOverBackground_Hover;
			}
		}
		QColor bgColor = pal.color(role);

		p.setBrush(bgColor);
		p.setPen(Qt::NoPen);

		const QRectF outerRect = opt.widgetRect;
		const QSizeF divSpaceT = transposeIfVert(opt.direction, QSizeF(divSpace ,0.0));
		const QRectF innerRect(QPointF(outerRect.x()+borderWidth*0.5, outerRect.y()+borderWidth*0.5),
			QSizeF(outerRect.width()-borderWidth, outerRect.height()-borderWidth)-divSpaceT);

		if (opt.isEnabled)
		{
			p.drawRoundedRect(innerRect, cornerRadius, cornerRadius);

			if (opt.isFocussed)
			{
				p.setBrush(Qt::NoBrush);
				p.setPen( QPen(pal.color(ColorRoleExt::Highlight_Normal), borderWidth));
				p.drawRoundedRect(innerRect, cornerRadius, cornerRadius);
			}
		}

		// Draw icon
		const auto& iconRect = collapsedIconRect(opt.widgetRect, opt.cellInfo, divSpace, opt.direction);
		if (opt.icon && opt.icon->hasIcon())
		{
			assert( iconRect.size() == opt.icon->size());
			opt.icon->paint(p, iconRect, pal, opt.isPressed, opt.isHover);
		}

		p.save();
		if (opt.direction == DisplaySide::Left)
		{
			p.translate(opt.widgetRect.left(), opt.widgetRect.top()+opt.widgetRect.height());
			p.rotate(-90.0);
		}

		// Draw label
		QColor textColor = pal.color(roleText);
		p.setPen(textColor);
		QFont font = p.font();
		staticSetupFontForLabel(font);
		p.setFont(font);

		if (opt.staticText)
		{
			const auto& labelRect = textRect(opt.widgetRect, opt.cellInfo.margin, divSpace, *opt.staticText, opt.direction);
			p.drawStaticText( labelRect.topLeft(), *opt.staticText);
		}

		// Draw arrow
		if (opt.arrow && opt.arrow->hasPixmap())
		{
			const auto& arrowR = arrowRect(opt.widgetRect, opt.cellInfo.margin, divSpace, *opt.arrow, opt.direction);
			const auto& arrowPix = opt.arrow->pixmapFor(roleDrawing, opt.palette);
			assert( arrowR.size() == arrowPix.size());
			p.drawPixmap(arrowR, arrowPix);
		}

		p.restore();
	}

	// Division
	if (opt.showDivisionBar)
	{
		ColorRoleExt roleDrawing =  ColorRoleExt::LinesOverBackground_Normal;

		QColor divisionColor = pal.color(roleDrawing);
		QSizeF widgetSizeT = transposeIfVert(opt.direction, opt.widgetRect.size());
		const QPoint divA(widgetSizeT.width()-(divisionSpace+divisionLineWidth)*0.5, 0.0);
		const QPoint divB(widgetSizeT.width()-(divisionSpace-divisionLineWidth)*0.5, widgetSizeT.height());
		const QRectF divisionRect(transposeIfVert(opt.direction, divA), transposeIfVert(opt.direction, divB));
		p.fillRect(divisionRect.toRect(), divisionColor);
	}
}

void QTopMenuGridGroup::initStyleOption(QTopMenuGridGroupStyleOptions& opt) const
{
	opt.isCollapsed = m_isCollapsed;

	opt.isEnabled = isEnabled();

	if (opt.isEnabled)
	{
		opt.isHover = m_cacheHovered;
		opt.isPressed = m_clickPressed;
		opt.isFocussed = hasFocus();
	}
	opt.palette = QWidget::palette();
	opt.icon = &m_icon;
	opt.arrow = &m_arrow;
	opt.widgetRect = rect();
	opt.staticText = &m_staticText;
	opt.direction = direction();
	opt.cellInfo = CellInfo{m_cellSize, m_margin, m_transversalCellNum};
	opt.transversalCellNum = m_transversalCellNum;
	opt.showDivisionBar = m_showDivisionBar;
}

void QTopMenuGridGroup::paintEvent(QPaintEvent* e)
{
	if (m_needRepositionWidgets)
	{
		repositionSubWidgets();
	}

	QWidget::paintEvent(e);

	QTopMenuGridGroupStyleOptions opt;
	initStyleOption(opt);
	QPainter p(this);
	p.setClipRect(e->rect());
	p.setRenderHint(QPainter::Antialiasing );
	drawControl(opt, p);
}

void QTopMenuGridGroup::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Enter:
	case Qt::Key_Return:
	case Qt::Key_Select:
	case Qt::Key_Space:
	{
		if (!e->isAutoRepeat() && collapsed())
		{
			togglePopup();
		}
	} break;
	default:
		e->ignore();
	}
}

bool QTopMenuGridGroup::event(QEvent* e)
{
	if (m_isCollapsed)
	{
		auto ret = m_clickManager.eventHandler( e );
		if (ret)
		{
			return true;
		}

		// Make hover to update
		if (e->type() == QEvent::Type::Enter || e->type() == QEvent::Type::Leave)
		{
			update();
		}
	}
	
	return QWidget::event(e);
}

QPointF QTopMenuGridGroup::transposeIfVert(DisplaySide dir, const QPointF& p)
{
	const bool isAtTop = (DisplaySide::Top==dir);
	return isAtTop ? p : p.transposed();
}

QSizeF QTopMenuGridGroup::transposeIfVert(DisplaySide dir, const QSizeF& s)
{
	const bool isAtTop = (DisplaySide::Top==dir);
	return isAtTop ? s : s.transposed();
}
