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

#ifndef QTOPMENUACTION_HPP
#define QTOPMENUACTION_HPP

#include <memory>
#include <string>
#include <vector>

class QWidget;

namespace Escain
{

class QTopMenuWidget;

/// Interface to to create custom top-menu entities. 
/// The top-menu entities instances, then, are kind of a factory for copies of the same
/// feature button/widget. E.g. A top-menu "button" instance would represent the feature "save", 
/// which can produce several "save" QWidgets buttons for different tabs in the Top-Menu.
class QTopMenuAction
{
public:
	virtual ~QTopMenuAction() = default;
	
	/// Generate a new QWidget, e.g. a new "save" button to put in any/many Top-Menu tabs
	virtual std::shared_ptr<QTopMenuWidget> createWidget( QWidget* parent=nullptr )=0;
	
	///Name: an id to programatically identify this QTopMenuAction
	inline const std::string& nameId() const {return m_name; }
	virtual void nameId( const std::string& id );

protected:

	/// List of cloned widgets representing this Action
	std::vector<std::shared_ptr<QTopMenuWidget>> m_widgetVector;
private:
	/// The identifier for the Library
	std::string m_name;
};

}

#endif
