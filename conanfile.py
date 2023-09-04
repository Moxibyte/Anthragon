"""
This file is defining all dependencies that anthragon may use

Dependencies are process in the following ways:
- Anthragon linked as static libs (per module)
  -> Dependencies are exported in the SDK-Package
  -> Dependencies are exported as package dependencies 
- Anthragon linked as DLLs (per module)
  -> Dependencies are linked statically in the modules DLLs
"""

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration

class AnthragonDependenciesRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    # === dependencies start ===
    def requirements_all(self):
        self.requires("libpng/1.6.40")
        self.requires("libjpeg-turbo/3.0.0")
        self.requires("freetype/2.13.0")
        self.requires("xxhash/0.8.2")

    def requirements_windows(self):
        pass

    def requirements_linux(self):
        pass

    # === dependencies end ===

    # === configure start ===
    def configure_all(self):
        pass

    def configure_windows(self):
        pass

    def configure_linux(self):
        pass

    # === configure end ===

    # === internals start ===
    def validate(self):
        if str(self.settings.os) not in ("Windows", "Linux"):
            raise ConanInvalidConfiguration("Anthragon is currently only supporting Windows and Linux!")

    def requirements(self):
        self.requirements_all()
        if self.settings.os == "Windows":
            self.requirements_windows()
        elif self.setting.os == "Linux":
            self.requirements_linux()
        else:
            pass

    def configure(self):
        self.configure_all()
        if self.settings.os == "Windows":
            self.configure_windows()
        elif self.setting.os == "Linux":
            self.configure_linux()
        else:
            pass

    # === internals end ===
