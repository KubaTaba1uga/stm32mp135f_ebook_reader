GUI
===

To simplify creating gui we are using icons available here https://fonts.google.com/icons.

If you need to do any changes on the image in data directory you can generate corresponding C header using `display_driver/examples/convert_image.py`. For exmaple to regenerate boot image do:
```
✦ ❯ python display_driver/examples/convert_image.py --name ebk_boot_img ebook_reader/data/boot_screen.png ebook_reader/src/gui/data/boot_screen.h
```


