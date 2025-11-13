#include "Tools/Selection.hpp"
#include "Clipboard.hpp"
#include "Window.hpp"
#include <iostream>

void removeImage(sf::Image& image, sf::IntRect rect, sf::Color alphaColor)
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

	image.copy(backgroundImage, sf::Vector2u(clipped.position), r, false);
}

void removeImage(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor)
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

	for (int y = 0; y < backgroundImage.getSize().y; y++) {
		for (int x = 0; x < backgroundImage.getSize().x; x++) {
			if (mask.getPixel(sf::Vector2u(x, y)) == sf::Color::White) {
				backgroundImage.setPixel(sf::Vector2u(x, y), alphaColor);
			}
			else {
				backgroundImage.setPixel(sf::Vector2u(x, y), image.getPixel(sf::Vector2u(clipped.position.x + (x - r.position.x), clipped.position.y + (y - r.position.y))));
			}
		}
	}
	image.copy(backgroundImage, sf::Vector2u(clipped.position), r, false);
}

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect) {
	
	sf::IntRect srcB(sf::Vector2i(0,0), sf::Vector2i(src.getSize()));
	/////////////

	if (!srcRect.findIntersection(srcB).has_value())
		return;

	sf::IntRect s = srcRect.findIntersection(srcB).value();

	if (s.size.x > dst.getSize().x)
		s.size.x = dst.getSize().x;

	if (s.size.y > dst.getSize().y)
		s.size.y = dst.getSize().y;

	if (s.size.x <= 0 || s.size.y <= 0) 
		return;

	dst.copy(src, sf::Vector2u(0,0), s, false);
}

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color emptyColor) {

	sf::IntRect srcB(sf::Vector2i(0,0), sf::Vector2i(src.getSize()));

	if (!srcRect.findIntersection(srcB).has_value())
		return;

	sf::IntRect s = srcRect.findIntersection(srcB).value();

	if (s.size.x > dst.getSize().x)
		s.size.x = dst.getSize().x;

	if (s.size.y > dst.getSize().y)
		s.size.y = dst.getSize().y;

	if (s.size.x <= 0 || s.size.y<= 0)
		return;

	sf::Image img;
	img.resize(src.getSize(), sf::Color::Transparent);

	for (int y = 0; y < img.getSize().y; y++) {
		for (int x = 0; x < img.getSize().x; x++) {
			if (!(src.getPixel(sf::Vector2u(x, y)) == sf::Color::Transparent || src.getPixel(sf::Vector2u(x,y)) == emptyColor)) {
				img.setPixel(sf::Vector2u(x, y), src.getPixel(sf::Vector2u(x, y)));
			}
		}
	}

	dst.copy(img, sf::Vector2u(0,0), s, false);

}

void copyImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor)
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

void pasteImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor)
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
	tmp.copy(src, sf::Vector2u(0, 0), s, true);
	tmp.createMaskFromColor(alphaColor);

	const sf::IntRect all(sf::Vector2i(0, 0), sf::Vector2i(tmp.getSize()));
	dst.copy(tmp, sf::Vector2u(dstX, dstY), all, true);
}



void pasteImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor)
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

//////////////////////////////////////////////////////////

Selection::Selection() {
	_state = SelectionState::None;
	_rect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(0, 0));
	_img = nullptr;
}

Selection::~Selection() {
	if(_img != nullptr)
		delete _img;
}

sf::IntRect Selection::normalizeRect() {
	sf::IntRect r = _rect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x= -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y= -r.size.y; }
	return r;
}

void Selection::paste(sf::Image* editorImage, sf::Image*& bufferSelection)
{

	if (bufferSelection != nullptr) {
		sf::IntRect rect = normalizeRect();

		int dstW = static_cast<int>(editorImage->getSize().x);
		int dstH = static_cast<int>(editorImage->getSize().y);
		int srcW = static_cast<int>(bufferSelection->getSize().x);
		int srcH = static_cast<int>(bufferSelection->getSize().y);

		int x0 = std::max(0, rect.position.x);
		int y0 = std::max(0, rect.position.y);
		int x1 = std::min(dstW, rect.position.x + rect.size.x);
		int y1 = std::min(dstH, rect.position.y + rect.size.y);

		for (int y = y0; y < y1; ++y) {
			int sy = y - rect.position.y;
			if (sy < 0 || sy >= srcH) continue;

			for (int x = x0; x < x1; ++x) {
				int sx = x - rect.position.x;
				if (sx < 0 || sx >= srcW) continue;

				editorImage->setPixel(sf::Vector2u(x,y), bufferSelection->getPixel(sf::Vector2u(sx, sy)));
			}
		}
	}
	else {
		bufferSelection = new sf::Image();
	}
	loadImageFromClipboard(*bufferSelection);
	_state = SelectionState::Selected;
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(bufferSelection->getSize()));
}


void Selection::copy(sf::Image* editorImage, sf::Image* bufferSelection)
{
	if (_state != SelectionState::Selected)
		return;

	sf::IntRect r = _rect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x = -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y = -r.size.y; }

	sf::IntRect imgRect(sf::Vector2i(0, 0), sf::Vector2i(editorImage->getSize()));
	if (!r.findIntersection(imgRect).has_value())
		return;

	sf::IntRect s = r.findIntersection(imgRect).value();


	bufferSelection = new sf::Image();
	bufferSelection->resize(sf::Vector2u(s.size), sf::Color::Transparent);
	bufferSelection->copy(*editorImage, sf::Vector2u(0, 0), s, false);

	copyImageToClipboard(*bufferSelection, sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(s.size.x, s.size.y)));
}


void Selection::cut(sf::Image* editorImage, sf::Image* bufferSelection, sf::Color emptyColor) {
	if (_state == SelectionState::Selected) {
		if (_img == nullptr) {
			bufferSelection = new sf::Image();
			bufferSelection->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);
			bufferSelection->copy(*editorImage, sf::Vector2u(0, 0), _rect, false);
			copyImageToClipboard(*bufferSelection, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(bufferSelection->getSize())));
		}
		else {
			copyImageToClipboard(*bufferSelection, _rect);
		}

		sf::Image empty_rect = sf::Image();
		empty_rect.resize(sf::Vector2u(_rect.size), emptyColor);
		editorImage->copy(empty_rect, sf::Vector2u(_rect.position));
		delete bufferSelection;
		bufferSelection = nullptr;
		_state = SelectionState::None;
		_rect = sf::IntRect(sf::Vector2i(-1, -1), sf::Vector2i(-1, -1));
	}
}

bool Selection::clickOnSelection(sf::Vector2i point) {

	return normalizeRect().contains(point);
}

void Selection::draw(const sf::Vector2i& canvasPos, sf::Vector2i canvasSize, float scale) {


	sf::IntRect normRect = normalizeRect();

	if (_state != SelectionState::None && normRect.size.x > 0 && normRect.size.y > 0) {



		if (_img != nullptr) {
			sf::Texture tex;
			tex.resize(_img->getSize());
			tex.loadFromImage(*_img);

			sf::IntRect canvasRect({ 0, 0 }, canvasSize);
			

			////////////////
			if (normRect.findIntersection(canvasRect).has_value()) {
				sf::IntRect visibleRect = normRect.findIntersection(canvasRect).value();
	
				sf::Vector2i txy = visibleRect.position - normRect.position;
				sf::IntRect texRect(txy, visibleRect.size);

				sf::Sprite spr(tex);
				spr.setTextureRect(texRect);
				spr.setPosition(sf::Vector2f(canvasPos) + sf::Vector2f(visibleRect.position.x * scale, visibleRect.position.y * scale));
				spr.setScale(sf::Vector2f(scale, scale));
				window->draw(spr);
			}

		}

		sf::RectangleShape r(sf::Vector2f(float(normRect.size.x) * scale, float(normRect.size.y) * scale));
		r.setPosition(sf::Vector2f(canvasPos) + sf::Vector2f(float(normRect.position.x) * scale, float(normRect.position.y) * scale));
		r.setFillColor(sf::Color::Transparent);
		r.setOutlineThickness(4.0f);
		r.setOutlineColor(sf::Color(47, 127, 127, 255));
		window->draw(r);

	}


}

Selection* selection = nullptr;