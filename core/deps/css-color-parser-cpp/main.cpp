#include "csscolorparser.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const CSSColorParser::Color& color) {
    return os << "rgba(" << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << color.a << ")";
}

int main() {
    using namespace CSSColorParser;

    std::cout << "rgba(255, 128, 12, 0.5) == " << parse(" rgba (255, 128, 12, 0.5)") << std::endl;
    std::cout << "rgba(255, 255, 255, 1) == " << parse("#fff") << std::endl;
    std::cout << "rgba(255, 0, 17, 1) == " << parse("#ff0011") << std::endl;
    std::cout << "rgba(106, 90, 205, 1) == " << parse("slateblue") << std::endl;
    std::cout << "rgba(0, 0, 0, 1) == " << parse("blah") << std::endl;
    std::cout << "rgba(0, 0, 0, 1) == " << parse("ffffff") << std::endl;
    std::cout << "rgba(226, 233, 233, 0.5) == " << parse("hsla(900, 15%, 90%, 0.5)") << std::endl;
    std::cout << "rgba(0, 0, 0, 1) == " << parse("hsla(900, 15%, 90%)") << std::endl;
    std::cout << "rgba(226, 233, 233, 1) == " << parse("hsl(900, 15%, 90%)") << std::endl;
    std::cout << "rgba(226, 233, 233, 1) == " << parse("hsl(900, 0.15, 90%)") << std::endl;  // NOTE: not spec compliant.
    std::cout << "rgba(0, 0, 0, 1) == " << parse("rgba(blah,blub,100,boing") << std::endl;

}
