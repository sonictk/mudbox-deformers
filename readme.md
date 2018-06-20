# Mudbox Deformers

## About

This is a plugin for [Autodesk Mudbox](https://www.autodesk.com/products/mudbox/overview)
that allows for applying simple deformation operations to meshes.

It is intended as a client-facing test of the Mudbox SDK and is not intended for
production-ready usage.


## Building

### Requirements

This plugin was developed against the following development environment(s):

* Windows 10 Fall Creator's Update
* Visual Studio 12 and Visual Studio 2017 Update 2

* Ubuntu 18.04 LTS
* GCC 7.2.0 

Please see below for the officially supported compiler and OS requirements from Autodesk.

#### Mudbox 2018

* Qt 4.8.6 (Mudbox has its own bundled Qt version, but you'll need the Qt tools
  like ``moc``/``uic``/``rcc`` to compile resources.)

##### Windows

* Windows 7
* Visual Studio 12

##### Linux

* Linux kernel 2.6.32
* GCC 4.4.6-3

*OSX Support for this plugin is not officially supported for now . I can't be
bothered to setup Mudbox on my Macbook. The code should work all the same,
though.*


### Instructions for building with CMake

Tested against CMake 3.11.1.

* Create a ``build`` directory and navigate to it.

* Run ``cmake ../ -DCMAKE_BUILD_TYPE=Release -DMUDBOX_SDK_ROOT=<path>``, where
  ``<path>`` is where your Mudbox SDK folder is located (usually in your Mudbox
  installation root directory.) You can also specify ``MUDBOX_PLUGINS_DEPLOYMENT_PATH``
  as well if you'd like to install plugins immediately to the specified directory
  after they are built.

* Run``cmake --build . --config Release --target install``. 


# Credits

Siew Yi Liang

License information is included in the ``LICENSE.txt`` file in the root of this
source distribution.
