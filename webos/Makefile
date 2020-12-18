all: bin

bin:
	cd ../; \
	./autogen.sh --host=arm-webos-linux-gnueabi --without-libsamplerate --prefix=$INSTALL_PREFIX --with-bashcompletiondir=/dev/null ;\
	make install

ipk: bin
	rm -rf pkg_${ARCH}; \
	mkdir pkg_${ARCH}; \
	cp appinfo.json pkg_${ARCH}/; \
	cp out/bin/chocolate-doom pkg_${ARCH}/; \
	convert -resize 80 out/share/icons/chocolate-doom.png pkg_${ARCH}/icon.png; \
	ares-package pkg_${ARCH}

clean:
	rm -rf out pkg_${ARCH}; \
	cd ..; \
	make clean