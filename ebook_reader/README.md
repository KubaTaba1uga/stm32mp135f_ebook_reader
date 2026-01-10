# generate git diff for lv_conf.h

To regenerate lvgl config execute these commands from repository roo:
```
$ patch -p1 < ebook_reader/subprojects/packagefiles/lvgl/lv_conf.h.patch
# apply changes to lv_conf.h
$ git add src/lv_conf.h
$ git diff --cached -- src/lv_conf.h > ebook_reader/subprojects/packagefiles/lvgl/lv_conf.h.patch
$ git rm --cached src/lv_conf.h
$ rm -rf src
```
