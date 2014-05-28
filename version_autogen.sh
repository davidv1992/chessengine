git describe --always | sed 's/.*/#define ENGINE_VERSION "&"/' > version_autogen.h
