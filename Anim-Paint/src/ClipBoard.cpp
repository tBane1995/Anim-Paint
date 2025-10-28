#include "ClipBoard.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Windows.h"

bool copyImageToClipboard(sf::Image* image, sf::IntRect rect) {

    if (image == nullptr)
        return false;

    if (image->getSize().x == 0 || image->getSize().y == 0) {
        return false;
    }

    int x0 = std::clamp(rect.position.x, 0, int(image->getSize().x));
    int y0 = std::clamp(rect.position.y, 0, int(image->getSize().y));
    int x1 = std::clamp(rect.position.x + rect.size.x, 0, int(image->getSize().x));
    int y1 = std::clamp(rect.position.y + rect.size.y, 0, int(image->getSize().y));

    std::wcout << x0 << "\n";
    std::wcout << y0 << "\n";
    std::wcout << x1 << "\n";
    std::wcout << y1 << "\n";

    const unsigned int w = (x1 > x0) ? unsigned(x1 - x0) : 0u;
    const unsigned int h = (y1 > y0) ? unsigned(y1 - y0) : 0u;
    sf::IntRect r({ 0, 0 }, { int(image->getSize().x), int(image->getSize().y) });

    
    if (!rect.findIntersection(r).has_value() || w == 0 || h == 0) {
        if(w == 0) std::cout << " w is 0\n"; // tu  sie włącza
        if(h == 0) std::cout << " h is 0\n"; // tu  sie włącza
        return false;
    }

    const uint8_t* pixels = image->getPixelsPtr();

    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = LONG(w);
    bi.biHeight = -LONG(h);
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    const SIZE_T pixelBytes = SIZE_T(w) * SIZE_T(h) * 4;
    HGLOBAL hMem = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + pixelBytes);
    if (!hMem) return false;

    void* pMem = GlobalLock(hMem);
    auto* pHeader = static_cast<BYTE*>(pMem);
    auto* dstPixels = pHeader + sizeof(BITMAPINFOHEADER);

    std::memcpy(pHeader, &bi, sizeof(BITMAPINFOHEADER));

    for (unsigned int y = 0; y < h; ++y) {
        const uint8_t* srcRow = pixels + ((y0 + y) * image->getSize().x + x0) * 4;
        uint8_t* dstRow = dstPixels + (y * w) * 4;

        for (unsigned int x = 0; x < w; ++x) {
            const uint8_t r = srcRow[4 * x + 0];
            const uint8_t g = srcRow[4 * x + 1];
            const uint8_t b = srcRow[4 * x + 2];
            const uint8_t a = srcRow[4 * x + 3];

            dstRow[4 * x + 0] = b;  // B
            dstRow[4 * x + 1] = g;  // G
            dstRow[4 * x + 2] = r;  // R
            dstRow[4 * x + 3] = a;  // A
        }
    }

    GlobalUnlock(hMem);

    if (!OpenClipboard(nullptr)) {
        GlobalFree(hMem);
        return false;
    }
    EmptyClipboard();

    if (!SetClipboardData(CF_DIB, hMem)) {
        CloseClipboard();
        GlobalFree(hMem);
        return false;
    }
    CloseClipboard();

    
    return true;
}

void loadImageFromClipboard(sf::Image& outImage) {
    if (!OpenClipboard(nullptr))
        return;

    HANDLE hData = GetClipboardData(CF_DIBV5);
    if (!hData) hData = GetClipboardData(CF_DIB);
    if (!hData) {
        CloseClipboard();
        return;
    }

    void* pData = GlobalLock(hData);
    if (!pData) { 
        CloseClipboard(); 
        return; 
    }

    auto* bih = reinterpret_cast<BITMAPINFOHEADER*>(pData);
    if (bih->biBitCount != 32 && bih->biBitCount != 24) {
        GlobalUnlock(hData); 
        CloseClipboard(); 
        return;
    }

    int  width = bih->biWidth;
    int  height = std::abs(bih->biHeight);
    const bool topDown = (bih->biHeight < 0);
    if (width <= 0 || height <= 0) {
        GlobalUnlock(hData); 
        CloseClipboard(); 
        return;
    }

    const DWORD compression = bih->biCompression; // BI_RGB | BI_BITFIELDS
    const size_t paletteEntries =
        bih->biClrUsed ? bih->biClrUsed : ((bih->biBitCount <= 8) ? (1u << bih->biBitCount) : 0u);

    const BYTE* srcPixels = reinterpret_cast<const BYTE*>(pData)
        + bih->biSize
        + ((compression == BI_BITFIELDS) ? 12 : 0)
        + paletteEntries * sizeof(RGBQUAD);

    const size_t srcStride = ((size_t(width) * bih->biBitCount + 31) / 32) * 4;

    std::vector<uint8_t> rgba(size_t(width) * size_t(height) * 4);

    for (int y = 0; y < height; ++y) {
        const BYTE* srcRow = srcPixels + (topDown ? y : (height - 1 - y)) * srcStride;
        uint8_t* dstRow = rgba.data() + size_t(y) * size_t(width) * 4;

        if (bih->biBitCount == 32) {
            // BGRA -> RGBA
            for (int x = 0; x < width; ++x) {
                const BYTE b = srcRow[4 * x + 0];
                const BYTE g = srcRow[4 * x + 1];
                const BYTE r = srcRow[4 * x + 2];
                const BYTE a = srcRow[4 * x + 3];
                dstRow[4 * x + 0] = r;
                dstRow[4 * x + 1] = g;
                dstRow[4 * x + 2] = b;
                dstRow[4 * x + 3] = a;
            }
        }
        else { // 24 bpp BGR
            for (int x = 0; x < width; ++x) {
                const BYTE b = srcRow[3 * x + 0];
                const BYTE g = srcRow[3 * x + 1];
                const BYTE r = srcRow[3 * x + 2];
                dstRow[4 * x + 0] = r;
                dstRow[4 * x + 1] = g;
                dstRow[4 * x + 2] = b;
                dstRow[4 * x + 3] = 255;
            }
        }
    }

    GlobalUnlock(hData);
    CloseClipboard();

    sf::Image src(sf::Vector2u(width, height), rgba.data());

    if (width <= outImage.getSize().x && height <= outImage.getSize().y) {
        outImage.copy(src, sf::Vector2u(0, 0), sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(width, height)), true);
    }
    else {
        outImage.resize(sf::Vector2u(width, height), rgba.data());
        outImage.copy(src, sf::Vector2u(0, 0), sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(width, height)), true);
    }
}
