#include "Tools/Lasso.hpp"
#include "Clipboard.hpp"
#include <iostream>
#include "Window.hpp"
#include "Textures.hpp"

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


Lasso::Lasso():
	_sprite(emptyTexture),
	_outlineSprite(emptyTexture) {

	_state = LassoState::None;

	_points.clear();

	_offset = sf::Vector2i(0, 0);

	_image = nullptr;
	_rect = sf::IntRect( sf::Vector2i( -1, -1), sf::Vector2i(-1, -1));
	_outlineOffset = sf::Vector2i(0, 0);

	_shader.loadFromMemory(shader_source, sf::Shader::Type::Fragment);

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
		for (auto& p : _points) {
			if (p == point) {
				return;
			}
		}
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
		_rect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(0,0));
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

	_rect = sf::IntRect(_outlineOffset, sf::Vector2i(width, height));
}

bool Lasso::clickOnSelection(sf::Vector2i point) {

	return _rect.contains(point);
}

void Lasso::copy(sf::Image* canvas, sf::Color emptyColor)
{

	if (_state != LassoState::Selected)
		return;

	generateRect();

	sf::IntRect r = _rect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x = -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y = -r.size.y; }

	sf::IntRect imgRect(sf::Vector2i(0,0), sf::Vector2i(canvas->getSize()));
	

	if (!r.findIntersection(imgRect).has_value())
		return;

	sf::IntRect s = r.findIntersection(imgRect).value();

	if (s.size.x <= 0 || s.size.y<= 0)
		return;

	sf::RenderTexture* mask = generateMask();
	sf::Image maskImg = mask->getTexture().copyToImage();
	

	for (int y = 0; y < s.size.y; ++y)
		for (int x = 0; x < s.size.x; ++x)
			if (maskImg.getPixel(sf::Vector2u(x, y)) != sf::Color::White || _image->getPixel(sf::Vector2u(x, y)) == emptyColor)
				_image->setPixel(sf::Vector2u(x, y), sf::Color::Transparent);

	copyImageToClipboard(_image, sf::IntRect(sf::Vector2i(0,0), s.size));
	delete mask;
}



void Lasso::paste(sf::Image* canvas, sf::Color emptyColor)
{

	if (_image != nullptr) {
		generateRect();

		int dstW = static_cast<int>(canvas->getSize().x);
		int dstH = static_cast<int>(canvas->getSize().y);
		int srcW = static_cast<int>(_image->getSize().x);
		int srcH = static_cast<int>(_image->getSize().y);

		int x0 = std::max(0, _rect.position.x);
		int y0 = std::max(0, _rect.position.y);
		int x1 = std::min(dstW, _rect.position.x + _rect.size.x);
		int y1 = std::min(dstH, _rect.position.y+ _rect.size.y);

		for (int y = y0; y < y1; ++y) {
			int sy = y - _rect.position.y;
			if (sy < 0 || sy >= srcH) continue;

			for (int x = x0; x < x1; ++x) {
				int sx = x - _rect.position.x;
				if (sx < 0 || sx >= srcW) continue;

				if (_image->getPixel(sf::Vector2u(sx, sy)) != emptyColor && _image->getPixel(sf::Vector2u(sx, sy)) != sf::Color::Transparent)
					canvas->setPixel(sf::Vector2u(sx, sy), _image->getPixel(sf::Vector2u(sx, sy)));
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
			_image = new sf::Image();
			_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);
			_image->copy(*canvas, sf::Vector2u(0, 0), _rect, false);
			copyImageToClipboard(_image, sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(_image->getSize())));
		}
		else {
			copyImageToClipboard(_image, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(_image->getSize())));
			//copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y), mask);
		}

		delete _image;
		_image = nullptr;
		_state = LassoState::None;
		_rect = sf::IntRect(sf::Vector2i(-1,-1), sf::Vector2i(-1,-1));
	}
}

void Lasso::generateOutline(bool selectionComplete) {
	//std::vector <sf::Vector2i> _points
	if (_points.size() < 3) return;


	if (_rect.size.x <= 0 || _rect.size.y <= 0)
		return;

	if (_outlineRenderTexture.getSize() != sf::Vector2u(_rect.size))
		_outlineRenderTexture.resize(sf::Vector2u(_rect.size));

	_outlineRenderTexture.clear(sf::Color(0, 0, 0, 0));

	sf::Color lassoColor = sf::Color(47, 127, 127, 255);

	sf::VertexArray lines(sf::PrimitiveType::LineStrip);
	for (auto& point : _points)
		lines.append(sf::Vertex{ sf::Vector2f(point), lassoColor });

	// first and last point
	sf::VertexArray p(sf::PrimitiveType::Points);

	p.append(sf::Vertex{ sf::Vector2f(_points.front()), lassoColor});
	p.append(sf::Vertex{ sf::Vector2f(_points.back()), lassoColor });

	if (selectionComplete)
		lines.append(sf::Vertex{ sf::Vector2f(_points.front()), lassoColor });

	sf::RenderStates rs;
	rs.blendMode = sf::BlendAlpha;
	rs.transform.translate(sf::Vector2f(0.5f, 0.5f));
	_outlineRenderTexture.draw(lines, rs);
	_outlineRenderTexture.draw(p, rs);
	_outlineRenderTexture.display();
}

sf::RenderTexture* Lasso::generateMask() {

	if (!_image) 
		return nullptr;

	if (_image->getSize().x == 0 || _image->getSize().y == 0) 
		return nullptr;

	sf::RenderTexture* mask = new sf::RenderTexture();
	mask->resize(_image->getSize());

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
		st.transform.translate(sf::Vector2f(0.5f, 0.5f));
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
	if (_image->getSize().x < 1 || _image->getSize().y < 1) return;
	generateRect();
	if (_rect.size.x < 2 || _rect.size.y < 2) return;

	sf::IntRect canvasRect(sf::Vector2i(0, 0), canvasSize);
	
	if (!_rect.findIntersection(canvasRect).has_value())
		return;

	sf::IntRect visibleRect = _rect.findIntersection(canvasRect).value();


	int tx = visibleRect.position.x - _outlineOffset.x;
	int ty = visibleRect.position.y - _outlineOffset.y;
	std::wcout << tx << ", " << ty << "\n";
	sf::IntRect texRect(sf::Vector2i(tx, ty), visibleRect.size);

	_texture = sf::Texture();
	_texture.resize(_image->getSize());
	_texture.loadFromImage(*_image);
	_texture.setSmooth(false);

	sf::RenderTexture* mask = generateMask();
	if (mask == nullptr) return;

	_shader.setUniform("texture", _texture);
	_shader.setUniform("mask", mask->getTexture());
	_shader.setUniform("alphaColor", sf::Vector3f(alphaColor.r, alphaColor.g, alphaColor.b));

	_sprite = sf::Sprite(_texture);
	_sprite.setTextureRect(texRect);
	_sprite.setScale(sf::Vector2f(scale, scale));
	_sprite.setPosition(canvasPosition + sf::Vector2f(visibleRect.position.x, visibleRect.position.y) * scale);

	sf::RenderStates rs;
	rs.shader = &_shader;
	window->draw(_sprite, rs);
	delete mask;


}

void Lasso::drawOutline(sf::Vector2f canvasPosition, float scale) {


	_outlineSprite = sf::Sprite(_outlineRenderTexture.getTexture());
	_outlineSprite.setScale(sf::Vector2f(scale, scale));
	_outlineSprite.setPosition(canvasPosition + sf::Vector2f(_outlineOffset) * scale);
	window->draw(_outlineSprite);

}

void Lasso::drawRect(sf::Vector2f canvasPosition, float scale) {
	sf::RectangleShape r(sf::Vector2f(float(_rect.size.x) * scale, float(_rect.size.y) * scale));
	r.setPosition(canvasPosition + sf::Vector2f(_rect.position.x * scale, _rect.position.y * scale));
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
			//generateOutline(true);
			//drawOutline(canvasPosition, scale);
		}
	}

}

Lasso* lasso = nullptr;