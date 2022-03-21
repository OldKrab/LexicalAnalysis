#include "pch.h"
#include "CppUnitTest.h"
#include "HelperFunctions.h"

#include "Exceptions/AnalysisExceptions.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SyntaxTests
{
	TEST_CLASS(InvalidIdentifier)
	{
	public:

		TEST_METHOD(InvalidFunctionIdentifier)
		{
			ExpectException<InvalidIdentifierException>(R"(
					void for(){}
				)");
		}

		TEST_METHOD(InvalidVariableIdentifier)
		{
			ExpectException<InvalidIdentifierException>(R"(
					void main(){int for;}
				)");
		}

		TEST_METHOD(InvalidParamIdentifier)
		{
			ExpectException<InvalidIdentifierException>(R"(
					void foo(int for){}
				)");
		}
	};

	TEST_CLASS(InvalidType)
	{
	public:
		TEST_METHOD(InvalidFunctionType)
		{
			ExpectException<InvalidTypeException>(R"(
					float main(){}
				)");
		}

		TEST_METHOD(InvalidVariableType)
		{
			ExpectException<InvalidTypeException>(R"(
					void main(){ float a; }
				)");
			ExpectException<InvalidTypeException>(R"(
					void a;
				)");
		}

		TEST_METHOD(InvalidParamType)
		{
			ExpectException<InvalidTypeException>(R"(
					void foo(float p){}
				)");
		}
	};

	TEST_CLASS(NotExpectedLexeme)
	{
	public:
		TEST_METHOD(NoFunctionClosePar)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main( {}
				)");
		}

		TEST_METHOD(NoDataDeclSemi)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					int a
				)");
			ExpectException<NotExpectedLexemeException>(R"(
					void foo(){int a}
				)");
		}
		TEST_METHOD(NoExprSemi)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){1 + 2}
				)");
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){int a = 1; a = a + 2}
				)");
		}

		TEST_METHOD(NoForOpenPar)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){for}
				)");
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){for{ int a;}}
				)");
		}

		TEST_METHOD(NoForSemi)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){for(int a = 1 a < 10; ++a)}
				)");
		}

		TEST_METHOD(NoForClosePar)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){ for(int a = 1; a < 10; ++a {} }
				)");
		}

		TEST_METHOD(NoAssignId)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){ for = 1; }
				)");
		}
		TEST_METHOD(NoFuncCallClosePar)
		{
			ExpectException<ExpectedExpressionException>(R"(
					void foo(){}
					void main(){ foo(; }
				)");

			ExpectException<ExpectedExpressionException>(R"(
					void foo(){}
					void main(){ foo(1,; }
				)");
			ExpectException<NotExpectedLexemeException>(R"(
					void foo(){}
					void main(){ foo(1; }
				)");
		}

		TEST_METHOD(NoExprClosePar)
		{
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){ 2 * (1 + 3; }
				)");
			ExpectException<NotExpectedLexemeException>(R"(
					void main(){ 2 * (1 + (3 - 4); }
				)");
		}
	};
}
