// Copyright Adrian Maire, all right reserved

#include "QSvgPixmapCache.hpp"

#include <QFile>

namespace Escain
{

std::shared_ptr<QPixmap> QSvgPixmapCache::m_default;

size_t QSvgPixmapCache::Hasher::operator()(const QSvgPixmapCacheKey& v) const
{
	return (static_cast<size_t>(v.role)&0xffffffffull) + 
		((static_cast<size_t>(v.group)&0xffffffffull)<<32ull); 
}

QSvgPixmapCache::QSvgPixmapCacheKey::QSvgPixmapCacheKey( const ColorRoleExt& r, const QPalette::ColorGroup& c)
	: role(r)
	, group(c)
{
}

bool QSvgPixmapCache::QSvgPixmapCacheKey::operator==( const QSvgPixmapCacheKey& c) const
{
	return c.role==role && c.group==group; 
}

const QPixmap& QSvgPixmapCache::pixmapFor(const ColorRoleExt& role, 
	const QPaletteExt& palette, size_t id, bool throwIfEmpty) const
{
	const auto sizeIt = m_cache.find(id);
	if ( sizeIt == m_cache.cend())
	{
		assert(false);
		throw std::runtime_error("Accessing QSvgPixmapCache for unknown id. First declare it.");
	}
	
	const auto color = palette.color(role);
	const QSvgPixmapCacheKey key(role, palette.currentColorGroup());
	auto& sizedCache = sizeIt->second.sizedCache;
	const auto& size = sizeIt->second.size;
	
	const auto mapIt = sizedCache.find(key);

	if (mapIt == sizedCache.cend() || (m_colorOverride && color != mapIt->second.color))
	{
		if (mapIt != sizedCache.cend())
		{
			sizedCache.erase(mapIt);
		}

		if (hasPixmap())
		{
			sizedCache.emplace(key, QSvgPixmapCacheValue{color, QSvgPixmap(m_pixmap, size,
			    m_policy, m_colorOverride ? color: QColor() )});
		}
		else
		{
			if (throwIfEmpty)
			{
				assert(false);
				throw std::runtime_error("No file is loaded to generate pixmaps.");
			}
		}
	}
	
	const auto retIt = sizedCache.find(key);
	if (retIt!=sizedCache.cend())
	{
		assert(!retIt->second.pixmap.isNull());
		return retIt->second.pixmap;
	}
	if (!m_default)
	{
		m_default = std::make_shared<QPixmap>();
	}
	return *m_default;
}

void QSvgPixmapCache::colorOverride(bool enable)
{
	if (enable != m_colorOverride)
	{
		m_colorOverride = enable;

		// Clear all caches
		for (auto& [id, cache]: m_cache)
		{
			cache.sizedCache.clear();
		}
	}
}

bool QSvgPixmapCache::colorOverride() const
{
	return m_colorOverride;
}

bool QSvgPixmapCache::hasPixmap() const
{
	return (m_pixmap.hasImage());
}


QSvgPixmapCache::QSvgPixmapCache
( 
	const QString& filePath,
	QSvgPixmap::Stretch policy
): m_policy(policy)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		throw std::runtime_error("File not accessible.");
	}
	m_pixmap = QSvgPixmap(file.readAll(), QSize(), policy);
}

QSvgPixmapCache::QSvgPixmapCache
( 
	const QByteArray& data,
	QSvgPixmap::Stretch policy
): m_policy(policy)
{
	m_pixmap = QSvgPixmap(data, QSize(), policy);
}

void QSvgPixmapCache::resize( const QSize& size, size_t id)
{
	assert(size.isValid());
	const auto sizeIt = m_cache.find(id);
	if ( sizeIt == m_cache.cend())
	{
		assert(false);
		throw std::runtime_error("Accessing QSvgPixmapCache for unknown id. First declare it.");
	}
	
	if (size != sizeIt->second.size)
	{
		sizeIt->second.sizedCache.clear();
	}
	sizeIt->second.size = size;
}

QSize QSvgPixmapCache::size(size_t id) const
{
	const auto sizeIt = m_cache.find(id);
	if ( sizeIt == m_cache.cend())
	{
		assert(false);
		throw std::runtime_error("Accessing QSvgPixmapCache for unknown id. First declare it.");
	}
	return sizeIt->second.size;
}


bool QSvgPixmapCache::forgetId( size_t id )
{
	const auto sizeIt = m_cache.find(id);
	if (sizeIt != m_cache.cend())
	{
		m_cache.erase(sizeIt);
		return true;
	}
	return false;
}

bool QSvgPixmapCache::declareId( size_t id)
{
	const auto sizeIt = m_cache.find(id);
	if (sizeIt == m_cache.cend())
	{
		m_cache.insert( std::make_pair(id, QSvgSizedCache{QSize(), {}}));
		return true;
	}
	return false;
}

bool QSvgPixmapCache::idExists( size_t id) const
{
	const auto sizeIt = m_cache.find(id);
	return (sizeIt != m_cache.cend());
}

const std::vector<size_t> QSvgPixmapCache::getAllIds() const
{
	std::vector<size_t> result;
	result.reserve(m_cache.size());
	for (const auto& p: m_cache)
	{
		result.push_back(p.first);
	}
    return result;
}

}
