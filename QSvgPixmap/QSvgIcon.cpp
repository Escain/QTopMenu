// Copyright Adrian Maire, all right reserved

#include "QSvgIcon.hpp"

#include <QByteArray>
#include <QPen>
#include <QPainter>
#include <QPaletteExt.hpp>

namespace Escain
{

constexpr bool isDebug()
{
#ifndef NDEBUG
	return true;
#else
	return false;
#endif
}

constexpr std::string_view QSvgIcon::noIconSvg()
{
	return R"DELIM(<?xml version="1.0" encoding="UTF-8"?>
	<svg xmlns="http://www.w3.org/2000/svg" width="32.0pt" height="32.0pt"
	  viewBox="0 0 8.25 8.25" version="1.1">
	  <g>
		<path
		  style="fill:#b3009a;stroke:none;"
		  d="m 2.6398789,2.7918893 -1.058865,-2.26e-5 c 0,0 -0.1628634,-2.59975999
			2.6458253,-2.62841645 C 5.7200029,0.14821601 6.4718587,1.368083 6.379103,2.5446462
			6.2848652,3.740009 5.2065967,4.4844932 4.763184,4.9356877 4.1399299,5.5698798
			4.2299216,6.5236725 4.2299216,6.5236725 l -1.0672537,-0.00393 c 0,0
			-0.1744359,-1.2606303 0.872781,-2.3215195 C 4.7464236,3.4779619 5.2144142,3.2261491
			5.2946557,2.5865086 5.3671213,2.0088527 5.096479,1.2212843 4.229438,1.2040355
			2.5594812,1.1708138 2.6398789,2.7918893 2.6398789,2.7918893 Z" />
		<rect
		  style="fill:#b3009a;"
		  width="1.0693687"
		  height="1.0596982"
		  x="3.1754689"
		  y="7.1995859"/>
	  </g>
	</svg>)DELIM";
}

QSvgIcon::QSvgIcon()
{
	m_iconForeground = QSvgPixmapCache(QByteArray{noIconSvg().data()});
	m_iconForeground.colorOverride(false);
}

void QSvgIcon::paint( QPainter& p, const QRect& r, const QPaletteExt& pal, 
	bool pressed, bool hovered, size_t id) const
{
	QPen border = Qt::NoPen;
	ColorRoleExt roleText = ColorRoleExt::TextOverBackground_Normal;
	ColorRoleExt roleHighlight = ColorRoleExt::Highlight_Normal;
	if ( pressed )
	{
		roleText = ColorRoleExt::TextOverBackground_Pressed;
		roleHighlight = ColorRoleExt::Highlight_Pressed;
	}
	else if (hovered)
	{
		roleText = ColorRoleExt::TextOverBackground_Hover;
		roleHighlight = ColorRoleExt::Highlight_Hover;
	}

	const QRect iconRect(r.x()+m_marginWidth, r.y()+m_marginWidth,
		r.width()-2.0*m_marginWidth, r.height()-2.0*m_marginWidth);

	if constexpr (isDebug())
	{
		const auto sizeIt = m_lastSizePerId.find(id);
		if(sizeIt == m_lastSizePerId.cend())
		{
			throw std::runtime_error("Drawing a QSvgIcon with unregistered id");
		}

		if (sizeIt->second != r.size())
		{
			throw std::runtime_error("Drawing a QSvgIcon with a rect (" +
			    std::to_string(r.width()) +"x" + std::to_string(r.height())+
			    ") different from it size (" + std::to_string(sizeIt->second.width()) + "x" +
			    std::to_string(sizeIt->second.height()) + ")");
		}
	}

	if (m_iconBackground.hasPixmap())
	{
		const QPixmap& pixmapBg = m_iconBackground.pixmapFor(roleText, pal, id);
		p.drawPixmap(iconRect, pixmapBg);
	}

	if (m_iconForeground.hasPixmap())
	{
		const QPixmap& pixmapFg = m_iconForeground.pixmapFor(roleHighlight, pal, id);
		p.drawPixmap(iconRect, pixmapFg);
	}
}

void QSvgIcon::margin( const int newMargin )
{
	m_marginWidth = newMargin;
	for (const auto& pairs: m_lastSizePerId)
	{
		if (pairs.second.isValid()) // avoid triggering resize with uninitialized size.
		{
			resize(pairs.second, pairs.first);
		}
	}
}

int QSvgIcon::margin() const
{
	return m_marginWidth;
}

void QSvgIcon::resize( const QSize& size, size_t id)
{
	assert(size.isValid());
	const auto sizeIt = m_lastSizePerId.find(id);
	if ( sizeIt == m_lastSizePerId.cend())
	{
		assert(false);
		throw std::runtime_error("Accessing QSvgIcon for unknown id. First declare it.");
	}

	const QSize innerSize( size.width()-2*m_marginWidth, size.height()-2*m_marginWidth);
	if (m_iconBackground.size(id) != innerSize)
	{
		m_iconBackground.resize(innerSize, id);
		m_iconForeground.resize(innerSize, id);
	}
	sizeIt->second = size;
}

QSize QSvgIcon::size(size_t id) const
{
	const auto it = m_lastSizePerId.find(id);
	if (it == m_lastSizePerId.cend())
	{
		return QSize();
	}
	return it->second;
}

bool QSvgIcon::hasIcon() const
{
	return m_iconBackground.hasPixmap() || m_iconForeground.hasPixmap();
}

void QSvgIcon::iconBackground(QSvgPixmapCache&& newPixmapCache)
{
	m_iconBackground = newPixmapCache;
}

const QSvgPixmapCache& QSvgIcon::iconBackground() const
{
	return m_iconBackground;
}

void QSvgIcon::iconForeground(QSvgPixmapCache&& newPixmapCache)
{
	m_iconForeground = newPixmapCache;
}

const QSvgPixmapCache& QSvgIcon::iconForeground() const
{
	return m_iconForeground;
}


bool QSvgIcon::forgetId( size_t id )
{
	bool ret1 = m_iconBackground.forgetId(id);
	bool ret2 = m_iconForeground.forgetId(id);
	
	m_lastSizePerId.erase(id);
	
	return ret1 && ret2;
}
bool QSvgIcon::declareId( size_t id)
{
	bool ret1 = m_iconBackground.declareId(id);
	bool ret2 = m_iconForeground.declareId(id);
	
	m_lastSizePerId.insert(std::make_pair(id, QSize()));
	
	return ret1 && ret2;
}
bool QSvgIcon::idExists( size_t id) const
{
	return m_iconBackground.idExists(id) && m_iconForeground.idExists(id);
}

}
