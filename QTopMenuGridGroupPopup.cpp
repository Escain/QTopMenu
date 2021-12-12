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

#include "QTopMenuGridGroupPopup.hpp"

#include <QPainter>
#include <QPaintEvent>

using namespace Escain;

QTopMenuGridGroupPopup::QTopMenuGridGroupPopup(  QWidget* parent )
: QWidget(parent)
{
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

	QTextOption textOption;
	textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	textOption.setAlignment(Qt::AlignCenter | Qt::AlignBaseline);
	m_staticText.setTextFormat(Qt::TextFormat::PlainText);
	m_staticText.setTextOption(textOption);
}

void QTopMenuGridGroupPopup::initStyleOption(QTopMenuGridGroupPopupStyleOptions& opt) const
{
	opt.isEnabled = isEnabled();

	if (opt.isEnabled)
	{
		const QPoint mousePos = mapFromGlobal(QCursor::pos());
		opt.isHover = rect().contains(mousePos);
	}
	opt.palette = QWidget::palette();
	opt.widgetRect = rect();
	opt.staticText = &m_staticText;
	opt.direction = direction();
	opt.margin = m_margin;
}

const std::string& QTopMenuGridGroupPopup::label() const
{
	return m_label;
}

void QTopMenuGridGroupPopup::label( const std::string_view& label )
{
	if (m_label != label)
	{
		m_label = label;
		prepareText();
		update();
	}
}

DisplaySide QTopMenuGridGroupPopup::direction() const
{
	return m_direction;
}

void QTopMenuGridGroupPopup::direction( const DisplaySide d )
{
	if (m_direction != d)
	{
		m_direction = d;
		update();
	}
}

qreal QTopMenuGridGroupPopup::margin() const
{
	return m_margin;
}

void QTopMenuGridGroupPopup::margin( qreal margin )
{
	if (margin != m_margin)
	{
		m_margin = margin;
		update();
	}
}

void QTopMenuGridGroupPopup::prepareText()
{
	auto qLabel = QString::fromUtf8(m_label.c_str());

	QFont font;
	setupFontForLabel(font);
	QFontMetrics metrics(font);
	QString elidedLabel;
	if (direction()==DisplaySide::Top)
	{
		elidedLabel = metrics.elidedText(qLabel, Qt::TextElideMode::ElideMiddle, width()-2.0*m_margin);
	}
	else
	{
		elidedLabel = metrics.elidedText(qLabel, Qt::TextElideMode::ElideMiddle, height()-2.0*m_margin);
	}

	m_staticText.setText(elidedLabel);
	m_staticText.prepare(QTransform(), font);
}

void QTopMenuGridGroupPopup::setupFontForLabel( QFont& f)
{
	f.setWeight(QFont::ExtraBold);
	f.setPointSizeF(f.pointSizeF() *0.75);
}

void QTopMenuGridGroupPopup::closeEvent(QCloseEvent* e)
{
	fade();
}

QRect QTopMenuGridGroupPopup::textRect(const QRect& widgetRect, qreal margin, const QStaticText& staticText, DisplaySide dir)
{
	QSizeF textSize = staticText.size();
	if (dir == DisplaySide::Top)
	{
		QPoint pos((widgetRect.width()-textSize.width())/2,
			widgetRect.height()-margin-textSize.height()*1.5);
		return QRect(pos, textSize.toSize());
	}


	QPoint pos((widgetRect.height()-textSize.width())/2,
		widgetRect.width()-margin-textSize.height()*1.5);
	return QRect(pos, textSize.toSize());
}

void QTopMenuGridGroupPopup::staticDrawControl( const QTopMenuGridGroupPopupStyleOptions& opt, QPainter& p)
{
	// Gather color for state/role
	QPaletteExt pal = opt.palette;
	if (!opt.isEnabled)
	{
		pal.setCurrentColorGroup(QPalette::Disabled);
	}

	ColorRoleExt roleText = ColorRoleExt::PlaceholderText_Normal;

	if (opt.isEnabled)
	{
		if (opt.isHover)
		{
			roleText = ColorRoleExt::PlaceholderText_Hover;
		}
	}

	// Draw label
	p.setPen(pal.color(roleText));
	QFont font;
	setupFontForLabel(font);
	p.setFont(font);

	if (opt.direction == DisplaySide::Left)
	{
		p.translate(opt.widgetRect.left(), opt.widgetRect.top()+opt.widgetRect.height());
		p.rotate(-90.0);
	}

	if (opt.staticText)
	{
		const auto& labelRect = textRect(opt.widgetRect, opt.margin, *opt.staticText, opt.direction);
		p.drawStaticText( labelRect.topLeft(), *opt.staticText);
	}
}

void QTopMenuGridGroupPopup::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);

	QTopMenuGridGroupPopupStyleOptions opt;
	initStyleOption(opt);
	QPainter p(this);
	p.setClipRect(e->rect());
	p.setRenderHint(QPainter::Antialiasing );
	drawControl(opt, p);
}


void QTopMenuGridGroupPopup::resizeEvent(QResizeEvent*)
{
	prepareText();
}
