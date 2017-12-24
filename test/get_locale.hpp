#include <sstream>

template<typename T>
std::locale get_locale(T &&name) try {
    return std::locale(name);
} catch (const std::runtime_error &e) {
    std::stringstream s;
    s << "Couldn't acquire locale: " << e.what() << ". Is '" << name << "' supported on your system?";

    FAIL(s.str());
    throw;
}
