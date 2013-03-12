#pragma once

#include "cinder/Vector.h"

#include <string>

class ResourceManager {
public:
    std::string GetResourcePath() const;
    void LoadPngImage(const std::string& name);
    /*void* GetImageData();
    ci::Vec2i GetImageSize();
    void UnloadImage();*/
private:
    //CFDataRef m_imageData;
    ci::Vec2i m_imageSize;
};