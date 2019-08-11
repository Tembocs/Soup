﻿// <copyright file="RecipeBuild.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "RecipeExtensions.h"

namespace Soup
{
    /// <summary>
    /// The recipe build that knows how to build a single recipe
    /// </summary>
    export class RecipeBuild
    {
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="RecipeBuild"/> class.
        /// </summary>
        RecipeBuild(std::shared_ptr<ICompiler> compiler) :
            _compiler(std::move(compiler))
        {
            if (_compiler == nullptr)
                throw std::runtime_error("Argument null: compiler");

            // Setup the output directories
            auto outputDirectory = Path("out");
            _objectDirectory = outputDirectory + Path("obj");
            _binaryDirectory = outputDirectory + Path("bin");
        }

        /// <summary>
        /// The Core Execute task
        /// </summary>
        void Execute(
            int projectId,
            const Path& workingDirectory,
            const Recipe& recipe,
            bool forceBuild)
        {
            Log::SetActiveId(projectId);
            Log::Info("Building '" + recipe.GetName() + "'");

            try
            {
                // Determine the include paths
                std::unordered_set<std::string> includePaths;
                for (auto& entry : std::filesystem::recursive_directory_iterator(workingDirectory.ToString()))
                {
                    if (entry.path().extension() == ".h")
                    {
                        includePaths.insert(
                            entry.path().parent_path().string());
                    }
                }

                // Add all dependency packages modules references
                auto includeModules = std::vector<Path>();
                for (auto dependecy : recipe.GetDependencies())
                {
                    auto packagePath = GetPackageReferencePath(workingDirectory, dependecy);
                    auto modulePath = GetRecipeModulePath(packagePath);
                    includeModules.push_back(std::move(modulePath));
                }

                // Add the dependency static library closure to link if targeting an executable
                std::vector<Path> linkLibraries;
                if (recipe.GetType() == RecipeType::Executable)
                {
                    GenerateDependecyStaticLibraryClosure(
                        workingDirectory,
                        recipe,
                        linkLibraries);
                }

                // Build up arguments to build this individual recipe
                auto arguments = BuildArguments();
                arguments.TargetName = recipe.GetName();
                arguments.WorkingDirectory = workingDirectory;
                arguments.ObjectDirectory = GetObjectDirectory();
                arguments.BinaryDirectory = GetBinaryDirectory();
                arguments.ModuleInterfaceSourceFile = 
                    recipe.HasPublic() ? recipe.GetPublicAsPath() : Path();
                arguments.SourceFiles = recipe.GetSourceAsPath();
                arguments.IncludeModules = std::move(includeModules);
                arguments.LinkLibraries = std::move(linkLibraries);
                arguments.IsIncremental = !forceBuild;
                arguments.PreprocessorDefinitions = std::vector<std::string>({
                    "SOUP_BUILD",
                });

                // Strip out the working directory from the include paths
                for (auto& entry : includePaths)
                {
                    auto entryPath = Path(entry);
                    auto directory = entryPath.GetRelativeTo(workingDirectory);
                    arguments.IncludeDirectories.push_back(directory);
                }

                // Convert the recipe type to the required build type
                switch (recipe.GetType())
                {
                    case RecipeType::Library:
                        arguments.TargetType = BuildTargetType::Library;
                        break;
                    case RecipeType::Executable:
                        arguments.TargetType = BuildTargetType::Executable;
                        break;
                    default:
                        throw std::runtime_error("Unknown build target type.");
                }

                // Perform the build
                auto buildEngine = BuildEngine(_compiler);
                buildEngine.Execute(arguments);
            }
            catch (std::exception& ex)
            {
                // Log the exception and convert to handled
                Log::Error(std::string("Build Failed: ") + ex.what());
                throw HandledException();
            }
        }

    private:
        void GenerateDependecyStaticLibraryClosure(
            const Path& workingDirectory,
            const Recipe& recipe,
            std::vector<Path>& closure) const
        {
            for (auto& dependecy : recipe.GetDependencies())
            {
                // Load this package recipe
                auto dependencyPackagePath = GetPackageReferencePath(workingDirectory, dependecy);
                auto packageRecipePath = dependencyPackagePath + Path(Constants::RecipeFileName);
                Recipe dependecyRecipe = {};
                if (!RecipeExtensions::TryLoadFromFile(packageRecipePath, dependecyRecipe))
                {
                    Log::Error("Failed to load the dependency package: " + packageRecipePath.ToString());
                    throw std::runtime_error("GenerateDependecyStaticLibraryClosure: Failed to load dependency.");
                }

                // Add this dependency
                auto dependencyStaticLibrary = 
                    dependencyPackagePath +
                    GetBinaryDirectory() +
                    Path(dependecyRecipe.GetName() + "." + std::string(_compiler->GetStaticLibraryFileExtension()));
                closure.push_back(std::move(dependencyStaticLibrary));

                // Add all recursive dependencies
                GenerateDependecyStaticLibraryClosure(dependencyPackagePath, dependecyRecipe, closure);
            }
        }

        Path GetObjectDirectory() const
        {
            return _objectDirectory + Path(_compiler->GetName());
        }

        Path GetBinaryDirectory() const
        {
            return _binaryDirectory + Path(_compiler->GetName());
        }

        Path GetRecipeModulePath(const Path& packagePath) const
        {
            auto packageRecipePath = packagePath + Path(Constants::RecipeFileName);
            Recipe dependecyRecipe = {};
            if (!RecipeExtensions::TryLoadFromFile(packageRecipePath, dependecyRecipe))
            {
                Log::Error("Failed to load the dependency package: " + packageRecipePath.ToString());
                throw std::runtime_error("GetRecipeModulePath: Failed to load dependency.");
            }

            auto packageBinaryPath = packagePath + GetBinaryDirectory();
            auto moduleFilename = Path(dependecyRecipe.GetName() + "." + std::string(_compiler->GetModuleFileExtension()));
            auto modulePath = packageBinaryPath + moduleFilename;

            return modulePath;
        }

        Path GetPackageReferencePath(const Path& workingDirectory, const PackageReference& reference) const
        {
            // If the path is relative then combine with the working directory
            auto packagePath = reference.GetPath();
            if (!packagePath.HasRoot())
            {
                packagePath = workingDirectory + packagePath;
            }

            return packagePath;
        }

    private:
        std::shared_ptr<ICompiler> _compiler;
        Path _objectDirectory;
        Path _binaryDirectory;
    };
}
