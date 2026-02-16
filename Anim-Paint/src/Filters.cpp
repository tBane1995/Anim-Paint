#include "Filters.hpp"
#include "DebugLog.hpp"

std::string palette_button_shader_source = R"(
    uniform float iTime;
    uniform sampler2D texture;
    uniform vec4 texRectUV;

    vec3 hsv2rgb(in vec3 c)
    {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    vec3 hsv2rgb_smooth(in vec3 c)
    {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        rgb = rgb*rgb*(3.0 - 2.0*rgb);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    void main()
    {
        vec2 uvAtlas = gl_TexCoord[0].xy;

        // scalling to [0..1]
        vec2 uv = (uvAtlas - texRectUV.xy) / (texRectUV.zw - texRectUV.xy);

        // moved color by time
        float hueShift = -iTime * 0.1;
        float hue = fract(uv.x + hueShift);

        // palette: H = X, S = 1, V = Y
        vec3 hsv = vec3(hue, 1.0, uv.y);

        vec3 rgb_o = hsv2rgb(hsv);
        vec3 rgb_s = hsv2rgb_smooth(hsv);

        float t = smoothstep(-0.2, 0.2, sin(2.0 * iTime));
        vec3 hsvRgb = mix(rgb_o, rgb_s, t);

        vec4 base = texture2D(texture, uvAtlas);

        // white pixels
        float whiteMask = float(base.r == 1.0 && base.g == 1.0 && base.b == 1.0);

        // white -> HSV, other -> texture
        vec3 outRgb = mix(base.rgb, hsvRgb, whiteMask);

        gl_FragColor = vec4(outRgb, base.a);
    }
)";

std::string palette_colors_shader_source = R"(
    uniform sampler2D texture;
    uniform vec4 texRectUV;

    vec3 hsv2rgb(in vec3 c) {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

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
    uniform float iTime;
    uniform sampler2D texture;
    uniform vec4 texRectUV;

    void main()
    {
        vec2 uvAtlas = gl_TexCoord[0].xy;

        vec2 uv = (uvAtlas - texRectUV.xy) / (texRectUV.zw - texRectUV.xy);
        vec4 base = texture2D(texture, uvAtlas);
        float t = uv.y;

        vec3 color = mix(base.rgb, vec3(0.0), t);

        gl_FragColor = vec4(color, base.a);
    }
)";

std::string rotation_shader_source = R"(
    uniform sampler2D texture;
    uniform float angle;
    uniform vec4 backgroundColor;

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
            gl_FragColor = backgroundColor;
        } else {
            gl_FragColor = texture2D(texture, rotated_uv);
        }
    }
)";

std::string brightness_shader_source = R"(
    uniform sampler2D texture;
    uniform int brightness;

    void main() {
        
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        // Brightness
        float b = (float)brightness / 100.0;
        color.rgb += vec3(b);

        gl_FragColor = color;
    }
)";

std::string contrast_shader_source = R"(
    uniform sampler2D texture;
    uniform int contrast;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        // Contrast
        float c = 1.0 + ((float)contrast / 100.0);
        color.rgb = (color.rgb - 0.5) * c + 0.5;

        gl_FragColor = color;
    }
)";

std::string saturation_shader_source = R"(
    uniform sampler2D texture;
    uniform int saturation;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        vec3 luma = vec3(0.3086, 0.6094, 0.0820);
        float gray = dot(color.rgb, luma);
        float s = (float)saturation / 100.0;
        color.rgb = mix(vec3(gray), color.rgb, s);

        gl_FragColor = color;
    }
)";


std::string sepia_shader_source = R"(
    uniform sampler2D texture;
    uniform int sepia; // 0 - no effect, 100 - max sepia

    void main() {
        vec2 uv = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, uv);

        // wyliczenie sepii
        float red   = dot(color.rgb, vec3(0.393, 0.769, 0.189));
        float green = dot(color.rgb, vec3(0.349, 0.686, 0.168));
        float blue  = dot(color.rgb, vec3(0.272, 0.534, 0.131));

        vec3 sepiaColor = vec3(red, green, blue);

        sepiaColor.r *= 1.4;   // +40% red
        sepiaColor.g *= 0.4;   // -60% green
        sepiaColor.b *= 0.4;   // -60% blue

        float s = (float)sepia / 100.0;
        vec3 finalColor = mix(color.rgb, sepiaColor, sepia);

        gl_FragColor = vec4(finalColor, color.a);
    }
)";

std::string outline_shader_source = R"(
    uniform sampler2D u_tex;
    uniform vec2 texelSize;           // 1.0 / textureSize
    uniform int outlineWidth;        // 0..8 px
    uniform vec4 backgroundColor;
    uniform vec4 outlineColor;        // color (rgb) + power (a) [0..1]
    uniform float threshold;           // background similarity tolerance (0.02..0.08)

    float colorDistance(vec3 a, vec3 b) { return length(a - b); }

    void main() {
        vec2 uv   = gl_TexCoord[0].xy;
        vec4 base = texture2D(u_tex, uv);

        float isBg = step(colorDistance(base.rgb, backgroundColor.rgb), threshold);
        
        float ow = (float)outlineWidth;
        int r = int(ceil(clamp(ow, 0.0, 8.0)));

        float hasNonBgNeighbor = 0.0;
        if (r > 0) {
            for (int dy = -outlineWidth; dy <= outlineWidth; dy++) {
                for (int dx = -outlineWidth; dx <= outlineWidth; dx++) {
                    if (abs(dx) > r || abs(dy) > r) continue;
                    if (dx*dx + dy*dy > r*r)       continue; // circle check
                    if (dx == 0 && dy == 0)        continue; // skip self

                    vec2 o = vec2(float(dx), float(dy)) * texelSize;
                    vec3 c = texture2D(u_tex, uv + o).rgb;

                    float isBgSample = step(colorDistance(c, backgroundColor.rgb), threshold);
                    hasNonBgNeighbor = max(hasNonBgNeighbor, 1.0 - isBgSample);
                }
            }
        }

        float edge = isBg * hasNonBgNeighbor;

        vec3  rgbOut = mix(base.rgb, outlineColor.rgb, edge * outlineColor.a);
        float aOut   = base.a;

        gl_FragColor = vec4(rgbOut, aOut);
    }
)";

std::string resize_shader_source = R"(
    uniform sampler2D texture;
    uniform vec2 originalSize;
    uniform vec2 targetSize;
    uniform vec4 backgroundColor;

    void main() {
        vec2 uv = gl_TexCoord[0].xy;

        vec2 scale = originalSize / targetSize;

        vec2 centeredUV = uv - vec2(0.5);
        vec2 scaledUV = centeredUV * scale;
        vec2 finalUV = scaledUV + vec2(0.5);

        if (finalUV.x < 0.0 || finalUV.x > 1.0 || finalUV.y < 0.0 || finalUV.y > 1.0) {
            gl_FragColor = backgroundColor;
        } else {
            gl_FragColor = texture2D(texture, finalUV);
        }
    }
)";

std::string chessboard_shader_source = R"(

    uniform sampler2D texture;
    uniform vec4 firstColor;
    uniform vec4 secondColor;
    uniform int tileCount;
    uniform int transparency; // 0–100

    void main()
    {
        float t = float(tileCount);
        vec2 uv = gl_TexCoord[0].xy * t;

        float c = mod(floor(uv.x) + floor(uv.y), 2.0);

        vec4 checkerColor = mix(firstColor, secondColor, c);
        vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);

        float alpha = float(transparency) / 100.0;

        gl_FragColor = mix(texColor, checkerColor, alpha);
    }
)";

std::string invert_rgb_shader_source = R"(

    uniform sampler2D texture;

    void main()
    {
        vec4 color = texture2D(texture, gl_TexCoord[0].xy);
        color.rgb = 1.0 - color.rgb;
        gl_FragColor = color;
    }
)";

std::string invert_hsv_shader_source = R"(

    uniform sampler2D texture;

    vec3 rgb2hsv(in vec3 c)
    {
        vec4 K = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);

        vec4 p = mix(vec4(c.bg, K.wz),vec4(c.gb, K.xy),step(c.b, c.g)); 

        vec4 q = mix(vec4(p.xyw, c.r),vec4(c.r, p.yzx),step(p.x, c.r));

        float d = q.x - min(q.w, q.y);
        float e = 1e-10;

        return vec3(
            abs(q.z + (q.w - q.y) / (6.0 * d + e)), // Hue
            d / (q.x + e),                          // Saturation
            q.x                                     // Value
        );
    }

    vec3 hsv2rgb(in vec3 c)
    {
        vec3 rgb = clamp(abs(mod(c.x*6.0 + vec3(0.0,4.0,2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    void main()
    {
        vec4 color = texture2D(texture, gl_TexCoord[0].xy);
        vec3 hsv = rgb2hsv(color.rgb);
        hsv.x = mod(hsv.x + 0.5, 1.0); // Invert hue
        color.rgb = hsv2rgb(hsv);
        gl_FragColor = color;
    }
)";


void set_rotation(sf::Image& image, int angle, bool set_smooth, sf::Color backgroundColor) {

    

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

    float a = (float)angle * 3.14159265f / 180.f;
    sh.setUniform("angle", a);
	sh.setUniform("backgroundColor", sf::Glsl::Vec4(backgroundColor.r / 255.0f, backgroundColor.g / 255.0f, backgroundColor.b / 255.0f, backgroundColor.a / 255.0f));
    tex.setSmooth(set_smooth);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_brightness(sf::Image& image, int value) {

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

void set_contrast(sf::Image& image, int value) {

	

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

void set_saturation(sf::Image& image, int value) {

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

void set_sepia(sf::Image& image, int value) {

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

void set_outline(sf::Image& image, int width, sf::Color backgroundColor, sf::Color outlineColor) {
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

    sh.setUniform("outlineWidth", (float)(width));
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

void set_resize(sf::Image& image, int width, int height, sf::Color backgroundColor) {

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

    sh.setUniform("targetSize", sf::Vector2f((float)(width), (float)(height)));
    sh.setUniform("originalSize", sf::Vector2f(tex.getSize()));
    sh.setUniform("backgroundColor", sf::Glsl::Vec4(float(backgroundColor.r) / 255.0f, float(backgroundColor.g) / 255.0f, float(backgroundColor.b) / 255.0f, float(backgroundColor.a) / 255.0f));

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}


void set_chessboard(sf::Image& image, int tileCount, int transparency, sf::Color firstColor, sf::Color secondColor) {

    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
        DebugError(L"Filter set_chessboard: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
        DebugError(L"Filter set_chessboard: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(chessboard_shader_source, sf::Shader::Type::Fragment)) {
        DebugError(L"Filter set_chessboard: failed to load shader from memory.");
        exit(0);
    }

    sh.setUniform("tileCount", tileCount);
	sh.setUniform("transparency", transparency); // 0–100
    sh.setUniform("firstColor", sf::Glsl::Vec4(float(firstColor.r) / 255.0f, float(firstColor.g) / 255.0f, float(firstColor.b) / 255.0f, float(firstColor.a) / 255.0f));
    sh.setUniform("secondColor", sf::Glsl::Vec4(float(secondColor.r) / 255.0f, float(secondColor.g) / 255.0f, float(secondColor.b) / 255.0f, float(secondColor.a) / 255.0f));

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_invert_rgb(sf::Image& image) {
    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
        DebugError(L"Filter set_invert_rgb: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
        DebugError(L"Filter set_invert_rgb: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(invert_rgb_shader_source, sf::Shader::Type::Fragment)) {
        DebugError(L"Filter set_invert_rgb: failed to load shader from memory.");
        exit(0);
    }

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_invert_hsv(sf::Image& image) {
    sf::Texture tex;
    if (!tex.loadFromImage(image)) {
        DebugError(L"Filter set_invert_rgb: failed to load texture from image.");
        exit(0);
    }

    sf::RenderTexture rtex;
    if (!rtex.resize(tex.getSize())) {
        DebugError(L"Filter set_invert_rgb: failed to resize render texture.");
        exit(0);
    }

    sf::Shader sh;
    if (!sh.loadFromMemory(invert_hsv_shader_source, sf::Shader::Type::Fragment)) {
        DebugError(L"Filter set_invert_rgb: failed to load shader from memory.");
        exit(0);
    }

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}