PLUGIN_NAME = quantizer

HEADERS = quantizer.h

SOURCES = quantizer.cpp\
          moc_quantizer.cpp\

LIBS = 

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile
