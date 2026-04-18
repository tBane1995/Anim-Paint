#include "Tools/Selection.hpp"
#include "Tools/Clipboard.hpp"
#include "Window.hpp"
#include "DebugLog.hpp"
#include "Textures.hpp"
#include "Animation/Animation.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Tools/Line.hpp"
#include "WorldToTileConverter.hpp"
#include "Cursor.hpp"
#include "Components/Canvas.hpp"
#include "Theme.hpp"
#include "History.hpp"
#include "Time.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Dialogs/Palette.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Tools/Filters.hpp"
#include <typeinfo>

std::string mask_shader_source = R"(
    uniform sampler2D texture;
    uniform sampler2D mask;
    uniform vec4 alphaColor;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
		
		if(texture2D(mask, uv) == vec4(1.0, 1.0, 1.0, 1.0)) {
			vec4 c = texture2D(texture, uv);

			if(c == alphaColor)
				gl_FragColor = vec4(0,0,0,0);
			else
				gl_FragColor = c;
		}else{
			gl_FragColor = vec4(0,0,0,0);
		}
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

void copyImageWithAlpha(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color alphaColor)
{

	// (opcjonalnie, ale bezpiecznie) normalizacja, gdyby size był ujemny
	sf::IntRect r = srcRect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x = -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y = -r.size.y; }

	if (r.size.x <= 0 || r.size.y <= 0)
		return;

	sf::IntRect srcB(sf::Vector2i(0, 0), sf::Vector2i(src.getSize()));
	auto inter = r.findIntersection(srcB);
	if (!inter.has_value())
		return;

	sf::IntRect s = inter.value();
	if (s.size.x <= 0 || s.size.y <= 0)
		return;

	if ((int)(dst.getSize().x) < s.size.x || (int)(dst.getSize().y) < s.size.y)
		dst.resize(sf::Vector2u(s.size.x, s.size.y), sf::Color::Transparent);

	sf::Image img;
	img.resize(sf::Vector2u(s.size.x, s.size.y), sf::Color::Transparent);

	for (int y = 0; y < s.size.y; ++y) {
		for (int x = 0; x < s.size.x; ++x) {
			int xx = s.position.x + x;
			int yy = s.position.y + y;

			sf::Color c = src.getPixel(sf::Vector2u(xx, yy));
			if (c == sf::Color::Transparent || c == alphaColor)
				continue;

			img.setPixel(sf::Vector2u(x, y), c);
		}
	}

	sf::IntRect local(sf::Vector2i(0, 0), sf::Vector2i(s.size));
	if (!dst.copy(img, sf::Vector2u(0, 0), local, false)) {
		DebugError(L"copyImageWithAlpha: image copy failed");
		exit(0);
	}
}

void copyImageWithMask(sf::Image* dst, sf::Image* src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor)
{
	if (!src || !dst) return;

	int dw = (int)dst->getSize().x;
	int dh = (int)dst->getSize().y;
	int sw = (int)src->getSize().x;
	int sh = (int)src->getSize().y;

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
		int sy = srcY + y;
		int dy = dstY + y;
		int my = my0 + y;

		for (int x = 0; x < maxW; ++x) {
			int sx = srcX + x;
			int dx = dstX + x;
			int mx = mx0 + x;

			const sf::Color m = mask.getPixel(sf::Vector2u(mx, my));
			if (m != sf::Color::White) continue; // tylko białe = kopiuj

			const sf::Color c = src->getPixel(sf::Vector2u(sx, sy));
			if (c.a == 0 || c == alphaColor) continue;

			dst->setPixel(sf::Vector2u(dx, dy), c);
		}
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




Selection::Selection() : ResizableTool() {

	_state = ResizableToolState::None;

	_points.clear();

	_offset = sf::Vector2i(0, 0);

	_image = nullptr;
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
	_outlineOffset = sf::Vector2i(0, 0);

	_maskImage = nullptr;

	if (!_shader.loadFromMemory(mask_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Lasso::Lasso: Failed to load shader from memory.");
		exit(0);
	}

	_outlineSprite = nullptr;

	_resizedMaskImage = nullptr;
	_resizedImage = nullptr;

	_moveTime = sf::Time::Zero;
}

Selection::~Selection() {

}

void Selection::reset() {

	paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent, *_resizedImage);
	_points.clear();
	_outlineOffset = sf::Vector2i(0, 0);
	generateRect();
	_resizedRect = _rect;
	_image = nullptr;
	_maskImage = nullptr;
	_resizedImage = nullptr;
	_resizedMaskImage = nullptr;
	_state = ResizableToolState::None;
}

void Selection::selectAll() {

	if (_state == ResizableToolState::Selected) {
		copyImageWithMask(&getCurrentAnimation()->getCurrentLayer()->_image, _resizedImage.get(), _rect.position.x, _rect.position.y, 0, 0, *_resizedMaskImage, (toolbar->_option_transparency->_checkbox->_value == 0) ? sf::Color::Transparent : toolbar->_second_color->_color);
	}

	_points.clear();
	_outlineOffset = sf::Vector2i(0, 0);

	std::shared_ptr<Animation> anim = getCurrentAnimation();
	int width = anim->getCurrentLayer()->_image.getSize().x - 1;
	int height = anim->getCurrentLayer()->_image.getSize().y - 1;

	addPoint(sf::Vector2i(0, 0));
	addPoint(sf::Vector2i(width, 0));
	addPoint(sf::Vector2i(width, height));
	addPoint(sf::Vector2i(0, height));

	generateRect();

	_image = std::make_shared<sf::Image>();
	_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);

	generateMask();

	_resizedRect = _rect;
	_resizedImage = _image;
	_resizedMaskImage = _maskImage;

	generateEdgePoints();

	copyImageWithMask(&*_image, &anim->getCurrentLayer()->_image, 0, 0, 0, 0, *_resizedMaskImage, (toolbar->_option_transparency->_checkbox->_value==0)?sf::Color::Transparent:toolbar->_second_color->_color);
	removeImageWithMask(anim->getCurrentLayer()->_image, _resizedRect, *_resizedMaskImage, sf::Color::Transparent);

	_state = ResizableToolState::Selected;
}



bool Selection::clickOnSelection(sf::Vector2i point) {

	if (_resizedRect.size.x <= 1 || _resizedRect.size.y <= 1)
		return false;

	return _resizedRect.contains(point);
}

void Selection::copy(sf::Image& canvas, sf::Color alphaColor)
{

	if (_state != ResizableToolState::Selected)
		return;


	sf::IntRect r = _resizedRect;
	if (r.size.x < 0) { r.position.x += r.size.x; r.size.x = -r.size.x; }
	if (r.size.y < 0) { r.position.y += r.size.y; r.size.y = -r.size.y; }

	pasteImageWithMask(canvas, *_resizedImage, r.position.x, r.position.y, *_resizedMaskImage, alphaColor);
	copyImageWithAlpha(*_resizedImage, canvas, r, alphaColor);

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

			int ox = s.position.x - r.position.x;
			int oy = s.position.y - r.position.y;

			int sx = x + ox;
			int sy = y + oy;

			int xx = sx * _rect.size.x / r.size.x;
			int yy = sy * _rect.size.y / r.size.y;

			if (xx < 0) xx = 0;
			if (yy < 0) yy = 0;
			if (xx >= _rect.size.x) xx = _rect.size.x - 1;
			if (yy >= _rect.size.y) yy = _rect.size.y - 1;

			if (_maskImage->getPixel(sf::Vector2u(xx, yy)) != sf::Color::White)
				copiedImage.setPixel(sf::Vector2u(x, y), sf::Color::Transparent);
			else
			{
				if (_image->getPixel(sf::Vector2u(xx, yy)) == alphaColor)
					copiedImage.setPixel(sf::Vector2u(x, y), sf::Color::Transparent);
				else
					copiedImage.setPixel(sf::Vector2u(x, y), _image->getPixel(sf::Vector2u(xx, yy)));
			}
		}

	copyImageToClipboard(copiedImage, sf::IntRect(sf::Vector2i(0, 0), s.size));

}

void Selection::pasteToCanvas() {

	if(_state == ResizableToolState::None)
		return;

	if (_resizedImage == nullptr)
		return;

	if (_resizedMaskImage == nullptr)
		return;

	if(getCurrentAnimation()->getCurrentLayer() == nullptr)
		return;

	copyImageWithMask(&getCurrentAnimation()->getCurrentLayer()->_image, _resizedImage.get(), _resizedRect.position.x, _resizedRect.position.y, 0, 0, *_resizedMaskImage, (toolbar->_option_transparency->_checkbox->_value == 0) ? sf::Color::Transparent : toolbar->_second_color->_color);


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

bool Selection::paste(sf::Image& canvas, sf::Color emptyColor, sf::Image image)
{
	_image = std::make_shared<sf::Image>(image);

	if (_image->getSize().x <= 0 || _image->getSize().y <= 0) {
		DebugError(L"Lasso::paste: Clipboard image has invalid size.");
		return false;
	}

	_state = ResizableToolState::Selected;

	_points.clear();
	_outlineOffset = sf::Vector2i(0, 0);

	addPoint(sf::Vector2i(0, 0));
	addPoint(sf::Vector2i(0, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, _image->getSize().y - 1));
	addPoint(sf::Vector2i(_image->getSize().x - 1, 0));

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(_image->getSize()));
	_outlineOffset = sf::Vector2i(0, 0);
	_resizedRect = _rect;
	generateRect();
	generateEdgePoints();
	generateMask();
	generateResizedMask();
	_resizedImage = _image;
	return true;
}

void Selection::cut(sf::Image& canvas, sf::Color emptyColor) {
	if (_state == ResizableToolState::Selected) {

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
		_state = ResizableToolState::None;
		_rect = sf::IntRect(sf::Vector2i(-1, -1), sf::Vector2i(-1, -1));
		_resizedRect = _rect;
	}
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


void Selection::resizeRect() {

	float scale = (float)(canvas->_zoom * canvas->_zoom_delta);

	sf::Vector2f p = (sf::Vector2f(cursor->_position) + sf::Vector2f(_edgePoints[0]->getSize()) / 2.0f - sf::Vector2f(_clickedEdgePoint->getPosition())) / scale;

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
	_resizedImage = nullptr;
	if (_image == nullptr) return;
	if (_rect.size.x < 1 || _rect.size.y < 1) return;
	if (_resizedRect.size.x < 1 || _resizedRect.size.y < 1) return;

	_resizedImage = std::make_shared<sf::Image>(*_image);
	set_resize(*_resizedImage, _resizedRect.size.x, _resizedRect.size.y);

}


void Selection::drawImage(bool useMask) {
	if (!_image) return;
	if (_image->getSize().x < 1 || _image->getSize().y < 1) return;
	if (_rect.size.x < 1 || _rect.size.y < 1) return;

	
	sf::IntRect canvasRect(sf::Vector2i(0, 0), canvas->_size);

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

	sf::Color alphaColor = (toolbar->_option_transparency->_checkbox->_value == 0) ? sf::Color::Transparent : toolbar->_second_color->_color;

	_shader.setUniform("texture", _texture);
	_shader.setUniform("mask", maskTexture);
	_shader.setUniform("alphaColor", sf::Glsl::Vec4(alphaColor.r, alphaColor.g, alphaColor.b, alphaColor.a));

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Sprite sprite(_texture);
	sprite.setTextureRect(texRect);
	sprite.setScale(sf::Vector2f(scale, scale));

	sf::Vector2f spritePos;
	spritePos.x = (float)(canvas->_position.x) + (float)(visibleRect.position.x) * scale;
	spritePos.y = (float)(canvas->_position.y) + (float)(visibleRect.position.y) * scale;
	sprite.setPosition(spritePos);

	sf::RenderStates rs;
	rs.shader = &_shader;
	window->draw(sprite, rs);

}


void Selection::drawResizedImage(bool useMask) {
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

	sf::Color alphaColor = (toolbar->_option_transparency->_checkbox->_value == 0) ? sf::Color::Transparent : toolbar->_second_color->_color;

	_shader.setUniform("texture", _texture);
	_shader.setUniform("mask", maskTexture);
	_shader.setUniform("alphaColor", sf::Glsl::Vec4(alphaColor.r/255.0f, alphaColor.g/255.0f, alphaColor.b/255.0f, alphaColor.a/255.0f));


	sf::Sprite sprite(_texture);
	sprite.setTextureRect(texRect);
	float scale = canvas->_zoom * canvas->_zoom_delta;
	sprite.setScale(sf::Vector2f(scale, scale));

	sf::Vector2f spritePos;
	spritePos.x = (float)(canvas->_position.x) + (float)(visibleRect.position.x) * scale;
	spritePos.y = (float)(canvas->_position.y) + (float)(visibleRect.position.y) * scale;
	sprite.setPosition(spritePos);

	sf::RenderStates rs;
	rs.shader = &_shader;
	window->draw(sprite, rs);
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

void Selection::generateOutline(bool selectionComplete) {

	if (_points.size() < 1) return;
	if (std::abs(_rect.size.x) <= 1 || std::abs(_rect.size.y) <= 1) return;

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

void Selection::generateEdgePoints() {

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_resizedRect.size.x) * scale;
	rectSize.y = float(_resizedRect.size.y) * scale;

	sf::Vector2f rectPos;
	rectPos.x = float(canvas->_position.x) + float(_resizedRect.position.x) * scale;
	rectPos.y = float(canvas->_position.y) + float(_resizedRect.position.y) * scale;

	float m = (float)(selection_border_width);

	_edgePoints.clear();
	_point_left_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(-m)));
	_point_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x / 2), (int)(-m)));
	_point_right_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(-m)));

	_point_left = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(rectSize.y / 2)));
	_point_right = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(rectSize.y / 2)));

	_point_left_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(rectSize.y + m)));
	_point_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x / 2), (int)(rectSize.y + m)));
	_point_right_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(rectSize.y + m)));

	_edgePoints.push_back(_point_left_top);
	_edgePoints.push_back(_point_top);
	_edgePoints.push_back(_point_right_top);
	_edgePoints.push_back(_point_left);
	_edgePoints.push_back(_point_right);
	_edgePoints.push_back(_point_left_bottom);
	_edgePoints.push_back(_point_bottom);
	_edgePoints.push_back(_point_right_bottom);

}

void Selection::setPosition(sf::Vector2i position) {

	if (_state == ResizableToolState::None)
		return;

	sf::Vector2i delta = position - _rect.position;

	_rect.position += delta;
	_resizedRect.position += delta;

	_outlineOffset += delta;

	for (auto& p : _edgePoints) {
		p->setPosition(p->getPosition() + delta);
	}

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_resizedRect.size.x) * scale;
	rectSize.y = float(_resizedRect.size.y) * scale;

	sf::Vector2f rectPos;
	rectPos.x = float(canvas->_position.x) + float(_resizedRect.position.x) * scale;
	rectPos.y = float(canvas->_position.y) + float(_resizedRect.position.y) * scale;

	float m = (float)(selection_border_width);

	_point_left_top->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(-m)));
	_point_top->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x / 2), (int)(-m)));
	_point_right_top->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(-m)));

	_point_left->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(rectSize.y / 2)));
	_point_right->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(rectSize.y / 2)));

	_point_left_bottom->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(rectSize.y + m)));
	_point_bottom->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x / 2), (int)(rectSize.y + m)));
	_point_right_bottom->setPosition(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(rectSize.y + m)));
}

void Selection::scale(sf::IntRect newRect) {
	// This function adjusts the selection points and images to fit the new rectangle after resizing.
	_resizedRect = newRect;

	float scaleX = float(_resizedRect.size.x - 1) / float(_rect.size.x - 1);
	float scaleY = float(_resizedRect.size.y - 1) / float(_rect.size.y - 1);

	for (auto& p : _points) {
		p.x = int(p.x * scaleX);
		p.y = int(p.y * scaleY);
	}

	_outlineOffset.x = int(_outlineOffset.x * scaleX);
	_outlineOffset.y = int(_outlineOffset.y * scaleY);

	_rect = _resizedRect;
	generateRect();

	*_maskImage = *_resizedMaskImage;
	*_image = *_resizedImage;
}

void Selection::drawRect() {
	if (!(_points.size() >= 3 && (_state == ResizableToolState::Selected || _state == ResizableToolState::Moving || _state == ResizableToolState::Resizing)))
		return;

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

void Selection::drawOutline() {

	if (_state != ResizableToolState::Selecting)
		return;

	if (_rect.size.x <= 1 || _rect.size.y <= 1)
		return;

	float scale = canvas->_zoom * canvas->_zoom_delta;

	_outlineSprite = std::make_shared<sf::Sprite>(_outlineRenderTexture.getTexture());
	_outlineSprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f outlineSpritePos;
	outlineSpritePos.x = (float)(canvas->_position.x) + (float)(_rect.position.x) * scale;
	outlineSpritePos.y = (float)(canvas->_position.y) + (float)(_rect.position.y) * scale;
	_outlineSprite->setPosition(outlineSpritePos);

	window->draw(*_outlineSprite);

}

void Selection::drawEdgePoints() {

	if (!(_points.size() >= 3 && (_state == ResizableToolState::Selected || _state == ResizableToolState::Resizing)))
		return;

	for (auto& point : _edgePoints) {
		point->draw();
	}
}

void Selection::cursorHover() {

	if (!dialogs.empty()) {
		return;
	}

	if (_state == ResizableToolState::None)
		return;

	sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);

	if (clickOnSelection(tile) || (Element_pressed.get() == this && _state == ResizableToolState::Selecting)) {
		Element_hovered = this->shared_from_this();
	}

	for (auto& edgePoint : _edgePoints) {
		edgePoint->cursorHover();
		if(Element_hovered == edgePoint) {
			_hoveredEdgePoint = edgePoint;
		}
	}

}

void Selection::handleEvent(const sf::Event& event) {

	if(!dialogs.empty()) {
		return;
	}

	if (main_menu->cursorOnAnyMenuButton()) {
		return;
	}

	if(main_menu->_state != MainMenuStates::Closed) {
		return;
	}

	if((_state == ResizableToolState::None || _state == ResizableToolState::Selected) && palette && palette->_rect.contains(cursor->_position)) {
		return;
	}

	bool clickedOnColor = false;
	for (auto& c : toolbar->_colors) {
		if (c->_rect.contains(cursor->_position)) {
			clickedOnColor = true;
			break;
		}
	}

	bool clickedOnFirstOrSecondColor = toolbar->_first_color->_rect.contains(cursor->_position) || toolbar->_second_color->_rect.contains(cursor->_position);
	
	bool clickedOnSameSelectionButton = false;
	if (toolbar->_selectedToolButton == toolbar->_btn_lasso && toolbar->_btn_lasso->_rect.contains(cursor->_position)) {
		clickedOnSameSelectionButton = true;
	}

	if (toolbar->_selectedToolButton == toolbar->_btn_select && toolbar->_btn_select->_rect.contains(cursor->_position)) {
		clickedOnSameSelectionButton = true;
	}
	
	bool clickedOnPaletteButton = false;
	if(toolbar->_btn_palette_colors->_rect.contains(cursor->_position)) {
		clickedOnPaletteButton = true;
	}

	if(clickedOnSameSelectionButton || clickedOnFirstOrSecondColor || clickedOnColor || clickedOnPaletteButton) {
		return;
	}

	if (toolbar->_btn_paste_menu->_isOpen || toolbar->_option_transparency->_state == ButtonState::Pressed) {
		return;
	}

	for (auto& layerBox : layers_panel->layersBoxes) {
		if(layerBox->_visibling->_rect.contains(cursor->_position)) {
			return;
		}
	}

	// selection resizing
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_state == ResizableToolState::Selected && _hoveredEdgePoint != nullptr && Element_hovered == _hoveredEdgePoint) {
			_clickedEdgePoint = _hoveredEdgePoint;
			Element_pressed = _clickedEdgePoint;
			_orginalEdgePointPosition = _point_left_top->getPosition();
			_state = ResizableToolState::Resizing;
			return;
		}
	}
	else if (_state == ResizableToolState::Resizing) {
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (Element_pressed == _clickedEdgePoint)
				Element_pressed = nullptr;
			_clickedEdgePoint = nullptr;
			_state = ResizableToolState::Selected;
		}
		return;
	}

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (Element_hovered.get() == this) {
			Element_pressed = this->shared_from_this();
		}
	}

	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
		if (Element_pressed.get() == this && !clickOnSelection(tile)) {
			Element_pressed = nullptr;
		}
	}

	// other selection interactions
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {

		if ((canvasIsHovered() || Element_hovered.get() == nullptr || Element_hovered.get() == this || canvasIsPressed() || Element_pressed.get() == nullptr || Element_pressed.get() == this) &&
			(_state == ResizableToolState::None || _state == ResizableToolState::Selected)) {
			sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);

			if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && clickOnSelection(tile)) {
				_state = ResizableToolState::Moving;
				_offset = tile - _resizedRect.position;
				DebugLog(L"Start Selection");
				
			}
			else if (toolbar->_toolType == ToolType::Lasso || toolbar->_toolType == ToolType::Selector) {

				if (toolbar->_btn_copy->_state == ButtonState::Idle && toolbar->_btn_cut->_state == ButtonState::Idle && toolbar->_btn_paste->_state == ButtonState::Idle) {
					if (canvas->_rect.contains(cursor->_position)) {
						if (_rect.size.x > 1 && _rect.size.y > 1) {
							pasteToCanvas();
							_image = nullptr;
							_resizedImage = nullptr;
							if(getCurrentAnimation()->getCurrentLayer() && canvas->_isEdited == false && _state == ResizableToolState::Selected)
								history->saveStep();
							
						}
						_state = ResizableToolState::Selecting;
						Element_pressed = this->shared_from_this();
						_points.clear();
						generateRect();
						_resizedRect = _rect;
						_outlineOffset = tile;
						
					}
					else {
						if (_rect.size.x > 1 && _rect.size.y > 1) {

							pasteToCanvas();
							_image = nullptr;
							_resizedImage = nullptr;
							if (getCurrentAnimation()->getCurrentLayer() && canvas->_isEdited == false && _state == ResizableToolState::Selected)
								history->saveStep();
						}

						_state = ResizableToolState::None;
						_points.clear();
						_outlineOffset = sf::Vector2i(0, 0);
						generateRect();
						_resizedRect = _rect;
						
					}
				}

			}
		}
	}

	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (toolbar->_toolType == ToolType::Lasso || toolbar->_toolType == ToolType::Selector) {

			generateRect();
			generateMask();

			if (_state == ResizableToolState::Selecting) {
				_resizedRect = _rect;
				_resizedMaskImage = _maskImage;
				_resizedImage = _image;
			}

			if (_state == ResizableToolState::Selecting) {
				if (_rect.size.x < 2 || _rect.size.y < 2) {
					_state = ResizableToolState::None;
					_points.clear();
					_outlineOffset = sf::Vector2i(0, 0);
				}
				else {
					if (_rect.size.x > 1 && _rect.size.y > 1) {
						toolbar->_option_transparency->_checkbox->_value = 0;
						if (getCurrentAnimation()->getCurrentLayer()) {
							copyImageWithMask(&*_image, &getCurrentAnimation()->getCurrentLayer()->_image, 0, 0, _rect.position.x, _rect.position.y, *_maskImage, (toolbar->_option_transparency->_checkbox->_value == 0) ? sf::Color::Transparent : toolbar->_second_color->_color);
							removeImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, _resizedRect, *_resizedMaskImage, sf::Color::Transparent);
						}
						*_resizedImage = *_image;
					}
					_state = ResizableToolState::Selected;
					generateEdgePoints();
				}

			}
			else if (_state == ResizableToolState::Moving) {
				_state = ResizableToolState::Selected;
				generateEdgePoints();
			}

			if (Element_pressed.get() == this)
				Element_pressed = nullptr;
		}
	}

	if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && _state == ResizableToolState::Moving) {
			sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);

			sf::Vector2i desiredRectPos = tile - _offset;
			desiredRectPos.x = std::clamp(desiredRectPos.x, -_resizedRect.size.x, canvas->_size.x);
			desiredRectPos.y = std::clamp(desiredRectPos.y, -_resizedRect.size.y, canvas->_size.y);

			int minX = INT_MAX, minY = INT_MAX;
			for (auto& p : _points) {
				minX = std::min(minX, p.x);
				minY = std::min(minY, p.y);
			}

			_outlineOffset = desiredRectPos - sf::Vector2i(minX, minY);
			generateRect();
			_resizedRect.position = _rect.position;
		}

		else if (toolbar->_toolType == ToolType::Selector) {
			if (_state == ResizableToolState::Selecting) {

				sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_size, canvas->_zoom, canvas->_zoom_delta);

				if (_image != nullptr) {
					_image = nullptr;
					_resizedImage = nullptr;
				}


				// LOCAL względem punktu kliknięcia
				sf::Vector2i local = tile - _outlineOffset;

				int minX = std::min(0, local.x);
				int minY = std::min(0, local.y);
				int maxX = std::max(0, local.x);
				int maxY = std::max(0, local.y);

				_points.clear();
				_points.push_back(sf::Vector2i(minX, minY)); // LT
				_points.push_back(sf::Vector2i(maxX, minY)); // RT
				_points.push_back(sf::Vector2i(maxX, maxY)); // RB
				_points.push_back(sf::Vector2i(minX, maxY)); // LB
				_points.push_back(sf::Vector2i(minX, minY)); // close

				generateRect();
				_resizedRect = _rect;
				_image = std::make_shared<sf::Image>();
				_image->resize(sf::Vector2u(1, 1), sf::Color::Transparent);
				if (_rect.size.x > 1 && _rect.size.y > 1) {
					_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);
				}

			}
		}
		else if (toolbar->_toolType == ToolType::Lasso) {

			if (_state == ResizableToolState::Selecting) {
				sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_size, canvas->_zoom, canvas->_zoom_delta);

				if (_image != nullptr) {
					_image = nullptr;
					_resizedImage = nullptr;
				}

				addPoint(tile);
				generateRect();
				_resizedRect = _rect;

				_image = std::make_shared<sf::Image>();
				_image->resize(sf::Vector2u(1, 1), sf::Color::Transparent);
				if (_rect.size.x > 1 && _rect.size.y > 1) {
					_image->resize(sf::Vector2u(_rect.size), sf::Color::Transparent);
				}

			}
		}
	}
}

void Selection::update() {

	if (!dialogs.empty()) {
		return;
	}

	if (main_menu->cursorOnAnyMenuButton()) {
		return;
	}

	if (main_menu->_state != MainMenuStates::Closed) {
		return;
	}

	if(toolbar->_btn_paste_menu->_state == ButtonState::Pressed || toolbar->_btn_paste_menu->_isOpen) {
		return;
	}

	if (_state == ResizableToolState::Resizing) {
		for (auto& point : _edgePoints) {
			point->update();
		}

		resizeRect();
		generateResizedMask();
		resizeImage();
		return;
	}

	if (_state == ResizableToolState::Selected) {

		if ((currentTime - _moveTime).asSeconds() > 0.075f) {

			_moveTime = currentTime;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
				_rect.position.x = std::clamp(_rect.position.x - 1, -_resizedRect.size.x, canvas->_size.x);
				_rect.position.y = std::clamp(_rect.position.y, -_resizedRect.size.y, canvas->_size.y);
				_resizedRect.position = _rect.position;
				generateEdgePoints();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
				_rect.position.x = std::clamp(_rect.position.x + 1, -_resizedRect.size.x, canvas->_size.x);
				_rect.position.y = std::clamp(_rect.position.y, -_resizedRect.size.y, canvas->_size.y);
				_resizedRect.position = _rect.position;
				generateEdgePoints();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
				_rect.position.x = std::clamp(_rect.position.x, -_resizedRect.size.x, canvas->_size.x);
				_rect.position.y = std::clamp(_rect.position.y - 1, -_resizedRect.size.y, canvas->_size.y);
				_resizedRect.position = _rect.position;
				generateEdgePoints();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
				_rect.position.x = std::clamp(_rect.position.x, -_resizedRect.size.x, canvas->_size.x);
				_rect.position.y = std::clamp(_rect.position.y + 1, -_resizedRect.size.y, canvas->_size.y);
				_resizedRect.position = _rect.position;
				generateEdgePoints();
			}
		}
	}
}

void Selection::draw() {

	generateOutline(false);
	drawOutline();
	drawRect();
	drawEdgePoints();

}

std::shared_ptr<Selection> selection;