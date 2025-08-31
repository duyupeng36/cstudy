#include <iostream>

int main()
{
    {   
        // const_cast 
        int a{20};
        std::cout << "a = " << a << "\n"; // a = 20

        const int *cpa = &a; // const 指针指向了非 const 对象的指针

        auto pa{const_cast<int *>(cpa)}; // 剔除 const 指针的 const 属性
        *pa = 30;

        std::cout << "a = " << a << "\n"; // a = 30
    }
    unsigned long p;
    {
        // static_cast
        
        // int a{2.4};  // error: narrowing conversion
        int a {static_cast<int>(2.3)};
        
        int *pi = new int{2};
        // float *p{static_cast<float *>(pi)}; // error: 无效的类型转换：不将 int* 转换为 float*
        p = (unsigned long)pi;
    }
    {
        // reinterpret_cast
        // 将一个整数视为指针
        auto pa = reinterpret_cast<int *>(p);
        std::cout << *pa << std::endl;
        
        // 将一个 int 类型整数是为 float
        // 3.14 的 binary32 表示的十六进制数为 0x4048F5C3
        int a = 0x4048F5C3;
        std::cout << "a = " << a << std::endl;
        // auto f = reinterpret_cast<float>(a);  // error: 类型转换无效
        float *pf = reinterpret_cast<float *>(&a); 
        std::cout << "*f = " << *pf << std::endl;
    }
    {
        // 范围for
        int nums[]{1,2,3,4,5,6,7,8,9};
        for (auto x : nums) {
            std::cout << x << " ";
        }
        std::cout << std::endl;

    }
}
