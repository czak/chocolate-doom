all: package

DOOM_PREFIX = ${PWD}/build/doom

doom:
	cd ../; \
	./autogen.sh --host=arm-webos-linux-gnueabi --without-libsamplerate --prefix=$(DOOM_PREFIX) --with-bashcompletiondir=/dev/null ;\
	make -j $(nproc) install

package: doom
	rm -rf build/pkg_${ARCH}; \
	mkdir -p build/pkg_${ARCH}; \
	cp appinfo.json build/pkg_${ARCH}/; \
	cp wads/* build/pkg_${ARCH}/; \
	cd build; \
	cp doom/bin/chocolate-doom pkg_${ARCH}/; \
	cp doom/bin/chocolate-doom-setup pkg_${ARCH}/; \
	cp chocolate-doom-launcher pkg_${ARCH}/; \
	convert -resize 80 doom/share/icons/chocolate-doom.png pkg_${ARCH}/icon.png; \
	ares-package pkg_${ARCH}

install: package
	ares-install build/org.chocolate-doom.demo_0.0.1_arm.ipk

launch: install
	ares-launch org.chocolate-doom.demo

clean:
	rm -rf build; \
	cd ..; \
	make clean