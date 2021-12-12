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

#ifndef QTOPMENUBUTTONWIDGET_HPP
#define QTOPMENUBUTTONWIDGET_HPP

#include <memory>

#include <QStaticText>

#include "QTopMenuWidget.hpp"
#include <QClickManager.hpp>
#include <QSvgIcon.hpp>
#include "QTopMenuButton.hpp"

class QWidget;

namespace Escain
{

enum class QTopMenuBuggonWidgetLayout
{
	Small,
	Big,
	Horizontal
};

struct QTopMenuButtonWidgetStyleOptions
{
	bool isHover = false;
	bool isEnabled = true;
	bool isPressed = false;
	bool isFocussed = false;
	QPaletteExt palette;
	const QSvgIcon* icon=nullptr;
	const QStaticText* staticText=nullptr;
	QRect widgetRect;
	QTopMenuBuggonWidgetLayout layout;
	DisplaySide direction;
	size_t id=0;
	qreal margin = 0.0;
};

class QSvgIcon;

class QTopMenuButtonWidget: public QTopMenuWidget
{
Q_OBJECT
public:
	explicit QTopMenuButtonWidget( const std::string& name, size_t id, QWidget* parent=nullptr);
	virtual ~QTopMenuButtonWidget() override;

	// See QTopMenuWidget for more details
	std::optional<QSizeF> bestSize(DisplaySide dir, const CellInfo& cellInfo,
	    const QSizeF& sizeHint, const QSizeF& maxSize) const override;

	/// Icon accessors
	virtual void icon(QSvgIcon* ic);
	virtual const QSvgIcon* icon() const;

	/// Label accessors
	virtual void label( const std::string& label);
	virtual const std::string& label() const;

	/// Space around content
	qreal margin() const;
	virtual void margin( qreal margin );

	using QTopMenuWidget::direction;
	void direction( const DisplaySide d ) override;

signals:
	void clicked(const QPointF& cursorPos, const std::unordered_set<size_t>& clickableRectangleIds, QTopMenuButtonWidget* me); //TODO remove me argument
protected:

	/// overridable call to the static equivalent, so it can be override by child classes.
	virtual void drawControl( const QTopMenuButtonWidgetStyleOptions& opt, QPainter& p) const { staticDrawControl(opt, p); }
	/// Draw a QTopMenuButtonWidget widget. Static so it can be reused externally or overrided.
	static void staticDrawControl( const QTopMenuButtonWidgetStyleOptions&, QPainter& p);
	/// Intialize QTopMenuButtonWidgetStyleOptions with current widget
	virtual void initStyleOption(QTopMenuButtonWidgetStyleOptions&) const;

	//TODO document: static call allows non-childs to draw similar content (and eventually move it to a QSyle class)
	//    virtual method allows child class to call another implementation of that static call
	//    (because otherwise, internal implementation only call this static methdo, and never the override one.)
	virtual QRect textRect() const;
	static QRect textRect(const QRect& widgetRect, qreal margin, const QFontMetrics& metrics,
	    const QString& label, const QTopMenuBuggonWidgetLayout layout);

	virtual QRectF iconRect() const;
	static QRectF iconRect(const QRect& widgetRect, const QTopMenuBuggonWidgetLayout layout, const DisplaySide);

	virtual void setupFontForLabel( QFont& f) const { staticSetupFontForLabel(f); }
	static void staticSetupFontForLabel( QFont& );

	virtual QString elidedText( const QFontMetrics& metrics, const QTopMenuBuggonWidgetLayout layout,
	    const QString& str, const QSize& widgetSize, const qreal margin) const;

	virtual void recomputeSize();


	virtual QTopMenuBuggonWidgetLayout detectLayout(const QSize& size) const;

	QClickManager m_clickManager;
	bool m_clickPressed=false;
	bool m_hovered = false;

	std::string m_label;
	QSvgIcon* m_icon=nullptr;
	qreal m_margin = 2.0;
	QTopMenuBuggonWidgetLayout m_cachedLayout = QTopMenuBuggonWidgetLayout::Big;
	QStaticText m_staticText;
	QRect m_textRect;
	QRect m_iconRect;

	bool m_recomputeSizeNeeded = true;

	constexpr static qreal cornerRadius= 0.0;
	constexpr static int borderWidth = 0.5;
	
	bool event(QEvent* e) override;
	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *) override;
	void keyPressEvent(QKeyEvent *e) override;
	
};

}

#endif
