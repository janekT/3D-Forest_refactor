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

/** @file EditorBase.cpp */

#include <EditorBase.hpp>
#include <FileIndexBuilder.hpp>
#include <unordered_set>

static const char *EDITOR_BASE_KEY_PROJECT_NAME = "projectName";
static const char *EDITOR_BASE_KEY_DATA_SET = "dataSets";
static const char *EDITOR_BASE_KEY_LAYER = "layers";
static const char *EDITOR_BASE_KEY_SETTINGS = "settings";
static const char *EDITOR_BASE_KEY_CLASSIFICATION = "classifications";
// static const char *EDITOR_BASE_KEY_CLIP_FILTER = "clipFilter";

EditorBase::EditorBase() : working_(this)
{
    close();
    setNumberOfViewports(1);
}

EditorBase::~EditorBase()
{
}

void EditorBase::close()
{
    path_ = File::currentPath() + "\\untitled.json";
    projectName_ = "Untitled";

    dataSets_.clear();
    layers_.clear();
    clipFilter_.clear();
    classification_.clear();

    boundary_.clear();
    boundaryView_.clear();

    for (auto &it : viewports_)
    {
        it->clear();
    }
    working_.clear();

    unsavedChanges_ = false;
}

void EditorBase::open(const std::string &path)
{
    close();

    Json in;
    in.read(path);

    if (!in.isObject())
    {
        THROW("Project file '" + path + "' is not in JSON object");
    }

    try
    {
        size_t i;
        size_t n;

        path_ = path;

        // Project name
        if (in.contains(EDITOR_BASE_KEY_PROJECT_NAME))
        {
            projectName_ = in[EDITOR_BASE_KEY_PROJECT_NAME].string();
        }

        // Data sets
        if (in.contains(EDITOR_BASE_KEY_DATA_SET))
        {
            i = 0;
            n = in[EDITOR_BASE_KEY_DATA_SET].array().size();
            dataSets_.resize(n);

            for (auto const &it : in[EDITOR_BASE_KEY_DATA_SET].array())
            {
                dataSets_[i] = std::make_shared<EditorDataSet>();
                dataSets_[i]->read(it, path_);
                i++;
            }
        }

        // Layers
        if (in.contains(EDITOR_BASE_KEY_LAYER))
        {
            layers_.read(in[EDITOR_BASE_KEY_LAYER]);
        }

        // Settings
        if (in.contains(EDITOR_BASE_KEY_SETTINGS))
        {
            settings_.read(in[EDITOR_BASE_KEY_SETTINGS]);
        }

        // Classifications
        if (in.contains(EDITOR_BASE_KEY_CLASSIFICATION))
        {
            classification_.read(in[EDITOR_BASE_KEY_CLASSIFICATION]);
        }

        // Clip filter
        // if (in.contains(EDITOR_BASE_KEY_CLIP_FILTER))
        // {
        //     clipFilter_.read(in[EDITOR_BASE_KEY_CLIP_FILTER]);
        // }
        // else
        // {
        //     clipFilter_.clear();
        // }
    }
    catch (std::exception &e)
    {
        close();
        throw;
    }

    openUpdate();
}

void EditorBase::write(const std::string &path)
{
    Json out;
    size_t i;

    // Project name
    out[EDITOR_BASE_KEY_PROJECT_NAME] = projectName_;

    // Data sets
    i = 0;
    for (auto const &it : dataSets_)
    {
        it->write(out[EDITOR_BASE_KEY_DATA_SET][i]);
        i++;
    }

    // Layers
    layers_.write(out[EDITOR_BASE_KEY_LAYER]);

    // Settings
    settings_.write(out[EDITOR_BASE_KEY_SETTINGS]);

    // Classifications
    classification_.write(out[EDITOR_BASE_KEY_CLASSIFICATION]);

    // Clip filter
    // clipFilter_.write(out[EDITOR_BASE_KEY_CLIP_FILTER]);

    out.write(path);

    unsavedChanges_ = false;
}

void EditorBase::addFile(const std::string &path, bool center)
{
    try
    {
        // Data sets
        std::shared_ptr<EditorDataSet> ds = std::make_shared<EditorDataSet>();
        ds->id = freeDataSetId();
        ds->read(path, path_);
        dataSets_.push_back(ds);

        if (center)
        {
            Vector3<double> c1 = boundary_.getCenter();
            Vector3<double> c2 = ds->boundaryFile.getCenter();
            c1[2] = boundary_.min(2);
            c2[2] = ds->boundaryFile.min(2);
            ds->translation = c1 - c2;
            ds->updateBoundary();
        }
        else
        {
            Vector3<double> s = 1.0 / ds->scalingFile;
            ds->translation = ds->translationFile * s;
            ds->updateBoundary();
        }
    }
    catch (std::exception &e)
    {
        throw;
    }

    openUpdate();
    unsavedChanges_ = true;
}

size_t EditorBase::freeDataSetId() const
{
    // Collect all ids
    std::unordered_set<size_t> hashTable;
    for (auto &it : dataSets_)
    {
        hashTable.insert(it->id);
    }

    // Return minimum available id value
    for (size_t rval = 0; rval < std::numeric_limits<size_t>::max(); rval++)
    {
        if (hashTable.find(rval) == hashTable.end())
        {
            return rval;
        }
    }

    THROW("New data set identifier is not available.");
}

bool EditorBase::hasFileIndex(const std::string &path)
{
    std::string pathFile = EditorDataSet::resolvePath(path, path_);
    std::string pathIndex = FileIndexBuilder::extension(pathFile);
    return File::exists(pathIndex);
}

void EditorBase::addFilter(EditorFilter *filter)
{
    filters_.push_back(filter);
}

void EditorBase::applyFilters(EditorTile *tile)
{
    for (auto &it : filters_)
    {
        /** @todo Collect enabled filters during preprocessing. */
        if (it->isFilterEnabled())
        {
            it->filterTile(tile);
        }
    }
}

void EditorBase::setVisibleDataSet(size_t i, bool visible)
{
    dataSets_[i]->visible = visible;
    unsavedChanges_ = true;
}

void EditorBase::setLayers(const EditorLayers &layers)
{
    layers_ = layers;
    unsavedChanges_ = true;
}

void EditorBase::setClassification(const EditorClassification &classification)
{
    classification_ = classification;
    // resetRendering();
    unsavedChanges_ = true;
}

void EditorBase::setClipFilter(const ClipFilter &clipFilter)
{
    clipFilter_ = clipFilter;
    clipFilter_.boxView.setPercent(boundaryView_, boundary_, clipFilter_.box);

    // unsavedChanges_ = true;
}

void EditorBase::resetClipFilter()
{
    clipFilter_.box = boundary_;
    setClipFilter(clipFilter_);
}

void EditorBase::setSettingsView(const EditorSettings::View &settings)
{
    settings_.setView(settings);
    resetRendering();
    unsavedChanges_ = true;
}

void EditorBase::select(std::vector<FileIndex::Selection> &selected)
{
    Aabb<double> box = selection();

    for (auto const &it : dataSets_)
    {
        if (it->visible)
        {
            it->index.selectNodes(selected, box, it->id);
        }
    }
}

Aabb<double> EditorBase::selection() const
{
    if (clipFilter_.enabled)
    {
        return clipFilter_.box;
    }

    return boundary_;
}

void EditorBase::setNumberOfViewports(size_t n)
{
    size_t i = viewports_.size();

    while (i < n)
    {
        std::shared_ptr<EditorCache> viewport =
            std::make_shared<EditorCache>(this);
        viewports_.push_back(viewport);
        i++;
    }

    while (n < i)
    {
        viewports_.pop_back();
        i--;
    }
}

void EditorBase::updateCamera(size_t viewport, const Camera &camera)
{
    viewports_[viewport]->updateCamera(camera);
}

void EditorBase::tileViewClear()
{
    for (auto &it : viewports_)
    {
        it->reload();
    }
}

bool EditorBase::loadView()
{
    bool rval = true;

    for (auto &it : viewports_)
    {
        if (!it->loadStep())
        {
            rval = false;
        }
    }

    return rval;
}
// -----------------------------------------------------------------------------
void EditorBase::openUpdate()
{
    updateBoundary();

    // if (clipFilter_.box.empty())
    // {
    //     clipFilter_.box = boundary_;
    // }
    clipFilter_.box = boundary_;
    clipFilter_.boxView = boundaryView_;
}

void EditorBase::updateBoundary()
{
    boundary_.clear();
    boundaryView_.clear();

    for (auto const &it : dataSets_)
    {
        if (it->visible)
        {
            boundary_.extend(it->boundary);
            boundaryView_.extend(it->boundaryView);
        }
    }
}

void EditorBase::resetRendering()
{
    for (auto &it : viewports_)
    {
        it->resetRendering();
    }
}
