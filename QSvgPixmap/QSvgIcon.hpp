// Copyright Adrian Maire, all right reserved

#ifndef QSVGICON_HPP
#define QSVGICON_HPP

#include <string_view>

#include "QSvgPixmapCache.hpp"

namespace Escain
{
/**
 * @brief QSvgIcon
 *
 * Implement an icon intended for buttons.
 * - The icon implement two layers of image: 
 *        1) background image which is coloured with OS text color
 *        2) foreground image which can (or not) be coloured with OS highlighting text color
 * 
 * It is recommended to use only background color for less important features
 * It is recommended to use background and colored foreground for the few important features
 * 
 * Note: paint() function assume the palette group is set correctly, especially for "disabled".
 *       It is not always set properly by the widget!!
 */
class QSvgIcon
{
public:
	QSvgIcon();
	QSvgIcon( const QSvgIcon& ) = default;
	virtual ~QSvgIcon() = default;
	virtual QSvgIcon& operator=( const QSvgIcon&) = default;
	
	/// Paint the icon with the given palette, rect and painter
	void paint( QPainter& p, const QRect& r, const QPaletteExt& pal,
		bool pressed, bool hovered, size_t id=0) const;
	
	/// Resize the intended QSvgIcon. This has significant cost
	virtual void resize( const QSize& size, size_t id=0);
	
	/// get the current size of cached icons
	virtual QSize size(size_t id=0) const;
	
	/// Return if there is an image (not default constructed)
	virtual bool hasIcon() const;
	
	/// Manage ids, id==0 is present by default.
	virtual bool forgetId( size_t id ); // Return if found, meaning removed
	virtual bool declareId( size_t id); // Does nothing if already existing. Return if created.
	virtual bool idExists( size_t id) const;
	
	/// Access the background image
	void iconBackground(QSvgPixmapCache&& newPixmapCache);
	const QSvgPixmapCache& iconBackground() const;
	
	/// Access the foreground image
	void iconForeground(QSvgPixmapCache&& newPixmapCache);
	const QSvgPixmapCache& iconForeground() const;
	
	/// Access the margin around the image
	void margin( const int newMargin);
	int margin() const;
protected:
	/// Default icon, if not filled
	static constexpr std::string_view noIconSvg();

	QSvgPixmapCache m_iconBackground;
	QSvgPixmapCache m_iconForeground;
	
	using LastSizePerIdType = std::unordered_map<size_t,QSize>;
	LastSizePerIdType m_lastSizePerId = {{0ull, QSize()}};
	
	int m_marginWidth=0;
};
}
#endif //QSVGICON_HPP
