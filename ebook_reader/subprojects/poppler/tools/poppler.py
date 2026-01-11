import os

from invoke import task
ROOT_PATH = os.path.dirname(os.path.abspath(os.path.join(__file__, "..", "..", "..", "..")))
APP_PATH = os.path.join(ROOT_PATH, "ebook_reader")
POPPLER_PATH = os.path.join(APP_PATH, "subprojects", "cmake_poppler")
LIBOPENJP2_PATH = os.path.join(APP_PATH, "subprojects", "openjpeg-2.5.4")
LIB_PATH = os.path.dirname(os.path.abspath(os.path.join(__file__, "..")))

@task
def build(c):
    print(f"{ROOT_PATH=}\n{APP_PATH=}\n{POPPLER_PATH=}\n{LIBOPENJP2_PATH=}\n{LIB_PATH=}\n")
    with c.cd("subprojects/poppler"):
         c.run("mkdir -p poppler.dir")
         c.run("mkdir -p poppler.dir/libopenjp2")
         c.run(f"cmake -S {LIBOPENJP2_PATH} -B poppler.dir/libopenjp2 -DBUILD_SHARED_LIBS=OFF")
         c.run("cd poppler.dir/libopenjp2 && make -j 8")
         c.run("cmake --install poppler.dir/libopenjp2 --prefix poppler.dir/libopenjp2/lib")
         os.environ["CMAKE_PREFIX_PATH"] = "poppler.dir/libopenjp2/lib/lib/cmake/openjpeg-2.5"
         c.run(f"cmake -DBUILD_GTK_TESTS=OFF -DBUILD_QT5_TESTS=OFF -DBUILD_QT6_TESTS=OFF -DBUILD_CPP_TESTS=OFF -DBUILD_MANUAL_TESTS=OFF -DENABLE_BOOST=OFF -DENABLE_UTILS=OFF -DENABLE_CPP=OFF -DENABLE_GLIB=OFF -DENABLE_GOBJECT_INTROSPECTION=OFF -DENABLE_GTK_DOC=OFF -DENABLE_QT5=OFF -DENABLE_QT6=OFF -DENABLE_LCMS=OFF -DENABLE_LIBCURL=OFF -DENABLE_LIBTIFF=OFF -DBUILD_SHARED_LIBS=OFF -DRUN_GPERF_IF_PRESENT=OFF -DENABLE_NSS3=OFF -DENABLE_GPGME=OFF -S {POPPLER_PATH} -B poppler.dir")
         c.run("cd poppler.dir && make -j 8")    
         print("BUILD done")
         c.run(f"cp poppler.dir/libpoppler.a ./")
         c.run(f"cp {POPPLER_PATH}/poppler/*.h {LIB_PATH}/include/")
         print("COPY done")
