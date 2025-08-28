/** C++ 变量
* + 变量概念：变量表示数据存储区域，用于储存位模式
*   + 变量在被引用时，存储的位模式会被解释为相应类型的值
*   + 变量通过变量名进行引用
* + 变量声明与初始化
* + 基本类型
* + 算术运算
* + 类型转换
*/

#include <iostream>

using namespace std;


/** Linux/x86-32 进程的虚拟内存布局
* 
* |			    |
* |	Kernal 	 	| 内核占用的内存
* |-------------|
* |argv,environ | 命令行参数和环境变量
* |-------------|
* |	栈(向下增长)| 自动分配和回收的内存
* |	    		|
* |-------------|
* |	    		|
* |	未分配内存  |
* |	    		|
* |	    		|
* |-------------|
* |	    		|
* |	堆(向上增长)| 动态分配内存
* |-------------|
* |未初始化数据 | 存储未初始化的静态变量
* |-------------|
* |初始化数据   | 存储已初始化的静态变量
* |-------------|
* |	 代码区域	| 存储程序指令
* |-------------|
* |				|  <- 0x08048000
* |				|
* |				|  <- 0x00000000
*/

// 全局变量：声明在任何 {} 之外的变量
// 文件作用域：从声明位置到文件结束
// 静态存储期限：执行 main() 之前完成内存的申请，当 main() 返回时才会释放全局变量占用的内存
int global{ 20 }; 

int main()
{
	// 变量声明的语法：类型 变量名{初始值};
	// 例如，在计算一个长方体的体积时，我们需要知道长方体的长、宽、高；
	// 为了计算长方体的体积将涉及 4 个变量：长、宽、高和体积。因此，需要声明 4 个变量
	int height{ 0 }, length{ 0 }, width{ 0 }, volume{ 0 };

	// 赋值: 变量通过赋值的方式获取值
	height = 20;
	length = 6;
	width = 3;

	cout << "长宽高分别为 "
		<< length << " "
		<< width << " "
		<< height << " "
		<< "长方体的体积为: "
		<< volume
		<< endl;

	// 基本类型：char int long float double
	// 类型的尺寸: sizeof(type or variable)
	cout << "类型的尺寸\n"
		<< "sizeof(char) = " << sizeof(char) << " bytes\n"
		<< "sizeof(short) = " << sizeof(short) << " bytes\n"
		<< "sizeof(int) = " << sizeof(int) << " bytes\n"
		<< "sizeof(long) = " << sizeof(long) << " bytes\n"
		<< "sizeof(long long) = " << sizeof(long long) << " bytes\n"
		<< "sizeof(float) = " << sizeof(float) << " bytes\n"
		<< "sizeof(double) = " << sizeof(double) << " bytes\n"
		<< "sizeof(long double) = " << sizeof(long double) << " bytes"
		<< endl;

	// 字面值：在程序中直接表示值的对象
	cout << "整数类型的字面值。以 123 和 -123 为例\n"
		<< "十进制表示: " << 123 << " " << -123 << "\n"
		<< "二进制表示: " << 0b111'1011 << " " << -0b111'1011 << "\n"
		<< "八进制表示: " << 0173 << " " << -0173 << "\n"
		<< "十六进制表示: " << 0x7b << " " << -0x7b << "\n"
		<< endl;

	cout << "浮点数的字面值\n"
		<< "浮点字面值形式: " << 3.14 << " " << -3.14 << "\n"
		<< "指数形式: " << .314e1 << " " << -0.314e1 << "\n"
		<< endl;

	// 字面值尺寸
	cout << "字面值的尺寸\n"
		<< "字面值 2147483647 的尺寸" << sizeof(2147483647) << ' ' << 2147483647 << "\n"  // 整数字面值的类型默认为 int。int 类型的最大值为 2147483647
		<< "字面值 2147483648 的尺寸" << sizeof(2147483648) << ' ' << 2147483647 << "\n"  // int 类型不能存储这个值，编译器就会采用 long 类型存储
		<< "字面值 0xffffffff 的尺寸" << sizeof(0xffffffff) << ' ' <<  0xffffffff << "\n"  // 0xffffffff 占用 4 字节其值为 4294967295，int 类型不能存储，但是该值刚好是 unsigned int 的最大值
		<< endl;
	// 十进制整数字面值的类型按照 int => long => long long 的顺序尝试，直到能存储字面值的最尺寸类型
	// 非十进制整数的字面值按照 int => unsigned int => long => unsigned long => long long => unsigned long long 的顺序尝试，知道能存储该字面值的最小尺寸类型

	// 变量的地址：使用运算符 `&变量名` 的方式获取变量的地址
	// 变量被声明后，就会占用计算机内存中的一块位置。现代操作系统使用虚拟内存管理计算机内存资源，因此我们可以使用 &变量名 的方式获取虚拟地址
	cout << "变量的地址\n"
		<< "&height = " << &height << "\n"
		<< "&width  = " << &width << "\n"
		<< "&length = " << &length << "\n"
		<< "&volume = " << &volume << "\n"
		<< endl;

	// 算术运算符：+(加法) -(减法) *(乘法) /(除法) %(模运算)
	// 计算长方体的体积: 长 x 宽 x 高
	volume = length * width * height;
	// + - * 在执行算术运算时的行为和数学上的概念无异。下面我们会详细介绍 / 和 % 的行为

	/* 除法行为
	* + 两个操作数都为整数时，除法运算符的结果也为整数
	* + 第二操作数为 0 时，行为是未定义的
	* + 出现负操作数时，结果 向零截断
	*/

	int a{ 2 }, b{ 3 };
	cout << "2 / 3 = " << a / b << "\n" // 两个整数执行除法运算时，结果为整数
		<< endl;  
	
	// 只要保证除法的两个操作数有一个浮点数就可以执行真实除法
	cout << "2.0 / 3 = " << 2. / 3 << "\n" << endl;

	//cout << "2 / 0 = " << 2 / 0 << "\n" << endl;  // 某些编译器会直接报错
	b = 0;
	//cout << "a(2) / b(0)" << a / b << "\n" << endl;  // 编译不会报错，在运行时会报错

	// 负操作数
	cout << "9 / -7 = " << 9 / -7 << "\n" << endl;  // 结果总是趋向 0，所以其结果为 -1
	cout << "-9 / 7 = " << -9 / 7 << "\n" << endl;  // 结果总是趋向 0，所以其结果为 -1	


	/* 模运算行为
	* + 操作数只能是整数，否则编译报错
	* + 模运算(%) 需要满足一个公式：即 i % j 的结果为 i - (i / j) * j
	*/

	//cout << "2.0 % 3.0 = " << 2.0 % 3.0 << "\n" << endl;  // 编译报错
	cout << "-9 % 7 = " << -9 % 7 << "\n 计算过程: -9 - (-9 / 7) * 7 = (-9 - (-1) * 7) = -9 + 7 = -2" << "\n" << endl;
	cout << "9 % -7 = " << 9 % -7 << "\n 计算过程: 9 - (9 / -7) * (-7) = (9 - (-1) * (-7)) = 9 - 7 = 2" << "\n" << endl;

	// 范围全局变量
	
	cout << "global = " << global << " &global = " << &global << endl;

	// 局部变量
	// 块作用域：声明位置开始，到当前块的结束位置，即最近的 } 结束
	// 自动存储期限：局部变量所属函数被调用时分配内存，函数返回时释放内存
	int local = 11;

	cout << "local  = " << local << " &local  = " << &local << endl;

	{
		/* 常量
		* C++ 中两种常量：运行时常量和编译时常量
		* + 运行时常量：在程序运行时求值的常量
		* + 编译时常量：在程序编译时求值的常量
		*/

		// const 类型 变量名{初始值}; 用于声明运行时常量
		const int cx{ 100 };
		// cx = 200; // error
		cout << "const 常量: " << cx << endl;

		// constexpr 类型 变量名{初始值}; 用于声明编译时常量.
		// constexpr 指示编译器在编译时求值。也就是说，初始值必须时一个可以在编译时求值的常量表达式
		constexpr int cex{ 100 };  // 100 字面值
		constexpr int cey{ cex + 200 }; // 可编译时求值的常量表达式
		cout << "constexpr 编译时常量: " << cex << "; " << cey << endl;
	}

	{
		/* 自动类型推导
		* auto 变量名{初始值};  编译器会从 ``初始值`` 中提取类型作为变量的类型
		* + 使用 auto 声明的变量一定时初始化的。因为必须提供初始值
		* + 简化代码
		*/

		auto pointX{ 2.0f };  // pointX 的类型是 float
		auto pointY{ 3.0 };  // pointY 的类型是 double

		cout << "auto pointX = " << pointX << endl;
		cout << "auto pointY = " << pointY << endl;

		constexpr int cx{ 10 };
		auto x = cx;  // x 的类型是 int

		// 需要需要自动推导常量类型，请添加 const 或 constexpr
		const auto cax{ 10 };  // const int
		constexpr auto cex{ 0xffLL }; // constexpr long long

		decltype(2.0) d{ 20. };  // double
	}
	{
		int a{ -10 };
		unsigned b{ 10 };
		cout << "int a = -10; unsigned b = 10; a < b: " << (a < b) << endl;
	}
}
