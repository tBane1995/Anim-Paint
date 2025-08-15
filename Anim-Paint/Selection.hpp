#ifndef Selection_hpp
#define Selection_hpp

enum class SelectionState { Idle, Selecting, Selected, Moving };

class Selection {
public:	
	SelectionState state;
	sf::IntRect rect;
	sf::Vector2i offset;
	bool hasImage;
	sf::Image img;

	Selection() {
		state = SelectionState::Idle;
		rect = sf::IntRect(0, 0, 0, 0);
		hasImage = false;
		img = sf::Image();
	}

	sf::IntRect normalizeRect(sf::IntRect r) {
		if (r.width < 0) { r.left += r.width; r.width = -r.width; }
		if (r.height < 0) { r.top += r.height; r.height = -r.height; }
		return r;
	}

	void copyImage(sf::Image& image, sf::IntRect rect) {

		// copy the image
		this->img = sf::Image();
		this->img.create(rect.width, rect.height, sf::Color::Transparent);
		this->img.copy(image, 0, 0, rect, false);
		hasImage = true;

	}

	void cutImage(sf::Image& image, sf::IntRect rect) {

		// cut the image
		sf::Image background;
		background.create(rect.width, rect.height, sf::Color::Transparent);
		image.copy(background, rect.left, rect.top, sf::IntRect(0, 0, rect.width, rect.height), false);
	}

	void pasteImage(sf::Image& image, sf::IntRect rect) {
		sf::IntRect image_rect;
		image_rect.left = 0;
		image_rect.top = 0;
		image_rect.width = image.getSize().x;
		image_rect.height = image.getSize().y;
		if(image_rect.intersects(rect))
			image.copy(this->img, rect.left, rect.top, sf::IntRect(0, 0, this->img.getSize().x, this->img.getSize().y), true);
		hasImage = false;
		this->img = sf::Image();
	}

	

	bool clickOnSelection(sf::Vector2i point) {

		return rect.contains(point);
	}

	void draw(const sf::Vector2f& canvasPos, sf::IntRect rect, float scale) {

		rect = normalizeRect(rect);

		if (rect.width > 0 && rect.height > 0) {

			if (hasImage) {
				sf::Texture tex;
				tex.create(img.getSize().x, img.getSize().y);
				tex.loadFromImage(img);
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