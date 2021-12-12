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

#include "QTopMenuButtonWidget.hpp"

#include <cmath>

#include <QPaletteExt.hpp>
#include <QSvgIcon.hpp>

#include <QAction>
#include <QCursor>
#include <QPainter>
#include <QWidget>

using namespace Escain;


QTopMenuButtonWidget::QTopMenuButtonWidget(const std::string& name, size_t id, QWidget* parent)
	: QTopMenuWidget(name, id, parent)
{
	QTextOption textOption;
	textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	textOption.setAlignment(Qt::AlignCenter | Qt::AlignBaseline);
	m_staticText.setTextFormat(Qt::TextFormat::PlainText);
	m_staticText.setTextOption(textOption);

	setFocusPolicy( Qt::StrongFocus);
	m_clickManager.enableHover(*this);
	
	connect(&m_clickManager, &QClickManager::clicked,
	[this](const QPointF& cursorPos, const std::unordered_set<size_t>& clickableRectangleIds)
	{
		if (isEnabled())
		{
			emit clicked(cursorPos, clickableRectangleIds, this); //TODO remove "me" argument
			emit fadePopup();
		}
	});
	
	connect(&m_clickManager, &QClickManager::pressed, 
	[this](const QPointF&, bool pressed)
	{
		m_clickPressed=isEnabled() && pressed;
		update();
	});

	connect(&m_clickManager, &QClickManager::hovered,
	[this](const QPointF& , bool hovered, const size_t , const QRectF& )
	{
		m_hovered= isEnabled() && hovered;
		update();
	});
}

QTopMenuButtonWidget::~QTopMenuButtonWidget()
{
	setParent(nullptr); //Avoid double deletion.

	if (nullptr != m_icon)
	{
		m_icon->forgetId(id());
	}
}

QTopMenuBuggonWidgetLayout QTopMenuButtonWidget::detectLayout(const QSize& currSize) const
{
	QFont font;
	staticSetupFontForLabel(font);
	QFontMetrics metrics(font);
	qreal estCellSize = metrics.height()*1.2;

	if (static_cast<qreal>(currSize.width()) / static_cast<qreal>(currSize.height()) >=2.0)
	{
		return QTopMenuBuggonWidgetLayout::Horizontal;
	}
	if (currSize.height() <= estCellSize*2.5+m_margin &&
	    currSize.width() <= estCellSize*2.5+m_margin)
	{
		return QTopMenuBuggonWidgetLayout::Small;
	}
	return QTopMenuBuggonWidgetLayout::Big; // Simplest drawing if any other layout is not detected.
}

qreal QTopMenuButtonWidget::margin() const
{
	return m_margin;
}

void QTopMenuButtonWidget::margin( qreal margin )
{
	if (m_margin != margin)
	{
		m_margin = margin;
		m_recomputeSizeNeeded=true;
		update(m_iconRect);
		emit bestSizeChanged();
	}

}

void QTopMenuButtonWidget::direction( const DisplaySide d )
{
	if (direction() != d)
	{
		direction(d);
		m_recomputeSizeNeeded=true;
		update();
		emit bestSizeChanged();
	}
}

QRectF QTopMenuButtonWidget::iconRect() const
{
	return iconRect(rect(), m_cachedLayout, direction());
}

QRectF QTopMenuButtonWidget::iconRect(const QRect& widgetRect, const QTopMenuBuggonWidgetLayout layout, const DisplaySide dir)
{
	QRectF iconRect;
	switch (layout)
	{
		case QTopMenuBuggonWidgetLayout::Small:
		{
			iconRect = widgetRect;
		}break;
		case QTopMenuBuggonWidgetLayout::Big:
		{
			if (DisplaySide::Top == dir)
			{
				QFont font;
				staticSetupFontForLabel(font);
				QFontMetrics metrics(font);
				qreal side = widgetRect.height()-metrics.height();
				iconRect = QRectF(QPointF(0.0, 0.0), QSize(side, side));
			}
			else
			{
				qreal side = widgetRect.width();
				iconRect = QRectF(QPointF(0.0, 0.0), QSize(side, side));
			}
		}break;
		case QTopMenuBuggonWidgetLayout::Horizontal:
		{
			qreal side = widgetRect.height();
			iconRect = QRectF(QPointF(0.0, 0.0), QSize(side, side));
		}
	}
	return iconRect;
}

QRect QTopMenuButtonWidget::textRect() const
{
	QFont font;
	staticSetupFontForLabel(font);
	QFontMetrics metrics(font);
	const auto qLabel = elidedText(metrics, m_cachedLayout, QString::fromUtf8(m_label.c_str()), size(), m_margin);
	return textRect( rect(), m_margin, metrics, qLabel, m_cachedLayout);
}

QRect QTopMenuButtonWidget::textRect(const QRect& widgetRect, qreal margin, const QFontMetrics& metrics, const QString& label, const QTopMenuBuggonWidgetLayout layout)
{
	QSizeF textSize = QSizeF(metrics.horizontalAdvance(label),metrics.height());

	if (layout == QTopMenuBuggonWidgetLayout::Big)
	{
		QPoint pos(/*(widgetRect.width()-textSize.width())/2*/(widgetRect.width()-textSize.width())/2,
			widgetRect.height()-margin-textSize.height());
		return QRect(pos, textSize.toSize());
	}
	if (layout == QTopMenuBuggonWidgetLayout::Horizontal)
	{
		QPoint pos( widgetRect.height(),
			(widgetRect.height()-textSize.height())/2);
		return QRect(pos, textSize.toSize());
	}

	return QRect();
}

void QTopMenuButtonWidget::staticSetupFontForLabel( QFont& f)
{
	f.setPointSizeF(f.pointSizeF() *0.75);
}

bool QTopMenuButtonWidget::event(QEvent* e)
{
	auto ret = m_clickManager.eventHandler( e );
	if (ret)
	{
		return true;
	}
	
	return QWidget::event(e);
}

void QTopMenuButtonWidget::initStyleOption(QTopMenuButtonWidgetStyleOptions& opt) const
{
	opt.isHover = m_hovered;
	opt.isEnabled = isEnabled();
	opt.isPressed = m_clickPressed;
	opt.isFocussed = hasFocus();
	opt.palette = QPaletteExt(QWidget::palette());
	opt.icon=m_icon;
	opt.staticText=&m_staticText;
	opt.widgetRect = rect();
	opt.layout = detectLayout(size());
	opt.id=id();
	opt.direction = direction();
	opt.margin = m_margin;
}

void QTopMenuButtonWidget::staticDrawControl( const QTopMenuButtonWidgetStyleOptions& opt, QPainter& p)
{
	QPaletteExt pal = opt.palette;
	if (!opt.isEnabled)
	{
		pal.setCurrentColorGroup(QPalette::Disabled);
	}

	ColorRoleExt role = ColorRoleExt::Background_Normal;
	ColorRoleExt roleText = ColorRoleExt::TextOverBackground_Normal;
	ColorRoleExt roleHighlight = ColorRoleExt::Highlight_Normal;

	if (opt.isEnabled)
	{
		if (opt.isPressed)
		{
			role = ColorRoleExt::Background_Pressed;
			roleText = ColorRoleExt::TextOverBackground_Pressed;
			roleHighlight = ColorRoleExt::Highlight_Pressed;
		}
		else if (opt.isHover)
		{
			role = ColorRoleExt::Background_Hover;
			roleText = ColorRoleExt::TextOverBackground_Hover;
			roleHighlight = ColorRoleExt::Highlight_Hover;
		}
	}

	QColor bgColor = pal.color(role);
	p.setBrush(bgColor);
	p.setPen(Qt::NoPen);

	const QRectF outerRect = opt.widgetRect;
	const QRectF innerRect(outerRect.x()+borderWidth*0.5, outerRect.y()+borderWidth*0.5,
		outerRect.width()-borderWidth, outerRect.height()-borderWidth);

	if (opt.isEnabled)
	{
		p.drawRoundedRect(innerRect, cornerRadius, cornerRadius);

		if (opt.isFocussed)
		{
			p.setBrush(Qt::NoBrush);
			p.setPen( QPen(pal.color(roleHighlight), 2.0));
			p.drawRoundedRect(innerRect, cornerRadius, cornerRadius);
		}
	}

	if (nullptr != opt.icon && opt.icon->hasIcon())
	{
		const QRectF iconR= iconRect(opt.widgetRect, opt.layout, opt.direction);

		const QRect innerIconRect(iconR.x()+opt.margin, iconR.y()+opt.margin,
			iconR.width()-2.0*opt.margin, iconR.height()-2.0*opt.margin);
		opt.icon->paint(p, innerIconRect, pal, opt.isPressed, opt.isHover, opt.id);
	}

	// Draw the label
	switch (opt.layout)
	{
		case QTopMenuBuggonWidgetLayout::Small:
			break;
		case QTopMenuBuggonWidgetLayout::Big:
		case QTopMenuBuggonWidgetLayout::Horizontal:
		{
			p.save();
			QColor textColor = pal.color(roleText);
			p.setPen(textColor);
			QFont font = p.font();
			staticSetupFontForLabel(font);
			p.setFont(font);
			QFontMetrics metrics(font);

			const QRectF textR = textRect(opt.widgetRect, opt.margin, metrics, opt.staticText->text(), opt.layout);
			assert(opt.staticText);
			p.drawStaticText( textR.topLeft().toPoint(), *opt.staticText);

			p.restore();
		} break;
	}
}
	
void QTopMenuButtonWidget::paintEvent(QPaintEvent *e)
{
	if (m_recomputeSizeNeeded)
	{
		recomputeSize();
	}

	QWidget::paintEvent(e);
	QPainter p(this);
	p.setClipRect(e->rect());
	p.setRenderHint(QPainter::Antialiasing );

	QTopMenuButtonWidgetStyleOptions opt;
	initStyleOption(opt);
	staticDrawControl(opt, p);
}

void QTopMenuButtonWidget::resizeEvent(QResizeEvent *)
{
	m_recomputeSizeNeeded=true;
}

QString QTopMenuButtonWidget::elidedText( const QFontMetrics& metrics, const QTopMenuBuggonWidgetLayout layout,
    const QString& str, const QSize& widgetSize, const qreal margin) const
{
	switch (layout)
	{
		case QTopMenuBuggonWidgetLayout::Small:
		{
			return "";
		}break;
		case QTopMenuBuggonWidgetLayout::Big:
		{
			return metrics.elidedText(str, Qt::TextElideMode::ElideMiddle, widgetSize.width()-2*margin);
		}break;
		case QTopMenuBuggonWidgetLayout::Horizontal:
		{
			return metrics.elidedText(str, Qt::TextElideMode::ElideMiddle, widgetSize.width()- margin - widgetSize.height());
		}
	}
	return "";
}

void QTopMenuButtonWidget::recomputeSize()
{
	// Detect layout
	m_cachedLayout = detectLayout(size());

	// Manage text
	auto qLabel = QString::fromUtf8(m_label.c_str());
	QFont font;
	setupFontForLabel(font);
	QFontMetrics metrics(font);
	QString elidedLabel = elidedText(metrics, m_cachedLayout, qLabel, size(),m_margin);
	m_staticText.setText(elidedLabel); //TODO document utf8
	m_iconRect = iconRect().toRect();

	// don't use QSvgIcon::margin, as the m_icon is shared among several widgets,
	//    while this->m_margin could eventually be different among them.
	const QSize innerIconSize(m_iconRect.width()-2.0*m_margin, m_iconRect.height()-2.0*m_margin);
	if (nullptr != m_icon && m_icon->size(id())!=innerIconSize && m_iconRect.isValid())
	{
		m_icon->resize(innerIconSize, id());
	}

	m_recomputeSizeNeeded=false;
}

void QTopMenuButtonWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Enter:
	case Qt::Key_Return:
	case Qt::Key_Select:
	case Qt::Key_Space:
	{
		if (!e->isAutoRepeat())
		{
			emit clicked({}, {}, this); //TODO remove "me" argument
			emit fadePopup();
		}
	} break;
	default:
		e->ignore();
	}
}

std::optional<QSizeF> QTopMenuButtonWidget::bestSize([[maybe_unused]] DisplaySide dir,
    const CellInfo& cellInfo, const QSizeF& sizeHint, const QSizeF& maxSize) const
{
	auto overlappedCells = [&cellInfo](qreal size) -> qreal
	{
		if (size<=0.0)
		{
			return 0.0;
		}

		return (0.1+(size - cellInfo.cellSize) /
		    (cellInfo.cellSize+cellInfo.margin)+1.0);
	};

	auto sizeForCells = [&cellInfo]( qreal cellNumR ) -> qreal
	{
		return cellNumR*cellInfo.cellSize + std::max(cellNumR-1.0, 0.0)*cellInfo.margin;
	};

	QFont font;
	staticSetupFontForLabel(font);
	QFontMetrics metrics(font);
	qreal fontHeight = metrics.height() * 1.5;

	qreal estCellRatio = std::round(overlappedCells(fontHeight));

	std::vector<QSizeF> candidates;
	candidates.push_back(QSizeF(sizeForCells(estCellRatio * 1.0), sizeForCells(estCellRatio * 1.0)));
	candidates.push_back(QSizeF(sizeForCells(estCellRatio * 2.0), sizeForCells(estCellRatio * 2.0)));
	candidates.push_back(QSizeF(sizeForCells(estCellRatio * 3.0), sizeForCells(estCellRatio * 3.0)));

	if (DisplaySide::Top == dir)
	{
		candidates.push_back(QSizeF(sizeForCells(estCellRatio * 5.0), sizeForCells(estCellRatio * 1.0)));
		candidates.push_back(QSizeF(sizeForCells(estCellRatio * 10.0), sizeForCells(estCellRatio * 1.0)));
	}
	else
	{
		candidates.push_back(QSizeF(sizeForCells(estCellRatio * 3.0), sizeForCells(estCellRatio * 1.0)));
	}

	for ( auto& s: candidates)
	{
		const auto layout = detectLayout(s.toSize());
		QString qlabel = elidedText(metrics, layout, QString::fromUtf8(m_label.c_str()), s.toSize(), m_margin);
		QRect r = QRect(QPoint(0,0), s.toSize());
		const auto& iconR = iconRect(r, layout, dir);

		if (layout == QTopMenuBuggonWidgetLayout::Small)
		{
			continue; //no text, no modifications to width
		}

		const auto& textR = textRect(r, m_margin, metrics, qlabel, layout);
		if (layout == QTopMenuBuggonWidgetLayout::Horizontal)
		{
			//Some instability between QFontMetrics::horizontalAdvance and elideText-> add 1.0
			qreal newWidth = iconR.toRect().width()+textR.width()+m_margin+1.0;
			if (newWidth < s.width())
			{
				s.setWidth(newWidth);
			}
		}
		else if (layout == QTopMenuBuggonWidgetLayout::Big)
		{
			if ( DisplaySide::Top == dir)
			{
				//Some instability between QFontMetrics::horizontalAdvance and elideText-> add 1.0
				qreal newWidth = std::max(iconR.width(), textR.width()+2*m_margin+1.0);
				if (newWidth < s.width())
				{
					s.setWidth(newWidth);
				}
			}
			else
			{
				s.setHeight(s.height()+metrics.height()+m_margin);
			}
		}
	}

	return bestSizeBetweenPossibles( candidates, sizeHint, maxSize);
}

void QTopMenuButtonWidget::icon(QSvgIcon* ic)
{
	// Even if ic==m_icon, do not skip: we need to update the margin and eventually declare m_id

	if (m_icon)
	{
		m_icon->forgetId(id());
	}

	m_icon = ic;

	if (m_icon)
	{
		m_icon->declareId(id());
	}

	update();
}

const QSvgIcon* QTopMenuButtonWidget::icon() const
{
	return m_icon;
}

void QTopMenuButtonWidget::label( const std::string& label)
{
	if (m_label != label)
	{
		m_label = label;
		m_recomputeSizeNeeded=true;
		update();
		emit bestSizeChanged();
	}
}

const std::string& QTopMenuButtonWidget::label() const
{
	return m_label;
}

