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

#ifndef QPALETTE_EXT_HPP
#define QPALETTE_EXT_HPP

#include <QPalette>

namespace Escain
{

enum class ColorRoleExt
{
	// Background color
	Background_Normal = 1,
	Background_Hover,
	Background_Pressed,
	BackgroundMid_Normal, // Slightly darker

	// Text, when rendered over standard widget background
	TextOverBackground_Normal,
	TextOverBackground_Hover,
	TextOverBackground_Pressed,

	// borders, lines, arrows
	LinesOverBackground_Normal,
	LinesOverBackground_Hover,
	LinesOverBackground_Pressed,

	// Text placeholder over inputs
	PlaceholderText_Normal,
	PlaceholderText_Hover,
	PlaceholderText_Pressed,

	// Selections, higlighting..
	Highlight_Normal,
	Highlight_Hover,
	Highlight_Pressed
	
	//Note: disabled, active, inactive is set by the group
};

/**
 * @brief Util to generate more colors for a given palette. 
 * DON'T SAVE the result: OS can change the palette at any moment, each drawing
 * must retrieve the palette, not get a saved one.
 */
class QPaletteExt: public QPalette
{
protected:
	constexpr static qreal m_darkSchemeThreshold = 0.5;
	bool m_isDarkTheme=false;
public:
	
	// Default constructor/destructor/..
	QPaletteExt() = default;
	QPaletteExt( const QPalette& p): QPalette(p) { m_isDarkTheme = isDarkTheme(); }
	QPaletteExt( const QPaletteExt& ) = default;
	~QPaletteExt() = default;
	
	QPaletteExt& operator=( const QPaletteExt& ) = default;
	bool operator==( const QPaletteExt& o) const { return QPalette::operator==(o);}
	bool operator!=( const QPaletteExt& o) const { return QPalette::operator!=(o);}
	
	
	
	
	const QBrush brush(ColorRoleExt role) const
	{
		return color(role);
	}
	const QColor color(ColorRoleExt role) const
	{
		switch (role)
		{
		case ColorRoleExt::Background_Normal: return QPalette::color(QPalette::Button);
		case ColorRoleExt::Background_Hover: return blendColor(QPalette::color(QPalette::Button), QPalette::color(QPalette::Highlight), 0.1);
		case ColorRoleExt::Background_Pressed: return blendColor(QPalette::color(QPalette::Button), QPalette::color(QPalette::Highlight), 0.2);
		case ColorRoleExt::BackgroundMid_Normal: return blendColor(QPalette::color(QPalette::Mid), QPalette::color(QPalette::Button), 0.6);

		case ColorRoleExt::TextOverBackground_Normal: return QPalette::color(QPalette::ButtonText);
		case ColorRoleExt::TextOverBackground_Hover: return QPalette::color(QPalette::ButtonText);
		case ColorRoleExt::TextOverBackground_Pressed: return QPalette::color(QPalette::Text);

		case ColorRoleExt::LinesOverBackground_Normal: return QPalette::color(QPalette::Mid);
		case ColorRoleExt::LinesOverBackground_Hover: return QPalette::color(QPalette::Mid);
		case ColorRoleExt::LinesOverBackground_Pressed: return QPalette::color(QPalette::Mid);

		case ColorRoleExt::PlaceholderText_Normal: return QPalette::color(QPalette::PlaceholderText);
		case ColorRoleExt::PlaceholderText_Hover: return QPalette::color(QPalette::PlaceholderText);
		case ColorRoleExt::PlaceholderText_Pressed: return QPalette::color(QPalette::PlaceholderText);

		case ColorRoleExt::Highlight_Normal: return QPalette::color(QPalette::Highlight);
		case ColorRoleExt::Highlight_Hover: return QPalette::color(QPalette::Highlight);
		case ColorRoleExt::Highlight_Pressed: return QPalette::color(QPalette::Highlight);
		default:
			throw std::runtime_error("Not yet implemented: " + std::to_string(static_cast<int>(role)));
		}
	}
	
	/// @brief detect if the palette is corresponding to a dark theme (white text on dark background)
	bool isDarkTheme() const
	{
		QColor bg = QPalette::window().color();
		QColor text = QPalette::text().color();
		
		qreal bgGrey = (bg.redF()+bg.greenF() + bg.blueF());
		qreal textGrey = (text.redF()+text.greenF()+text.blueF());
		if (bgGrey/m_darkSchemeThreshold > textGrey) return false;
		return true;
	}

	static QColor blendColor( const QColor& color1, const QColor& color2, qreal prop)
	{
		QColor result;
		qreal antProp = 1.0-prop;
		result.setRedF (color1.redF()*antProp + color2.redF()*prop);
		result.setGreenF (color1.greenF()*antProp + color2.greenF()*prop);
		result.setBlueF (color1.blueF()*antProp + color2.blueF()*prop);
		result.setAlphaF (color1.alphaF()*antProp + color2.alphaF()*prop);
		return result;
	}

};


}


#endif //QTOPMENU_HPP
