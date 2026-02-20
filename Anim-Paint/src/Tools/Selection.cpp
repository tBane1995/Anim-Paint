#include "Tools/Selection.hpp"
#include "Tools/Clipboard.hpp"
#include "Window.hpp"
#include "DebugLog.hpp"
#include "Textures.hpp"
#include "Animation/Animation.hpp"
#include "Tools/Toolbar.hpp"
#include "Tools/Line.hpp"
#include "WorldToTileConverter.hpp"
#include "Cursor.hpp"
#include "Canvas.hpp"
#include "Theme.hpp"

std::string mask_shader_source = R"(
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

std::string resize_mask_shader_source = R"(
    uniform sampler2D texture;
    uniform vec2 oldSize;
    uniform vec2 newSize;

    void main() {
        vec2 uv = gl_TexCoord[0].xy * (oldSize / newSize);

        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
            discard;

        gl_FragColor = texture2D(texture, uv);
    }
)";

void removeImageWithAlpha(sf::Image& image, sf::IntRect rect, sf::Color alphaColor)
{
	if (rect.size.x <= 0 || rect.size.y <= 0)
		return;

	const sf::Vector2u imgSize = image.getSize();

	for (int y = 0; y < rect.size.y; ++y) {
		for (int x = 0; x < rect.size.x; ++x) {

			int px = rect.position.x + x;
			int py = rect.position.y + y;

			if (px < 0 || py < 0 || px >= (int)(imgSize.x) || py >= (int)(imgSize.y))
				continue;

			image.setPixel(sf::Vector2u(px, py), alphaColor);
		}
	}
}

void removeImageWithMask(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor)
{
	if (rect.size.x <= 0 || rect.size.y <= 0)
		return;

	if (mask.getSize().x <= 0 || mask.getSize().y <= 0)
		return;

	for (int y = 0; y < rect.size.y; ++y) {
		for (int x = 0; x < rect.size.x; ++x) {

			if (mask.getPixel(sf::Vector2u(x, y)) != sf::Color::White)
				continue;

			int px = rect.position.x + x;
			int py = rect.position.y + y;

			if (px < 0 || py < 0 || px >= (int)(image.getSize().x) || py >= (int)(image.getSize().y))
				continue;

			image.setPixel(sf::Vector2u(px, py), alphaColor);
		}
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




Selection::Selection() {

	_state = SelectionState::None;

	_points.clear();

	_offset = sf::Vector2i(0, 0);

	_image = nullptr;
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
	_outlineOffset = sf::Vector2i(0, 0);

	_maskImage = nullptr;

	if (!_maskShader.loadFromMemory(mask_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Lasso::Lasso: Failed to load shader from memory.");
		exit(0);
	}

	_sprite = nullptr;
	_outlineSprite = nullptr;

	_resizedMaskImage = nullptr;
	_resizedImage = nullptr;

	if (!_resizedMaskShader.loadFromMemory(resize_mask_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Lasso::Lasso: Failed to load shader from memory.");
		exit(0);
	}
}

Selection::~Selection() {

}

void Selection::shiftOriginIfNeeded(sf::Vector2i& point)
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

void Selection::addPoint(sf::Vector2i point) {
	point -= _outlineOffset;
	shiftOriginIfNeeded(point);

	if (_points.empty()) {
		_points.push_back(point);
		return;
	}

	if (_points.back() == point) return;

	_points.push_back(point);
}

void Selection::unselect() {
	_points.clear();
	_outlineOffset = sf::Vector2i(0, 0);
}

void Selection::selectAll() {
	unselect();

	std::shared_ptr<Animation> anim = getCurrentAnimation();
	int width = anim->getCurrentLayer()->_image.getSize().x - 1;
	int height = anim->getCurrentLayer()->_image.getSize().y - 1;

	selection->addPoint(sf::Vector2i(0, 0));
	selection->addPoint(sf::Vector2i(width, 0));
	selection->addPoint(sf::Vector2i(width, height));
	selection->addPoint(sf::Vector2i(0, height));

	generateRect();

	_image = std::make_shared<sf::Image>();
	_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);

	generateMask();

	_resizedRect = _rect;
	_resizedImage = _image;
	_resizedMaskImage = _maskImage;

	generateEdgePoints();

	copyImageWithMask(*_image, anim->getCurrentLayer()->_image, 0, 0, 0, 0, *_resizedMaskImage, toolbar->_second_color->_color);
	removeImageWithMask(anim->getCurrentLayer()->_image, _resizedRect, *_resizedMaskImage, toolbar->_second_color->_color);

	_state = SelectionState::Selected;
}

void Selection::generateRect() {

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

bool Selection::clickOnSelection(sf::Vector2i point) {

	if (_resizedRect.size.x <= 1 || _resizedRect.size.y <= 1)
		return false;

	return _resizedRect.contains(point);
}

void Selection::copy(sf::Image& canvas, sf::Color emptyColor)
{

	if (_state != SelectionState::Selected)
		return;

	generateRect();

	sf::IntRect r = _rect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x = -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y = -r.size.y; }

	pasteImageWithMask(canvas, *_image, r.position.x, r.position.y, *_maskImage, emptyColor);
	copyImageWithAlpha(*_image, canvas, _rect, emptyColor);

	sf::IntRect canvasRect(sf::Vector2i(0, 0), sf::Vector2i(canvas.getSize()));

	if (!r.findIntersection(canvasRect).has_value())
		return;

	sf::IntRect s = r.findIntersection(canvasRect).value();

	if (s.size.x <= 0 || s.size.y <= 0)
		return;

	generateMask();

	sf::Image copiedImage = sf::Image();
	copiedImage.resize(sf::Vector2u(s.size), sf::Color::Transparent);

	for (int y = 0; y < s.size.y; ++y)
		for (int x = 0; x < s.size.x; ++x) {

			if (_maskImage->getPixel(sf::Vector2u(x, y)) != sf::Color::White)
				copiedImage.setPixel(sf::Vector2u(x, y), sf::Color::Transparent);
			else {

				if (_image->getPixel(sf::Vector2u(x, y)) == emptyColor)
					copiedImage.setPixel(sf::Vector2u(x, y), sf::Color::Transparent);
				else
					copiedImage.setPixel(sf::Vector2u(x, y), _image->getPixel(sf::Vector2u(x, y)));

			}
		}

	copyImageToClipboard(copiedImage, sf::IntRect(sf::Vector2i(0, 0), s.size));

}

void Selection::paste(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor)
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

bool Selection::paste(sf::Image& canvas, sf::Color emptyColor)
{

	if (_image != nullptr) {
		paste(canvas, *_resizedImage, _rect.position.x, _rect.position.y, *_resizedMaskImage, emptyColor);
	}
	else {
		_image = std::make_shared<sf::Image>();
	}

	loadImageFromClipboard(*_image);

	if (_image->getSize().x <= 0 || _image->getSize().y <= 0) {
		DebugError(L"Lasso::paste: Clipboard image has invalid size.");
		return false;
	}

	_state = SelectionState::Selected;

	unselect();
	addPoint(sf::Vector2i(0, 0));
	addPoint(sf::Vector2i(0, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, 0));

	_rect.position = sf::Vector2i(0, 0);
	_resizedRect.position = sf::Vector2i(0, 0);
	generateEdgePoints();
	return true;
}

void Selection::cut(sf::Image& canvas, sf::Color emptyColor) {
	if (_state == SelectionState::Selected) {

		if (_image == nullptr) {
			_image = std::make_shared<sf::Image>();
			_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);

			if (!_image->copy(canvas, sf::Vector2u(0, 0), _rect, false)) {
				DebugError(L"Lasso::cut: Failed to copy image from canvas.");
				exit(0);
			}

			copyImageToClipboard(*_image, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(_image->getSize())));
		}
		else {
			copyImageToClipboard(*_image, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(_image->getSize())));
			//copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y), mask);
		}

		_image = nullptr;
		_state = SelectionState::None;
		_rect = sf::IntRect(sf::Vector2i(-1, -1), sf::Vector2i(-1, -1));
	}
}

void Selection::generateOutline(bool selectionComplete) { 
	//std::vector <sf::Vector2i> _points 

	if (_points.size() < 1) return; 
	if (std::abs(_rect.size.x) == 0 || std::abs(_rect.size.y) == 0) return; 

	if (!_outlineRenderTexture.resize(sf::Vector2u(std::abs(_rect.size.x), std::abs(_rect.size.y)))) { 
		DebugError(L"Lasso::generateOutline: Failed to resize outline render texture."); 
		exit(0); 
	} 
	
	// calc the shift to move points to (0,0) based coordinates for rendering
	int minX = std::numeric_limits<int>::max();
	int minY = std::numeric_limits<int>::max();
	for (const auto& pt : _points) {
		minX = std::min(minX, pt.x);
		minY = std::min(minY, pt.y);
	}

	sf::Vector2f shift = sf::Vector2f((float)(-minX), (float)(-minY));

	// generate outline texture
	_outlineRenderTexture.clear(sf::Color(0, 0, 0, 0)); 
	sf::Color selectionColor = selection_border_color;
	sf::VertexArray lines(sf::PrimitiveType::LineStrip); 
	
	for (auto& point : _points) 
		lines.append(sf::Vertex{ sf::Vector2f(point) + shift, selectionColor }); 
	
	// first and last point 
	sf::VertexArray p(sf::PrimitiveType::Points); 
	p.append(sf::Vertex{ sf::Vector2f(_points.front()) + shift, selectionColor }); 
	p.append(sf::Vertex{ sf::Vector2f(_points.back()) + shift, selectionColor }); 
	
	if (selectionComplete) lines.append(sf::Vertex{ sf::Vector2f(_points.front()) + shift, selectionColor }); 
	
	sf::RenderStates rs; 
	rs.blendMode = sf::BlendAlpha; 
	rs.transform.translate(sf::Vector2f(0.5f, 0.5f)); 
	_outlineRenderTexture.draw(lines, rs); 
	_outlineRenderTexture.draw(p, rs); 
	_outlineRenderTexture.display(); 

}

bool Selection::pointOnSegment(sf::Vector2i p, sf::Vector2i a, sf::Vector2i b)
{
	int cross = 1LL * (b.x - a.x) * (p.y - a.y) - 1LL * (b.y - a.y) * (p.x - a.x);

	if (cross != 0)
		return false;

	int minx = std::min(a.x, b.x);
	int maxx = std::max(a.x, b.x);
	int miny = std::min(a.y, b.y);
	int maxy = std::max(a.y, b.y);

	return(p.x >= minx && p.x <= maxx && p.y >= miny && p.y <= maxy);
}

bool Selection::isPointInPolygon(sf::Vector2i p, std::vector < sf::Vector2i >& poly)
{
	size_t n = poly.size();
	if (n < 3)
		return false;

	bool inside = false;
	for (size_t i = 0, j = n - 1; i < n; j = i++) {
		sf::Vector2i& a = poly[j];
		sf::Vector2i& b = poly[i];

		if (pointOnSegment(p, a, b))
			return true;

		bool crossesY = ((a.y > p.y) != (b.y > p.y));
		if (!crossesY)
			continue;

		int dy = b.y - a.y;
		int lhs = (p.x - a.x) * dy;
		int rhs = (b.x - a.x) * (p.y - a.y);

		bool hit = (dy > 0) ? (lhs < rhs)
			: (lhs > rhs);
		if (hit) inside = !inside;

	}
	return inside;
}

void Selection::generateMask()
{
	if (_rect.size.x <= 1 || _rect.size.y <= 1) return;

	_maskImage = std::make_shared<sf::Image>();
	_maskImage->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);

	for (int y = 0; y < _rect.size.y; ++y) {
		for (int x = 0; x < _rect.size.x; ++x) {

			int lx = (_rect.position.x + x) - _outlineOffset.x;
			int ly = (_rect.position.y + y) - _outlineOffset.y;

			if (isPointInPolygon(sf::Vector2i(lx, ly), _points)) {
				_maskImage->setPixel(sf::Vector2u(x, y), sf::Color::White);
			}
		}
	}
}

void Selection::generateResizedMask() {
	if (_rect.size.x <= 1 || _rect.size.y <= 1) return;
	if (_resizedRect.size.x <= 1 || _resizedRect.size.y <= 1) return;

	_resizedMaskImage = std::make_shared<sf::Image>();
	_resizedMaskImage->resize(sf::Vector2u(_resizedRect.size), sf::Color::Transparent);

	for (int y = 0; y < _resizedRect.size.y; ++y) {
		for (int x = 0; x < _resizedRect.size.x; ++x) {

			int lx = (_rect.position.x + (x * _rect.size.x) / _resizedRect.size.x) - _outlineOffset.x;
			int ly = (_rect.position.y + (y * _rect.size.y) / _resizedRect.size.y) - _outlineOffset.y;

			if (isPointInPolygon(sf::Vector2i(lx, ly), _points)) {
				_resizedMaskImage->setPixel(sf::Vector2u(x, y), sf::Color::White);
			}
		}
	}
}

void Selection::generateEdgePoints() {

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_resizedRect.size.x) * scale;
	rectSize.y = float(_resizedRect.size.y) * scale;

	sf::Vector2f rectPos;
	rectPos.x = float(canvas->_position.x) + float(_resizedRect.position.x) * scale;
	rectPos.y = float(canvas->_position.y) + float(_resizedRect.position.y) * scale;

	float m = selection_border_width;

	_edgePoints.clear();
	_point_left_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(-m, -m));
	_point_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(rectSize.x/2, -m));
	_point_right_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(rectSize.x+m, -m));

	_point_left = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(-m, rectSize.y / 2));
	_point_right = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(rectSize.x+m, rectSize.y / 2));

	_point_left_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(-m, rectSize.y+m));
	_point_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(rectSize.x / 2, rectSize.y+m));
	_point_right_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i(rectSize.x+m, rectSize.y+m));
	
	_edgePoints.push_back(_point_left_top);
	_edgePoints.push_back(_point_top);
	_edgePoints.push_back(_point_right_top);
	_edgePoints.push_back(_point_left);
	_edgePoints.push_back(_point_right);
	_edgePoints.push_back(_point_left_bottom);
	_edgePoints.push_back(_point_bottom);
	_edgePoints.push_back(_point_right_bottom);



}

void Selection::resizeRect() {

	float scale = (float)(canvas->_zoom * canvas->_zoom_delta);

	sf::Vector2f p = (sf::Vector2f(cursor->_worldMousePosition) + sf::Vector2f(_edgePoints[0]->getSize()) / 2.0f - sf::Vector2f(_clickedEdgePoint->getPosition())) / scale;

	float minX = (float)_point_left->getPosition().x;
	float minY = (float)_point_top->getPosition().y;
	float maxX = (float)_point_right->getPosition().x;
	float maxY = (float)_point_bottom->getPosition().y;

	float dx = (float)((int)p.x) * scale;
	float dy = (float)((int)p.y) * scale;

	if (_clickedEdgePoint == _point_left_top) {
		minX = (float)_point_left->getPosition().x + dx;
		minY = (float)_point_top->getPosition().y + dy;
	}
	else if (_clickedEdgePoint == _point_right_top) {
		maxX = (float)_point_right->getPosition().x + dx;
		minY = (float)_point_top->getPosition().y + dy;
	}
	else if (_clickedEdgePoint == _point_left_bottom) {
		minX = (float)_point_left->getPosition().x + dx;
		maxY = (float)_point_bottom->getPosition().y + dy;
	}
	else if (_clickedEdgePoint == _point_right_bottom) {
		maxX = (float)_point_right->getPosition().x + dx;
		maxY = (float)_point_bottom->getPosition().y + dy;
	}
	else if (_clickedEdgePoint == _point_left) {
		minX = (float)_point_left->getPosition().x + dx;
	}
	else if (_clickedEdgePoint == _point_right) {
		maxX = (float)_point_right->getPosition().x + dx;
	}
	else if (_clickedEdgePoint == _point_top) {
		minY = (float)_point_top->getPosition().y + dy;
	}
	else if (_clickedEdgePoint == _point_bottom) {
		maxY = (float)_point_bottom->getPosition().y + dy;
	}

	const float m = (float)(selection_border_width);
	const float MIN_SIZE_WORLD = scale + 2.0f * m;

	if (_clickedEdgePoint == _point_left_top || _clickedEdgePoint == _point_left || _clickedEdgePoint == _point_left_bottom)
		minX = std::min(minX, maxX - MIN_SIZE_WORLD);

	if (_clickedEdgePoint == _point_right_top || _clickedEdgePoint == _point_right || _clickedEdgePoint == _point_right_bottom)
		maxX = std::max(maxX, minX + MIN_SIZE_WORLD);

	if (_clickedEdgePoint == _point_left_top || _clickedEdgePoint == _point_top || _clickedEdgePoint == _point_right_top)
		minY = std::min(minY, maxY - MIN_SIZE_WORLD);

	if (_clickedEdgePoint == _point_left_bottom || _clickedEdgePoint == _point_bottom || _clickedEdgePoint == _point_right_bottom)
		maxY = std::max(maxY, minY + MIN_SIZE_WORLD);

	float iminX = std::min(minX, maxX);
	float iminY = std::min(minY, maxY);
	float imaxX = std::max(minX, maxX);
	float imaxY = std::max(minY, maxY);

	_point_left_top->setPosition(sf::Vector2i((int)iminX, (int)iminY));
	_point_top->setPosition(sf::Vector2i((int)((iminX + imaxX) / 2.0f), (int)iminY));
	_point_right_top->setPosition(sf::Vector2i((int)imaxX, (int)iminY));

	_point_left->setPosition(sf::Vector2i((int)iminX, (int)((iminY + imaxY) / 2.0f)));
	_point_right->setPosition(sf::Vector2i((int)imaxX, (int)((iminY + imaxY) / 2.0f)));

	_point_left_bottom->setPosition(sf::Vector2i((int)iminX, (int)imaxY));
	_point_bottom->setPosition(sf::Vector2i((int)((iminX + imaxX) / 2.0f), (int)imaxY));
	_point_right_bottom->setPosition(sf::Vector2i((int)imaxX, (int)imaxY));

	sf::Vector2i minT = worldToTile(sf::Vector2i((int)(iminX + m), (int)(iminY + m)), canvas->_position, canvas->_zoom, canvas->_zoom_delta);

	sf::Vector2i maxT = worldToTile(sf::Vector2i((int)(imaxX - m), (int)(imaxY - m)), canvas->_position, canvas->_zoom, canvas->_zoom_delta);

	sf::Vector2i size = maxT - minT;

	_resizedRect = sf::IntRect(minT, size);
}

void Selection::resizeImage() {
	if (_image == nullptr) return;
	if (_rect.size.x <= 1 || _rect.size.y <= 1) return;
	if (_resizedRect.size.x <= 1 || _resizedRect.size.y <= 1) return;

	_resizedImage = std::make_shared<sf::Image>();
	_resizedImage->resize(sf::Vector2u(_resizedRect.size), sf::Color::Transparent);

	sf::Vector2u imgSize = _image->getSize();

	for (int y = 0; y < _resizedRect.size.y; ++y) {
		for (int x = 0; x < _resizedRect.size.x; ++x) {

			int sx = (x * _rect.size.x) / _resizedRect.size.x;
			int sy = (y * _rect.size.y) / _resizedRect.size.y;

			if (sx < 0 || sy < 0 || sx >= (int)imgSize.x || sy >= (int)imgSize.y)
				continue;

			_resizedImage->setPixel(sf::Vector2u(x, y), _image->getPixel(sf::Vector2u(sx, sy)));
		}
	}
}
void Selection::drawImage(sf::Color alphaColor, bool useMask) {
	if (!_image) return;
	if (_image->getSize().x < 1 || _image->getSize().y < 1) return;
	if (_rect.size.x < 1 || _rect.size.y < 1) return;

	// TO-DO
	sf::IntRect canvasRect(sf::Vector2i(0, 0), canvas->_size);
	//

	if (!_rect.findIntersection(canvasRect).has_value())
		return;

	sf::IntRect visibleRect = _rect.findIntersection(canvasRect).value();


	int tx = visibleRect.position.x - _rect.position.x;
	int ty = visibleRect.position.y - _rect.position.y;
	sf::IntRect texRect(sf::Vector2i(tx, ty), visibleRect.size);

	if (_image->getSize().x <= 0 || _image->getSize().y <= 0) {
		DebugError(L"Lasso::drawImage: Image has invalid size.");
		return;
	}


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

	std::shared_ptr<sf::Image> maskImage;
	if (useMask && _rect.size.x > 0 && _rect.size.y > 0) {
		maskImage = _maskImage;
	}
	else {
		maskImage = std::make_shared<sf::Image>();
		maskImage->resize(sf::Vector2u(_rect.size), sf::Color::White);
	}

	sf::Texture maskTexture;
	if (!maskTexture.loadFromImage(*maskImage)) {
		DebugError(L"Lasso::drawImage: Failed to load mask texture from image.");
		exit(0);
	}

	_maskShader.setUniform("texture", _texture);
	_maskShader.setUniform("mask", maskTexture);
	_maskShader.setUniform("alphaColor", sf::Vector3f(alphaColor.r, alphaColor.g, alphaColor.b));

	float scale = canvas->_zoom * canvas->_zoom_delta;

	_sprite = std::make_shared<sf::Sprite>(_texture);
	_sprite->setTextureRect(texRect);
	_sprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f spritePos;
	spritePos.x = (float)(canvas->_position.x) + (float)(visibleRect.position.x) * scale;
	spritePos.y = (float)(canvas->_position.y) + (float)(visibleRect.position.y) * scale;
	_sprite->setPosition(spritePos);

	sf::RenderStates rs;
	rs.shader = &_maskShader;
	window->draw(*_sprite, rs);

}


void Selection::drawResizedImage(sf::Color alphaColor, bool useMask) {
	if (!_resizedImage) return;
	if (_resizedImage->getSize().x < 1 || _resizedImage->getSize().y < 1) return;
	if (_resizedRect.size.x < 1 || _resizedRect.size.y < 1) return;

	sf::IntRect canvasRect(sf::Vector2i(0, 0), canvas->_size);

	if (!_resizedRect.findIntersection(canvasRect).has_value())
		return;

	sf::IntRect visibleRect = _resizedRect.findIntersection(canvasRect).value();


	int tx = visibleRect.position.x - _resizedRect.position.x;
	int ty = visibleRect.position.y - _resizedRect.position.y;
	sf::IntRect texRect(sf::Vector2i(tx, ty), visibleRect.size);

	if (_resizedImage->getSize().x <= 0 || _resizedImage->getSize().y <= 0) {
		DebugError(L"Lasso::drawResizedImage: Image has invalid size.");
		return;
	}


	_texture = sf::Texture();
	if (!_texture.resize(_resizedImage->getSize())) {
		DebugError(L"Lasso::drawResizedImage: Failed to resize texture.");
		exit(0);
	}

	if (!_texture.loadFromImage(*_resizedImage)) {
		DebugError(L"Lasso::drawResizedImage: Failed to load texture from image.");
		exit(0);
	}

	_texture.setSmooth(false);

	std::shared_ptr<sf::Image> maskImage;
	if (useMask && _resizedRect.size.x > 0 && _resizedRect.size.y > 0) {
		maskImage = _resizedMaskImage;
	}
	else {
		maskImage = std::make_shared<sf::Image>();
		maskImage->resize(sf::Vector2u(_resizedRect.size), sf::Color::White);
	}

	sf::Texture maskTexture;
	if (!maskTexture.loadFromImage(*_resizedMaskImage)) {
		DebugError(L"Lasso::drawImage: Failed to load mask texture from image.");
		exit(0);
	}

	_maskShader.setUniform("texture", _texture);
	_maskShader.setUniform("mask", maskTexture);
	_maskShader.setUniform("alphaColor", sf::Vector3f(alphaColor.r, alphaColor.g, alphaColor.b));


	_sprite = std::make_shared<sf::Sprite>(_texture);
	_sprite->setTextureRect(texRect);
	float scale = canvas->_zoom * canvas->_zoom_delta;
	_sprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f spritePos;
	spritePos.x = (float)(canvas->_position.x) + (float)(visibleRect.position.x) * scale;
	spritePos.y = (float)(canvas->_position.y) + (float)(visibleRect.position.y) * scale;
	_sprite->setPosition(spritePos);

	sf::RenderStates rs;
	rs.shader = &_maskShader;
	window->draw(*_sprite, rs);

}

void Selection::drawOutline() {

	float scale = canvas->_zoom * canvas->_zoom_delta;

	_outlineSprite = std::make_shared<sf::Sprite>(_outlineRenderTexture.getTexture());
	_outlineSprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f outlineSpritePos;
	outlineSpritePos.x = (float)(canvas->_position.x) + (float)(_rect.position.x) * scale;
	outlineSpritePos.y = (float)(canvas->_position.y) + (float)(_rect.position.y) * scale;
	_outlineSprite->setPosition(outlineSpritePos);

	window->draw(*_outlineSprite);

}

void Selection::drawRect() {

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_resizedRect.size.x) * scale;
	rectSize.y = float(_resizedRect.size.y) * scale;

	sf::RectangleShape rect(rectSize);

	sf::Vector2f rectPos;
	rectPos.x = float(canvas->_position.x) + float(_resizedRect.position.x) * scale;
	rectPos.y = float(canvas->_position.y) + float(_resizedRect.position.y) * scale;
	rect.setPosition(rectPos);

	rect.setFillColor(selection_color);
	rect.setOutlineColor(selection_border_color);
	rect.setOutlineThickness((float)(selection_border_width));

	window->draw(rect);
}


void Selection::draw(sf::Color alphaColor) {


	if (_state == SelectionState::Selecting) {
		if (_points.size() >= 1) {
			drawImage(alphaColor, false);
			generateOutline(false);
			drawOutline();
		}

	}

	if (_state == SelectionState::Selected || _state == SelectionState::Moving || _state == SelectionState::Resizing) {
		if (_points.size() >= 3) {
			drawResizedImage(alphaColor, false);
			drawRect();
			
			if (_state == SelectionState::Selected || _state == SelectionState::Resizing) {
				for (auto& point : _edgePoints) {
					point->draw();
				}
					
			}
		}
	}

}

std::shared_ptr<Selection> selection;