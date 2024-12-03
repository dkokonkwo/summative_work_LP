from setuptools import setup, Extension

# extension module
module = Extension('sysmonitor', sources=['sys_calc.c'])

# running the setup
setup(name='sysmonitor',
    version='1.0',
    description='A C extension for measuring system performance',
    ext_modules=[module])