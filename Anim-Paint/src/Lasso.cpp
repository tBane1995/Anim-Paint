#include "Lasso.hpp"
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
	state = LassoState::None;

	points.clear();

	offset = sf::Vector2i(0, 0);

	image = nullptr;
	rect = sf::IntRect(0, 0, 0, 0);
	outlineOffset = sf::Vector2i(0, 0);

	shader.loadFromMemory(shader_source, sf::Shader::Fragment);

}

Lasso::~Lasso() {

}

void Lasso::shiftOriginIfNeeded(sf::Vector2i& point)
{
	sf::Vector2i shift(0, 0);
	if (point.x < 0) { shift.x = point.x; point.x = 0; }
	if (point.y < 0) { shift.y = point.y; point.y = 0; }

	if (shift.x != 0 || shift.y != 0) {

		outlineOffset += shift;

		for (auto& p : points)
			p -= shift;
	}
}

void Lasso::addPoint(sf::Vector2i point)   // tile = globalny punkt w kafelkach
{
	point = point - outlineOffset;

	shiftOriginIfNeeded(point);

	if (points.empty() || std::hypot(float(points.back().x - point.x), float(points.back().y - point.y)) > 0.0f)
	{
		points.push_back(point);
		// std::cout << "add point local: " << point.x << ", " << point.y << "\n";
	}
}

void Lasso::unselect() {
	points.clear();
	this->outlineOffset = sf::Vector2i(0, 0);
	generateRect();
}

void Lasso::generateRect() {
	if (points.size() < 3) {
		rect = sf::IntRect(0, 0, 0, 0);
		return;
	}

	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();

	for (auto& p : points) {
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
	}

	int width = maxX + 1;
	int height = maxY + 1;

	rect = sf::IntRect(outlineOffset.x, outlineOffset.y, width, height);
}

bool Lasso::clickOnSelection(sf::Vector2i point) {

	return rect.contains(point);
}

void Lasso::copy(sf::Image* canvas, sf::Color emptyColor)
{

	if (state != LassoState::Selected)
		return;

	this->generateRect();

	sf::IntRect r = rect;
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
			if (maskImg.getPixel(x, y) != sf::Color::White || image->getPixel(x, y) == emptyColor)
				image->setPixel(x, y, sf::Color::Transparent);

	copyImageToClipboard(image, sf::IntRect(0, 0, s.width, s.height));

}



void Lasso::paste(sf::Image* canvas, sf::Color emptyColor)
{

	if (image != nullptr) {
		generateRect();

		int dstW = static_cast<int>(canvas->getSize().x);
		int dstH = static_cast<int>(canvas->getSize().y);
		int srcW = static_cast<int>(image->getSize().x);
		int srcH = static_cast<int>(image->getSize().y);

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

				if (image->getPixel(sx, sy) != emptyColor && image->getPixel(sx, sy) != sf::Color::Transparent)
					canvas->setPixel(x, y, image->getPixel(sx, sy));
			}
		}
	}
	else {
		image = new sf::Image();
	}
	loadImageFromClipboard(*image);
	state = LassoState::Selected;
	this->unselect();
	addPoint(sf::Vector2i(0, 0));
	addPoint(sf::Vector2i(0, image->getSize().y - 1));
	addPoint(sf::Vector2i(image->getSize().x - 1, image->getSize().y - 1));
	addPoint(sf::Vector2i(image->getSize().x - 1, 0));
}

void Lasso::cut(sf::Image* canvas, sf::Color emptyColor) {
	if (state == LassoState::Selected) {

		if (image == nullptr) {
			std::cout << "click\n";
			image = new sf::Image();
			image->create(rect.width, rect.height, sf::Color::Transparent);
			image->copy(*canvas, 0, 0, rect, false);
			copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y));
		}
		else {
			copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y));
			//copyImageToClipboard(image, sf::IntRect(0, 0, image->getSize().x, image->getSize().y), mask);
		}

		delete image;
		image = nullptr;
		state = LassoState::None;
		rect = sf::IntRect(-1, -1, -1, -1);
	}
}

void Lasso::generateOutline(bool selectionComplete) {
	if (points.size() < 3) return;

	if (rect.width <= 0 || rect.height <= 0)
		return;

	if (outlineRenderTexture.getSize() != sf::Vector2u(rect.width, rect.height))
		outlineRenderTexture.create(rect.width, rect.height);

	outlineRenderTexture.clear(sf::Color(0, 0, 0, 0));

	sf::Color lassoColor = sf::Color(47, 127, 127, 255);

	sf::VertexArray lines(sf::LineStrip);
	for (auto& point : points)
		lines.append(sf::Vertex(sf::Vector2f(point), lassoColor));

	// first and last point
	sf::VertexArray p(sf::Points);
	p.append(sf::Vertex(sf::Vector2f(points.front()), lassoColor));
	p.append(sf::Vertex(sf::Vector2f(points.back()), lassoColor));

	if (selectionComplete)
		lines.append(sf::Vertex(sf::Vector2f(points.front()), lassoColor));

	sf::RenderStates rs;
	rs.blendMode = sf::BlendAlpha;
	rs.transform.translate(0.5f, 0.5f);
	outlineRenderTexture.draw(lines, rs);
	outlineRenderTexture.draw(p, rs);
	outlineRenderTexture.display();
}

sf::RenderTexture* Lasso::generateMask() {
	sf::RenderTexture* mask = new sf::RenderTexture();
	mask->create(image->getSize().x, image->getSize().y);

	//std::cout << image->getSize().x << ", " << image->getSize().y << "\n";

	if (points.size() >= 3) {
		mask->clear(sf::Color::Transparent);

		sf::ConvexShape poly;
		poly.setPointCount(points.size());
		for (std::size_t i = 0; i < points.size(); ++i)
			poly.setPoint(i, sf::Vector2f(points[i]));

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
	if (!image) return;

	if (image->getSize().x < 1 || image->getSize().y < 1)
		return;

	generateRect();
	sf::IntRect canvasRect(0, 0, canvasSize.x, canvasSize.y);
	sf::IntRect visibleRect;

	if (rect.intersects(canvasRect, visibleRect)) {
		int tx = visibleRect.left - outlineOffset.x;
		int ty = visibleRect.top - outlineOffset.y;

		sf::IntRect texRect(tx, ty, visibleRect.width, visibleRect.height);

		texture = sf::Texture();
		texture.loadFromImage(*image);
		texture.setSmooth(false);

		sf::RenderTexture* mask = generateMask();

		shader.setUniform("texture", texture);
		shader.setUniform("mask", mask->getTexture());
		shader.setUniform("alphaColor", sf::Vector3f(alphaColor.r, alphaColor.g, alphaColor.b));

		sprite = sf::Sprite(texture);
		sprite.setTextureRect(texRect);
		sprite.setScale(scale, scale);
		sprite.setPosition(canvasPosition + sf::Vector2f(visibleRect.left, visibleRect.top) * scale);

		sf::RenderStates rs;
		rs.shader = &shader;
		window->draw(sprite, rs);
		delete mask;

	}


}

void Lasso::drawOutline(sf::Vector2f canvasPosition, float scale) {
	outlineSprite = sf::Sprite(outlineRenderTexture.getTexture());
	outlineSprite.setScale(scale, scale);
	outlineSprite.setPosition(canvasPosition + sf::Vector2f(outlineOffset) * scale);
	window->draw(outlineSprite);

}

void Lasso::drawRect(sf::Vector2f canvasPosition, float scale) {
	sf::RectangleShape r(sf::Vector2f(float(rect.width) * scale, float(rect.height) * scale));
	r.setPosition(canvasPosition + sf::Vector2f(rect.left * scale, rect.top * scale));
	r.setFillColor(sf::Color(127, 47, 47, 127));
	r.setOutlineColor(sf::Color(47, 127, 127, 255));
	r.setOutlineThickness(4.0f);
	window->draw(r);
}


void Lasso::draw(sf::Vector2f canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor) {

	if (points.size() >= 3) {
		if (state == LassoState::Selecting) {
			drawImage(canvasPosition, canvasSize, scale, alphaColor, false);
			generateOutline(false);
			drawOutline(canvasPosition, scale);

		}

		if (state == LassoState::Selected || state == LassoState::Moving) {
			drawImage(canvasPosition, canvasSize, scale, alphaColor, true);
			drawRect(canvasPosition, scale);
			generateOutline(true);
			drawOutline(canvasPosition, scale);

		}
	}

}

Lasso* lasso = nullptr;