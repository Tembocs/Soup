﻿// <copyright file="AssertExtensions.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup
{
	export class AssertExtensions
	{
	public:
		static void AreEqual(
			Build::Extensions::GraphNodeWrapper& expected,
			Build::Extensions::GraphNodeWrapper& actual)
		{
			Assert::AreEqual(
				expected.GetTitle(),
				actual.GetTitle(),
				"Verify build node title match expected.");
			Assert::AreEqual(
				expected.GetProgram(),
				actual.GetProgram(),
				"Verify build node program match expected.");
			Assert::AreEqual(
				expected.GetArguments(),
				actual.GetArguments(),
				"Verify build node arguments match expected.");
			Assert::AreEqual(
				expected.GetWorkingDirectory(),
				actual.GetWorkingDirectory(),
				"Verify build node working directory match expected.");
			Assert::AreEqual(
				expected.GetInputFileList().CopyAsStringVector(),
				actual.GetInputFileList().CopyAsStringVector(),
				"Verify build node input files match expected.");
			Assert::AreEqual(
				expected.GetOutputFileList().CopyAsStringVector(),
				actual.GetOutputFileList().CopyAsStringVector(),
				"Verify build node output files match expected.");
			
			AreEqual(expected.GetChildList(), actual.GetChildList());
		}

		static void AreEqual(
			Memory::Reference<Build::Runtime::BuildGraphNode>& expected,
			Memory::Reference<Build::Runtime::BuildGraphNode>& actual)
		{
			AreEqual(
				Build::Extensions::GraphNodeWrapper(expected), 
				Build::Extensions::GraphNodeWrapper(actual));
		}

		static void AreEqual(
			Memory::Reference<Build::Runtime::BuildGraphNode>& expected,
			Build::Extensions::GraphNodeWrapper& actual)
		{
			AreEqual(
				Build::Extensions::GraphNodeWrapper(expected), 
				actual);
		}

		static void AreEqual(
			Build::Extensions::GraphNodeListWrapper& expected,
			Build::Extensions::GraphNodeListWrapper& actual)
		{
			Assert::AreEqual(
				expected.GetSize(),
				actual.GetSize(),
				"Verify build nodes size match expected.");

			for (size_t i = 0; i < expected.GetSize(); i++)
			{
				AreEqual(expected.GetValueAt(i), actual.GetValueAt(i));
			}
		}

		static void AreEqual(
			std::vector<Memory::Reference<Build::Runtime::BuildGraphNode>>& expected,
			std::vector<Build::Extensions::GraphNodeWrapper>& actual)
		{
			Assert::AreEqual(
				expected.size(),
				actual.size(),
				"Verify build nodes size match expected.");

			for (size_t i = 0; i < expected.size(); i++)
			{
				AreEqual(expected.at(i), actual.at(i));
			}
		}
	};
}