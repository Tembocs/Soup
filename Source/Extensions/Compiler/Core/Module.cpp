﻿module;

#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

export module SoupCompiler;

import Opal;
import Soup.Build.Extensions;

using namespace Opal;

#include "ICompiler.h"
#include "CompileArguments.h"
#include "MockCompiler.h"

#include "BuildEngine.h"
#include "BuildUtilities.h"
