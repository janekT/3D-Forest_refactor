/*
    Copyright 2020 VUKOZ

    This file is part of 3D Forest.

    3D Forest is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    3D Forest is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 3D Forest.  If not, see <https://www.gnu.org/licenses/>.
*/

/** @file Error.hpp */

#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdexcept>
#include <string>

/** Throw std exception. */
#define THROW(msg) throw std::runtime_error(msg)

/** Throw std exception with the last error. */
#define THROW_ERRNO(msg) throw std::runtime_error(getErrorString(msg))

/** Get error string for given error number. */
std::string getErrorString(int errnum);

/** Get the last error string. */
std::string getErrorString();

/** Get the last error string with extra description. */
std::string getErrorString(const std::string &message);

#endif /* ERROR_HPP */
