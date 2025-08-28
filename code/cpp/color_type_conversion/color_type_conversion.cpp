/** color_type_conversion: 颜色类型转换
* + RGB convert to YUV
*   + R = Y + 1.3983 * (V - 128)
*   + G = Y - 0.39465 * (U - 128) - 0.58060 * (V - 128)
*   + B = Y + 2.03211 * (U - 128)
* + YUV convert to RGB
*   + Y = 0.299 * R + 0.587 * G + 0.114 * B
*   + U = -0.14713 * R - 0.28886 * G + 0.436 * B + 128
*   + V = 0.615 * R - 0.51498 * G - 0.10001 * B + 128
*/

#include <iostream>
using namespace std;

#define cutoff_255(X) ((X) > 255 ? 255: (X))
#define cutoff_zero(X) ((X) < 0 ? 0 : (X))

int main()
{
	int Y{ 0 }, U{ 0 }, V{ 0 };
	int R{ 0 }, G{ 0 }, B{ 0 };

	cout << "请输入 Y U V(以空格分隔)" << endl;
	cin >> Y >> U >> V;
	cout << "Y: " << Y << " U: " << U << " V: " << V << endl;
	R = Y + 1.3983 * (V - 128);
	R = cutoff_255(R);
	R = cutoff_zero(R);

	G = Y - 0.39465 * (U - 128) - 0.58060 * (V - 128); 
	G = cutoff_255(G);
	G = cutoff_zero(G);

	B = Y + 2.03211 * (U - 128);
	B = cutoff_255(B);
	B = cutoff_zero(B);
	cout << "R: " << R << " G: " << G << " B: " << B << endl;

	R = G = B = 0;
	cout << "请输入 R G B(以空格分隔)" << endl;
	cin >> R >> G >> B;
	cout << "R: " << R << " G: " << G << " B: " << B << endl;

	Y = 0.299 * R + 0.587 * G + 0.114 * B;
	U = -0.14713 * R - 0.28886 * G + 0.436 * B + 128;
	V = 0.615 * R - 0.51498 * G - 0.10001 * B + 128;
	cout << "Y: " << Y << " U: " << U << " V: " << V << endl;
}

