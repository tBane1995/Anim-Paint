#ifndef Lasso_hpp
#define Lasso_hpp

std::string lasso_only_outline_source = R"(
uniform sampler2D mask;
uniform vec2  maskSize;     // px
uniform float outlineSize;  // PROMIEŃ w px
uniform vec4  outlineColor;

void main() {
    vec2 uv  = gl_TexCoord[0].xy;
    vec2 pix = uv * maskSize;
    vec2 base = floor(pix);

    // 1) r zaokrąglij do najbliższej całości
    int r = int(floor(outlineSize + 0.5));

    float aMax = 0.0;
    float aMin = 1.0;

    // 2) zakres ZAMKNIĘTY [-r .. r]
    for (int j = -r; j < r; ++j) {
        for (int i = -r; i < r; ++i) {
            vec2 p = base + vec2(float(i), float(j));

            // out-of-bounds = 0 (shader ogranicza współrzędne)
            if (p.x < 0.0 || p.y < 0.0 || p.x > maskSize.x - 1.0 || p.y > maskSize.y - 1.0) {
                aMax = max(aMax, 0.0);
                aMin = min(aMin, 0.0);
            } else {
                vec2 coords = (p + vec2(0.5, 0.5)) / maskSize;
                float s = texture2D(mask, coords).a;
                aMax = max(aMax, s);
                aMin = min(aMin, s);
            }
        }
    }

    float dilated = step(0.5, aMax);
    float eroded  = step(0.5, aMin);
    float edge    = clamp(dilated - eroded, 0.0, 1.0);

    gl_FragColor = vec4(outlineColor.rgb, outlineColor.a * edge);
}
)";

enum class LassoState { None, Selecting, Selected, Moving };

class Lasso {
public:
	LassoState state;

	std::vector<sf::Vector2f> points;
	sf::VertexArray outline;
	
	sf::Texture texture;
	sf::Sprite sprite;

	// mask
	float margin = 1;
	sf::RenderTexture mask;
	sf::Sprite maskSprite;
	sf::Shader shaderOutline;

	Lasso() {
		state = LassoState::None;
		points.clear();
		outline = sf::VertexArray(sf::LineStrip);


	}

	~Lasso() { }

	void startSelecting(sf::Vector2f canvas_position, float pixelSize, float zoom, float zoom_delta) {

		float scale = zoom * zoom_delta;

		texture.loadFromImage(layers_dialog->getCurrentLayer()->image);
		texture.setSmooth(false);

		sprite = sf::Sprite(texture);
		sprite.setScale(scale, scale);
		sprite.setPosition(canvas_position);
		margin = 1.0f;

		// UWAGA: prawdziwy margines: +2*pad
		mask.create(texture.getSize().x + 2*margin, texture.getSize().y + 2*margin);
		mask.setSmooth(false);
		mask.clear(sf::Color(127, 47, 47, 127));	// czerwone tlo żeby było widać 
		mask.display();

		maskSprite = sf::Sprite(mask.getTexture());
		maskSprite.setScale(scale, scale);

		// Wyrównanie: maska jest większa o pad po każdej stronie, więc cofamy sprite o pad*scale
		maskSprite.setPosition(canvas_position - sf::Vector2f(margin * scale, margin * scale));

		shaderOutline.loadFromMemory(lasso_only_outline_source, sf::Shader::Fragment);
		shaderOutline.setUniform("mask", mask.getTexture());
		shaderOutline.setUniform("maskSize", sf::Glsl::Vec2(mask.getSize().x, mask.getSize().y));
		shaderOutline.setUniform("outlineSize", 1.0f);
		shaderOutline.setUniform("outlineColor", sf::Glsl::Vec4(47.0f/255.0f, 127.0f/255.0f, 127.0f/255.0f, 255.0f/255.0f)); // cyan

		points.clear();
		outline.clear();
	}

	void addPoint(sf::Vector2f p, sf::Vector2f canvas_position, float pixelSize, float zoom, float zoom_delta) {
		
		float scale = zoom * zoom_delta;
		// do pikseli OBRAZKA (bez pada)
		float lx = (p.x - canvas_position.x) / scale;
		float ly = (p.y - canvas_position.y) / scale;

		// do pikseli MASKI (z padem)
		float mx = lx + margin;
		float my = ly + margin;

		int w = int(mask.getSize().x);
		int h = int(mask.getSize().y);

		// centrum okna [-r..r] musi mieć pełne sąsiedztwo → clamp do [pad .. size-1-pad]
		mx = std::clamp<float>(std::floor(mx), float(margin), float(w - 1 - 2*margin));
		my = std::clamp<float>(std::floor(my), float(margin), float(h - 1 - 2*margin));

		sf::Vector2f q(mx, my);

		if (points.empty() || std::hypot(points.back().x - q.x, points.back().y - q.y) >= 1.0f) {
			points.push_back(q);
			outline.append(sf::Vertex(q, sf::Color::Cyan));
			generateOutline(zoom, zoom_delta);
		}
	}

	void completeSelection(float zoom, float zoom_delta) {
		if (!points.empty() && outline.getVertexCount() > 0) {
			points.push_back(points.front());
			outline.append(outline[0]);
			generateOutline(zoom, zoom_delta);
		}
		
	}

	void unselect() {
		points.clear();
		outline.clear();
		std::cout << "unselect\n";
	}

	void generateOutline(float zoom, float zoom_delta) {
		if (points.size() < 3) return;

		sf::VertexArray lines(sf::LineStrip);

		for (auto& p : points)
			lines.append(sf::Vertex(p + sf::Vector2f(margin, margin), sf::Color::White));

		mask.create(texture.getSize().x + 2, texture.getSize().y + 2);
		mask.clear(sf::Color(127, 47, 47, 127));
		mask.draw(lines, sf::RenderStates(sf::BlendNone));
		mask.display();

		shaderOutline.setUniform("mask", mask.getTexture());
		shaderOutline.setUniform("maskSize", sf::Glsl::Vec2(mask.getSize().x, mask.getSize().y));

	}

	void draw() {
		if (state != LassoState::None && outline.getVertexCount() >= 3) {
			
			//window->draw(outline);

			sf::RenderStates rs;
			rs.shader = &shaderOutline;
			window->draw(maskSprite, rs);
		}
	}
};

Lasso* lasso = nullptr;
#endif