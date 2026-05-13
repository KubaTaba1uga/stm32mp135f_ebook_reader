# Run on amd64

To run on amd64 i installed deps in /opt (lvgl, it8951) and build ebook with:
```bash
meson setup build --prefix=/opt/ebook_reader/app -Ddisplay=x11
meson compile -C build
meson install -C build
```

Then run app with:
```bash
export PKG_CONFIG_PATH=/opt/ebk_reader/it8951/lib/x86_64-linux-gnu/pkgconfig/:/opt/ebk_reader/lvgl/share/pkgconfig/:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=/opt/ebk_reader/lvgl/lib/:/opt/ebk_reader/it8951/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
/opt/ebook_reader/app/bin/ebook_reader
```

