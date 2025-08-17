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
	sf::Image* img = nullptr;

	Selection() {
		state = SelectionState::None;
		rect = sf::IntRect(0, 0, 0, 0);
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
			editorImage->copy(*bufferSelection, rect.left, rect.top, sf::IntRect(0,0,rect.width, rect.height), true);
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

		return rect.contains(point);
	}

	void draw(const sf::Vector2f& canvasPos, sf::IntRect rect, float scale) {

		rect = normalizeRect();

		if (rect.width > 0 && rect.height > 0) {

			if (img != nullptr) {
				sf::Texture tex;
				tex.create(img->getSize().x, img->getSize().y);
				tex.loadFromImage(*img);
				sf::Sprite spr(tex);
				spr.setPosition(canvasPos + sf::Vector2f(rect.left * scale, rect.top * scale));
				spr.setScale(scale, scale);
				window->draw(spr);
			}

			sf::RectangleShape r(sf::Vector2f(float(rect.width) * scale, float(rect.height) * scale));
			r.setPosition(canvasPos + sf::Vector2f(float(rect.left) * scale, float(rect.top) * scale));
			r.setFillColor(sf::Color(255, 47, 47, 127));
			window->draw(r);

			
		}
		

	}
};

Selection* selection = nullptr;
#endif