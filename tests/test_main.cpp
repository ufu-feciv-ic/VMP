#include "test_framework.h"

int main() {
    return TestFramework::TestRegistry::instance().run_all();
}
