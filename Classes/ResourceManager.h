#pragma once

#include "Vector.h"

#include <string>

class ResourceManager {
public:
    std::string GetResourcePath() const;
    void LoadPngImage(const std::string& name);
    /*void* GetImageData();
    ivec2 GetImageSize();
    void UnloadImage();*/
private:
    //CFDataRef m_imageData;
    ivec2 m_imageSize;
};