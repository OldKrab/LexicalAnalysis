#include "pch.h"
#include "CppUnitTest.h"
#include "HelperFunctions.h"

#include "Exceptions/AnalysisExceptions.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpretationTests
{
	TEST_CLASS(Functions)
	{
		TEST_METHOD(FuncCall)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
				int var = 1;
				void foo(){ var = var + 41; }
				void main() { foo(); })");
			auto value = GetValueOfVariable(sa, "var");
			Assert::AreEqual(value->intVal, 42);
		}

		TEST_METHOD(FuncPassArg)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
				int var = 1;
				void foo(int param){ var = var + param; }
				void main() { foo(41); })");
			auto value = GetValueOfVariable(sa, "var");
			Assert::AreEqual(value->intVal, 42);
		}

		TEST_METHOD(FuncPassSeveralArgs)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
				int var = 1;
				void foo(int p1, int p2, long p3){ p1 = p2 + p3; var = var + p1; }
				void main() { foo(1, 2, 3); })");
			auto value = GetValueOfVariable(sa, "var");
			Assert::AreEqual(value->intVal, 6);
		}

		TEST_METHOD(FuncNestedCall)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
				int var = 1;
				void foo1(long p){ var = var + p; }
				void foo2(int p1, int p2){ foo1(p1 * 2); var = var + p2; }
				void main() { foo2(2, 3); })");
			auto value = GetValueOfVariable(sa, "var");
			Assert::AreEqual(value->intVal, 8);
		}

		TEST_METHOD(ArgCopied)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
				int arg = 1, res;
				void foo2(int p){ p = p + 1; res = p; }
				void main() { foo2(arg); })");
			auto argVal = GetValueOfVariable(sa, "arg");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(argVal->intVal, 1);
			Assert::AreEqual(resVal->intVal, 2);
		}

		TEST_METHOD(RecursiveCall)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int  res = 0;
					void foo(int cnt) {
						res = cnt;
						for (int loops = 1; cnt < 10 * loops; --loops)
							foo(cnt + 1);
					}
					void main() { foo(0); })");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 10);
		}

		TEST_METHOD(ParamsNotSave)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int  res = 0;
					void foo(int p) {
						res = res + p;
						p = p + 1;
					}
					void main() { foo(10); foo(10); })");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 20);
		}
	};

	TEST_CLASS(Expressions)
	{
		TEST_METHOD(Add)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 12 + 13; }
				)"
			);
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 25);
		}
		TEST_METHOD(Sub)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 12 - 13; }
				)"
			);
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, -1);
		}
		TEST_METHOD(Mul)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 12 * 2; }
				)"
			);
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 24);
		}
		TEST_METHOD(Div)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 12 / 5; }
				)"
			);
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 2);
		}
		TEST_METHOD(Modul)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 12 % 5; }
				)"
			);
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 2);
		}

		TEST_METHOD(ComplexExpr)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 5 * 430 / ((3 - -2 * 5) * (-3 + 12) - ((5 % 3))); }
				)"
			);
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 18);
		}

		TEST_METHOD(Less)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int trueRes, falseRes;
					void main() { trueRes = 2 < 3; falseRes = 3 < 2; }
				)"
			);
			auto trueRes = GetValueOfVariable(sa, "trueRes");
			auto falseRes = GetValueOfVariable(sa, "falseRes");
			Assert::AreEqual(trueRes->intVal, 1);
			Assert::AreEqual(falseRes->intVal, 0);
		}

		TEST_METHOD(LessEqual)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int trueRes, falseRes;
					void main() { trueRes = 3 <= 3; falseRes = 3 <= 2; }
				)"
			);
			auto trueRes = GetValueOfVariable(sa, "trueRes");
			auto falseRes = GetValueOfVariable(sa, "falseRes");
			Assert::AreEqual(trueRes->intVal, 1);
			Assert::AreEqual(falseRes->intVal, 0);
		}

		TEST_METHOD(Greater)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int trueRes, falseRes;
					void main() { trueRes = 3 > 1; falseRes = 2 > 2; }
				)"
			);
			auto trueRes = GetValueOfVariable(sa, "trueRes");
			auto falseRes = GetValueOfVariable(sa, "falseRes");
			Assert::AreEqual(trueRes->intVal, 1);
			Assert::AreEqual(falseRes->intVal, 0);
		}

		TEST_METHOD(GreaterEqual)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int trueRes, falseRes;
					void main() { trueRes = 3 >= 3; falseRes = 1 >= 2; }
				)"
			);
			auto trueRes = GetValueOfVariable(sa, "trueRes");
			auto falseRes = GetValueOfVariable(sa, "falseRes");
			Assert::AreEqual(trueRes->intVal, 1);
			Assert::AreEqual(falseRes->intVal, 0);
		}

		TEST_METHOD(Equal)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int trueRes, falseRes;
					void main() { trueRes = 3 == 3; falseRes = 1 == 2; }
				)"
			);
			auto trueRes = GetValueOfVariable(sa, "trueRes");
			auto falseRes = GetValueOfVariable(sa, "falseRes");
			Assert::AreEqual(trueRes->intVal, 1);
			Assert::AreEqual(falseRes->intVal, 0);
		}


		TEST_METHOD(NotEqual)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int trueRes, falseRes;
					void main() { trueRes = 2 != 3; falseRes = 2 != 2; }
				)"
			);
			auto trueRes = GetValueOfVariable(sa, "trueRes");
			auto falseRes = GetValueOfVariable(sa, "falseRes");
			Assert::AreEqual(trueRes->intVal, 1);
			Assert::AreEqual(falseRes->intVal, 0);
		}

		TEST_METHOD(Icnrement)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res = 1, prevRes;
					void main() { prevRes = res; ++res; }
				)"
			);
			auto res = GetValueOfVariable(sa, "res");
			auto prevRes = GetValueOfVariable(sa, "prevRes");
			Assert::AreEqual(prevRes->intVal, 1);
			Assert::AreEqual(res->intVal, 2);
		}

		TEST_METHOD(Decrement)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res = 1, prevRes;
					void main() { prevRes = res; --res; }
				)"
			);
			auto res = GetValueOfVariable(sa, "res");
			auto prevRes = GetValueOfVariable(sa, "prevRes");
			Assert::AreEqual(prevRes->intVal, 1);
			Assert::AreEqual(res->intVal, 0);
		}

		TEST_METHOD(ComplexIncDec)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res = 1, prevRes;
					void main() { prevRes = res; --++----++++++res; }
				)"
			);
			auto res = GetValueOfVariable(sa, "res");
			auto prevRes = GetValueOfVariable(sa, "prevRes");
			Assert::AreEqual(prevRes->intVal, 1);
			Assert::AreEqual(res->intVal, 2);
		}

		TEST_METHOD(PassExprToFunc)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void foo(int p) { res = p; }
					void main() { foo(1 + 2 * 3); }
				)"
			);
			auto res = GetValueOfVariable(sa, "res");
			Assert::AreEqual(res->intVal, 7);
		}
	};

	TEST_CLASS(Cast)
	{
		TEST_METHOD(CastIntToLong)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					long res;
					void main() { res = 12; })");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->longVal, 12LL);
		}

		TEST_METHOD(CastLongToInt)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res;
					void main() { res = 12L; })");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 12);
		}
	};

	TEST_CLASS(For)
	{
		TEST_METHOD(SingleFor)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res = 0;
					void main() { 
						for(int i = 0; i < 10; ++i)
							++res;
					})");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 10);
		}

		TEST_METHOD(NestedFor)
		{
			auto sa = RunSyntaxAnalyser(
				R"(
					int res = 0;
					void main() { 
						for(int i = 0; i < 3; ++i)
							for(int j = 0; j < 3; ++j)
								++res;
					})");
			auto resVal = GetValueOfVariable(sa, "res");
			Assert::AreEqual(resVal->intVal, 9);
		}
	};

	TEST_CLASS(ComplexTests)
	{
		TEST_METHOD(Fibonacci)
		{
			auto sa = RunSyntaxAnalyser(R"(
					int res, res5, res10;
					void fib(int n)
					{
						int a = 0, b = 1;
						for (int i = 2; i < n; ++i)
						{
							int c = a + b;
							a = b;
							b = c;
						}
						res = b;
					}
					void main()
					{
						fib(5);
						res5 = res;
						fib(10);
						res10 = res;
					})");
			auto res5 = GetValueOfVariable(sa, "res5");
			auto res10 = GetValueOfVariable(sa, "res10");
			Assert::AreEqual(res5->intVal, 3);
			Assert::AreEqual(res10->intVal, 34);
		}

		TEST_METHOD(Factorial)
		{
			auto sa = RunSyntaxAnalyser(R"(
					int res, res5, res10;
					void fact(int n)
					{
						res = 1;
						for (int i = 2; i <= n; ++i)
							res = res * i;
					}
					void main()
					{
						fact(5);
						res5 = res;
						fact(10);
						res10 = res;
					})");
			auto res5 = GetValueOfVariable(sa, "res5");
			auto res10 = GetValueOfVariable(sa, "res10");
			Assert::AreEqual(res5->intVal, 120);
			Assert::AreEqual(res10->intVal, 3628800);
		}
	};
}

