#include "test_framework.hpp"

int main() {
    return TestFramework::TestRegistry::instance().run_all();
}
