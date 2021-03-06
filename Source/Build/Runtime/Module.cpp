﻿module;

#include <any>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

export module Soup.Build.Runtime;

import Opal;
import Soup.Build;
import Soup.Build.Extensions;

using namespace Opal;

#include "BuildSystem.h"
#include "BuildState.h"
#include "ValueList.h"
#include "ValuePrimitive.h"
#include "ValueTable.h"
