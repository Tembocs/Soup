
lexer grammar CppLexerKeywords;

// Define all of the C++ keyword tokens
AlignAs: 'alignas';
AlignOf : 'alignof';
Asm : 'asm';
Auto : 'auto';
Bool : 'bool';
Break : 'break';
Case : 'case';
Catch : 'catch';
Char : 'char';
Char16 : 'char16_t';
Char32 : 'char32_t';
Class : 'class';
Const : 'const';
ConstExpr : 'constexpr';
ConstCast : 'const_cast';
Continue : 'continue';
DeclType : 'decltype';
Default : 'default';
Delete : 'delete';
Do : 'do';
Double : 'double';
DynamicCast : 'dynamic_cast';
Else : 'else';
Enum : 'enum';
Explicit : 'explicit';
Export : 'export';
Extern : 'extern';
False : 'false';
Float : 'float';
For : 'for';
Friend : 'friend';
GoTo : 'goto';
If : 'if';
Inline : 'inline';
Int : 'int';
Long : 'long';
Mutable : 'mutable';
Namespace : 'namespace';
New : 'new';
NoExcept : 'noexcept';
Nullptr : 'nullptr';
Operator : 'operator';
Private : 'private';
Protected : 'protected';
Public : 'public';
Register : 'register';
ReinterpretCast : 'reinterpret_cast';
Return : 'return';
Short : 'short';
Signed : 'signed';
SizeOf : 'sizeof';
Static : 'static';
StaticAssert : 'static_assert';
StaticCast : 'static_cast';
Struct : 'struct';
Switch : 'switch';
Template : 'template';
This : 'this';
ThreadLocal : 'thread_local';
Throw : 'throw';
True : 'true';
Try : 'try';
TypeDef : 'typedef';
TypeId : 'typeid';
TypeName : 'typename';
Union : 'union';
Unsigned : 'unsigned';
Using : 'using';
Virtual : 'virtual';
Void : 'void';
Volatile : 'volatile';
WChar : 'wchar_t';
While : 'while';

// Special identifiers that can be used s names or objects and functions
Override: 'override'; // C++ 11
Final: 'final'; // C++ 11