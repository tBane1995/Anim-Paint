#include "Filters.hpp"
#include "DebugLog.hpp"

std::string palette_button_shader_source = R"(
    uniform float iTime;            // ustawiane z C++
    uniform sampler2D texture;      // SFML podstawi teksturę sprite'a
    uniform vec4 texRectUV;         // (u0, v0, u1, v1) – NORMALIZOWANE granice wycinka

    // --- HSV -> RGB (oficjalne) ---
    vec3 hsv2rgb(in vec3 c)
    {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    // --- HSV -> RGB (wygładzone) ---
    vec3 hsv2rgb_smooth(in vec3 c)
    {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        rgb = rgb*rgb*(3.0 - 2.0*rgb);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    void main()
    {
        // oryginalne współrzędne tekstury (pod-wycinek atlasu)
        vec2 uvAtlas = gl_TexCoord[0].xy;

        // przeskaluj do zakresu lokalnego [0..1] w obrębie sprite'a
        vec2 uv = (uvAtlas - texRectUV.xy) / (texRectUV.zw - texRectUV.xy);

        // przesunięcie koloru w lewo w czasie
        float hueShift = -iTime * 0.1;  // prędkość przesuwania w lewo (0.1 to tempo)
        float hue = fract(uv.x + hueShift);

        // paleta: H = X przesunięty, S = 1, V = Y
        vec3 hsv = vec3(hue, 1.0, uv.y);

        vec3 rgb_o = hsv2rgb(hsv);
        vec3 rgb_s = hsv2rgb_smooth(hsv);

        float t = smoothstep(-0.2, 0.2, sin(2.0 * iTime));
        vec3 hsvRgb = mix(rgb_o, rgb_s, t);

        // kolor z tekstury
        vec4 base = texture2D(texture, uvAtlas);

        // białe piksele (dokładnie 1.0)
        float whiteMask = float(base.r == 1.0 && base.g == 1.0 && base.b == 1.0);

        // białe → HSV, pozostałe → tekstura
        vec3 outRgb = mix(base.rgb, hsvRgb, whiteMask);

        gl_FragColor = vec4(outRgb, base.a);
    }
)";

std::string palette_colors_shader_source = R"(
    uniform sampler2D texture;      // SFML podstawi teksturę sprite'a
    uniform vec4 texRectUV;         // (u0, v0, u1, v1) – NORMALIZOWANE granice wycinka

    // --- HSV -> RGB (oficjalne) ---
    vec3 hsv2rgb(in vec3 c) {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    // --- HSV -> RGB (wygładzone) ---
    vec3 hsv2rgb_smooth(in vec3 c) {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        rgb = rgb * rgb * (3.0 - 2.0 * rgb);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    void main() {
        vec2 uvAtlas = gl_TexCoord[0].xy;

        vec2 uv = (uvAtlas - texRectUV.xy) / (texRectUV.zw - texRectUV.xy);

        float hue = fract(uv.x);
        vec3 hsv = vec3(hue, 1.0 - uv.y, 1.0);
        vec3 hsvRgb = hsv2rgb_smooth(hsv);

        gl_FragColor = vec4(hsvRgb, 1.0);
    }
)";

std::string palette_values_shader_source = R"(
    uniform float iTime;            // ustawiane z C++
    uniform sampler2D texture;      // SFML podstawi teksturę sprite'a
    uniform vec4 texRectUV;         // (u0, v0, u1, v1) – NORMALIZOWANE granice wycinka

    void main()
    {
        // współrzędne atlasu
        vec2 uvAtlas = gl_TexCoord[0].xy;

        // przeskalowanie do [0..1] lokalnie w obrębie sprite’a
        vec2 uv = (uvAtlas - texRectUV.xy) / (texRectUV.zw - texRectUV.xy);

        // pobierz kolor bazowy z tekstury (np. z palety barw)
        vec4 base = texture2D(texture, uvAtlas);

        // współczynnik przejścia: 0.0 (lewa) → 1.0 (prawa)
        float t = uv.y;

        // im dalej w prawo, tym bardziej czarny
        vec3 color = mix(base.rgb, vec3(0.0), t);

        gl_FragColor = vec4(color, base.a);
    }
)";

std::string rotation_shader_source = R"(
    uniform sampler2D texture;
    uniform float angle;

    vec2 rotate(vec2 uv, vec2 pivot, float a) {
        float c = cos(a);
        float s = sin(a);
        mat2 R = mat2(c, -s,
                      s,  c);
        return (R * (uv - pivot)) + pivot;
    }

    void main() {
        vec2 uv = gl_TexCoord[0].xy;

        vec2 rotated_uv = rotate(uv, vec2(0.5, 0.5), angle);

        if (rotated_uv.x < 0.0 || rotated_uv.x > 1.0 ||
            rotated_uv.y < 0.0 || rotated_uv.y > 1.0) {
            gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        } else {
            gl_FragColor = texture2D(texture, rotated_uv);
        }
    }
)";

std::string brightness_shader_source = R"(
    uniform sampler2D texture;
    uniform float brightness;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        // Brightness
        color.rgb += vec3(brightness);

        gl_FragColor = color;
    }
)";

std::string contrast_shader_source = R"(
    uniform sampler2D texture;
    uniform float contrast;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        color.rgb = (color.rgb - 0.5) * contrast + 0.5;

        gl_FragColor = color;
    }
)";

std::string saturation_shader_source = R"(
    uniform sampler2D texture;
    uniform float saturation;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        vec3 luma = vec3(0.3086, 0.6094, 0.0820);
        float gray = dot(color.rgb, luma);
        color.rgb = mix(vec3(gray), color.rgb, saturation);

        gl_FragColor = color;
    }
)";


std::string sepia_shader_source = R"(
    uniform sampler2D texture;
    uniform float sepia; // 0 - brak efektu, 100 - pełna sepia

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        // wyliczenie sepii
        float red   = dot(color.rgb, vec3(0.393, 0.769, 0.189));
        float green = dot(color.rgb, vec3(0.349, 0.686, 0.168));
        float blue  = dot(color.rgb, vec3(0.272, 0.534, 0.131));

        vec3 sepiaColor = vec3(red, green, blue);

        // mocniejszy czerwony odcień
        sepiaColor.r *= 1.4;   // +40% czerwonego
        sepiaColor.g *= 0.4;   // -60% zielonego
        sepiaColor.b *= 0.4;   // -60% niebieskiego

        // interpolacja między oryginalnym a sepią
        vec3 finalColor = mix(color.rgb, sepiaColor, sepia);

        gl_FragColor = vec4(finalColor, color.a);
    }
)";

std::string outline_shader_source = R"(
    uniform sampler2D u_tex;
    uniform vec2  texelSize;           // 1.0 / textureSize
    uniform float outlineWidth;        // 0..8 px
    uniform vec4  backgroundColor;     // kolor tła (color key)
    uniform vec4  outlineColor;        // kolor obrysu (rgb) + siła a [0..1]
    uniform float threshold;           // tolerancja podobieństwa do tła (0.02..0.08)

    float colorDistance(vec3 a, vec3 b) { return length(a - b); }

    void main() {
        vec2 uv   = gl_TexCoord[0].xy;
        vec4 base = texture2D(u_tex, uv);

        // bieżący piksel to tło?
        float isBg = step(colorDistance(base.rgb, backgroundColor.rgb), threshold);

        // promień próbkowania (max 8)
        int r = int(ceil(clamp(outlineWidth, 0.0, 8.0)));

        // wykryj, czy w sąsiedztwie jest piksel NIE-będący tłem (czyli „obiekt”)
        float hasNonBgNeighbor = 0.0;
        if (r > 0) {
            for (int dy = -outlineWidth; dy <= outlineWidth; dy++) {
                for (int dx = -outlineWidth; dx <= outlineWidth; dx++) {
                    if (abs(dx) > r || abs(dy) > r) continue;
                    if (dx*dx + dy*dy > r*r)       continue; // koło
                    if (dx == 0 && dy == 0)        continue; // pomijamy siebie

                    vec2 o = vec2(float(dx), float(dy)) * texelSize;
                    vec3 c = texture2D(u_tex, uv + o).rgb;

                    float isBgSample = step(colorDistance(c, backgroundColor.rgb), threshold);
                    hasNonBgNeighbor = max(hasNonBgNeighbor, 1.0 - isBgSample);
                }
            }
        }

        // malujemy TYLKO na pikselach tła stykających się z obiektem (zewnętrzny obrys)
        float edge = isBg * hasNonBgNeighbor;

        // kolorujemy tło na krawędzi; piksele obiektu zostają nietknięte; alfa bez zmian
        vec3  rgbOut = mix(base.rgb, outlineColor.rgb, edge * outlineColor.a);
        float aOut   = base.a;

        gl_FragColor = vec4(rgbOut, aOut);
    }
)";

std::string resize_shader_source = R"(
    uniform sampler2D texture;
    uniform vec2 originalSize;
    uniform vec2 targetSize;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;

        vec2 scale = originalSize / targetSize;

        vec2 centeredUV = uv - vec2(0.5);
        vec2 scaledUV = centeredUV * scale;
        vec2 finalUV = scaledUV + vec2(0.5);

        gl_FragColor = texture2D(texture, finalUV);
    }
)";


void set_rotation(sf::Image& image, float angle, bool set_smooth) {

    angle = angle * 3.14159265f / 180.f;

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_rotation: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_rotation: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(rotation_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_rotation: failed to load shader from memory.");
        exit(0);
    }

    sh.setUniform("angle", angle);

    tex.setSmooth(set_smooth);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_brightness(sf::Image& image, float value) {

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_brightness: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_brightness: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(brightness_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_brightness: failed to load shader from memory.");
        exit(0);
    }

    sh.setUniform("brightness", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_contrast(sf::Image& image, float value) {

    value = 1.0f + value;

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_contrast: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_contrast: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(contrast_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_contrast: failed to load shader from memory.");
        exit(0);
    }

    sh.setUniform("contrast", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_saturation(sf::Image& image, float value) {

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_saturation: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_saturation: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(saturation_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_saturation: failed to load shader from memory.");
        exit(0);
    }
    sh.setUniform("saturation", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_sepia(sf::Image& image, float value) {

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_sepia: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_sepia: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(sepia_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_sepia: failed to load shader from memory.");
        exit(0);
    }
    sh.setUniform("sepia", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_outline(sf::Image& image, float width, sf::Color backgroundColor, sf::Color outlineColor) {
    // value 0 - 8

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_outline: failed to load texture from image.");
        exit(0);
	}

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_outline: failed to resize render texture.");
        exit(0);
	}

    sf::Shader sh;
    if (!sh.loadFromMemory(outline_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_outline: failed to load shader from memory.");
        exit(0);
	}

    sh.setUniform("outlineWidth", width);
    sh.setUniform("texelSize", sf::Vector2f(1.0f / float(image.getSize().x), 1.0f / float(image.getSize().y)));
    sh.setUniform("threshold", 0.02f);
    sh.setUniform("backgroundColor", sf::Glsl::Vec4(float(backgroundColor.r)/255.0f, float(backgroundColor.g)/255.0f, float(backgroundColor.b)/255.0f, float(backgroundColor.a)/255.0f));
    sh.setUniform("outlineColor", sf::Glsl::Vec4(float(outlineColor.r)/255.0f, float(outlineColor.g)/255.0f, float(outlineColor.b)/255.0f, float(outlineColor.a)/255.0f));

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_resize(sf::Image& image, float width, float height) {

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
		DebugError(L"Filter set_resize: failed to load texture from image.");
        exit(0);
	}

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
		DebugError(L"Filter set_resize: failed to resize render texture.");
        exit(0);
	}

    sf::Shader sh;
    if (!sh.loadFromMemory(resize_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"Filter set_resize: failed to load shader from memory.");
        exit(0);
	}

    sh.setUniform("targetSize", sf::Vector2f(width, height));
    sh.setUniform("originalSize", sf::Vector2f(tex.getSize()));

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}