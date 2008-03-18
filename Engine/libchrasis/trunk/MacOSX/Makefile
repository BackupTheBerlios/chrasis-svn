CHRASIS_VERSION=0.2.0
CONFIG?=Release
DSTROOT?=/
prefix?=/usr/local

all:
	xcodebuild -configuration $(CONFIG) -alltargets $(MAKEFLAGS)

clean:
	xcodebuild -configuration $(CONFIG) -alltargets clean $(MAKEFLAGS)

#install:
#	xcodebuild -configuration $(CONFIG) -alltargets install $(MAKEFLAGS)

install:
	mkdir -p \
		$(DSTROOT)/$(prefix)/lib \
		$(DSTROOT)/$(prefix)/share/chrasis \
		$(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/chrasis
	cp build/$(CONFIG)/libchrasis.dylib \
		$(DSTROOT)/$(prefix)/lib
	cp chr_data.db empty_db.db \
		$(DSTROOT)/$(prefix)/share/chrasis
	cp ../include/chrasis.h \
		$(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/
	cp ../include/chrasis/{character,database,global,platform}.h \
		$(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/chrasis

uninstall:
	rm -f $(DSTROOT)/$(prefix)/lib/libchrasis.dylib
	rm -f $(DSTROOT)/$(prefix)/share/chrasis/{chr_data,empty_db}.db
	rm -f $(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/chrasis.h
	rm -f $(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/chrasis/{character,database,global,platform}.h
	if [ -d $(DSTROOT)/$(prefix)/lib/ ];then \
		rmdir $(DSTROOT)/$(prefix)/lib/; \
	fi
	if [ -d $(DSTROOT)/$(prefix)/share/chrasis/ ];then \
		rmdir $(DSTROOT)/$(prefix)/share/chrasis/; \
	fi
	if [ -d $(DSTROOT)/$(prefix)/share/ ];then \
		rmdir $(DSTROOT)/$(prefix)/share/; \
	fi
	if [ -d $(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/chrasis/ ];then \
		rmdir $(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/chrasis/; \
	fi
	if [ -d $(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/ ];then \
		rmdir $(DSTROOT)/$(prefix)/include/chrasis-$(CHRASIS_VERSION)/; \
	fi
	if [ -d $(DSTROOT)/$(prefix)/include/ ];then \
		rmdir $(DSTROOT)/$(prefix)/include/; \
	fi
	if [ -d $(DSTROOT)/$(prefix)/ ];then \
		rmdir $(DSTROOT)/$(prefix)/; \
	fi