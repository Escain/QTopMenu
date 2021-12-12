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

#ifndef QCLICKMANAGER_HPP
#define QCLICKMANAGER_HPP

#include <cassert>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>


namespace Escain
{

/**
 * @brief Utility to manage clicks: it reads press/unpress events on widget and generate clicks 
 * A click is defined as follow:
 * - A press event inside a clickable area
 * - The pointer is not moved further than a threshold
 * - the pointer does not exit the clickable area
 * - A release event
 * 
 * The manager allows to define several clickable areas. Each scope the click to a zone.
 * The manager allows to define a threshold: the maximum displacement of the mouse while clicking.
 * The manager detect the press, move and release events, and ouput the click event when required.
 * 
 * For simplification, if no rectangle is defined, it trigger click on the full widget, exactly
 * as if a single area is defined and ajusted to the geometry of the widget. The id set is provided 
 * empty to the signal.
 * 
 * In case of overlapping click areas, the click event is generated as soon as the click is 
 * valid for one of the areas: that is, at least one area match all conditions for a click (press,
 * release, threshold, etc).
 */
class QClickManager: public QObject
{
	Q_OBJECT
public:
	/// Constructor, the parameter qobject is merely for QObject initialization
	explicit QClickManager( QObject* parent=nullptr): QObject(parent){}
	
	/// Setter for mouse-click thershold, indicates the maximum allowed movement of
	/// the mouse while clicking (over this, it is considered either drag/drop or nothing)
	int threshold( int newValue );
	/// Threshold getter (see the setter for more info)
	int threshold() const;
	
	/// Define a clickable rectangle with an id, if already existing, it override it
	void addClickableRectangle( size_t id, const QRect& r);
	/// Remove a clickable rectangle, if missing, throw an exception
	void removeClickableRectangle( size_t id);
	/// Get the list of all registered ids
	std::unordered_set<size_t> getAllClickableRectangleIds() const;
	/// set the click area for the given Id, if it does not exist, it throw out-of-bound
	void clickableRectangleById( size_t id, const QRect& r);
	/// Constant getter for the area given it id. If it does not exist, it throw out-of-bound
	const QRect& clickableRectangleById( size_t id ) const;
	
	/// Even handler, must be called by the "bool event(QEvent*) override" of the widget
	bool eventHandler( QEvent* e );
	
	/// Enable or disable the ClickManager, effectively avoiding to emit signals if disabled
	bool enabled() const;
	bool enabled( bool en );

	/// Considering that signaling hover has a cost, it is disabled by default.
	/// Enable Hover detection to get it.
	/// To detect the initial state, the widget in question is required.
	void enableHover(const QWidget& wid);
	void disableHover();
	bool isHoverEnabled() const;
	
protected:
	/// internal representation of the clickable areas
	std::unordered_map<size_t, QRect> m_clickableRectangles;
	/// Mouse initial press position, for threshold comparison
	QPoint m_mousePressPosition;
	/// Threshold displacement of the cursor while clicking, in manhattan distance
	int m_clickThreshold = 10;
	/// List of clickable areas ids candidates for the click event. (between the press and release event)
	std::unordered_set<size_t> m_candidateRectanglesForClick;
	/// List of ongoing hovered rectangles
	std::unordered_set<size_t> m_hoveredRectangles;
	bool m_hoveredNoRectangles = false; // when no rectangles are set, use widget full rectangle
	/// Remove from _candidateRectanglesForClick those areas which are not animore candidates for a click event.
	void filterRectanglesContainingTheClick(const QPoint& cursorPos);
	/// Update the list of hovered rectangles and eventually emit when relevant
	///    isLeave allows to force hover-out
	void manageHover( const QPoint& localCursorPos, bool isLeave=false );
	/// If a click is in progress
	bool m_clickInProgress = false;
	/// Manage if the QClickManager is enabled or disabled. When disabled it does not emit
	bool m_enabled = true;
	/// The widget expected to receive hover events, null if Hover is disabled
	const QWidget* m_hoverWidget=nullptr;
	
signals: 
	/// Signal for an effective click
	/// the cursor pos is the latest position, the clickableRectangleIds is the list of candidates areas
	void clicked(const QPoint& cursorPos, const std::unordered_set<size_t>& clickableRectangleIds);
	
	/// Signals wether the mouse press an area: on button down, it call the signal with pressed=true
	/// while when any action that cancel the click, or that triggers the click, it release the pressed state.
	/// pressed signal is rised before clicked.
	void pressed(const QPoint& cursorPos, bool pressed, const std::unordered_set<size_t>& clickableRectangleIds);

	/// Trigers when a clickableRectangle get or lose hover
	/// It is called for each change on each rectangle.
	void hovered(const QPoint& cursorPos, bool hovered, const size_t hoveredRectangleId, const QRect& rect);
};


}


#endif //QCLICKMANAGER_HPP
