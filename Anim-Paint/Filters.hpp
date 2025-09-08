#ifndef Filters_hpp
#define Filter_hpp

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
        float power = 2.0;

        // wyliczenie sepii
        float red   = dot(color.rgb, vec3(0.393, 0.769, 0.189));
        float green = dot(color.rgb, vec3(0.349, 0.686, 0.168));
        float blue  = dot(color.rgb, vec3(0.272, 0.534, 0.131));

        vec3 sepiaColor = vec3(red, green, blue);

        // interpolacja między oryginalnym a sepią
        vec3 finalColor = mix(color.rgb, sepiaColor, sepia);
        
        // mocniejszy czerwony odcień
        sepiaColor.r *= 1.2;   // np. +20% czerwieni
        sepiaColor.b *= 0.9;   // trochę mniej niebieskiego

        gl_FragColor = vec4(finalColor, color.a);
    }
)";

void set_rotation(sf::Image& image, float angle, bool set_smooth) {

    angle = angle * 3.14159265f / 180.f;

    sf::Texture tex;
    tex.loadFromImage(image);

    sf::RenderTexture rtex;
    rtex.create(tex.getSize().x, tex.getSize().y);

    sf::Shader sh;
    sh.loadFromMemory(rotation_shader_source, sf::Shader::Fragment);
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
    tex.loadFromImage(image);

    sf::RenderTexture rtex;
    rtex.create(tex.getSize().x, tex.getSize().y);

    sf::Shader sh;
    sh.loadFromMemory(brightness_shader_source, sf::Shader::Fragment);
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
    tex.loadFromImage(image);

    sf::RenderTexture rtex;
    rtex.create(tex.getSize().x, tex.getSize().y);

    sf::Shader sh;
    sh.loadFromMemory(contrast_shader_source, sf::Shader::Fragment);
    sh.setUniform("contrast", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_saturation(sf::Image& image, float value) {


    sf::Texture tex;
    tex.loadFromImage(image);

    sf::RenderTexture rtex;
    rtex.create(tex.getSize().x, tex.getSize().y);

    sf::Shader sh;
    sh.loadFromMemory(saturation_shader_source, sf::Shader::Fragment);
    sh.setUniform("saturation", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

void set_sepia(sf::Image& image, float value) {


    sf::Texture tex;
    tex.loadFromImage(image);

    sf::RenderTexture rtex;
    rtex.create(tex.getSize().x, tex.getSize().y);

    sf::Shader sh;
    sh.loadFromMemory(sepia_shader_source, sf::Shader::Fragment);
    sh.setUniform("sepia", value);

    sf::Sprite spr(tex);
    rtex.clear(sf::Color::Transparent);
    rtex.draw(spr, &sh);
    rtex.display();

    image = rtex.getTexture().copyToImage();
}

#endif