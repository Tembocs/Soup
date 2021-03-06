﻿// <copyright file="InitializeCommand.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Soup.Client
{
    using System;
    using System.IO;
    using System.Threading.Tasks;

    /// <summary>
    /// Initialize Command
    /// </summary>
    internal class InitializeCommand
    {
        /// <summary>
        /// Invoke
        /// </summary>
        public async Task InvokeAsync(InitializeOptions options)
        {
            Log.Info("The initialize utility will walk through the creation of the most basic Console recipe.\n");

            // Use the current directory as the default names
            var currentDirectory = new DirectoryInfo(Directory.GetCurrentDirectory());

            var recipe = new Recipe(
                currentDirectory.Name)
            {
                Version = new SemanticVersion(1, 0, 0)
            };

            Log.Info($"Name: ({recipe.Name}) ");
            var newName = Console.ReadLine();
            if (!string.IsNullOrWhiteSpace(newName))
            {
                recipe.Name = newName;
            }

            bool setVersion = false;
            while (!setVersion)
            {
                Log.Info($"Version: ({recipe.Version}) ");
                var newVersion = Console.ReadLine();
                if (string.IsNullOrEmpty(newVersion))
                {
                    // Use the default
                    setVersion = true;
                }
                else
                {
                    if (SemanticVersion.TryParse(newVersion, out SemanticVersion value))
                    {
                        recipe.Version = value;
                        setVersion = true;
                    }
                    else
                    {
                        Log.Warning($"Invalid version: \"{newVersion}\"");
                    }
                }
            }

            // Save the state of the recipe if it has changed
            await RecipeManager.SaveToFileAsync(recipe);
        }
    }
}
