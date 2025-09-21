#include "Tools/Selection.hpp"
#include "Clipboard.hpp"
#include "Window.hpp"

void remove(sf::Image& image, sf::IntRect rect, sf::Color alphaColor)
{
	if (rect.width <= 0 || rect.height <= 0)
		return;

	sf::Image backgroundImage;
	backgroundImage.create(rect.width, rect.height, alphaColor);

	sf::IntRect clipped;
	if (!rect.intersects(rect, clipped)) {
		return;
	}

	sf::IntRect r(clipped.left - rect.left, clipped.top - rect.top, clipped.width, clipped.height);

	image.copy(backgroundImage, clipped.left, clipped.top, r, false);
}

void remove(sf::Image& image, sf::IntRect rect, sf::RenderTexture* mask, sf::Color alphaColor)
{
	if (rect.width <= 0 || rect.height <= 0)
		return;

	sf::Image backgroundImage;
	backgroundImage.create(rect.width, rect.height, alphaColor);

	sf::IntRect clipped;
	if (!rect.intersects(rect, clipped)) {
		return;
	}

	sf::IntRect r(clipped.left - rect.left, clipped.top - rect.top, clipped.width, clipped.height);

	sf::Image m = sf::Image();
	m = mask->getTexture().copyToImage();

	for (int y = 0; y < backgroundImage.getSize().y; y++) {
		for (int x = 0; x < backgroundImage.getSize().x; x++) {
			if (m.getPixel(x, y) == sf::Color::White) {
				backgroundImage.setPixel(x, y, alphaColor);
			}
			else {
				backgroundImage.setPixel(x, y, image.getPixel(clipped.left + (x - r.left), clipped.top + (y - r.top)));
			}
		}
	}
	image.copy(backgroundImage, clipped.left, clipped.top, r, false);
}

void copy(sf::Image* dst, sf::Image* src, sf::IntRect srcRect) {
	
	sf::IntRect srcB(0, 0, src->getSize().x, src->getSize().y);
	
	sf::IntRect s;
	if (!srcRect.intersects(srcB, s)) 
		return;

	if (s.width > dst->getSize().x)
		s.width = dst->getSize().x;

	if (s.height > dst->getSize().y)
		s.height = dst->getSize().y;

	if (s.width <= 0 || s.height <= 0) 
		return;

	dst->copy(*src, 0, 0, s, false); // how not copy sf::Color transparentColor
}

void copy(sf::Image* dst, sf::Image* src, sf::IntRect srcRect, sf::Color emptyColor) {

	sf::IntRect srcB(0, 0, src->getSize().x, src->getSize().y);

	sf::IntRect s;
	if (!srcRect.intersects(srcB, s))
		return;

	if (s.width > dst->getSize().x)
		s.width = dst->getSize().x;

	if (s.height > dst->getSize().y)
		s.height = dst->getSize().y;

	if (s.width <= 0 || s.height <= 0)
		return;

	sf::Image img;
	img.create(src->getSize().x, src->getSize().y, sf::Color::Transparent);

	for (int y = 0; y < img.getSize().y; y++) {
		for (int x = 0; x < img.getSize().x; x++) {
			if (!(src->getPixel(x, y) == sf::Color::Transparent || src->getPixel(x, y) == emptyColor)) {
				img.setPixel(x, y, src->getPixel(x, y));
			}
		}
	}

	dst->copy(img, 0, 0, s, false); // how not copy sf::Color transparentColor

}

void paste(sf::Image* dst, sf::Image* src, int dstX, int dstY, sf::Color alphaColor)
{
	sf::IntRect s(0, 0, int(src->getSize().x), int(src->getSize().y));

	if (dstX < 0) { s.left -= dstX; s.width += dstX; dstX = 0; }
	if (dstY < 0) { s.top -= dstY; s.height += dstY; dstY = 0; }

	const int dw = int(dst->getSize().x), dh = int(dst->getSize().y);
	if (dstX >= dw || dstY >= dh) return;
	if (dstX + s.width > dw) s.width = dw - dstX;
	if (dstY + s.height > dh) s.height = dh - dstY;
	if (s.width <= 0 || s.height <= 0) return;

	sf::Image tmp;
	tmp.create(s.width, s.height, sf::Color::Transparent);
	tmp.copy(*src, 0, 0, s, true);
	tmp.createMaskFromColor(alphaColor);

	const sf::IntRect all(0, 0, int(tmp.getSize().x), int(tmp.getSize().y));
	dst->copy(tmp, unsigned(dstX), unsigned(dstY), all, true);
}

void paste(sf::Image* dst, sf::Image* src, int dstX, int dstY, sf::RenderTexture* mask, sf::Color alphaColor)
{
	if (!dst || !src) return;

	int dw = dst->getSize().x;
	int dh = dst->getSize().y;
	int sw = src->getSize().x;
	int sh = src->getSize().y;

	if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0) 
		return;

	sf::IntRect place(dstX, dstY, sw, sh);
	sf::IntRect canvas(0, 0, dw, dh);
	sf::IntRect clip;

	if (!place.intersects(canvas, clip)) 
		return;

	sf::Image m;
	bool useMask = (mask != nullptr);
	if (useMask) 
		m = mask->getTexture().copyToImage();

	for (int y = clip.top; y < clip.top + clip.height; ++y) {
		for (int x = clip.left; x < clip.left + clip.width; ++x) {

			int sx = x - dstX;
			int sy = y - dstY;

			if (useMask) {
				if (m.getPixel(sx, sy) != sf::Color::White) 
					continue;
			}

			sf::Color c = src->getPixel(sx, sy);
			if (c == alphaColor || c.a == 0) 
				continue;

			dst->setPixel(x, y, c);
		}
	}
}

//////////////////////////////////////////////////////////

Selection::Selection() {
	state = SelectionState::None;
	rect = sf::IntRect(0, 0, 0, 0);
	img = nullptr;
}

Selection::~Selection() {

}

sf::IntRect Selection::normalizeRect() {
	sf::IntRect r = rect;
	if (r.width < 0) { r.left += r.width; r.width = -r.width; }
	if (r.height < 0) { r.top += r.height; r.height = -r.height; }
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

		int x0 = std::max(0, rect.left);
		int y0 = std::max(0, rect.top);
		int x1 = std::min(dstW, rect.left + rect.width);
		int y1 = std::min(dstH, rect.top + rect.height);

		for (int y = y0; y < y1; ++y) {
			int sy = y - rect.top;
			if (sy < 0 || sy >= srcH) continue;

			for (int x = x0; x < x1; ++x) {
				int sx = x - rect.left;
				if (sx < 0 || sx >= srcW) continue;

				editorImage->setPixel(x, y, bufferSelection->getPixel(sx, sy));
			}
		}
	}
	else {
		bufferSelection = new sf::Image();
	}
	loadImageFromClipboard(*bufferSelection);
	state = SelectionState::Selected;
	rect = sf::IntRect(0, 0, bufferSelection->getSize().x, bufferSelection->getSize().y);
}


void Selection::copy(sf::Image* editorImage, sf::Image* bufferSelection)
{
	if (state != SelectionState::Selected)
		return;

	sf::IntRect r = rect;
	if (r.width < 0) { r.left += r.width; r.width = -r.width; }
	if (r.height < 0) { r.top += r.height; r.height = -r.height; }

	sf::IntRect imgRect(0, 0, editorImage->getSize().x, editorImage->getSize().y);
	sf::IntRect s;
	if (!r.intersects(imgRect, s))
		return;

	bufferSelection = new sf::Image();
	bufferSelection->create(s.width, s.height, sf::Color::Transparent);
	bufferSelection->copy(*editorImage, 0, 0, s, false);

	copyImageToClipboard(bufferSelection, sf::IntRect(0, 0, s.width, s.height));
}


void Selection::cut(sf::Image* editorImage, sf::Image* bufferSelection, sf::Color emptyColor) {
	if (state == SelectionState::Selected) {
		if (img == nullptr) {
			bufferSelection = new sf::Image();
			bufferSelection->create(rect.width, rect.height, sf::Color::Transparent);
			bufferSelection->copy(*editorImage, 0, 0, rect, false);
			copyImageToClipboard(bufferSelection, sf::IntRect(0, 0, bufferSelection->getSize().x, bufferSelection->getSize().y));
		}
		else {
			copyImageToClipboard(bufferSelection, rect);
		}

		sf::Image empty_rect = sf::Image();
		empty_rect.create(rect.width, rect.height, emptyColor);
		editorImage->copy(empty_rect, rect.left, rect.top);
		delete bufferSelection;
		bufferSelection = nullptr;
		state = SelectionState::None;
		rect = sf::IntRect(-1, -1, -1, -1);
	}
}

bool Selection::clickOnSelection(sf::Vector2i point) {

	return normalizeRect().contains(point);
}

void Selection::draw(const sf::Vector2f& canvasPos, sf::Vector2i canvasSize, float scale) {


	sf::IntRect normRect = normalizeRect();

	if (state != SelectionState::None && normRect.width > 0 && normRect.height > 0) {



		if (img != nullptr) {
			sf::Texture tex;
			tex.create(img->getSize().x, img->getSize().y);
			tex.loadFromImage(*img);

			sf::IntRect canvasRect(0, 0, canvasSize.x, canvasSize.y);
			sf::IntRect visibleRect;

			if (normRect.intersects(canvasRect, visibleRect)) {
				int tx = visibleRect.left - normRect.left;
				int ty = visibleRect.top - normRect.top;

				sf::IntRect texRect(tx, ty, visibleRect.width, visibleRect.height);

				sf::Sprite spr(tex);
				spr.setTextureRect(texRect);
				spr.setPosition(canvasPos + sf::Vector2f(visibleRect.left * scale, visibleRect.top * scale));
				spr.setScale(scale, scale);
				window->draw(spr);
			}
		}

		sf::RectangleShape r(sf::Vector2f(float(normRect.width) * scale, float(normRect.height) * scale));
		r.setPosition(canvasPos + sf::Vector2f(float(normRect.left) * scale, float(normRect.top) * scale));
		r.setFillColor(sf::Color::Transparent);
		r.setOutlineThickness(4.0f);
		r.setOutlineColor(sf::Color(47, 127, 127, 255));
		window->draw(r);

	}


}

Selection* selection = nullptr;