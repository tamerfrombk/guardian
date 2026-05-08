#pragma once

#include <iostream>

namespace gdn {

[[nodiscard]] constexpr auto run(int argc, char **argv) -> int {
    std::cout << argv[0] << " called with: " << "\n";
    for (auto i = 1; i < argc; ++i) {
        std::cout << "\t" << argv[i] << "\n";
    }

    return 0;
}

} // namespace gdn
