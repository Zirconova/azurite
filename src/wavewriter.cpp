#include "wavewriter.h"

WaveBuffer::WaveBuffer() : length(0)
{
    data = new float[1000000];
}
WaveBuffer::~WaveBuffer()
{
    std::cout << "WaveBuffer destructor called!\n";
    delete [] data;
}

struct wave16Header {
    char riff[4] = {'R', 'I', 'F', 'F'};
    int size;  // Overall size (data bytes + 44)
    char wave[4] = {'W', 'A', 'V', 'E'};
    char fmt[4] = {'f', 'm', 't', ' '};
    int fmt_length = 16;
    short type = 1;
    short num_channels;
    int sample_rate = 44100;
    int bytes_per_second;
    short bytes_per_sample; // bytes per "sample" = one point in time, all channel samples
    short bits_per_sample = 16; // bits per sample (one channel)
    char data[4] = {'d', 'a', 't', 'a'};
    int data_size;

    wave16Header(int length, int num_channels) 
    :   size(length * 2 * num_channels + 44),
        num_channels(num_channels),
        bytes_per_second(44100 * 2 * num_channels),
        bytes_per_sample(2 * num_channels),
        data_size(length * 2 * num_channels) {}
};

void write_wave_file(std::string filename, WaveBuffer* buffer, int num_channels)
{
    std::cout << "trying to write to wav\n";
    std::ofstream file(filename, std::ios::out | std::ios::binary);

    wave16Header header(buffer->length, num_channels);

    // convert float buffer to short buffer
    short* pcm_data = new short[1000000];
    for (int i = 0; i < buffer->length; i++)
    {
        pcm_data[i] = (short) (((buffer->data[i] + 1.f) * 0.5f * 65535.f / 65536.f * 2.f - 1.f) * 32768); 
    }

    file.write((const char*)&header, 44);
    file.write((const char*)(pcm_data), header.data_size);

    delete [] pcm_data;

    file.close();
}