#include "Tools/Lasso.hpp"
#include "Clipboard.hpp"
#include <iostream>
#include "Window.hpp"

std::string shader_source = R"(
    uniform sampler2D texture;
    uniform sampler2D mask;
	uniform vec3 alphaColor;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec2 muv = uv;

		muv.y = 1.0 - muv.y;	// flip Y dla renderTexture
        
		vec4 c = texture2D(texture, uv);
		float alpha;
		if( c.r == alphaColor.r/255.0 && c.g == alphaColor.g/255.0 && c.b == alphaColor.b/255.0)
			alpha = 0;
		else
			alpha = texture2D(mask, muv).a;
        gl_FragColor = vec4(c.rgb, c.a * alpha);
    }
)";


Lasso::Lasso() {
	_state = LassoState::None;

	_points.clear();

	_offset = sf::Vector2i(0, 0);

	_image = nullptr;
	_rect = sf::IntRect(0, 0, 0, 0);
	_outlineOffset = sf::Vector2i(0, 0);

	_shader.loadFromMemory(shader_source, sf::Shader::Fragment);

}

Lasso::~Lasso() {

}

void Lasso::shiftOriginIfNeeded(sf::Vector2i& point)
{
	sf::Vector2i shift(0, 0);
	if (point.x < 0) { shift.x = point.x; point.x = 0; }
	if (point.y < 0) { shift.y = point.y; point.y = 0; }

	if (shift.x != 0 || shift.y != 0) {

		_outlineOffset += shift;

		for (auto& p : _points)
			p -= shift;
	}
}

void Lasso::addPoint(sf::Vector2i point)   // tile = globalny punkt w kafelkach
{
	point = point - _outlineOffset;

	shiftOriginIfNeeded(point);

	if (_points.empty() || std::hypot(float(_points.back().x - point.x), float(_points.back().y - point.y)) > 0.0f)
	{
		_points.push_back(point);
		// std::cout << "add point local: " << point.x << ", " << point.y << "\n";
	}
}

void Lasso::unselect() {
	_points.clear();
	_outlineOffset = sf::Vector2i(0, 0);
	generateRect();
}

void Lasso::generateRect() {
	if (_points.size() < 3) {
		_rect = sf::IntRect(0, 0, 0, 0);
		return;
	}

	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();

	for (auto& p : _points) {
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
	}

	int width = maxX + 1;
	int height = maxY + 1;

	_rect = sf::IntRect(_outlineOffset.x, _outlineOffset.y, width, height);
}

bool Lasso::clickOnSelection(sf::Vector2i point) {

	return _rect.contains(point);
}

void Lasso::copy(sf::Image* canvas, sf::Color emptyColor)
{

	if (_state != LassoState::Selected)
		return;

	this->generateRect();

	sf::IntRect r = _rect;
	if (r.width < 0) { r.left += r.width; r.width = -r.width; }
	if (r.height < 0) { r.top += r.height; r.height = -r.height; }

	sf::IntRect imgRect(0, 0, canvas->getSize().x, canvas->getSize().y);
	sf::IntRect s;
	if (!r.intersects(imgRect, s))
		return;

	if (s.width <= 0 || s.height <= 0)
		return;

	sf::RenderTexture* mask = generateMask();
	sf::Image maskImg = mask->getTexture().copyToImage();
	delete mask;

	for (int y = 0; y < s.height; ++y)
		for (int x = 0; x < s.width; ++x)
			if (maskImg.getPixel(x, y) != sf::Color::White || _image->getPixel(x, y) == emptyColor)
				_image->setPixel(x, y, sf::Color::Transparent);

	copyImageToClipboard(_image, sf::IntRect(0, 0, s.width, s.height));

}



void Lasso::paste(sf::Image* canvas, sf::Color emptyColor)
{

	if (_image != nullptr) {
		generateRect();

		int dstW = static_cast<int>(canvas->getSize().x);
		int dstH = static_cast<int>(canvas->getSize().y);
		int srcW = static_cast<int>(_image->getSize().x);
		int srcH = static_cast<int>(_image->getSize().y);

		int x0 = std::max(0, _rect.left);
		int y0 = std::max(0, _rect.top);
		int x1 = std::min(dstW, _rect.left + _rect.width);
		int y1 = std::min(dstH, _rect.top + _rect.height);

		for (int y = y0; y < y1; ++y) {
			int sy = y - _rect.top;
			if (sy < 0 || sy >= srcH) continue;

			for (int x = x0; x < x1; ++x) {
				int sx = x - _rect.left;
				if (sx < 0 || sx >= srcW) continue;

				if (_image->getPixel(sx, sy) != emptyColor && _image->getPixel(sx, sy) != sf::Color::Transparent)
					canvas->setPixel(x, y, _image->getPixel(sx, sy));
			}
		}
	}
	else {
		_image = new sf::Image();
	}
	loadImageFromClipboard(*_image);
	_state = LassoState::Selected;
	
	unselect();
	addPoint(sf::Vector2i(0, 0));
	addPoint(sf::Vector2i(0, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, 0));
}

void Lasso::cut(sf::Image* canvas, sf::Color emptyColor) {
	if (_state == LassoState::Selected) {

		if (_image == nullptr) {
			std::cout << "click\n";
			_image = new sf::Image();
			_image->create(_rect.width, _rect.height, sf::Color::Transparent);
			_image->copy(*canvas, 0, 0, _rect, false);
			copyImageToClipboard(_image, sf::IntRect(0, 0, _image->getSize().x, _image->getSize().y));
		}
		else {
			copyImageToClipboard(_image, sf::IntRect(0, 0, _image->getSize().x, _image->getSize().y));
			//copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y), mask);
		}

		delete _image;
		_image = nullptr;
		_state = LassoState::None;
		_rect = sf::IntRect(-1, -1, -1, -1);
	}
}

void Lasso::generateOutline(bool selectionComplete) {
	if (_points.size() < 3) return;

	if (_rect.width <= 0 || _rect.height <= 0)
		return;

	if (_outlineRenderTexture.getSize() != sf::Vector2u(_rect.width, _rect.height))
		_outlineRenderTexture.create(_rect.width, _rect.height);

	_outlineRenderTexture.clear(sf::Color(0, 0, 0, 0));

	sf::Color lassoColor = sf::Color(47, 127, 127, 255);

	sf::VertexArray lines(sf::LineStrip);
	for (auto& point : _points)
		lines.append(sf::Vertex(sf::Vector2f(point), lassoColor));

	// first and last point
	sf::VertexArray p(sf::Points);
	p.append(sf::Vertex(sf::Vector2f(_points.front()), lassoColor));
	p.append(sf::Vertex(sf::Vector2f(_points.back()), lassoColor));

	if (selectionComplete)
		lines.append(sf::Vertex(sf::Vector2f(_points.front()), lassoColor));

	sf::RenderStates rs;
	rs.blendMode = sf::BlendAlpha;
	rs.transform.translate(0.5f, 0.5f);
	_outlineRenderTexture.draw(lines, rs);
	_outlineRenderTexture.draw(p, rs);
	_outlineRenderTexture.display();
}

sf::RenderTexture* Lasso::generateMask() {
	sf::RenderTexture* mask = new sf::RenderTexture();
	mask->create(_image->getSize().x, _image->getSize().y);

	//std::cout << image->getSize().x << ", " << image->getSize().y << "\n";

	if (_points.size() >= 3) {
		mask->clear(sf::Color::Transparent);

		sf::ConvexShape poly;
		poly.setPointCount(_points.size());
		for (std::size_t i = 0; i < _points.size(); ++i)
			poly.setPoint(i, sf::Vector2f(_points[i]));

		poly.setFillColor(sf::Color::White);
		poly.setOutlineThickness(0.5f);
		poly.setOutlineColor(sf::Color::White);

		sf::RenderStates st;
		st.blendMode = sf::BlendNone;
		st.transform.translate(0.5f, 0.5f);
		mask->draw(poly, st);
	}
	else {
		mask->clear(sf::Color::White);
	}

	mask->display();
	mask->setSmooth(false);
	return mask;
}

void Lasso::drawImage(sf::Vector2f canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor, bool useMask) {
	if (!_image) return;

	if (_image->getSize().x < 1 || _image->getSize().y < 1)
		return;

	generateRect();
	sf::IntRect canvasRect(0, 0, canvasSize.x, canvasSize.y);
	sf::IntRect visibleRect;

	if (_rect.intersects(canvasRect, visibleRect)) {
		int tx = visibleRect.left - _outlineOffset.x;
		int ty = visibleRect.top - _outlineOffset.y;

		sf::IntRect texRect(tx, ty, visibleRect.width, visibleRect.height);

		_texture = sf::Texture();
		_texture.loadFromImage(*_image);
		_texture.setSmooth(false);

		sf::RenderTexture* mask = generateMask();

		_shader.setUniform("texture", _texture);
		_shader.setUniform("mask", mask->getTexture());
		_shader.setUniform("alphaColor", sf::Vector3f(alphaColor.r, alphaColor.g, alphaColor.b));

		_sprite = sf::Sprite(_texture);
		_sprite.setTextureRect(texRect);
		_sprite.setScale(scale, scale);
		_sprite.setPosition(canvasPosition + sf::Vector2f(visibleRect.left, visibleRect.top) * scale);

		sf::RenderStates rs;
		rs.shader = &_shader;
		window->draw(_sprite, rs);
		delete mask;

	}


}

void Lasso::drawOutline(sf::Vector2f canvasPosition, float scale) {
	_outlineSprite = sf::Sprite(_outlineRenderTexture.getTexture());
	_outlineSprite.setScale(scale, scale);
	_outlineSprite.setPosition(canvasPosition + sf::Vector2f(_outlineOffset) * scale);
	window->draw(_outlineSprite);

}

void Lasso::drawRect(sf::Vector2f canvasPosition, float scale) {
	sf::RectangleShape r(sf::Vector2f(float(_rect.width) * scale, float(_rect.height) * scale));
	r.setPosition(canvasPosition + sf::Vector2f(_rect.left * scale, _rect.top * scale));
	r.setFillColor(sf::Color(127, 47, 47, 127));
	r.setOutlineColor(sf::Color(47, 127, 127, 255));
	r.setOutlineThickness(4.0f);
	window->draw(r);
}


void Lasso::draw(sf::Vector2f canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor) {

	if (_points.size() >= 3) {
		if (_state == LassoState::Selecting) {
			drawImage(canvasPosition, canvasSize, scale, alphaColor, false);
			generateOutline(false);
			drawOutline(canvasPosition, scale);

		}

		if (_state == LassoState::Selected || _state == LassoState::Moving) {
			drawImage(canvasPosition, canvasSize, scale, alphaColor, true);
			drawRect(canvasPosition, scale);
			generateOutline(true);
			drawOutline(canvasPosition, scale);

		}
	}

}

Lasso* lasso = nullptr;