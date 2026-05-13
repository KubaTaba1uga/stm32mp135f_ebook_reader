# Run on amd64

To run on amd64 build LVGL with:
```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/opt/ebk_reader/lvgl/ -DCMAKE_C_FLAGS="-Wall -Wextra -DEBK_DISPLAY_X11=1"
cmake --build build/ -j
cmake --install build/
```

