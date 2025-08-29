#ifndef Selection_hpp
#define Selection_hpp

void remove(sf::Image& image, sf::IntRect rect)
{
	sf::Image src;
	src.create(rect.width, rect.height, sf::Color::Transparent);

	if (rect.width <= 0 || rect.height <= 0) {
		rect.width = int(src.getSize().x);
		rect.height = int(src.getSize().y);
	}

	const sf::IntRect canvas(0, 0, int(image.getSize().x), int(image.getSize().y));

	sf::IntRect clipped;
	if (!rect.intersects(canvas, clipped)) {
		return;
	}

	const sf::IntRect srcRect(clipped.left - rect.left, clipped.top - rect.top, clipped.width, clipped.height);

	image.copy(src,unsigned(clipped.left), unsigned(clipped.top), srcRect, false);
}

// 1) Zrzut fragmentu z warstwy do bufora selection->img (na (0,0))
void copy(sf::Image* dst, const sf::Image* src, sf::IntRect srcRect) {
	const sf::IntRect srcB(0, 0, int(src->getSize().x), int(src->getSize().y));
	sf::IntRect s;
	if (!srcRect.intersects(srcB, s)) return;

	int dw = int(dst->getSize().x), dh = int(dst->getSize().y);
	if (s.width > dw) s.width = dw;
	if (s.height > dh) s.height = dh;
	if (s.width <= 0 || s.height <= 0) return;

	dst->copy(*src, 0u, 0u, s, false);
}

void paste(sf::Image* dst, sf::Image* src, int dstX, int dstY) {
	sf::IntRect s(0, 0, int(src->getSize().x), int(src->getSize().y));

	if (dstX < 0) { s.left -= dstX; s.width += dstX; dstX = 0; }
	if (dstY < 0) { s.top -= dstY; s.height += dstY; dstY = 0; }

	const int dw = int(dst->getSize().x), dh = int(dst->getSize().y);
	if (dstX >= dw || dstY >= dh) return;
	if (dstX + s.width > dw) s.width = dw - dstX;
	if (dstY + s.height > dh) s.height = dh - dstY;
	if (s.width <= 0 || s.height <= 0) return;

	dst->copy(*src, unsigned(dstX), unsigned(dstY), s, true);
}

enum class SelectionState { None, Selecting, Selected, Moving };

class Selection {
public:	
	SelectionState state;
	sf::IntRect rect;
	sf::Vector2i offset;
	sf::Image* img;

	Selection() {
		state = SelectionState::None;
		rect = sf::IntRect(0, 0, 0, 0);
		img = nullptr;
	}

	sf::IntRect normalizeRect() {
		sf::IntRect r = rect;
		if (r.width < 0) { r.left += r.width; r.width = -r.width; }
		if (r.height < 0) { r.top += r.height; r.height = -r.height; }
		return r;
	} 

	void paste(sf::Image* editorImage, sf::Image*& bufferSelection)
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


	void copy(sf::Image* editorImage, sf::Image* bufferSelection)
	{
		if (state != SelectionState::Selected) 
			return;

		sf::IntRect r = rect;
		if (r.width < 0) { r.left += r.width; r.width = -r.width; }
		if (r.height < 0) { r.top += r.height; r.height = -r.height; }

		sf::IntRect imgRect(0, 0,editorImage->getSize().x, editorImage->getSize().y);
		sf::IntRect s;
		if (!r.intersects(imgRect, s)) 
			return;

		bufferSelection = new sf::Image();
		bufferSelection->create(s.width, s.height, sf::Color::Transparent);
		bufferSelection->copy(*editorImage, 0, 0, s, false);

		copyImageToClipboard(bufferSelection, sf::IntRect(0, 0, s.width, s.height));
	}


	void cut(sf::Image* editorImage, sf::Image* bufferSelection, sf::Color emptyColor) {
		if (state == SelectionState::Selected) {
			if (img == nullptr) {
				bufferSelection = new sf::Image();
				bufferSelection->create(rect.width, rect.height, sf::Color::Transparent);
				bufferSelection->copy(*editorImage, 0, 0, rect, false);
				copyImageToClipboard(bufferSelection, sf::IntRect(0,0,bufferSelection->getSize().x, bufferSelection->getSize().y));

				sf::Image empty_rect = sf::Image();
				empty_rect.create(rect.width, rect.height, emptyColor);
				editorImage->copy(empty_rect, rect.left, rect.top);
	
			}
			else {
				copyImageToClipboard(bufferSelection, rect);
				delete bufferSelection;
				bufferSelection = nullptr;
				state == SelectionState::None;
				rect = sf::IntRect(-1, -1, -1, -1);
			}
		}
	}

	bool clickOnSelection(sf::Vector2i point) {

		return normalizeRect().contains(point);
	}

	void draw(const sf::Vector2f& canvasPos, sf::Vector2i canvasSize, float scale) {


		sf::IntRect normRect = normalizeRect();

		if (state!= SelectionState::None && normRect.width > 0 && normRect.height > 0) {

			

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
};

Selection* selection = nullptr;
#endif