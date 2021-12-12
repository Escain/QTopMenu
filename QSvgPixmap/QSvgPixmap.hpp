// Copyright Adrian Maire, all right reserved

#ifndef QSVGPIXMAP_HPP
#define QSVGPIXMAP_HPP

#include <memory>

#include <QSvgRenderer>		// Required to load/render the svg
#include <QPixmap>			// Base class

namespace Escain
{


//IMPROVE:
// Alignment
// Transformation
// Animation

/**
 * @brief QSvgPixmap
 *
 * Convenience extended behavior for Pixmaps, allowing to load svg of specific size and
 * resize rendering.
 * ColorOverride allow to draw the full pixmap in a specific color (set it to QColor() to clear)
 */
class QSvgPixmap: public QPixmap
{
public:

	enum class Stretch
	{
		Cover,
		Contain,
		Resized
	};
	
private:
	std::shared_ptr<QSvgRenderer> m_renderer; // pointer so that QSvgPixmap is movable
	Stretch m_stretch;
	QColor m_colorOverride;
public:

	// Constructor & Destructor
	QSvgPixmap() = default;
	
	QSvgPixmap( const QSvgPixmap&, // Copies are sharing the QSvgRenderer
				const QSize& size = QSize(),
				Stretch policy=Stretch::Contain,
				const QColor& colorOverride = QColor());

	QSvgPixmap( const QString& filepath,
				const QSize& size = QSize(),
				Stretch policy=Stretch::Contain,
				const QColor& colorOverride = QColor());

	QSvgPixmap( const QByteArray& data,
				const QSize& size = QSize(),
				Stretch policy=Stretch::Contain,
				const QColor& colorOverride = QColor());
	
	QSvgPixmap& operator= ( const QSvgPixmap& c) = default; // Copies are sharing the QSvgRenderer
	QSvgPixmap( const QSvgPixmap&& c);
	
	/// This return if there is an image to render (even if the current size is QSize()).
	/// this is different from QPixmap::isNull() which return false if size()==QSize().
	virtual bool hasImage() const;
	
	virtual void resize( const QSize& size);
	
	const QColor& colorOverride() const {return m_colorOverride; }
	QColor& colorOverride( const QColor& c) { m_colorOverride = c; resize(size()); return m_colorOverride; }
	
	const Stretch& stretch() const { return m_stretch; }
	Stretch& stretch( const Stretch& c) { m_stretch = c; resize(size()); return m_stretch; }

};


}


#endif //QSVGPIXMAP_HPP
