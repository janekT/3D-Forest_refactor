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

/**
    @file las.cpp
*/

#include <Aabb.hpp>
#include <Error.hpp>
#include <LasFile.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>

enum Command
{
    COMMAND_NONE,
    COMMAND_CREATE_INDEX,
    COMMAND_FORMAT,
    COMMAND_PRINT,
    COMMAND_RANDOMIZE,
    COMMAND_SELECT
};

void getarg(size_t *v, int &opt, int argc, char *argv[])
{
    opt++;
    if (opt < argc)
    {
        *v = std::stoul(argv[opt]);
    }
}

void getarg(double *v, int &opt, int argc, char *argv[])
{
    opt++;
    if (opt < argc)
    {
        *v = std::stod(argv[opt]);
    }
}

void getarg(const char **v, int &opt, int argc, char *argv[])
{
    opt++;
    if (opt < argc)
    {
        *v = argv[opt];
    }
}

void cmd_create_index(const char *outputPath,
                      const char *inputPath,
                      size_t maxLeafSize1,
                      size_t maxLeafSize2)
{
    if (!inputPath)
    {
        THROW("Missing input file path argument");
    }

    if (!outputPath)
    {
        outputPath = inputPath;
    }

    LasFile::index(outputPath, inputPath, maxLeafSize1, maxLeafSize2);
}

void cmd_format(const char *outputPath, const char *inputPath)
{
    if (!inputPath)
    {
        THROW("Missing input file path argument");
    }

    if (!outputPath)
    {
        outputPath = inputPath;
    }

    LasFile::format(outputPath, inputPath);
}

void cmd_print(const char *inputPath)
{
    if (!inputPath)
    {
        THROW("Missing input file path argument");
    }

    LasFile::print(inputPath);
}

void cmd_randomize(const char *outputPath, const char *inputPath)
{
    if (!inputPath)
    {
        THROW("Missing input file path argument");
    }

    if (!outputPath)
    {
        outputPath = inputPath;
    }

    LasFile::randomize(outputPath, inputPath);
}

void cmd_select(const char *inputPath, const Aabb<double> &window)
{
    if (!inputPath)
    {
        THROW("Missing input file path argument");
    }

    (void)window;
}

int main(int argc, char *argv[])
{
    int command = COMMAND_NONE;
    size_t maxLeafSize1 = 10000;
    size_t maxLeafSize2 = 32;
    double wx1 = 0, wy1 = 0, wz1 = 0, wx2 = 0, wy2 = 0, wz2 = 0;
    Aabb<double> window;
    const char *outputPath = nullptr;
    const char *inputPath = nullptr;

    // Parse command line arguments
    for (int opt = 1; opt < argc; opt++)
    {
        // Command
        if (strcmp(argv[opt], "-c") == 0)
        {
            command = COMMAND_CREATE_INDEX;
        }
        else if (strcmp(argv[opt], "-f") == 0)
        {
            command = COMMAND_FORMAT;
        }
        else if (strcmp(argv[opt], "-p") == 0)
        {
            command = COMMAND_PRINT;
        }
        else if (strcmp(argv[opt], "-r") == 0)
        {
            command = COMMAND_RANDOMIZE;
        }
        else if (strcmp(argv[opt], "-s") == 0)
        {
            command = COMMAND_SELECT;
        }

        // Create index options
        else if (strcmp(argv[opt], "-m1") == 0)
        {
            getarg(&maxLeafSize1, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-m2") == 0)
        {
            getarg(&maxLeafSize2, opt, argc, argv);
        }

        // Input/Output filenames
        else if (strcmp(argv[opt], "-i") == 0)
        {
            getarg(&inputPath, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-o") == 0)
        {
            getarg(&outputPath, opt, argc, argv);
        }

        // Selection box
        else if (strcmp(argv[opt], "-x1") == 0)
        {
            getarg(&wx1, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-y1") == 0)
        {
            getarg(&wy1, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-z1") == 0)
        {
            getarg(&wz1, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-x2") == 0)
        {
            getarg(&wx2, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-y2") == 0)
        {
            getarg(&wy2, opt, argc, argv);
        }
        else if (strcmp(argv[opt], "-z2") == 0)
        {
            getarg(&wz2, opt, argc, argv);
        }
    }

    // Execute command
    try
    {
        switch (command)
        {
            case COMMAND_CREATE_INDEX:
                cmd_create_index(outputPath,
                                 inputPath,
                                 maxLeafSize1,
                                 maxLeafSize2);
                break;
            case COMMAND_FORMAT:
                cmd_format(outputPath, inputPath);
                break;
            case COMMAND_PRINT:
                cmd_print(inputPath);
                break;
            case COMMAND_RANDOMIZE:
                cmd_randomize(outputPath, inputPath);
                break;
            case COMMAND_SELECT:
                window.set(wx1, wy1, wz1, wx2, wy2, wz2);
                cmd_select(inputPath, window);
                break;
            case COMMAND_NONE:
            default:
                THROW("Unknown command");
                break;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}