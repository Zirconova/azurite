#include <iostream>
#include <fstream>

class WaveBuffer
{
public:
    int length;
    float* data;

    WaveBuffer();
    ~WaveBuffer();
};

void write_wave_file(std::string filename, WaveBuffer* buffer, int num_channels);