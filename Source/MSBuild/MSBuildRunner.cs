﻿// <copyright company="Soup" file="MSBuildRunner.cs">
//   Copyright (c) Soup.  All rights reserved.
// </copyright>

namespace Soup.MSBuild
{
	using System;
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.IO;
	using System.Linq;
	using System.Text.RegularExpressions;
	using Newtonsoft.Json;

	/// <summary>
	/// MSBuild runner
	/// </summary>
	internal static class MSBuildRunner
	{
		private static readonly IReadOnlyList<Regex> KnownStatusText =
			new List<Regex>()
			{
				new Regex(@"^\w+:$", RegexOptions.Compiled),
				new Regex(@"^Project ", RegexOptions.Compiled),
				new Regex(@"^Done Building Project ", RegexOptions.Compiled),
			};

		private static readonly IReadOnlyList<Regex> KnownWarningText =
			new List<Regex>()
			{
				new Regex(@"^    [1-9]\d* Warning\(s\)$", RegexOptions.Compiled),
				new Regex(@": warning", RegexOptions.Compiled),
			};

		private static readonly IReadOnlyList<Regex> KnownErrorText =
			new List<Regex>()
			{
				new Regex(@"^    [1-9]\d* Error\(s\)$", RegexOptions.Compiled),
				new Regex(@"c1xx : fatal error", RegexOptions.Compiled),
			};

		private static readonly IReadOnlyList<Regex> KnownSuccessText =
			new List<Regex>()
			{
				new Regex(@"^Build succeeded\.$", RegexOptions.Compiled),
			};

		public static bool Build(string buildPath, bool showOutput, bool debug)
		{
			string compiler = FindCompiler();
			string configuration = debug ? "Debug" : "Release";
			var buildProjectPath = Path.Combine(buildPath, MSBuildConstants.VS2017ProjectName);
			using (Process process = new Process())
			{
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.RedirectStandardOutput = true;
				process.StartInfo.FileName = compiler;
				process.StartInfo.Arguments = $"{buildProjectPath} -p:Platform=x64;Configuration={configuration}";
				process.Start();

				if (showOutput)
				{
					while (!process.StandardOutput.EndOfStream)
					{
						string line = process.StandardOutput.ReadLine();
						WriteMSBuildLine(line);
					}
				}

				process.WaitForExit();

				return process.ExitCode == 0;
			}
		}

		private static string FindCompiler()
		{
			var result = string.Empty;
			var programFilesDir = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86);
			var vswhere = Path.Combine(
				programFilesDir,
				@"Microsoft Visual Studio\Installer\vswhere.exe");
			using (Process process = new Process())
			{
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.RedirectStandardOutput = true;
				process.StartInfo.FileName = vswhere;
				process.StartInfo.Arguments = "-format json -latest -requires Microsoft.Component.MSBuild";
				process.Start();

				// To avoid deadlocks, always read the output stream first and then wait.
				string output = process.StandardOutput.ReadToEnd();
				process.WaitForExit();

				// Parse the properties
				var vsInstalls = JsonConvert.DeserializeObject<List<VSInstallation>>(output);
				if (vsInstalls.Count == 1)
				{
					result = Path.Combine(vsInstalls[0].InstallationPath, @"MSBuild\15.0\Bin\MSBuild.exe");
				}
			}

			return result;
		}

		private static void WriteMSBuildLine(string line)
		{
			if (KnownStatusText.Any(pattern => pattern.IsMatch(line)))
			{
				Log.Message(line, ConsoleColor.Cyan);
			}
			else if (KnownWarningText.Any(pattern => pattern.IsMatch(line)))
			{
				Log.Message(line, ConsoleColor.Yellow);
			}
			else if (KnownErrorText.Any(pattern => pattern.IsMatch(line)))
			{
				Log.Message(line, ConsoleColor.Red);
			}
			else if (KnownSuccessText.Any(pattern => pattern.IsMatch(line)))
			{
				Log.Message(line, ConsoleColor.Green);
			}
			else
			{
				Log.Message(line);
			}
		}
	}
}