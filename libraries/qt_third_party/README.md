# Qt third-party libraries

Pinned source versions:

| Library | Version | Upstream | License |
|---|---:|---|---|
| QCustomPlot | 2.1.1 | https://www.qcustomplot.com/ | GPLv3 or commercial |
| QXlsx | 1.5.1.1 | https://github.com/QtExcel/QXlsx | MIT |
| QWindowKit | 1.5.0 | https://github.com/stdware/qwindowkit | Apache-2.0 |

QWindowKit's pinned build dependencies are included recursively:

- qmsetup `85c6c3c783be8af8d3f2fa492748a82da8ec9bad` (MIT).
- syscmdline `5a67673ff96acbfd894ea653fbaca872fded758a` (MIT).

The vendored QWindowKit package contains its Core, Widgets, and Qt Quick
modules. Examples, generated-package resources, and documentation sources are
omitted.

The repository contains the source and license files needed by this template,
but not generated binaries. Build outputs under each `vendor/*/prebuilt/`
directory are ignored by Git and separated by operating system, architecture,
compiler, Qt version, static/shared linkage, and build configuration.

QCustomPlot 2.1.1 is the latest stable upstream release. Its official release
page lists support through Qt 6.4; newer Qt releases may produce deprecation
warnings. Closed-source distribution requires an appropriate QCustomPlot
commercial license instead of the GPL option.
