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

/** @file EditorDataSet.hpp */

#ifndef EDITOR_DATA_SET_HPP
#define EDITOR_DATA_SET_HPP

#include <Aabb.hpp>
#include <FileIndex.hpp>
#include <Json.hpp>
#include <Vector3.hpp>
#include <string>

/** Editor Data Set. */
class EditorDataSet
{
public:
    // Stored
    Vector3<double> translation;
    Vector3<double> scaling;

    std::string pathUnresolved;
    std::string dateCreated; /**< Inconsistent with LAS in shared projects */
    std::string label;       /**< Inconsistent with LAS in shared projects */
    size_t id;
    bool visible;

    // Derived
    std::string path;
    std::string fileName;

    // Data
    FileIndex index;
    Vector3<double> translationFile;
    Vector3<double> scalingFile;
    Aabb<double> boundaryFile;
    Aabb<double> boundary;
    Aabb<double> boundaryView;

    EditorDataSet();
    ~EditorDataSet();

    void read(const std::string &filePath, const std::string &projectPath);
    void read(const Json &in, const std::string &projectPath);
    Json &write(Json &out) const;

    void updateBoundary();

    static std::string resolvePath(const std::string &unresolved,
                                   const std::string &projectPath);

protected:
    void setPath(const std::string &unresolved, const std::string &projectPath);
    void read();
};

#endif /* EDITOR_DATA_SET_HPP */
