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

#include "QClickManager.hpp"

#include <QDebug> //TODO remove

namespace Escain
{

//*////////////////////////////////////////////////////////////////////////////////////////////////
int QClickManager::threshold( int newValue )
{
	m_clickThreshold = newValue;
	return m_clickThreshold;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
int QClickManager::threshold() const
{
	return m_clickThreshold;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
bool QClickManager::enabled() const
{
	return m_enabled;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
bool QClickManager::enabled( bool en )
{
	m_enabled = en;
	return m_enabled;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
void QClickManager::enableHover(const QWidget& wid)
{
	m_hoverWidget = &wid;
	manageHover(wid.mapFromGlobal(QCursor::pos()));
}
void QClickManager::disableHover()
{
	m_hoverWidget = nullptr;
}
bool QClickManager::isHoverEnabled() const
{
	return nullptr!=m_hoverWidget;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
void QClickManager::addClickableRectangle( size_t id, const QRect& r)
{
	m_clickableRectangles.insert(std::make_pair(id, r));
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
void QClickManager::removeClickableRectangle( size_t id)
{
	m_clickableRectangles.erase(id);
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
std::unordered_set<size_t> QClickManager::getAllClickableRectangleIds() const
{
	std::unordered_set<size_t> ids;
	for (const auto& p: m_clickableRectangles)
	{
		ids.insert(p.first);
	}
	return ids;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
void QClickManager::clickableRectangleById( size_t id, const QRect& r)
{
	auto it = m_clickableRectangles.find(id);
	if (it != m_clickableRectangles.cend())
	{
		it->second = r;
		return;
	}
	
	// not found
	throw std::out_of_range("Provided 'id' not found.");
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
const QRect& QClickManager::clickableRectangleById( size_t id) const
{
	auto it = m_clickableRectangles.find(id);
	if (it != m_clickableRectangles.cend())
	{
		return it->second;
	}
	
	// not found
	throw std::out_of_range("Provided 'id' not found.");
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
bool QClickManager::eventHandler( QEvent* e )
{
	if(e->type() == QEvent::Leave)
	{
		m_clickInProgress = false;
		m_candidateRectanglesForClick.clear();

		if (m_hoverWidget)
		{
			// when the parent widget is smaller than this m_hoverWidget,
			//    the cursor can fall inside a valid rect-> we need to hover-out
			//    and avoid to re-hover, thus, the isLeave boolean to force hover-out.
			manageHover(QPoint(), true);
		}
	}
	else if(e->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);

		if (mouseEvent->button() != Qt::LeftButton)
		{
			return false;
		}

		QPoint cursorPos = mouseEvent->pos();
		if ( m_clickInProgress)
		{
			// Filter those containig the cursor position
			filterRectanglesContainingTheClick( cursorPos );

			emit pressed(cursorPos, false, m_candidateRectanglesForClick);

			// Check the mouse did not moved away from the press position
			if ( !((cursorPos-m_mousePressPosition).manhattanLength()>m_clickThreshold))
			{
				if (m_enabled)
				{
					emit clicked(cursorPos, m_candidateRectanglesForClick);
				}
			}
			m_candidateRectanglesForClick.clear();
			m_clickInProgress = false;
			return true;
		}
	}
	else if(e->type() == QEvent::MouseMove)
	{
		if (m_clickInProgress)
		{
			// Filter those containig the cursor position
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
			QPoint cursorPos = mouseEvent->pos();
			filterRectanglesContainingTheClick( cursorPos );

			// Check the mouse did not moved away from the press position
			if ( (cursorPos-m_mousePressPosition).manhattanLength()>m_clickThreshold)
			{
				m_clickInProgress = false;
				m_candidateRectanglesForClick.clear();
			}

			m_clickInProgress = m_clickInProgress && (m_clickableRectangles.empty() ||
				!m_candidateRectanglesForClick.empty());

			if (!m_clickInProgress)
			{
				if (m_enabled)
				{
					emit pressed(cursorPos, false, m_candidateRectanglesForClick);
				}
			}

			return true;
		}
		else if (m_hoverWidget)
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
			QPoint cursorPos = mouseEvent->pos();
			manageHover(cursorPos);
		}
	}
	else if(e->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);

		if (mouseEvent->button() != Qt::LeftButton)
		{
			return false;
		}

		// Reset the list of containing rectangles
		m_candidateRectanglesForClick.clear();
		m_candidateRectanglesForClick.reserve( m_clickableRectangles.size());
		for ( auto it: m_clickableRectangles )
		{
			m_candidateRectanglesForClick.insert(it.first);
		}
		
		// Filter those containig the cursor position
		QPoint cursorPos = mouseEvent->pos();
		filterRectanglesContainingTheClick( cursorPos );
		m_mousePressPosition = cursorPos;

		m_clickInProgress = (m_clickableRectangles.empty() || !m_candidateRectanglesForClick.empty());
		
		if ( m_clickInProgress )
		{
			if ( m_enabled)
			{
				emit pressed(cursorPos, true, m_candidateRectanglesForClick);
			}
			return true;
		}
	}
	else if(e->type() == QEvent::Enter && m_hoverWidget)
	{
		QPoint cursorPos = m_hoverWidget->mapFromGlobal(QCursor::pos());
		manageHover(cursorPos);
	}
	
	return false;
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
void QClickManager::manageHover( const QPoint& curPos, bool isLeave )
{

	assert(m_hoverWidget);
	const auto widSize = m_hoverWidget->size();

	// first un-hover then, hover.
	for (auto it = m_clickableRectangles.begin(); it!=m_clickableRectangles.cend(); ++it)
	{
		const auto& rect = it->second.intersected(QRect(QPoint(0,0), widSize));

		if (isLeave || !(rect.contains(curPos)))
		{
			auto hoveredIt = m_hoveredRectangles.find(it->first);
			if (hoveredIt != m_hoveredRectangles.cend())
			{
				//hover end
				m_hoveredRectangles.erase(hoveredIt);
				if (m_enabled)
				{
					emit hovered(curPos, false, it->first, it->second);
				}
			}
		}
	}
	// Manage un-hover when clickable rectangle list is empty -> use the full widget rectangle
	if (m_hoveredNoRectangles && (!m_hoverWidget->rect().contains(curPos) || isLeave) )
	{
		//hover begin
		m_hoveredNoRectangles = false;
		if(m_clickableRectangles.empty()) // avoid to send events if a click rectangle is registered in the while
		{
			emit hovered(curPos, false, std::numeric_limits<size_t>::max(), m_hoverWidget->rect());
		}
	}

	if (!isLeave)
	{
		for (auto it = m_clickableRectangles.begin(); it!=m_clickableRectangles.cend(); ++it)
		{
			const auto& rect = it->second.intersected(QRect(QPoint(0,0), widSize));

			if (rect.contains(curPos))
			{
				if (m_hoveredRectangles.find(it->first) == m_hoveredRectangles.cend())
				{
					//hover begin
					if (m_enabled)
					{
						m_hoveredRectangles.emplace(it->first);
						emit hovered(curPos, true, it->first, it->second);
					}
				}
			}
		}

		// Manage empty clickable rectangle list -> use the full widget rectangle
		if (m_clickableRectangles.empty() && m_hoveredRectangles.empty() && !m_hoveredNoRectangles)
		{
			if (m_hoverWidget->rect().contains(curPos))
			{
				//hover begin
				if (m_enabled)
				{
					m_hoveredNoRectangles = true;
					emit hovered(curPos, true, std::numeric_limits<size_t>::max(), m_hoverWidget->rect());
				}
			}
		}
	}
}

//*////////////////////////////////////////////////////////////////////////////////////////////////
void QClickManager::filterRectanglesContainingTheClick(const QPoint& cursorPos)
{
	for (auto idIt = m_candidateRectanglesForClick.begin(); 
			idIt!=m_candidateRectanglesForClick.end();
		/*increase in loop*/)
	{
		size_t id = *idIt;
		
		bool keepIt = true;
		auto it = m_clickableRectangles.find(id);
		if ( it == m_clickableRectangles.cend())
		{
			keepIt = false;
		}
		else if ( !it->second.contains( cursorPos ))
		{
			keepIt = false;
		}
		
		if ( !keepIt )
		{
			idIt = m_candidateRectanglesForClick.erase(idIt);
		}
		else
		{
			++idIt;
		}
	}
}

}
