from conan import ConanFile

class MoxPPRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("spdlog/1.17.0")      # Logging

    def configure(self):
        # self.options["spdlog"].shared = True
        pass
