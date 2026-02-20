#include "Tools/ClipBoard.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Windows.h"

bool copyImageToClipboard(sf::Image& image, sf::IntRect rect)
{
    const auto imgW = static_cast<int>(image.getSize().x);
    const auto imgH = static_cast<int>(image.getSize().y);
    if (imgW == 0 || imgH == 0) return false;

	// intersection with image bounds
    const sf::IntRect imgRect({ 0, 0 }, { imgW, imgH });
    std::optional<sf::IntRect> interOpt = rect.findIntersection(imgRect);
    if (!interOpt.has_value()) {
        std::cout << "Brak przeciecia z obrazem.\n";
        return false;
    }
    const sf::IntRect inter = *interOpt;

	// sizes and positions
    const int x0 = inter.position.x;
    const int y0 = inter.position.y;
    const unsigned w = static_cast<unsigned>(inter.size.x > 0 ? inter.size.x : 0);
    const unsigned h = static_cast<unsigned>(inter.size.y > 0 ? inter.size.y : 0);
    if (w == 0 || h == 0) {
        if (w == 0) std::cout << "w == 0\n";
        if (h == 0) std::cout << "h == 0\n";
        return false;
    }

    std::wcout << x0 << L"\n" << y0 << L"\n"
        << (x0 + static_cast<int>(w)) << L"\n"
        << (y0 + static_cast<int>(h)) << L"\n";

    const uint8_t* src = image.getPixelsPtr(); // RGBA (R,G, B, A)

    // header DIB 32bpp (BGRA), top-down
    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = static_cast<LONG>(w);
    bi.biHeight = -static_cast<LONG>(h); // top-down
    bi.biPlanes = 1;
    bi.biBitCount = 32;                    // 32 bpp
    bi.biCompression = BI_RGB;
    bi.biSizeImage = w * h * 4;             // nieobowiązkowe przy BI_RGB, ale pomocne

    const SIZE_T headerSize = sizeof(BITMAPINFOHEADER);
    const SIZE_T pixelBytes = static_cast<SIZE_T>(w) * static_cast<SIZE_T>(h) * 4;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, headerSize + pixelBytes);
    if (!hMem) return false;

    void* pMem = GlobalLock(hMem);
    if (!pMem) {
        GlobalFree(hMem);
        return false;
    }

    auto* pHeader = static_cast<uint8_t*>(pMem);
    auto* dstPixels = pHeader + headerSize;

    std::memcpy(pHeader, &bi, headerSize);

    // RGBA (SFML) -> BGRA (DIB)
    const unsigned srcStride = static_cast<unsigned>(image.getSize().x) * 4;
    const unsigned dstStride = w * 4;

    for (unsigned y = 0; y < h; ++y) {
        const uint8_t* srcRow = src + (static_cast<unsigned>(y0) + y) * srcStride + static_cast<unsigned>(x0) * 4;
        uint8_t* dstRow = dstPixels + y * dstStride;

        for (unsigned x = 0; x < w; ++x) {
            const uint8_t r = srcRow[4 * x + 0];
            const uint8_t g = srcRow[4 * x + 1];
            const uint8_t b = srcRow[4 * x + 2];
            const uint8_t a = srcRow[4 * x + 3];

            // DIB (CF_DIB, BI_RGB, 32bpp) BGRA
            dstRow[4 * x + 0] = b; // B
            dstRow[4 * x + 1] = g; // G
            dstRow[4 * x + 2] = r; // R
            dstRow[4 * x + 3] = a; // A
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
    if (!hData) { CloseClipboard(); return; }

    void* pData = GlobalLock(hData);
    if (!pData) { CloseClipboard(); return; }

    auto* bih = reinterpret_cast<BITMAPINFOHEADER*>(pData);
    if (bih->biBitCount != 32 && bih->biBitCount != 24) {
        GlobalUnlock(hData); CloseClipboard(); return;
    }

    // sizes and directions
    const bool rightToLeft = (bih->biWidth < 0);
    const int  width = std::abs(bih->biWidth);
    const int  height = std::abs(bih->biHeight);
    const bool topDown = (bih->biHeight < 0);
    if (width <= 0 || height <= 0) {
        GlobalUnlock(hData); CloseClipboard(); return;
    }

    const DWORD compression = bih->biCompression; // BI_RGB | BI_BITFIELDS
    const size_t paletteEntries =
        bih->biClrUsed ? bih->biClrUsed : ((bih->biBitCount <= 8) ? (1u << bih->biBitCount) : 0u);

    const BYTE* base = reinterpret_cast<const BYTE*>(pData);

    // pixels offset
    const BYTE* srcPixels = base + bih->biSize
        + ((compression == BI_BITFIELDS) ? 12 : 0) // 3 masks 4 bajts - BITMAPINFOHEADER
        + paletteEntries * sizeof(RGBQUAD);

    const size_t srcStride = ((size_t(width) * bih->biBitCount + 31) / 32) * 4;

    // Masks for 32bpp
    auto maskShift = [](uint32_t m) -> int {
        if (m == 0) return 0;
        int s = 0; while ((m & 1u) == 0u) { m >>= 1; ++s; } return s;
        };
    auto maskExtract = [&](uint32_t v, uint32_t m) -> uint8_t {
        if (m == 0) return 0;
        const int s = maskShift(m);
        uint32_t comp = (v & m) >> s;
        int bits = 0; uint32_t t = (m >> s);
        while (t) { ++bits; t >>= 1; }
        if (bits >= 8) return static_cast<uint8_t>(comp >> (bits - 8));
        return static_cast<uint8_t>((comp * 255 + ((1u << bits) / 2)) / ((1u << bits) - 1u));
        };

    uint32_t rMask = 0x00FF0000u, gMask = 0x0000FF00u, bMask = 0x000000FFu, aMask = 0xFF000000u;
    if (bih->biBitCount == 32 && compression == BI_BITFIELDS) {
        if (bih->biSize >= sizeof(BITMAPV5HEADER)) {
            auto* v5 = reinterpret_cast<BITMAPV5HEADER*>(pData);
            rMask = v5->bV5RedMask; gMask = v5->bV5GreenMask; bMask = v5->bV5BlueMask; aMask = v5->bV5AlphaMask;
        }
        else {
            const uint32_t* masks = reinterpret_cast<const uint32_t*>(base + sizeof(BITMAPINFOHEADER));
            rMask = masks[0]; gMask = masks[1]; bMask = masks[2];
            if (bih->biSize >= sizeof(BITMAPV4HEADER))
                aMask = reinterpret_cast<BITMAPV4HEADER*>(pData)->bV4AlphaMask;
            else
                aMask = 0; // brak alfy w maskach
        }
    }

    std::vector<uint8_t> rgba(size_t(width) * size_t(height) * 4);
    bool anyAlphaNonZero = false;

    for (int y = 0; y < height; ++y) {
        const BYTE* srcRow = srcPixels + (topDown ? y : (height - 1 - y)) * srcStride;
        uint8_t* dstRow = rgba.data() + size_t(y) * size_t(width) * 4;

        if (bih->biBitCount == 32) {
            if (compression == BI_RGB) {
                // BGRA/XRGB
                for (int x = 0; x < width; ++x) {
                    const int sx = rightToLeft ? (width - 1 - x) : x;
                    const BYTE b = srcRow[4 * sx + 0];
                    const BYTE g = srcRow[4 * sx + 1];
                    const BYTE r = srcRow[4 * sx + 2];
                    const BYTE a = srcRow[4 * sx + 3];
                    dstRow[4 * x + 0] = r;
                    dstRow[4 * x + 1] = g;
                    dstRow[4 * x + 2] = b;
                    dstRow[4 * x + 3] = a;
                    anyAlphaNonZero |= (a != 0);
                }
            }
            else { // BI_BITFIELDS
                const uint32_t* row32 = reinterpret_cast<const uint32_t*>(srcRow);
                for (int x = 0; x < width; ++x) {
                    const int sx = rightToLeft ? (width - 1 - x) : x;
                    const uint32_t px = row32[sx];
                    const uint8_t R = maskExtract(px, rMask);
                    const uint8_t G = maskExtract(px, gMask);
                    const uint8_t B = maskExtract(px, bMask);
                    const uint8_t A = (aMask ? maskExtract(px, aMask) : 0xFF);
                    dstRow[4 * x + 0] = R;
                    dstRow[4 * x + 1] = G;
                    dstRow[4 * x + 2] = B;
                    dstRow[4 * x + 3] = A;
                    anyAlphaNonZero |= (A != 0);
                }
            }
        }
        else { // 24bpp BGR
            for (int x = 0; x < width; ++x) {
                const int sx = rightToLeft ? (width - 1 - x) : x;
                const BYTE b = srcRow[3 * sx + 0];
                const BYTE g = srcRow[3 * sx + 1];
                const BYTE r = srcRow[3 * sx + 2];
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
    outImage = std::move(src);
    return;
}