CHRASIS_VERSION=0.2.0
CONFIG?=Release
DSTROOT?=/

all:
	xcodebuild -configuration $(CONFIG) -alltargets $(MAKEFLAGS)

clean:
	xcodebuild -configuration $(CONFIG) -alltargets clean $(MAKEFLAGS)

install:
	xcodebuild -configuration $(CONFIG) -target Chrasis install $(MAKEFLAGS)

uninstall:
	xcodebuild -configuration $(CONFIG) -target Chrasis uninstall $(MAKEFLAGS)
