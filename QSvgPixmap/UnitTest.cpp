 
#include <iostream>
#include <QDebug>
#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>
#include <QStyleOptionFrame>
#include <QPaintEvent>
#include <QPainter>
#include "QSvgPixmap.hpp"


namespace Escain
{
class SimpleImage: public QWidget
{
	Q_OBJECT


	QSvgPixmap pix;
public:
	SimpleImage( const QString &path, QSvgPixmap::Stretch stretch, QWidget *parent=nullptr):
		  QWidget(parent)
		, pix(path, size(), stretch)
	{}

	void paintEvent(QPaintEvent *e) override
	{
		QStyleOptionFrame opt;
		opt.initFrom(this);

		QPainter p(this);
		p.setClipRect(e->rect());
		p.setRenderHint(QPainter::Antialiasing );
		
		p.setBrush(QColor(128,128,128,64));
		p.drawRect(rect());

		p.drawPixmap(rect(),pix);
	}

	void resizeEvent(QResizeEvent *) override
	{
		pix.resize(size());
	}
	
	void setColorOverride( const QColor& c){pix.colorOverride(c);}
};
}
#include "UnitTest.moc"




int main (int argn, char *argv[])
{
	QApplication app( argn, argv);
	
	QWidget window;
	QHBoxLayout layout(&window);

	Escain::SimpleImage
			img1("test1.svg", Escain::QSvgPixmap::Stretch::Contain, &window),
			img2("test1.svg", Escain::QSvgPixmap::Stretch::Cover, &window),
			img3("test1.svg", Escain::QSvgPixmap::Stretch::Resized, &window),
			img4("test1.svg", Escain::QSvgPixmap::Stretch::Contain, &window);
	img4.setColorOverride(QColor(128,192,224));
	layout.addWidget(&img1);
	layout.addWidget(&img2);
	layout.addWidget(&img3);
	layout.addWidget(&img4);

	window.resize(800,200);
	window.show();

	
	return app.exec();
}
