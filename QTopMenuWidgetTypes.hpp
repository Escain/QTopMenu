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

#ifndef QTOPMENUWIDGETTYPES_HPP
#define QTOPMENUWIDGETTYPES_HPP

namespace Escain
{

/// Indicates if the widget is intended for a Horizontal container layout (habitual)
/// or a Vertical container layout
enum class DisplaySide
{
	Top,
	Left
};

/// The container layout tries to align widgets to a grid. This grid is made of cells of
/// the given size and with given space between.
struct CellInfo
{
	/// The contaier is made of a grid, this is the side of each cell
	double cellSize;
	/// The border applied between cells in the grid. (applied only once)
	double margin;
	/// Miximum number of cells vertically (for Horizontal direction), horizontally (for Vertical)
	size_t transversalCellNum;
};

}

#endif
