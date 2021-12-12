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

#ifndef QTOPMENUWIDGET_HPP
#define QTOPMENUWIDGET_HPP

#include <QWidget>

#include "QTopMenuWidgetTypes.hpp"

#include <optional>

namespace Escain
{

/// This interface is a QWiget which can be inserted in QTopMenuGrid. 
/// It is intended to be cloned by a QTopMenuAction
class QTopMenuWidget: public QWidget
{
	Q_OBJECT
public:
	explicit QTopMenuWidget( const std::string& nameId, size_t id, QWidget* parent=nullptr );
	virtual ~QTopMenuWidget() = default;
	
	/// Get best Size for this widget given a sizeHint, Grid cell infor and grid direction
	/// It is strongly recommended to keep alignment with the Grid when possible, especially
	/// perpendicular to the direction (height for Horizontal, width for Vertical).
	/// @param dir: the direction of the container layout: vertical/horizontal
	/// @param cellInfo: The widget must try to align to that size.
	/// @param sizeHint: The widget will choose among possible sizes, the one closest to sizeHint
	/// @param maxSize: The maximum acceptable size. The best size must be smaller or equal.
	/// @return the best available size for the widget.
	virtual std::optional<QSizeF> bestSize(
		DisplaySide dir,
		const CellInfo& cellInfo,
	const QSizeF& sizeHint,
	const QSizeF& maxSize
		) const = 0;

	///Direction: Set if the widget are arranged horizontally or vertically
	inline DisplaySide direction() const { return m_direction; }
	virtual void direction( const DisplaySide d );

	///Name: an id to programatically identify this widget to it QTopMenuAction
	inline const std::string& nameId() const { return m_name; }
	virtual void nameId( const std::string& id );

	/// identifier of this widget among QTopMenuAction widgets
	inline size_t id() const { return m_id; }

signals:
	// After the interaction, this signal should be called to indicate the QTopMenu can fade the
	// popup containing this widget (if collapsed).
	void fadePopup();

	/// Emitted when the size of the widget requires to be requested and applied.
	void bestSizeChanged();
		
protected:
	static std::optional<QSizeF> bestSizeBetweenPossibles( const std::vector<QSizeF>& possibles,
	const QSizeF& hint, const QSizeF& maxSize);

private:
	DisplaySide m_direction = DisplaySide::Top;
	std::string m_name;
	const size_t m_id=0;
};
}

#endif
