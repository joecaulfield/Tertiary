/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   About_CompanyLogo_png;
    const int            About_CompanyLogo_pngSize = 19084;

    extern const char*   About_PluginTitle1_png;
    const int            About_PluginTitle1_pngSize = 15750;

    extern const char*   CompanyLogo_png;
    const int            CompanyLogo_pngSize = 1626302;

    extern const char*   FaderKnob_png;
    const int            FaderKnob_pngSize = 1566;

    extern const char*   TopBanner_Black_png;
    const int            TopBanner_Black_pngSize = 40163;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

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
