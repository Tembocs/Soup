﻿module;

#include <any>
#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <optional>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

export module SoupCore;

import Opal;
import Opal.Extensions;
import Soup.Build;
import Soup.Build.Extensions;
import Soup.Build.Runtime;
import LzmaSdk;
import json11;
import toml11;

using namespace Opal;

#include "Utils/Helpers.h"
#include "Utils/HandledException.h"

#include "Api/SoupApi.h"

#include "Build/Runner/BuildHistory.h"
#include "Build/Runner/BuildHistoryChecker.h"
#include "Build/Runner/BuildHistoryJson.h"
#include "Build/Runner/BuildHistoryManager.h"
#include "Build/Runner/BuildRunner.h"

#include "Config/LocalUserConfigExtensions.h"

#include "Package/PackageManager.h"
#include "Package/Recipe.h"
#include "Package/RecipeBuildManager.h"
#include "Package/RecipeExtensions.h"
#include "Package/RecipeJson.h"
#include "Package/RecipeToml.h"
