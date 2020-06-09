import os
import distutils
from setuptools import setup
from setuptools import Extension
from distutils import sysconfig
from distutils.version import LooseVersion
#from distutils.core import setup
#from distutils.extension import Extension
from subprocess import call
import numpy
import warnings
import platform
import glob
import sys

def uncythonize(extensions, **_ignore):
    for extension in extensions:
        sources = []
        for sfile in extension['sources']:
            path, ext = os.path.splitext(sfile)
            if ext in ('.pyx', '.py'):
                if extension.get('language',None) == 'c++':
                    ext = '.cpp'
                else:
                    ext = '.c'
                sfile = path + ext
            sources.append(sfile)
        extension['sources'][:] = sources
    return extensions

with_cython = False
cythonize = lambda x: x
try:
    try:
        from Cython import __version__ as version
    except:
        from Cython.Compiler.Version import version
    if LooseVersion(version) >= LooseVersion('0.20'):
        from Cython.Build import cythonize
        with_cython = True
    else:
        warnings.warn("Cython is too old ({0}), building from pre-compiled cython".format(version), UserWarning)
except:
    warnings.warn("Cython not found, building from pre-compiled cython", UserWarning)

#create datafiles
if call([
    sys.executable,
    os.path.join(
        os.path.dirname(os.path.realpath(__file__)),
        'mmstructlib', 'radii', 'load_radii.py')
]) != 0:
    raise("Error generating data file")

ext_mods = [
    {
        'name' : 'mmstructlib.IO.cif_parser',
        'sources' : ['mmstructlib/IO/cif_parser.pyx'],
        'include_dirs' : ['src/', 'src/vendor/boost_1.63/'],
#        'include_dirs' : ['src/', 'src/vendor/boost_1.55/'],
#        'include_dirs' : ['src/'],
        'language' : "c++",
        'extra_compile_args' : ['-O3', '-march=native', '-DNDEBUG', '-fno-strict-aliasing', '-fwrapv', '-std=c++11']
    },
    {
        'name' : 'mmstructlib.sas.nsc',
        'sources' : ['mmstructlib/sas/nsc.pyx', 'src/nsc/nsc.c', 'src/nsc/nsc_weighted.c'],
        'include_dirs' : ['src/nsc/', numpy.get_include()],
#        'extra_compile_args' : ['-g']
        'extra_compile_args' : ['-O3', '-march=native', '-DNDEBUG']
    },
    {
        'name' : 'mmstructlib.crystal.sginfo',
        'sources' : [
            'mmstructlib/crystal/sginfo.pyx',
            'src/vendor/sginfo_1_01/sgclib.c',
            'src/vendor/sginfo_1_01/sgfind.c',
            'src/vendor/sginfo_1_01/sghkl.c',
            'src/vendor/sginfo_1_01/sgio.c',
            'src/vendor/sginfo_1_01/sgsi.c',
            'src/sginfo_python/sg_info.cpp'],
        'include_dirs' : ['src/vendor/', 'src/'],
        'language' : "c++",
        'extra_compile_args' : ['-O3', '-march=native', '-DNDEBUG']
    }
]

#conditional builds
#CGAL-dependent
def get_library(path, name):
        files = glob.glob(os.path.join(path,"{}-*.lib".format(name)))
        files = list(filter(lambda x: x.find('-gd-') == -1, files))
        if len(files) != 1:
            warnings.warn(
                "Couldn't identify library {} at given path {}, possibly multiple versions available".format(name, path),
                UserWarning
            )
            return None, None
        path, lib = os.path.split(files[0])
        return path, lib[:-4]
build_cgal_ext = False
if platform.system() == 'Windows':
    if 'CGAL_DIR' in os.environ and 'CGAL_LIB' in os.environ and 'BOOST_INCLUDEDIR' in os.environ:
        cgal_inc = [
            os.path.join(os.environ['CGAL_DIR'], 'include'),
            os.path.join(os.environ['CGAL_DIR'], 'auxiliary', 'gmp', 'include'),
            os.path.join(os.environ['CGAL_LIB'], '..', 'include'),
            os.environ['BOOST_INCLUDEDIR']
        ]
        cgal_path, cgal_lib = get_library(os.environ['CGAL_LIB'],"CGAL")
        gmp_path, gmp_lib = get_library(
            os.path.join(os.environ['CGAL_DIR'], 'auxiliary', 'gmp', 'lib'), 'libgmp')
        if cgal_path and gmp_path:
            cgal_lib = [cgal_lib, gmp_lib]
            cgal_lib_path = [cgal_path, gmp_path]
            build_cgal_ext = True

else:
    cgal_inc = []
    cgal_lib = ['CGAL', 'gmp']
    cgal_lib_path = []
    build_cgal_ext = True

#build_cgal_ext = False #don't do it, client code is out of date
if build_cgal_ext:
    ext_mods += [
        {
            'name' : 'mmstructlib.sas.alpha_shapes',
            'sources' : ['mmstructlib/sas/alpha_shapes.pyx'],
            'include_dirs' : ['src', numpy.get_include()] + cgal_inc,
            'libraries' : cgal_lib,
            'library_dirs' : cgal_lib_path,
            'language' : "c++",
    #        'extra_compile_args' : ['-g']
            'extra_compile_args' : ['-O3', '-march=native', '-DNDEBUG']
        },
        {
            'name' : 'mmstructlib.sas.alpha_shape_weighted_area',
            'sources' : ['mmstructlib/sas/alpha_shape_weighted_area.pyx'],
            'include_dirs' : ['src', numpy.get_include()] + cgal_inc,
            'libraries' : cgal_lib,
            'library_dirs' : cgal_lib_path,
            'language' : "c++",
    #        'extra_compile_args' : ['-g']
            'extra_compile_args' : ['-O3', '-march=native', '-DNDEBUG']
        }
    ]

if not with_cython:
    uncythonize(ext_mods)

#remove unnecessary flags
if platform.system() != 'Windows':  # When compiling on visual no -g is added to params
    cflags = sysconfig.get_config_var('CFLAGS')
    opt = sysconfig.get_config_var('OPT')
    sysconfig._config_vars['CFLAGS'] = cflags.replace(' -g ', ' ')
    sysconfig._config_vars['OPT'] = opt.replace(' -g ', ' ')

if platform.system() == 'Linux':  # In macos there seems not to be -g in LDSHARED
    ldshared = sysconfig.get_config_var('LDSHARED')
    sysconfig._config_vars['LDSHARED'] = ldshared.replace(' -g ', ' ')

setup(
    name='mmstructlib',
    version='0.1',
    author='Christopher J Oldfield',
    author_email='cjoldfield@gmail.com',
    scripts=['bin/mmsl_residue_disorder', 'bin/mmsl_nia_nma', 'bin/mmsl_missing_density_fasta'],
    packages=[
        'mmstructlib',
        'mmstructlib.model',
        'mmstructlib.IO',
        'mmstructlib.radii',
        'mmstructlib.radii.data',
        'mmstructlib.sas',
        'mmstructlib.tools',
        'mmstructlib.util',
        'mmstructlib.math',
        'mmstructlib.assembly',
        'mmstructlib.crystal',
        'mmstructlib.elastic_network'],
    package_data={
        'mmstructlib.radii' : ['data/reduced_atom.siz', 'data/reduced_atom.pickle']},
    license='',
    install_requires=[''],
    test_suite="test",
    classifiers=['Probably broken'],
    zip_safe=True,
    ext_modules=cythonize([Extension(**x) for x in ext_mods])
)

