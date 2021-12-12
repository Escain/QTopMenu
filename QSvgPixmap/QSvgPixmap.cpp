// Copyright Adrian Maire, all right reserved
#include "QSvgPixmap.hpp"


#include <QBitmap>			// To replace the color by another
#include <QCoreApplication>	// Get current path for relative paths
#include <QDir>				// Manage relative paths for loading
#include <QImage>			// Required to render the svg
#include <QPainter>			// Required to render the svg

namespace Escain
{



	// Constructor & Destructor
	QSvgPixmap::QSvgPixmap
	( 
		const QString& filepath,
		const QSize& size,
		Stretch policy,
		const QColor& colorOverride
	)
		: QPixmap()
		, m_renderer( std::make_shared<QSvgRenderer>())
		, m_colorOverride(colorOverride)
	{
		m_stretch = policy;
		QString file = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(filepath);
		m_renderer->load(file);

		resize(size);
	}
	
	QSvgPixmap::QSvgPixmap( const QSvgPixmap& c, const QSize& size, 
		Stretch policy,const QColor& colorOverride)
		: QPixmap()
		, m_renderer( c.m_renderer)
		, m_stretch(policy)
		, m_colorOverride(colorOverride)
	{
		resize(size);
	}

	QSvgPixmap::QSvgPixmap
	(
		const QByteArray& data,
		const QSize& size,
		Stretch policy,
		const QColor& colorOverride
	)
		: QPixmap()
		, m_renderer( std::make_shared<QSvgRenderer>())
		, m_colorOverride(colorOverride)
	{
		m_stretch = policy;
		m_renderer->load(data);
		resize(size);
	}
	
	QSvgPixmap::QSvgPixmap( const QSvgPixmap&& c)
		: QPixmap()
		, m_renderer(std::move(c.m_renderer))
		, m_stretch(c.m_stretch)
		, m_colorOverride(c.m_colorOverride)
	{
		resize(c.size());
	}

	void QSvgPixmap::resize( const QSize& size)
	{
		if (!m_renderer || !m_renderer->isValid()) return;

		// Calculate the image size based on Stretch
		QSizeF imgSize = size;
		QSizeF svgSize = m_renderer->defaultSize();
		QRectF renderRect(QPointF(0.0,0.0), imgSize);
		if (m_stretch!=Stretch::Resized && !imgSize.isEmpty() && !svgSize.isEmpty())
		{
			double svgRatio = svgSize.width() / svgSize.height();
			double imgRatio = imgSize.width() / imgSize.height();

			if ((m_stretch==Stretch::Cover && svgRatio > imgRatio ) ||
				(m_stretch==Stretch::Contain && svgRatio <= imgRatio))
			{ // Adjust to Height
				renderRect.setSize(QSizeF(imgSize.height()*svgRatio, imgSize.height()));
				renderRect.moveTopLeft(QPointF((imgSize.width()-renderRect.width())*0.5,0.0));
			}
			else // Adjust to Width
			{
				renderRect.setSize(QSizeF(imgSize.width(), imgSize.width()/svgRatio));
				renderRect.moveTopLeft(QPointF(0.0, (imgSize.height()-renderRect.height())*0.5));
			}
		}

		if (!renderRect.isEmpty() && !imgSize.isEmpty())
		{
			QImage img(imgSize.toSize(),QImage::Format_ARGB32);
			img.fill(QColor(0,0,0,0));
			QPainter p(&img);
			p.setRenderHint(QPainter::Antialiasing, true);
			
			if (m_colorOverride.isValid())
			{
				p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	
				m_renderer->render(&p, renderRect);
				p.setCompositionMode(QPainter::CompositionMode_SourceIn);
				p.fillRect(renderRect, m_colorOverride);
			}
			else
			{
				m_renderer->render(&p, renderRect);
			}
				
			convertFromImage(img);
		}
	}
	
	bool QSvgPixmap::hasImage() const
	{
		return (m_renderer && m_renderer->isValid() && !m_renderer->defaultSize().isNull());
	}
}
