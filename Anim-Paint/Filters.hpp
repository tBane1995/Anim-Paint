#ifndef Filters_hpp
#define Filter_hpp

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

#endif