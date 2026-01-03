```sh
gcc -shared -fPIC clipboard.c -o clipboard.so `pkg-config --cflags --libs lxpanel gtk+-3.0`
sudo cp clipboard.so /usr/lib/x86_64-linux-gnu/lxpanel/plugins/
```
