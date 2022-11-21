#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <string>
using namespace std;


typedef unsigned char uint8_t;
uint8_t MAXIMUM = 1;
uint8_t LISTED = 2;
uint8_t PROCESSED = 4;
uint8_t MAX_AREA = 8;
uint8_t EQUAL = 16;
uint8_t MAX_POINT = 32;
uint8_t ELIMINATED = 64;


// Functions and Dependencies
bool isWithin(int x, int y, int direction, int width, int height) {
    // Depending on where we are and where we are heading, return the appropriate inequality.
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


// Functions and Dependencies
vector<long long> getSortedMaxPoints(vector<uint8_t> img_data, vector<uint8_t> &out, int width, int height) {
    float globalMin = *min_element(img_data.begin(), img_data.end());
    float globalMax = *max_element(img_data.begin(), img_data.end());
    int dir_x[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
    int dir_y[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
    int nMax = 0;

    // Goes through each pixel
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t v = img_data[y * width + x];
            // If the pixel is local to the minima of the whole image, can't be maxima.
            if (v == globalMin) continue;

            // Is a maxima until proven that it is not.
            bool isMax = true;
            bool isInner = (y!=0 && y!=height-1) && (x!=0 && x!=width-1);
            for (int d = 0; d < 8; ++d) {
                if (isInner || isWithin(x, y, d, width, height)) {
                    uint8_t vNeighbor = img_data[(y+dir_y[d]) * width + x + dir_x[d]];
                    if (vNeighbor > v) {
                        // We have found there is larger pixel in the neighbourhood.
                        // So this cannot be a local maxima.
                        isMax = false;
                        break;
                    }
                }
            }
            if (isMax) {
                out[y * width + x] = MAXIMUM;
                nMax++;
            }
        }
    }

    float vFactor = (float)(2e9 / (globalMax-globalMin));              // for converting float values into a 32-bit int
    vector<long long> maxPoints(nMax);                                 // value (int) is in the upper 32 bit, pixel offset in the lower
    int iMax = 0;
    for (int y = 0; y < height; y++)                                   // enter all maxima into an array
        for (int x = 0, p = x+y*width; x < width; x++, p++)
            if (out[p] == MAXIMUM) {
                float fValue = img_data[p];
                long long iValue = (int)((fValue-globalMin)*vFactor);  // 32-bit int, linear function of float value
                maxPoints[iMax++] = (long long)(iValue<<32|p);
            }
    //sort the maxima by value
    sort(maxPoints.begin(), maxPoints.end());

    return maxPoints;
}


// Functions and Dependencies
bool comparer(vector<int> &a, vector<int> &b) {
    if (a[1] != b[1])
        return (a[1] < b[1]);
    else
        return (a[1] < b[1]);
}


void analyzeAndMarkMaxima(vector<uint8_t> img_data, int width, int height, float tolerance, bool excludeEdgesNow, bool strict,
                          vector<int> &x_arr, vector<int> &y_arr) {
    vector<uint8_t> types(width * height);
    vector<long long> maxPoints = getSortedMaxPoints(img_data, types, width, height);
    int nMax = maxPoints.size();
    vector<int> pList(width * height);
    int dirOffset[8] = {-width, -width+1, +1, +width+1, +width, +width-1, -1, -width-1};
    int dir_x[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
    int dir_y[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
    float maxSortingError = 0;

    // Find local maxima coordinates
    // Now we iterate through each of a local maxima and prune the sub-maximal peaks.
    // This extends the neighbourhood and combines and prunes away unwanted maxima using the
    // noise tolerance to decide what counts and what doesn't
    for (int i = nMax-1; i >= 0; i--) {
        int offset0 = (int) maxPoints[i];
        if ((types[offset0] & PROCESSED) != 0)
            // If processed already then skip this pixel, it won't be maxima.
            continue;
        // we create a list of connected points and start the list at the current maximum
        int x0 = offset0 % width;
        int y0 = offset0 / width;
        float v0 = img_data[y0 * width + x0];
        bool sortingError = true;
        while (sortingError) {                      // repeat if we have encountered a sortingError
            pList[0] = offset0;
            types[offset0] |= (EQUAL | LISTED);     // mark first point as equal height (to itself) and listed
            int listLen = 1;                        // number of elements in the list
            int listI = 0;                          // index of current element in the list
            bool isEdgeMaximum = (x0 == 0 || x0 == width - 1 || y0 == 0 || y0 == height - 1);
            sortingError = false;                   // if sorting was inaccurate: a higher maximum was not handled so far
            bool maxPossible = true;                // it may be a true maximum
            double xEqual = x0;                     // for creating a single point: determine average over the
            double yEqual = y0;                     // coordinates of contiguous equal-height points
            int nEqual = 1;                         // counts xEqual/yEqual points that we use for averaging

            while (listI < listLen) {
                int offset = pList[listI];
                int x = offset % width;
                int y = offset / width;
                bool isInner = (y != 0 && y != height - 1) &&
                               (x != 0 && x != width - 1);  // not necessary, but faster than isWithin
                for (int d = 0; d < 8; ++d) {
                    int offset2 = offset + dirOffset[d];
                    if ((isInner || isWithin(x, y, d, width, height)) && (types[offset2] & LISTED) == 0) {
                        if ((types[offset2] & PROCESSED) != 0) {
                            maxPossible = false;  // we have reached a point processed previously, thus it is no maximum now
                            break;
                        }
                        int x2 = x + dir_x[d];
                        int y2 = y + dir_y[d];
                        float v2 = img_data[y2 * width + x2];
                        if (v2 > v0 + maxSortingError) {
                            maxPossible = false;  // we have reached a higher point, thus it is no maximum
                            break;
                        } else if (v2 >= v0 - (float) tolerance) {
                            if (v2 > v0) {  // maybe this point should have been treated earlier
                                sortingError = true;
                                offset0 = offset2;
                                v0 = v2;
                                x0 = x2;
                                y0 = y2;

                            }
                            pList[listLen] = offset2;
                            listLen++;  // we have found a new point within the tolerance
                            types[offset2] |= LISTED;
                            if ((x2 == 0 || x2 == width - 1 || y2 == 0 || y2 == height - 1) && (strict || v2 >= v0)) {
                                isEdgeMaximum = true;
                                if (excludeEdgesNow) {
                                    maxPossible = false;
                                    break;  //we have an edge maximum
                                }
                            }
                            if (v2 == v0) {  // prepare finding center of equal points (in case single point needed)
                                types[offset2] |= EQUAL;
                                xEqual += x2;
                                yEqual += y2;
                                nEqual++;
                            }
                        }
                    } // if isWithin & not LISTED
                } // for directions d
                listI++;
            }

            if (sortingError) {  // if x0,y0 was not the true maximum but we have reached a higher one
                for (listI = 0; listI < listLen; listI++)
                    types[pList[listI]] = 0;  // reset all points encountered, then retry
            } else {
                int resetMask = ~(maxPossible ? LISTED : (LISTED | EQUAL));
                xEqual /= nEqual;
                yEqual /= nEqual;
                double minDist2 = 1e20;
                int nearestI = 0;
                for (listI = 0; listI < listLen; listI++) {
                    int offset = pList[listI];
                    int x = offset % width;
                    int y = offset / width;
                    types[offset] &= resetMask;     // reset attributes no longer needed
                    types[offset] |= PROCESSED;     // mark as processed
                    if (maxPossible) {
                        types[offset] |= MAX_AREA;
                        if ((types[offset] & EQUAL) != 0) {
                            double dist2 = (xEqual - x) * (double) (xEqual - x) + (yEqual - y) * (double) (yEqual - y);
                            if (dist2 < minDist2) {
                                minDist2 = dist2;   // this could be the best "single maximum" point
                                nearestI = listI;
                            }
                        }
                    }
                } // for listI
                if (maxPossible) {
                    int offset = pList[nearestI];
                    types[offset] |= MAX_POINT;

                    x_arr.push_back(int(offset % width));
                    y_arr.push_back(int(offset / width));
                }
            }  // if !sortingError
        }
    } // for all maxima iMax

    // sort according to y_arr
    vector<vector<int>> points(x_arr.size());
    for (int i = 0; i < x_arr.size(); ++i) {
        points[i].push_back(x_arr[i]);
        points[i].push_back(y_arr[i]);
    }
    sort(points.begin(), points.end(), comparer);
    for (int i = 0; i < x_arr.size(); ++i) {
        x_arr[i] = points[i][0];
        y_arr[i] = points[i][1];
    }
}


// compile command:
// g++ -o cfindmaxima.so -shared -fPIC cfindmaxima.cpp
extern "C" {
    __declspec(dllexport)
    void find_maxima(int *img_data, int width, int height, float tolerance, const char * filename);
}


// python calls
void find_maxima(int *data, int width, int height, float tolerance, const char * filename){
    vector<uint8_t> img_data;
    for (int i = 0; i < width * height; ++i)
        img_data.push_back(data[i]);

    vector<int> x_arr, y_arr;
    analyzeAndMarkMaxima(img_data, width, height, tolerance, true, false, x_arr, y_arr);

    ofstream outFile;
    outFile.open(filename, ios::out);
    outFile << " ,X,Y" << endl;
    for (int i = 0; i < x_arr.size(); ++i)
        outFile << to_string(i + 1) + "," + to_string(x_arr[i]) + "," + to_string(y_arr[i]) << endl;
    outFile.close();
}


int main() {

    int w = 976, h = 976;
    const char * filename = "E:\\Collection\\C++\\MaximaFinderC\\phantom.raw";

    vector<uint8_t> image(w * h);
    FILE* fp = fopen(filename, "rb");
    fread(&image[0], sizeof(uint8_t), w*h, fp);
    fclose(fp);

    vector<int> x_arr, y_arr;
    time_t start, end;
    start = clock();
    analyzeAndMarkMaxima(image, w, h, 10, true, false, x_arr, y_arr);
    end = clock();
    cout << "Time(s): " << (double)(end - start) / 1000 << endl;
    cout << "Count: " << x_arr.size() << endl;
    for (int i = 0; i < x_arr.size(); ++i)
        cout << "(" << x_arr[i] << "," << y_arr[i]<< ")" << endl;

    return 0;
}
