/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Mult0_png;
    const int            Mult0_pngSize = 339;

    extern const char*   Mult1_png;
    const int            Mult1_pngSize = 315;

    extern const char*   Mult2_png;
    const int            Mult2_pngSize = 456;

    extern const char*   Mult3_png;
    const int            Mult3_pngSize = 419;

    extern const char*   Mult4_png;
    const int            Mult4_pngSize = 530;

    extern const char*   Mult5_png;
    const int            Mult5_pngSize = 504;

    extern const char*   TitleHeader_png;
    const int            TitleHeader_pngSize = 21643;

    extern const char*   TitleWaveShape_png;
    const int            TitleWaveShape_pngSize = 12024;

    extern const char*   TitleWaveTiming_png;
    const int            TitleWaveTiming_pngSize = 11807;

    extern const char*   TitleCrossover_png;
    const int            TitleCrossover_pngSize = 9410;

    extern const char*   TitleGainControls_png;
    const int            TitleGainControls_pngSize = 8161;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 11;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
