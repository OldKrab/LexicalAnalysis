﻿




//void foo(int a){ int b = a; }		// параметр всегда инициализирован
//
//int a, b = 3;
//
//void foo2(){
//	foo(5l);						// приведение типов
//	long b = 0x2A;					// переменная b в другой области видимости
//	long c = 0;
//	
//	for(int i = 0; i < 10; foo(i++))
//		a = 2 / c * 5L +			// инициализация переменной
//			b - (0777 * 0XBC);		
//	for(int a=1; a == 2; 1)			// переменная a в другой области видимости
//	{	int a = 100;	}			// переменная a в другой области видимости
//
//	long foo2;					// переменная foo2 в другой области видимости
//}

// errors
//void foo(){}				// Функция уже описана с таким именем
//void errors(){			
	//foo(1, 2);				// Несоотвествие количества аргументов и параметров функции
	//foo(0) = 10;				// Нельзя присваивать результату функции
	//foo = 10;					// Нельзя присваивать функции
	//foo(1+notInit);			// Использование неинициализированной переменной
	//int e, e = 2;				// Переменная уже определена
	//{int r;} r = 2;			// Вне области видимости
	//int errors; errors();		// Тут уже используется переменная, а не функция
	//int b = foo / 2;			// Функция используется как переменная
	//foo(notDefined);			// Использование необъявленной переменной 
	//int c = foo(20);			// Ошибка приведения типов
	//foo(foo(20));				// Ошибка приведения аргумента
	//foo(9999999999999999999999999999999) // несуществующая константа
	//foo(1) + 2;					// Ошибка операции
//	++foo(1);					// Ошибка унарной операции
	//for(int i; foo(1); i++);	// Неверный тип условия
//}

