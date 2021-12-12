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

#ifndef QTOPMENUGRIDGROUPPOPUP_HPP
#define QTOPMENUGRIDGROUPPOPUP_HPP

#include <QStaticText>
#include <QWidget>
#include <QPaletteExt.hpp>

#include "QTopMenuWidgetTypes.hpp"

namespace Escain
{

/// StyleOption, allowing static painting of the widget (and thus easy customization).
struct QTopMenuGridGroupPopupStyleOptions
{
	bool isHover = false;
	bool isEnabled = true;
	QPaletteExt palette;
	const QStaticText* staticText=nullptr;
	QRect widgetRect;
	DisplaySide direction;
	qreal margin;
};

/// This class allows the painting of the Popup frame, for a QTopMenuGridGroup
class QTopMenuGridGroupPopup: public QWidget
{
	Q_OBJECT
public:
	explicit QTopMenuGridGroupPopup( QWidget* parent = nullptr);
	virtual ~QTopMenuGridGroupPopup() = default;

	/// overridable call to the static equivalent
	virtual void drawControl( const QTopMenuGridGroupPopupStyleOptions& opt, QPainter& p) const { staticDrawControl(opt, p); }
	static void staticDrawControl( const QTopMenuGridGroupPopupStyleOptions& opt, QPainter& p);

	///Direction: Set if the widget are arranged horizontally or vertically
	virtual DisplaySide direction() const;
	virtual void direction( const DisplaySide d );

	/// Label
	virtual const std::string& label() const;
	virtual void label( const std::string_view& label );

	/// Space between cells, in Pts
	virtual qreal margin() const;
	virtual void margin( qreal margin );
signals:
	virtual void fade();
protected:
	/// Fill QTopMenuGridGroupPopupStyleOptions with current configuration for static drawing.
	virtual void initStyleOption(QTopMenuGridGroupPopupStyleOptions&) const;

	/// Returns the rect where to draw the Group Label. In vertical mode, the rect
	///     expects the QPainter to be rotated CW 90º (the rect is rotated CCW 90º)
	static QRect textRect(const QRect& widgetRect, qreal margin, const QStaticText& staticText, DisplaySide dir);
	static void setupFontForLabel( QFont& );

	void prepareText();

	void paintEvent(QPaintEvent* e) override;
	void closeEvent(QCloseEvent* e) override;
	void resizeEvent(QResizeEvent*) override;

	QStaticText m_staticText;
	qreal m_margin = 4.0;
	std::string m_label;

	DisplaySide m_direction = DisplaySide::Top;
};

}
#endif
