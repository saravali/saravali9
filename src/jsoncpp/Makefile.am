
noinst_LIBRARIES = libjsoncpp.a
libjsoncpp_a_SOURCES = json_reader.cpp json_tool.h json_value.cpp json_valueiterator.inl \
	json_writer.cpp json/allocator.h json/assertions.h json/config.h json/forwards.h \
	json/json_features.h json/json.h json/reader.h json/value.h json/version.h json/writer.h

EXTRA_DIST = LICENSE Makefile.mingw jsoncpp.vcxproj

clean:
	rm -f lib*.a *.o *~ *.bak 
	rm -fr obj bin .libs


