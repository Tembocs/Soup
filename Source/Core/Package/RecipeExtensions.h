﻿// <copyright file="RecipeExtensions.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "IFileSystem.h"
#include "RecipeJson.h"

namespace Soup
{
    /// <summary>
    /// The recipe extensions
    /// </summary>
    export class RecipeExtensions
    {
    public:
        /// <summary>
        /// Attempt to load from file
        /// </summary>
        static bool TryLoadFromFile(
            const Path& recipeFile,
            Recipe& result)
        {
            // Verify the requested file exists
            if (!IFileSystem::Current().Exists(recipeFile))
            {
                Log::Info("Recipe file does not exist.");
                return false;
            }

            // Open the file to read from
            auto file = IFileSystem::Current().OpenRead(recipeFile);

            // Read the contents of the recipe file
            try
            {
                result = RecipeJson::Deserialize(*file);
                return true;
            }
            catch (std::exception& ex)
            {
                Log::Trace(std::string("Deserialze Threw: ") + ex.what());
                Log::Info("Failed to parse Recipe.");
                return false;
            }
        }

        /// <summary>
        /// Build up the recipe file location from the package reference
        /// </summary>
        static Path GetPackageRecipeFile(
            const Path& workingDirectory,
            const PackageReference& package)
        {
            if (package.IsLocal())
            {
                auto recipeFile = workingDirectory + package.GetPath();
                return recipeFile;
            }
            else
            {
                throw std::runtime_error("Non-local packages not supported.");
            }
        }

        /// <summary>
        /// Save the recipe to file
        /// </summary>
        static void SaveToFile(
            const Path& recipeFile,
            Recipe& recipe)
        {
            // Open the file to write to
            auto file = IFileSystem::Current().OpenWrite(recipeFile);

            // Write the recipe to the file stream
            RecipeJson::Serialize(recipe, *file);
        }

        /// <summary>
        /// Get the recipe path
        /// </summary>
        static Path GetRecipeModulePath(const Path& packagePath, const Path& binaryDirectory, const std::string& modileFileExtension)
        {
            auto packageRecipePath = packagePath + Path(Constants::RecipeFileName);
            Recipe dependecyRecipe = {};
            if (!RecipeExtensions::TryLoadFromFile(packageRecipePath, dependecyRecipe))
            {
                Log::Error("Failed to load the dependency package: " + packageRecipePath.ToString());
                throw std::runtime_error("GetRecipeModulePath: Failed to load dependency.");
            }

            auto packageBinaryPath = packagePath + binaryDirectory;
            auto moduleFilename = Path(dependecyRecipe.GetName() + "." + modileFileExtension);
            auto modulePath = packageBinaryPath + moduleFilename;

            return modulePath;
        }

        /// <summary>
        /// Get the package reference path
        /// </summary>
        static Path GetPackageReferencePath(
            const Path& workingDirectory,
            const PackageReference& reference)
        {
            // If the path is relative then combine with the working directory
            auto packagePath = reference.GetPath();
            if (!packagePath.HasRoot())
            {
                packagePath = workingDirectory + packagePath;
            }

            return packagePath;
        }
    };
}