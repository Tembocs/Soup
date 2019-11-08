﻿// <copyright file="BuildCommand.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "ICommand.h"
#include "BuildOptions.h"

namespace Soup::Client
{
	/// <summary>
	/// Build Command
	/// </summary>
	class BuildCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="BuildCommand"/> class.
		/// </summary>
		BuildCommand(BuildOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Trace("BuildCommand::Run");

			// Load the user config
			auto config =  LocalUserConfigExtensions::LoadFromFile();

			auto systemCompiler = std::make_shared<Compiler::Clang::Compiler>(
				Path(config.GetClangToolPath()));
			std::shared_ptr<ICompiler> runtimeCompiler = nullptr;
			if (config.GetRuntimeCompiler() == "clang")
			{
				runtimeCompiler = std::make_shared<Compiler::Clang::Compiler>(
					Path(config.GetClangToolPath()));
			}
			else if (config.GetRuntimeCompiler() == "msvc")
			{
				runtimeCompiler = std::make_shared<Compiler::MSVC::Compiler>(
					Path(config.GetMSVCRootPath()) + Path("bin/Hostx64/x64/"),
					Path("cl.exe"),
					Path("link.exe"),
					Path("lib.exe"));
			}
			else
			{
				throw std::runtime_error("Unknown compiler.");
			}

			auto workingDirectory = Path();
			if (_options.Path.empty())
			{
				// Buildin the current directory
				workingDirectory = Path::GetCurrentDirectory();
			}
			else
			{
				workingDirectory = Path(_options.Path);

				// Check if this is relative to current directory
				if (!workingDirectory.HasRoot())
				{
					workingDirectory = Path::GetCurrentDirectory() + workingDirectory;
				}
			}
			
			auto recipePath = 
				workingDirectory +
				Path(Constants::RecipeFileName);
			Recipe recipe = {};
			if (!RecipeExtensions::TryLoadFromFile(recipePath, recipe))
			{
				Log::Error("Could not load the recipe file.");
				return;
			}

			// Setup the build arguments
			auto arguments = RecipeBuildArguments();
			arguments.ForceRebuild = _options.Force;
			if (!_options.Configuration.empty())
				arguments.Configuration = _options.Configuration;
			else
				arguments.Configuration = "release";

			// TODO: Hard coded to windows MSVC runtime libraries
			arguments.PlatformIncludePaths = std::vector({
				Path(config.GetMSVCRootPath()) + Path("include/"),
				Path(config.GetWindowsSDKIncludesPath()) + Path("ucrt/"),
			});
			arguments.PlatformLibraryPaths = std::vector({
				Path(config.GetMSVCRootPath()) + Path("lib/x64/"),
				Path(config.GetWindowsSDKLibrariesPath()) + Path("ucrt/x64/"),
				Path(config.GetWindowsSDKLibrariesPath()) + Path("um/x64/"),
			});

			// Now build the current project
			Log::Verbose("Begin Build:");
			auto buildManager = RecipeBuildManager(systemCompiler, runtimeCompiler);
			buildManager.Execute(workingDirectory, recipe, arguments);
		}

	private:
		BuildOptions _options;
	};
}
