import io
import os
import re
import numpy as np

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as _build_ext


try:
    from Cython.Build import cythonize

    USE_CYTHON = True
except ImportError:
    USE_CYTHON = False

ext = ".pyx" if USE_CYTHON else ".cpp"


def find_boost(hint=None, verbose=True):
    """ Find the boost include directory,
    returns ``None`` if not found.

    This is based upon the version in https://github.com/dfm/transit/
    Copyright (c) 2014 Daniel Foreman-Mackey
    which is licenced under the MIT licence.
    """

    search_dirs = [] if hint is None else hint

    if "BOOST_DIR" in os.environ:
        search_dirs.append(os.path.join(os.environ["BOOST_DIR"], "include"))

    if "CONDA_PREFIX" in os.environ:
        search_dirs.append(os.path.join(os.environ["CONDA_PREFIX"], "include"))

    search_dirs += [
        "/usr/include",
        "/usr/local/include",
        "/opt/local/include",
    ]

    for d in search_dirs:
        path = os.path.join(d, "boost", "geometry", "index", "rtree.hpp")
        if os.path.exists(path):
            vf = os.path.join(d, "boost", "version.hpp")
            src = open(vf, "r").read()
            v = re.findall('#define BOOST_LIB_VERSION "(.+)"', src)
            if not len(v):
                continue
            v = v[0]
            if verbose:
                print("Found Boost version {0} in: {1}".format(v, d))
            return d
    return None


class build_ext(_build_ext):
    def build_extension(self, ext):
        dirs = ext.include_dirs + self.compiler.include_dirs

        boost_include = find_boost(hint=dirs)
        if boost_include is None:
            raise RuntimeError("Boost not found, see the README for possible solutions")

        ext.include_dirs.append(boost_include)

        import numpy as np

        ext.include_dirs.append(np.get_include())

        _build_ext.build_extension(self, ext)


# read the contents of your README file
this_directory = os.path.abspath(os.path.dirname(__file__))
with io.open(os.path.join(this_directory, "README.md"), encoding="utf-8") as f:
    long_description = f.read()


# Define macros and libraries based on whether Boost is needed
DEFINE_MACROS = []
INCLUDE_DIRS = [np.get_include(), "/home/tcastro/include"]
LIBRARY_DIRS = ["/home/tcastro/lib"]
LIBRARIES = [
    "boost_log_setup", "boost_log", "boost_thread", "boost_date_time",
    "boost_system", "boost_filesystem", "pthread"
]
EXTRA_COMPILE_ARGS = ["-std=c++17", "-Wno-return-type"]
EXTRA_LINK_ARGS = ["-Wl,-rpath,/home/tcastro/lib"]

extensions = [
    Extension("ygg",
              sources=["pyfof/pyfof.pyx", "pyfof/fof.cc", "pyfof/fof_brute.cc"],
              define_macros=DEFINE_MACROS,
              include_dirs=INCLUDE_DIRS,
              library_dirs=LIBRARY_DIRS,
              libraries=LIBRARIES,
              extra_compile_args=EXTRA_COMPILE_ARGS,
              extra_link_args=EXTRA_LINK_ARGS,
              language="c++")
]

if USE_CYTHON:
    extensions = cythonize(extensions)

setup(
    name="YGGDRASIL",
    version="0.1-dev",
    description="YGGDRASIL: Yielder of Galactic Groups and Dynamic Rungs Architectural Structuring In Layers",
    author="Tiago Castro",
    author_email="tiagobscastro@gmail.com",
    url="https://github.com/TiagoBsCastro/YGGDRASIL",
    license="MIT",
    keywords=["clustering", "friends-of-friends"],
    install_requires=["numpy"],
    setup_requires=["numpy", "cython"],
    cmdclass={"build_ext": build_ext},
    ext_modules=extensions,
    long_description=long_description,
    long_description_content_type="text/markdown",
)
