#include "Tools/Selection.hpp"
#include "Clipboard.hpp"
#include "Window.hpp"
#include "DebugLog.hpp"

void removeImageWithAlpha(sf::Image& image, sf::IntRect rect, sf::Color alphaColor)
{
	if (rect.size.x <= 0 || rect.size.y <= 0)
		return;

	sf::Image backgroundImage;
	backgroundImage.resize(sf::Vector2u(rect.size), alphaColor);

	sf::IntRect clipped;
	if (!rect.findIntersection(rect).has_value())
		return;
	
	clipped = rect.findIntersection(rect).value();

	sf::IntRect r(clipped.position - rect.position, clipped.size);

	if (!image.copy(backgroundImage, sf::Vector2u(clipped.position), r, false)) {
		DebugError(L"removeImageWithAlpha: image copy failed");
		exit(0);
	}
	
}

void removeImageWithMask(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor)
{
	if (rect.size.x <= 0 || rect.size.y <= 0)
		return;

	if (mask.getSize().x <= 0 || mask.getSize().y <= 0) return;

	sf::Image backgroundImage;
	backgroundImage.resize(sf::Vector2u(rect.size), alphaColor);

	sf::IntRect clipped;
	if (!rect.findIntersection(rect).has_value())
		return;

	clipped = rect.findIntersection(rect).value();

	sf::IntRect r(clipped.position - rect.position, clipped.size);

	for (int y = 0; y < (int)(backgroundImage.getSize().y); y++) {
		for (int x = 0; x < (int)(backgroundImage.getSize().x); x++) {
			if (mask.getPixel(sf::Vector2u(x, y)) == sf::Color::White) {
				backgroundImage.setPixel(sf::Vector2u(x, y), alphaColor);
			}
			else {
				backgroundImage.setPixel(sf::Vector2u(x, y), image.getPixel(sf::Vector2u(clipped.position.x + (x - r.position.x), clipped.position.y + (y - r.position.y))));
			}
		}
	}
	
	if (!image.copy(backgroundImage, sf::Vector2u(clipped.position), r, false)) {
		DebugError(L"removeImageWithMask: image copy failed");
		exit(0);
	}
}

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect) {
	
	sf::IntRect srcB(sf::Vector2i(0,0), sf::Vector2i(src.getSize()));
	/////////////

	if (!srcRect.findIntersection(srcB).has_value())
		return;

	sf::IntRect s = srcRect.findIntersection(srcB).value();

	if (s.size.x > (int)(dst.getSize().x))
		s.size.x = (int)(dst.getSize().x);

	if (s.size.y > (int)(dst.getSize().y))
		s.size.y = (int)(dst.getSize().y);

	if (s.size.x <= 0 || s.size.y <= 0) 
		return;

	if (!dst.copy(src, sf::Vector2u(0, 0), s, false)) {
		DebugError(L"copyImage: image copy failed");
		exit(0);
	}
}

void copyImageWithAlpha(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color alphaColor) {

	sf::IntRect srcB(sf::Vector2i(0,0), sf::Vector2i(src.getSize()));

	if (!srcRect.findIntersection(srcB).has_value())
		return;

	sf::IntRect s = srcRect.findIntersection(srcB).value();

	if (s.size.x < (int)(dst.getSize().x))
		s.size.x = (int)(dst.getSize().x);

	if (s.size.y > (int)(dst.getSize().y))
		s.size.y = (int)(dst.getSize().y);

	if (s.size.x <= 0 || s.size.y<= 0)
		return;

	sf::Image img;
	img.resize(src.getSize(), sf::Color::Transparent);

	for (int y = 0; y < (int)(img.getSize().y); y++) {
		for (int x = 0; x < (int)(img.getSize().x); x++) {
			if (!(src.getPixel(sf::Vector2u(x, y)) == sf::Color::Transparent || src.getPixel(sf::Vector2u(x,y)) == alphaColor)) {
				img.setPixel(sf::Vector2u(x, y), src.getPixel(sf::Vector2u(x, y)));
			}
		}
	}

	if(!dst.copy(img, sf::Vector2u(0, 0), s, false)) {
		DebugError(L"copyImageWithAlpha: image copy failed");
		exit(0);
	}

}

void copyImageWithMask(sf::Image& dst, sf::Image& src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor)
{
	const int dw = (int)dst.getSize().x;
	const int dh = (int)dst.getSize().y;
	const int sw = (int)src.getSize().x;
	const int sh = (int)src.getSize().y;

	if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0) return;

	// Startowe przesunięcia w maskce (lokalne 0..mw-1)
	int mx0 = 0, my0 = 0, mw = 0, mh = 0;
	mw = (int)mask.getSize().x;
	mh = (int)mask.getSize().y;
	if (mw <= 0 || mh <= 0) return;

	// Skoryguj ujemne dstX/dstY (gdy wklejasz "za lewą/górną" krawędź)
	if (dstX < 0) { srcX -= dstX;  mx0 -= dstX; dstX = 0; }
	if (dstY < 0) { srcY -= dstY;  my0 -= dstY; dstY = 0; }

	// Skoryguj ujemne srcX/srcY (gdy zaczynasz wycinek przed obrazem źródłowym)
	if (srcX < 0) { dstX -= srcX;  mx0 -= srcX; srcX = 0; }
	if (srcY < 0) { dstY -= srcY;  my0 -= srcY; srcY = 0; }

	// Maksymalny rozmiar kopiowania po przycięciu do granic buforów
	int maxW = std::min(dw - dstX, sw - srcX);
	int maxH = std::min(dh - dstY, sh - srcY);
	maxW = std::min(maxW, mw - mx0);
	maxH = std::min(maxH, mh - my0);

	if (maxW <= 0 || maxH <= 0) return;

	for (int y = 0; y < maxH; ++y) {
		const int sy = srcY + y;
		const int dy = dstY + y;
		const int my = my0 + y;

		for (int x = 0; x < maxW; ++x) {
			const int sx = srcX + x;
			const int dx = dstX + x;
			const int mx = mx0 + x;

			const sf::Color m = mask.getPixel({ (unsigned)mx, (unsigned)my });
			if (m != sf::Color::White) continue; // tylko białe = kopiuj

			const sf::Color c = src.getPixel({ (unsigned)sx, (unsigned)sy });
			if (c.a == 0 || c == alphaColor) continue;

			dst.setPixel({ (unsigned)dx, (unsigned)dy }, c);
		}
	}
} 

void pasteImageWithAlpha(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor)
{
	sf::IntRect s(sf::Vector2i(0,0), sf::Vector2i(src.getSize()));

	if (dstX < 0) { s.position.x -= dstX; s.size.x += dstX; dstX = 0; }
	if (dstY < 0) { s.position.y -= dstY; s.size.y += dstY; dstY = 0; }

	const int dw = int(dst.getSize().x), dh = int(dst.getSize().y);
	
	if (dstX >= dw || dstY >= dh) 
		return;

	if (dstX + s.size.x > dw) s.size.x = dw - dstX;
	if (dstY + s.size.y > dh) s.size.y = dh - dstY;

	if (s.size.x <= 0 || s.size.y <= 0) 
		return;

	sf::Image tmp;
	tmp.resize(sf::Vector2u(s.size), sf::Color::Transparent);
	if (!tmp.copy(src, sf::Vector2u(0, 0), s, true)) {
		DebugError(L"pasteImageWithAlpha: image copy failed");
		exit(0);
	}
	tmp.createMaskFromColor(alphaColor);

	const sf::IntRect all(sf::Vector2i(0, 0), sf::Vector2i(tmp.getSize()));
	if(!dst.copy(tmp, sf::Vector2u(dstX, dstY), all, true)) {
		DebugError(L"pasteImageWithAlpha: image copy to dst failed");
		exit(0);
	}
}



void pasteImageWithMask(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor)
{

	int dw = dst.getSize().x;
	int dh = dst.getSize().y;
	int sw = src.getSize().x;
	int sh = src.getSize().y;

	if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0) 
		return;

	sf::IntRect place(sf::Vector2i(dstX, dstY), sf::Vector2i(sw, sh));
	sf::IntRect canvas(sf::Vector2i(0,0), sf::Vector2i(dw, dh));
	sf::IntRect clip;


	if (!place.findIntersection(canvas).has_value())
		return;

	clip = place.findIntersection(canvas).value();

	sf::Image m = sf::Image(sf::Vector2u(src.getSize()), sf::Color::White);

	for (int y = clip.position.y; y < clip.position.y + clip.size.y; ++y) {
		for (int x = clip.position.x; x < clip.position.x + clip.size.x; ++x) {

			int sx = x - dstX;
			int sy = y - dstY;

			if (m.getPixel(sf::Vector2u(sx, sy)) != sf::Color::White)
				continue;

			sf::Color c = src.getPixel(sf::Vector2u(sx, sy));
			if (c == alphaColor || c.a == 0) 
				continue;

			dst.setPixel(sf::Vector2u(x, y), c);
		}
	}
}