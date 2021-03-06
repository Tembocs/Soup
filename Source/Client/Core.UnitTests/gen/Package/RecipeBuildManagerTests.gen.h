#pragma once
#include "Package/RecipeBuildManagerTests.h"

TestState RunRecipeBuildManagerTests() 
{
	auto className = "RecipeBuildManagerTests";
	auto testClass = std::make_shared<Soup::Build::Runtime::UnitTests::RecipeBuildManagerTests>();
	TestState state = { 0, 0 };
	state += SoupTest::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });

	return state;
}