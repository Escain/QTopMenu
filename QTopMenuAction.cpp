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

#include "QTopMenuAction.hpp"
#include "QTopMenuWidget.hpp"

namespace Escain
{

void QTopMenuAction::nameId(const std::string& newId)
{
	if (m_name != newId)
	{
		m_name=newId;

		for( auto& widgetPtr: m_widgetVector)
		{
			if (widgetPtr)
			{
				widgetPtr->nameId(m_name);
			}
		}
	}
}
	
}
