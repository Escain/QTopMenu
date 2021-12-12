// Copyright Adrian Maire, all right reserved

#ifndef QSVGPIXMAPCACHE_HPP
#define QSVGPIXMAPCACHE_HPP

#include <unordered_map>

#include "QSvgPixmap.hpp"
#include <QPaletteExt.hpp>

namespace Escain
{
/**
 * @brief QSvgPixmapCache
 *
 * QSvgPixmap are commonly used to print icons or logos, which in a user interface, need
 * to addapt to specific OS colors (QPalette), and to group/roles (disabled, hover, active, normal..)
 * 
 * This class bring a cache for a single svg image at a single rendering size, 
 * from a Group+Role -> QSvgPixmap. This makes redrawing much faster
 * at the expense of some memory.
 * 
 * Changing the OS color will triggers lazy renewal of the cache for each group/role.
 * 
 * Usually, a Cache finish by containing one pixmap for each role x group, usually 5-8 elements.
 * 
 * Note: Resizing the target pixmap will invalidate all the cache.
 */
class QSvgPixmapCache
{
public:

	// Default stuff
	QSvgPixmapCache() = default;
	QSvgPixmapCache( const QSvgPixmapCache& ) = default;
	virtual ~QSvgPixmapCache() = default;
	virtual QSvgPixmapCache& operator=( const QSvgPixmapCache&) = default;
	
	QSvgPixmapCache( 
		const QString& filepath,
		QSvgPixmap::Stretch policy=QSvgPixmap::Stretch::Contain);

	QSvgPixmapCache( 
		const QByteArray& data,
		QSvgPixmap::Stretch policy=QSvgPixmap::Stretch::Contain);
	
	/// get the required pixmap, eventually rendering it
	virtual const QPixmap& pixmapFor(const ColorRoleExt& role, 
		const QPaletteExt& palette, size_t id=0, bool throwIfEmpty=true) const;
	
	/// Clear the cache and store the new size for new requests
	virtual void resize( const QSize& size, size_t id=0);
	
	/// get the current size of cached icons
	virtual QSize size(size_t id=0) const;
	
	/// Manage ids, id==0 is present by default.
	virtual bool forgetId( size_t id ); // Return if found, meaning removed
	virtual bool declareId( size_t id); // Does nothing if already existing. Return if created.
	virtual bool idExists( size_t id) const;
	const std::vector<size_t> getAllIds() const; // O(n)
	
	/// Default constructed pixmap is empty, this function check if a pixmap has been set.
	virtual bool hasPixmap() const;

	virtual void colorOverride(bool enable);
	virtual bool colorOverride() const;

protected:

	struct QSvgPixmapCacheKey
	{
		ColorRoleExt role;
		QPalette::ColorGroup group;
		QSvgPixmapCacheKey()=default;
		QSvgPixmapCacheKey( const QSvgPixmapCacheKey&) = default;
		QSvgPixmapCacheKey( const ColorRoleExt& r, const QPalette::ColorGroup& c);
		bool operator==( const QSvgPixmapCacheKey& c)const;
	};
	
	struct QSvgPixmapCacheValue
	{
		QColor color;
		QSvgPixmap pixmap;
	};
	
	struct Hasher
	{
		size_t operator()(const QSvgPixmapCacheKey& v) const;
	};
	
	struct QSvgSizedCache
	{
		QSize size;
		std::unordered_map<QSvgPixmapCacheKey, QSvgPixmapCacheValue, Hasher> sizedCache;
	};
	
	// This is a cache, thus, make it mutable so we can access pixmaps through const functions.
	mutable std::unordered_map<size_t, QSvgSizedCache> m_cache = { std::make_pair(0ull, QSvgSizedCache{QSize(), {}}) };
	
	bool m_colorOverride=true;
	QSvgPixmap m_pixmap;
	QSvgPixmap::Stretch m_policy;
	static std::shared_ptr<QPixmap> m_default; //QPixmap cannot be created before QGuiApplication...
};

}


#endif //QSVGPIXMAPCACHE_HPP
