#ifndef ClipBoard_hpp
#define ClipBoard_hpp

bool copyImageToClipboard(sf::Image& image, sf::Vector2i pos, sf::Vector2i size) {
    const sf::Vector2u imgSz = image.getSize();
    if (imgSz.x == 0 || imgSz.y == 0) return false;


    int x0 = std::clamp(pos.x, 0, int(imgSz.x));
    int y0 = std::clamp(pos.y, 0, int(imgSz.y));
    int x1 = std::clamp(pos.x + size.x, 0, int(imgSz.x));
    int y1 = std::clamp(pos.y + size.y, 0, int(imgSz.y));

    const unsigned int w = (x1 > x0) ? unsigned(x1 - x0) : 0u;
    const unsigned int h = (y1 > y0) ? unsigned(y1 - y0) : 0u;
    if (w == 0 || h == 0) return false;

    const sf::Uint8* pixels = image.getPixelsPtr();

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
        const sf::Uint8* srcRow = pixels + ((y0 + y) * imgSz.x + x0) * 4;
        sf::Uint8* dstRow = dstPixels + (y * w) * 4;

        for (unsigned int x = 0; x < w; ++x) {
            const sf::Uint8 r = srcRow[4 * x + 0];
            const sf::Uint8 g = srcRow[4 * x + 1];
            const sf::Uint8 b = srcRow[4 * x + 2];
            const sf::Uint8 a = srcRow[4 * x + 3];

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

bool loadImageFromClipboard(sf::Image& outImage) {
    if (!OpenClipboard(nullptr))
        return false;

    HANDLE hData = GetClipboardData(CF_DIBV5);
    if (!hData) hData = GetClipboardData(CF_DIB);
    if (!hData) { CloseClipboard(); return false; }

    void* pData = GlobalLock(hData);
    if (!pData) { CloseClipboard(); return false; }

    auto* bih = reinterpret_cast<BITMAPINFOHEADER*>(pData);
    if (bih->biBitCount != 32 && bih->biBitCount != 24) {
        GlobalUnlock(hData); CloseClipboard(); return false;
    }

    const int  width = bih->biWidth;
    const int  height = std::abs(bih->biHeight);
    const bool topDown = (bih->biHeight < 0);
    if (width <= 0 || height <= 0) {
        GlobalUnlock(hData); CloseClipboard(); return false;
    }

    const DWORD compression = bih->biCompression; // BI_RGB | BI_BITFIELDS
    const size_t paletteEntries =
        bih->biClrUsed ? bih->biClrUsed : ((bih->biBitCount <= 8) ? (1u << bih->biBitCount) : 0u);

    const BYTE* srcPixels = reinterpret_cast<const BYTE*>(pData)
        + bih->biSize
        + ((compression == BI_BITFIELDS) ? 12 : 0)
        + paletteEntries * sizeof(RGBQUAD);

    const size_t srcStride = ((size_t(width) * bih->biBitCount + 31) / 32) * 4;

    std::vector<sf::Uint8> rgba(size_t(width) * size_t(height) * 4);

    for (int y = 0; y < height; ++y) {
        const BYTE* srcRow = srcPixels + (topDown ? y : (height - 1 - y)) * srcStride;
        sf::Uint8* dstRow = rgba.data() + size_t(y) * size_t(width) * 4;

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

    sf::Image src;
    src.create(width, height, rgba.data());

    if (width <= outImage.getSize().x && height <= outImage.getSize().y) {
        outImage.copy(src, 0, 0, sf::IntRect(0, 0, width, height), true);
    }
    else {
        outImage.create(width, height, rgba.data());
        outImage.copy(src, 0, 0, sf::IntRect(0, 0, width, height), true);
    }

    return true;
}
#endif