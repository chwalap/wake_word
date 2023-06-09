#include <Arduino.h>

#include <miniz.h>

void setup()
{
    auto original_data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque metus neque, consequat nec urna quis, lacinia ornare felis. Sed pulvinar metus augue, sed sagittis ligula luctus nec. Integer lectus augue, molestie id leo sit amet, accumsan aliquam erat. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Etiam luctus felis eu tincidunt ornare. Sed ultricies euismod nisi. Morbi tellus odio, dapibus et tincidunt ut, consequat ut dolor. Phasellus bibendum, nulla eget laoreet tincidunt, leo magna sodales arcu, vitae viverra diam urna sed nisi. Sed at tortor congue, rutrum dui ac, aliquet eros. Pellentesque nulla dolor, rhoncus at efficitur non, rutrum a orci.";
    const mz_ulong original_length = sizeof(original_data);

    // Compress the data
    mz_ulong compressed_len = mz_compressBound(original_length);
    auto compressed_data = new byte[compressed_len];
    if (compress(compressed_data, &compressed_len, (unsigned char *)original_data, original_length) != MZ_OK)
        abort();

    // Uncompress
    auto uncompressed_data = new byte[original_length]; // Note buffer can/should be larger!
    mz_ulong uncompressed_len = original_length;
    if (uncompress(uncompressed_data, &uncompressed_len, compressed_data, compressed_len) != MZ_OK)
        abort();

    if (strcmp(original_data, (const char *)uncompressed_data) != 0)
        abort();

    delete[] uncompressed_data;
    delete[] compressed_data;
}

void loop()
{
}