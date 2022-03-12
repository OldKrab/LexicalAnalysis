﻿#include "pch.h"
#include "CppUnitTest.h"
#include "HelperFunctions.h"

#include "Exceptions/AnalysisExceptions.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SemanticTests
{
	TEST_CLASS(RedefinedIdentifier)
	{
		TEST_METHOD(RedefinedFunction)
		{
			ExpectException<RedefinedIdentifierException>(R"(
				void foo(){}
				void foo(int a){}
			)");
		}

		TEST_METHOD(RedefinedVariable)
		{
			ExpectException<RedefinedIdentifierException>(R"(
				void main() {
					int a = 1;
					long a;
				}
			)");
		}
		TEST_METHOD(RedefinedParam)
		{
			ExpectException<RedefinedIdentifierException>(R"(
				void main(int a, long a) {}
			)");
		}
	};

	TEST_CLASS(UndefinedIdentifier)
	{
		TEST_METHOD(UndefinedVariable)
		{
			ExpectException<UndefinedIdentifierException>(R"(
				void main(){ a = 10; }
			)");
			ExpectException<UndefinedIdentifierException>(R"(
				void main(){ int a = 10 + b; }
			)");
			ExpectException<UndefinedIdentifierException>(R"(
				void main(){ int a; a = 11 - b; }
			)");
		}

		TEST_METHOD(UndefinedFunction)
		{
			ExpectException<UndefinedIdentifierException>(R"(
				void main(){ foo(); }
			)");
			ExpectException<UndefinedIdentifierException>(R"(
				void main(){ foo(1); }
			)");
		}
	};

	TEST_CLASS(UncastableVariable)
	{
		TEST_METHOD(UncastableForCond)
		{
			ExpectException<UncastableVariableException>(R"(
				void foo(){}	
				void main(){ for(int i; foo(); ++i); }
			)");
		}
		TEST_METHOD(UncastableVariableAssign)
		{
			ExpectException<UncastableVariableException>(R"(
				void foo(){}	
				void main(){ 
					int a;
					a = foo(); 
				}
			)");
		}
		TEST_METHOD(UncastableVariableDecl)
		{
			ExpectException<UncastableVariableException>(R"(
				void foo(){}	
				void main(){ int a = foo(); }
			)");
		}

		TEST_METHOD(UncastableFuncCallArg)
		{
			ExpectException<UncastableVariableException>(R"(
				void foo(int a){}
				void bar(){}	
				void main(){ foo(bar()); }
			)");
		}
	};

	TEST_CLASS(InvalidOperands)
	{
		TEST_METHOD(InvalidOperandPlus)
		{
			ExpectException<InvalidOperandsException>(R"(
				void foo(){}	
				void main(){ int a = 1 + foo(); }
			)");
		}

		TEST_METHOD(InvalidOperandMul)
		{
			ExpectException<InvalidOperandsException>(R"(
				void foo(){}	
				void main(){ int a = 1 * foo(); }
			)");
		}

		TEST_METHOD(InvalidOperandInc)
		{
			ExpectException<InvalidOperandsException>(R"(
				void foo(){}	
				void main(){ int a = ++foo(); }
			)");
		}

		TEST_METHOD(InvalidOperandEqual)
		{
			ExpectException<InvalidOperandsException>(R"(
				void foo(){}	
				void main(){ int a = 1 == foo(); }
			)");
		}

		TEST_METHOD(InvalidOperandCmp)
		{
			ExpectException<InvalidOperandsException>(R"(
				void foo(){}	
				void main(){ int a = 1 > foo(); }
			)");
		}
	};

	TEST_CLASS(DivisionOnZero)
	{
		TEST_METHOD(DivOnZero)
		{
			ExpectException<DivisionOnZeroException>(R"(
				void main(){ int a = 1 / 0; }
			)");
			ExpectException<DivisionOnZeroException>(R"(
				void main(){ int a = 1 / 0L; }
			)");
		}
		TEST_METHOD(ModulOnZero)
		{
			ExpectException<DivisionOnZeroException>(R"(
				void main(){ int a = 1 % 0; }
			)");
			ExpectException<DivisionOnZeroException>(R"(
				void main(){ int a = 1 % 0L; }
			)");
		}
	};

	TEST_CLASS(NumberConstant)
	{
		TEST_METHOD(BigDecConstant)
		{
			ExpectException<InvalidNumberException>(R"(
				void main(){int a = 9223372036854775808;}
			)");
		}
		TEST_METHOD(NotBigDecConstant)
		{
			RunSyntaxAnalyser(R"(
				void main(){int a = 9223372036854775807;}
			)");
		}

		TEST_METHOD(BigOctConstant)
		{
			ExpectException<InvalidNumberException>(R"(
				void main(){int a = 01000000000000000000000;}
			)");
		}
		TEST_METHOD(NotBigOctConstant)
		{
			RunSyntaxAnalyser(R"(
				void main(){int a = 0777777777777777777777;}
			)");
		}

		TEST_METHOD(BigHexConstant)
		{
			ExpectException<InvalidNumberException>(R"(
				void main(){int a = 0x8000000000000000;}
			)");
		}
		TEST_METHOD(NotBigHexConstant)
		{
			RunSyntaxAnalyser(R"(
				void main(){int a = 0x7FFFFFFFFFFFFFFF;}
			)");
		}
	};

	TEST_CLASS(ArgsCount)
	{
		TEST_METHOD(ArgsCountLess)
		{
			ExpectException<WrongArgsCountException>(R"(
				void foo(int a, int b){}
				void main(){ foo(1); }
			)");
		}

		TEST_METHOD(ArgsCountMore)
		{
			ExpectException<WrongArgsCountException>(R"(
				void foo(int a, int b){}
				void main(){ foo(1,2,3); }
			)");
		}
		TEST_METHOD(ArgsCountEqual)
		{
			RunSyntaxAnalyser(R"(
				void foo(int a, int b){}
				void main(){ foo(1,2); }
			)");
		}
	};

	TEST_CLASS(UsingUninitializedVariable)
	{
		TEST_METHOD(UninitializedVariable)
		{
			ExpectException<UsingUninitializedVariableException>(R"(
				void main(){ int a; int b = a; }
			)");
			ExpectException<UsingUninitializedVariableException>(R"(
				void main(){ int a; int b; b = a; }
			)");
		}

		TEST_METHOD(UninitializedArg)
		{
			ExpectException<UsingUninitializedVariableException>(R"(
				void foo(int a){}
				void main(){ int a; foo(a); }
			)");
		}
	};

	TEST_CLASS(WrongUsing)
	{
		TEST_METHOD(UseVariableAsFunction)
		{
			ExpectException<UsingVariableAsFunctionException>(R"(
				void main(){ int a; a(); }
			)");
		}
		TEST_METHOD(UseFunctionAsVariable)
		{
			ExpectException<UsingFunctionAsVariableException>(R"(
				void foo(){}
				void main(){ int a = foo + 2; }
			)");
			ExpectException<UsingFunctionAsVariableException>(R"(
				void foo(int a){}
				void main(){ int a = foo(foo); }
			)");
		}
	};
}
