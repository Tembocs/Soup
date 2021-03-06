﻿// <copyright file="Compiler.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Soup.Compiler.MSVC
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Threading.Tasks;

    /// <summary>
    /// The MSVC compiler implementation
    /// </summary>
    public class Compiler : ICompiler
    {
        private string _toolsPath;
        private string _windowsTargetVersion;

        private string CompilerPath => Path.Combine(_toolsPath, "bin/Hostx64/x64/cl.exe");
        private string LibrarianPath => Path.Combine(_toolsPath, "bin/Hostx64/x64/lib.exe");
        private string LinkerPath => Path.Combine(_toolsPath, "bin/Hostx64/x64/link.exe");
        private string WindowsKitsPath => "C:/Program Files (x86)/Windows Kits";

        /// <summary>
        /// Gets the unique name for the compiler
        /// </summary>
        public string Name => "MSVC";

        /// <summary>
        /// Gets the object file extension for the compiler
        /// </summary>
        public string ObjectFileExtension => "obj";

        /// <summary>
        /// Gets the module file extension for the compiler
        /// </summary>
        public string ModuleFileExtension => "ifc";

        /// <summary>
        /// Gets the static library file extension for the compiler
        /// TODO: This is platform specific
        /// </summary>
        public string StaticLibraryFileExtension => "lib";

        /// <summary>
        /// Initializes a new instance of the <see ref="Compiler" /> class
        /// </summary>
        public Compiler(string toolsPath, string windowsTargetVersion)
        {
            _toolsPath = toolsPath;
            _windowsTargetVersion = windowsTargetVersion;
        }

        /// <summary>
        /// Compile
        /// </summary>
        public Task<CompileResults> CompileAsync(CompileArguments args)
        {
            // Set the working directory to the output directory
            var workingDirectory = args.RootDirectory;

            var commandArgs = BuildCompilerArguments(args, workingDirectory);

            Log.Verbose($"PWD={workingDirectory}");
            Log.Verbose($"{CompilerPath} {commandArgs}");

            using (Process process = new Process())
            {
                process.StartInfo.UseShellExecute = false;
                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.FileName = CompilerPath;
                process.StartInfo.WorkingDirectory = workingDirectory;
                process.StartInfo.Arguments = commandArgs;
                process.Start();

                while (!process.StandardOutput.EndOfStream)
                {
                    string line = process.StandardOutput.ReadLine();
                    ProcessLine(line);
                }

                process.WaitForExit();

                if (process.ExitCode != 0)
                {
                    throw new InvalidOperationException();
                }

                return Task.FromResult(new CompileResults());
            }
        }

        /// <summary>
        /// Link library
        /// </summary>
        public Task LinkLibraryAsync(LinkerArguments args)
        {
            // Set the working directory to the output directory
            var workingDirectory = args.RootDirectory;

            var librarianArgs = BuildLibrarianArguments(args);

            Log.Verbose($"PWD={workingDirectory}");
            Log.Verbose($"{LibrarianPath} {librarianArgs}");

            using (Process process = new Process())
            {
                process.StartInfo.UseShellExecute = false;
                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.FileName = LibrarianPath;
                process.StartInfo.WorkingDirectory = workingDirectory;
                process.StartInfo.Arguments = librarianArgs;
                process.Start();

                while (!process.StandardOutput.EndOfStream)
                {
                    string line = process.StandardOutput.ReadLine();
                    ProcessLine(line);
                }

                process.WaitForExit();

                if (process.ExitCode != 0)
                {
                    throw new InvalidOperationException();
                }

                return Task.CompletedTask;
            }
        }

        /// <summary>
        /// Link Executable
        /// </summary>
        public Task LinkExecutableAsync(LinkerArguments args)
        {
            // Set the working directory to the output directory
            var workingDirectory = args.RootDirectory;
            var linkerArgs = BuildLinkerExecutableArguments(args);

            Log.Verbose($"PWD={workingDirectory}");
            Log.Verbose($"{LinkerPath} {linkerArgs}");

            using (Process process = new Process())
            {
                process.StartInfo.UseShellExecute = false;
                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.FileName = LinkerPath;
                process.StartInfo.WorkingDirectory = workingDirectory;
                process.StartInfo.Arguments = linkerArgs;
                process.Start();

                while (!process.StandardOutput.EndOfStream)
                {
                    string line = process.StandardOutput.ReadLine();
                    ProcessLine(line);
                }

                process.WaitForExit();

                if (process.ExitCode != 0)
                {
                    throw new InvalidOperationException();
                }

                return Task.CompletedTask;
            }
        }

        private string BuildCompilerArguments(CompileArguments args, string rootPath)
        {
            var commandArgs = new List<string>();

            // Disable the logo text
            commandArgs.Add("-nologo");

            // Set the language standard
            switch (args.Standard)
            {
                case LanguageStandard.CPP11:
                    throw new NotSupportedException("11 is not supported any longer. 14 is now the default.");
                case LanguageStandard.CPP14:
                    // Default value commandArgs.Add("-std:c++14");
                    break;
                case LanguageStandard.CPP17:
                    commandArgs.Add("-std:c++17");
                    break;
                case LanguageStandard.Latest:
                    commandArgs.Add("-std:c++latest");
                    commandArgs.Add("-experimental:module");
                    break;
                default:
                    throw new NotSupportedException("Unknown language standard.");
            }

            // TODO: For now we allow exports to be large
            commandArgs.Add("/bigobj");

            // Only run preprocess, compile, and assemble steps
            commandArgs.Add("-c");

            // Set the include paths
            foreach (var directory in args.IncludeDirectories)
            {
                commandArgs.Add($"-I\"{directory}\"");
            }

            // Set the preprocessor definitions
            foreach (var definition in args.PreprocessorDefinitions)
            {
                commandArgs.Add($"-D{definition}");
            }

            // Ignore Standard Include Paths to prevent pulling in accidental headers
            commandArgs.Add("-X");

            // Add in the std include paths
            // TODO : May want to place behind flag
            // TODO : Investigate placing these as environment variables before calling exe
            // C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\ATLMFC\include
            // C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\include
            // C:\Program Files(x86)\Windows Kits\NETFXSDK\4.6.1\include\um;
            // C:\Program Files (x86)\Windows Kits\10\include\10.0.17134.0\ucrt
            // C:\Program Files (x86)\Windows Kits\10\include\10.0.17134.0\shared
            // C:\Program Files (x86)\Windows Kits\10\include\10.0.17134.0\um
            // C:\Program Files (x86)\Windows Kits\10\include\10.0.17134.0\winrt
            // C:\Program Files (x86)\Windows Kits\10\include\10.0.17134.0\cppwinrt
            commandArgs.Add($"-I\"{Path.Combine(_toolsPath, "ATLMFC/include")}\"");
            commandArgs.Add($"-I\"{Path.Combine(_toolsPath, "include")}\"");
            commandArgs.Add($"-I\"{Path.Combine(WindowsKitsPath, "10/include", _windowsTargetVersion, "ucrt")}\"");
            commandArgs.Add($"-I\"{Path.Combine(WindowsKitsPath, "10/include", _windowsTargetVersion, "shared")}\"");
            commandArgs.Add($"-I\"{Path.Combine(WindowsKitsPath, "10/include", _windowsTargetVersion, "um")}\"");
            commandArgs.Add($"-I\"{Path.Combine(WindowsKitsPath, _windowsTargetVersion, _windowsTargetVersion, "winrt")}\"");
            commandArgs.Add($"-I\"{Path.Combine(WindowsKitsPath, "10/include", _windowsTargetVersion, "cppwinrt")}\"");

            // Add the object output file
            var objectPath = args.OutputDirectory.EnsureTrailingSlash().Replace(@"\", @"\\");
            commandArgs.Add($"-Fo\"{objectPath}\"");

            // Enable c++ exceptions
            commandArgs.Add("-EHs");

            // Add the module references
            foreach (var module in args.Modules)
            {
                commandArgs.Add("-module:reference");
                commandArgs.Add(Path.Combine(rootPath, module));
            }

            if (args.ExportModule)
            {
                commandArgs.Add("-module:export");

                // There must be only one source file
                if (args.SourceFiles.Count != 1)
                {
                    throw new ArgumentException("Export module expects only one source file.");
                }

                // Place the ifc in the output directory
                var sourceFile = args.SourceFiles[0];
                var outputFile = $"{args.OutputDirectory.EnsureTrailingSlash()}{Path.GetFileNameWithoutExtension(sourceFile)}.ifc";
                commandArgs.AddRange(new string[] { "-module:output", outputFile });
            }

            // Lastly add the file
            commandArgs.AddRange(args.SourceFiles);

            return string.Join(" ", commandArgs);
        }

        private static string BuildLibrarianArguments(LinkerArguments args)
        {
            var commandArgs = new List<string>();

            // Disable the logo text
            commandArgs.Add("-nologo");

            // Set the machine type
            commandArgs.Add("-machine:x64");

            // Add the library output file
            var ouputPath = args.OutputDirectory.EnsureTrailingSlash().Replace(@"\", @"\\");
            commandArgs.Add($"-out:\"{ouputPath}{args.Name}.lib\"");

            // Lastly add the file
            commandArgs.AddRange(args.SourceFiles);

            return string.Join(" ", commandArgs);
        }

        private string BuildLinkerExecutableArguments(LinkerArguments args)
        {
            var commandArgs = new List<string>();

            // Disable the logo text
            commandArgs.Add("-nologo");

            // Set the machine configuration
            commandArgs.Add("-machine:x64");

            // Add the library output file
            var ouputPath = args.OutputDirectory.EnsureTrailingSlash().Replace(@"\", @"\\");
            commandArgs.Add($"-out:\"{ouputPath}{args.Name}.exe\"");

            // Add in the std lib paths
            // TODO : May want to place behind flag
            // TODO : Investigate placing these as environment variables before calling exe
            // C:\Program Files(x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\ATLMFC\lib\x64;
            // C:\Program Files(x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\lib\x64;
            // C:\Program Files(x86)\Windows Kits\NETFXSDK\4.6.1\lib\um\x64;
            // C:\Program Files(x86)\Windows Kits\10\lib\10.0.17134.0\ucrt\x64;
            // C:\Program Files(x86)\Windows Kits\10\lib\10.0.17134.0\um\x64
            commandArgs.Add($"-libpath:\"{Path.Combine(_toolsPath, "ATLMFC/lib/x64")}\"");
            commandArgs.Add($"-libpath:\"{Path.Combine(_toolsPath, "lib/x64")}\"");
            commandArgs.Add($"-libpath:\"{Path.Combine(WindowsKitsPath, "10/lib", _windowsTargetVersion, "ucrt/x64")}\"");
            commandArgs.Add($"-libpath:\"{Path.Combine(WindowsKitsPath, "10/lib", _windowsTargetVersion, "um/x64")}\"");

            // Add the library files
            commandArgs.AddRange(args.LibraryFiles);

            // Lastly add the file
            commandArgs.AddRange(args.SourceFiles);

            return string.Join(" ", commandArgs);
        }

        private static void ProcessLine(string line)
        {
            if (line.Contains("error"))
            {
                Log.Error(line);
            }
            else if (line.Contains("warning"))
            {
                Log.Warning(line);
            }
            else
            {
                Log.Info(line);
            }
        }
    }
}
