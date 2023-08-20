#include <iostream>
using namespace std;

int main()
{
    int i=0;
    float shadow = 0.0;
    float bias = 0.05;
    float samples = 4.0;
    float offset = 0.1;
    for (float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            // for (float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                cout<<i++<<" ";
            }
        }
    }
    return 0;   
}