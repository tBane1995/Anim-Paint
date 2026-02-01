#include "Tools/Lasso.hpp"
#include "Clipboard.hpp"
#include "Window.hpp"
#include "Textures.hpp"
#include "Tools/Selection.hpp"
#include "DebugLog.hpp"

std::string shader_source = R"(
    uniform sampler2D texture;
    uniform sampler2D mask;
	uniform vec3 alphaColor;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
		vec4 c = texture2D(texture, uv);
		float alpha = (c.r == alphaColor.r/255.0 && c.g == alphaColor.g/255.0 && c.b == alphaColor.b/255.0)
					  ? 0.0
					  : texture2D(mask, uv).a;
		gl_FragColor = vec4(c.rgb, c.a * alpha);
    }
)";


Lasso::Lasso() {

	_state = LassoState::None;

	_points.clear();

	_offset = sf::Vector2i(0, 0);

	_image = nullptr;
	_rect = sf::IntRect( sf::Vector2i( 0,0), sf::Vector2i(0,0));
	_outlineOffset = sf::Vector2i(0, 0);

	generateMask();

	if (!_shader.loadFromMemory(shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Lasso::Lasso: Failed to load shader from memory.");
		exit(0);
	}

	_sprite = nullptr;
	_outlineSprite = nullptr;
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

void Lasso::addPoint(sf::Vector2i point) {
	point -= _outlineOffset;
	shiftOriginIfNeeded(point);

	if (_points.empty()) {
		_points.push_back(point);
		return;
	}

	if (_points.back() == point) return;

	_points.push_back(point);
}

void Lasso::unselect() {
	_points.clear();
	_outlineOffset = sf::Vector2i(0, 0);
}

void Lasso::generateRect() {

	if (_points.empty()) {
		_rect = sf::IntRect({ 0,0 }, { 0,0 });
		return;
	}

	int minX = std::numeric_limits<int>::max();
	int minY = std::numeric_limits<int>::max();
	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();

	for (auto& p : _points) {
		minX = std::min(minX, p.x);
		minY = std::min(minY, p.y);
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
	}

	_rect = sf::IntRect(
		_outlineOffset + sf::Vector2i(minX, minY),
		sf::Vector2i(maxX - minX + 1, maxY - minY + 1)
	);
}

bool Lasso::clickOnSelection(sf::Vector2i point) {

	if (_rect.size.x <= 1 || _rect.size.y <= 1)
		return false;

	return _rect.contains(point);
}

void Lasso::copy(sf::Image& canvas, sf::Color emptyColor)
{

	if (_state != LassoState::Selected)
		return;

	generateRect();

	sf::IntRect r = _rect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x = -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y = -r.size.y; }
	
	pasteImageWithMask(canvas, *_image, r.position.x, r.position.y, _maskImage, emptyColor);
	copyImageWithAlpha(*lasso->_image, canvas, _rect, emptyColor);

	sf::IntRect canvasRect(sf::Vector2i(0,0), sf::Vector2i(canvas.getSize()));

	if (!r.findIntersection(canvasRect).has_value())
		return;

	sf::IntRect s = r.findIntersection(canvasRect).value();

	if (s.size.x <= 0 || s.size.y<= 0)
		return;

	generateMask();

	sf::Image copiedImage = sf::Image();
	copiedImage.resize(sf::Vector2u(s.size), sf::Color::Transparent);

	for (int y = 0; y < s.size.y; ++y)
		for (int x = 0; x < s.size.x; ++x) {

			if (_maskImage.getPixel(sf::Vector2u(x, y)) != sf::Color::White)
				copiedImage.setPixel(sf::Vector2u(x, y), sf::Color::Transparent);
			else {

				if(_image->getPixel(sf::Vector2u(x,y)) == emptyColor)
					copiedImage.setPixel(sf::Vector2u(x, y), sf::Color::Transparent);
				else
					copiedImage.setPixel(sf::Vector2u(x, y), _image->getPixel(sf::Vector2u(x, y)));

			}
		}

	copyImageToClipboard(copiedImage, sf::IntRect(sf::Vector2i(0,0), s.size));

}

void Lasso::paste(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor)
{

	int dw = dst.getSize().x;
	int dh = dst.getSize().y;
	int sw = src.getSize().x;
	int sh = src.getSize().y;

	if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0)
		return;

	sf::IntRect place(sf::Vector2i(dstX, dstY), sf::Vector2i(sw, sh));
	sf::IntRect canvas(sf::Vector2i(0, 0), sf::Vector2i(dw, dh));
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

void Lasso::paste(sf::Image& canvas, sf::Color emptyColor)
{

	if (_image != nullptr) {
		paste(canvas, *_image, _rect.position.x, _rect.position.y, _maskImage, emptyColor);
	}
	else {
		_image = std::make_shared<sf::Image>();
	}

	loadImageFromClipboard(*_image);
	_state = LassoState::Selected;
	
	unselect();
	addPoint(sf::Vector2i(0, 0));
	addPoint(sf::Vector2i(0, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, 0));
}

void Lasso::cut(sf::Image& canvas, sf::Color emptyColor) {
	if (_state == LassoState::Selected) {

		if (_image == nullptr) {
			_image = std::make_shared<sf::Image>();
			_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);
			
			if (!_image->copy(canvas, sf::Vector2u(0, 0), _rect, false)) {
				DebugError(L"Lasso::cut: Failed to copy image from canvas.");
				exit(0);
			}

			copyImageToClipboard(*_image, sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(_image->getSize())));
		}
		else {
			copyImageToClipboard(*_image, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(_image->getSize())));
			//copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y), mask);
		}

		_image = nullptr;
		_state = LassoState::None;
		_rect = sf::IntRect(sf::Vector2i(-1,-1), sf::Vector2i(-1,-1));
	}
}

void Lasso::generateOutline(bool selectionComplete) {
	//std::vector <sf::Vector2i> _points
	if (_points.size() < 1) return;

	if (_rect.size.x <= 0 || _rect.size.y <= 0)
		return;

	
	if (!_outlineRenderTexture.resize(sf::Vector2u(_rect.size))) {
		DebugError(L"Lasso::generateOutline: Failed to resize outline render texture.");
		exit(0);
	}

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
bool Lasso::pointOnSegment( sf::Vector2i p, sf::Vector2i a, sf::Vector2i b )
{
    int cross = 1LL *( b.x - a.x ) *( p.y - a.y ) - 1LL *( b.y - a.y ) *( p.x - a.x );
   
    if( cross != 0 )
         return false;
   
    int minx = std::min( a.x, b.x );
    int maxx = std::max( a.x, b.x );
    int miny = std::min( a.y, b.y );
    int maxy = std::max( a.y, b.y );
   
    return( p.x >= minx && p.x <= maxx && p.y >= miny && p.y <= maxy );
}

bool Lasso::isPointInPolygon( sf::Vector2i p, std::vector < sf::Vector2i > & poly )
{
    size_t n = poly.size();
    if( n < 3 )
         return false;
   
    bool inside = false;
    for( size_t i = 0, j = n - 1; i < n; j = i++ ) {
        sf::Vector2i & a = poly[ j ];
        sf::Vector2i & b = poly[ i ];
       
        if( pointOnSegment( p, a, b ) )
             return true;
       
        bool crossesY =(( a.y > p.y ) !=( b.y > p.y ) );
        if( !crossesY )
             continue;
       
        int dy = b.y - a.y;
        int lhs =( p.x - a.x ) * dy;
        int rhs =( b.x - a.x ) *( p.y - a.y );
       
        bool hit =( dy > 0 ) ?( lhs < rhs )
            :( lhs > rhs );
        if( hit ) inside = !inside;
       
    }
    return inside;
}

void Lasso::generateMask() {

	_maskImage = sf::Image();

	if (!_image) return;
	if (_rect.size.x <= 1 || _rect.size.y <= 1) return;
	if (_image->getSize().x <= 1 || _image->getSize().y <= 1) return;

	sf::Vector2u size = _image->getSize();
	_maskImage.resize(size, sf::Color::Transparent);

	if (_points.size() < 3) {
		for (auto& p : _points) {
			_maskImage.setPixel(sf::Vector2u(p), sf::Color::White);
		}
		return;
	}
	

	for (unsigned int y = 0; y < size.y; ++y) {
		for (unsigned int x = 0; x < size.x; ++x) {
			if (isPointInPolygon(sf::Vector2i(x,y), _points)) {
				//std::wcout << x << L", " << y << L": is inside\n";
				_maskImage.setPixel(sf::Vector2u(x, y), sf::Color::White);
			}
		}
	}
}

void Lasso::drawImage(sf::Vector2i canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor, bool useMask) {
	if (!_image) return;
	if (_image->getSize().x < 1 || _image->getSize().y < 1) return;
	
	generateRect();

	if (_rect.size.x < 1 || _rect.size.y < 1) return;

	sf::IntRect canvasRect(sf::Vector2i(0, 0), canvasSize);
	
	if (!_rect.findIntersection(canvasRect).has_value())
		return;

	sf::IntRect visibleRect = _rect.findIntersection(canvasRect).value();


	int tx = visibleRect.position.x - _outlineOffset.x;
	int ty = visibleRect.position.y - _outlineOffset.y;
	sf::IntRect texRect(sf::Vector2i(tx, ty), visibleRect.size);

	_texture = sf::Texture();
	if (!_texture.resize(_image->getSize())) {
		DebugError(L"Lasso::drawImage: Failed to resize texture.");
		exit(0);
	}

	if (!_texture.loadFromImage(*_image)) {
		DebugError(L"Lasso::drawImage: Failed to load texture from image.");
		exit(0);
	}

	_texture.setSmooth(false);

	sf::Image maskImage;
	if (useMask) {
		maskImage = _maskImage;
	}
	else {
		maskImage = sf::Image();
		if (_image->getSize().x == 0 || _image->getSize().y == 0)
			return;

		maskImage.resize(_image->getSize(), sf::Color::White);
	}

	sf::Texture maskTexture;

	if (!maskTexture.loadFromImage(maskImage)) {
		DebugError(L"Lasso::drawImage: Failed to load mask texture from image.");
		exit(0);
	}

	_shader.setUniform("texture", _texture);
	_shader.setUniform("mask", maskTexture);
	_shader.setUniform("alphaColor", sf::Vector3f(alphaColor.r, alphaColor.g, alphaColor.b));


	_sprite = std::make_shared<sf::Sprite>(_texture);
	_sprite->setTextureRect(texRect);
	_sprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f spritePos;
	spritePos.x = (float)(canvasPosition.x) + (float)(visibleRect.position.x)*scale;
	spritePos.y = (float)(canvasPosition.y) + (float)(visibleRect.position.y)*scale;
	_sprite->setPosition(spritePos);

	sf::RenderStates rs;
	rs.shader = &_shader;
	window->draw(*_sprite, rs);

}

void Lasso::drawOutline(sf::Vector2i canvasPosition, float scale) {


	_outlineSprite = std::make_shared<sf::Sprite>(_outlineRenderTexture.getTexture());
	_outlineSprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f outlineSpritePos;
	outlineSpritePos.x = (float)(canvasPosition.x) + (float)(_outlineOffset.x) * scale;
	outlineSpritePos.y = (float)(canvasPosition.y) + (float)(_outlineOffset.y) * scale;
	_outlineSprite->setPosition(outlineSpritePos);

	window->draw(*_outlineSprite);

}

void Lasso::drawRect(sf::Vector2i canvasPosition, float scale) {

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x) * scale;
	rectSize.y = float(_rect.size.y) * scale;
	
	sf::RectangleShape rect(rectSize);

	sf::Vector2f rectPos;
	rectPos.x = float(canvasPosition.x) + float(_rect.position.x) * scale;
	rectPos.y = float(canvasPosition.y) + float(_rect.position.y) * scale;
	rect.setPosition(rectPos);

	rect.setFillColor(sf::Color(127, 47, 47, 127));
	rect.setOutlineColor(sf::Color(47, 127, 127, 255));
	rect.setOutlineThickness(4.0f);

	window->draw(rect);
}


void Lasso::draw(sf::Vector2i canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor) {

	if (_state == LassoState::Selecting) {
		if (_points.size() >= 1) {
			drawImage(canvasPosition, canvasSize, scale, alphaColor, false);
			generateOutline(false);
			drawOutline(canvasPosition, scale);
		}
		
	}

	if (_state == LassoState::Selected || _state == LassoState::Moving) {
		if (_points.size() >= 3) {
			drawImage(canvasPosition, canvasSize, scale, alphaColor, true);
			drawRect(canvasPosition, scale);
			//generateOutline(true);
			//drawOutline(canvasPosition, scale);
		}
	}

}

std::shared_ptr<Lasso> lasso;