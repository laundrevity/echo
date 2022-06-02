from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class EchoPackage(ConanFile):
    name = "echo"
    version = "0.1"

    # binary configuration
    settings = "os", "compiler", "build_type", "arch"
    requires = ["boost/1.71.0"]
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # srcs located in same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["echo"]
