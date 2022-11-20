#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef unsigned char uint8_t;


bool isWithin(int x, int y, int direction, int width, int height) {
    int xmax = width - 1;
    int ymax = height -1;
    switch(direction) {
        case 0:
            return (y>0);
        case 1:
            return (x<xmax && y>0);
        case 2:
            return (x<xmax);
        case 3:
            return (x<xmax && y<ymax);
        case 4:
            return (y<ymax);
        case 5:
            return (x>0 && y<ymax);
        case 6:
            return (x>0);
        case 7:
            return (x>0 && y>0);
    }
    return false;
}


vector<int> find_local_maxima(vector<int> img_data){
    return img_data;
}


// compile command:
// g++ -o cfindmaxima.so -shared -fPIC cfindmaxima.cpp
extern "C" {
    __declspec(dllexport)
    bool getMax(int *matrix, int rows, int columns, const char * filename);
}


bool getMax(int *matrix, int rows, int columns, const char * filename){
    vector<uint8_t> b;
    for (int i = 0; i < rows * columns; ++i)
        b.push_back(matrix[i]);

    int maxNum = *max_element(b.begin(), b.end());
    int minNum = *min_element(b.begin(), b.end());
    cout << "max is: " << maxNum << endl;
    cout << "min is: " << minNum << endl;

    FILE* fp = fopen(filename, "wb");
    fwrite(&b[0], sizeof(uint8_t), rows * columns, fp);
    fclose(fp);

    return true;
}


int main() {

    int w = 976, h = 976;
    const char * filename = "E:\\Collection\\C++\\MaximaFinderC\\phantom.raw";

    vector<uint8_t> a(w * h);
    FILE* fp = fopen(filename, "rb");
    fread(&a[0], sizeof(uint8_t), w*h, fp);
    fclose(fp);

    int maxNum = *max_element(a.begin(), a.end());
    int minNum = *min_element(a.begin(), a.end());
    cout << "max is: " << maxNum << endl;
    cout << "min is: " << minNum << endl;

    return 0;
}
