import os

from invoke import task

# run with invoke -c build_poppler -r ebook_reader/tools/ build
ROOT_PATH = os.path.dirname(os.path.abspath(os.path.join(__file__, "..", "..")))
APP_PATH = os.path.join(ROOT_PATH, "ebook_reader")
POPPLER_PATH = os.path.join(APP_PATH, "subprojects", "poppler")
LIBOPENJP2_PATH = os.path.join(APP_PATH, "subprojects", "openjpeg-2.5.4")

os.chdir(ROOT_PATH)

@task
def build(c, output=""):
    if output:
        output = os.path.abspath(output)
    c.run("mkdir -p build")
    c.run("mkdir -p build/poppler")
    c.run("mkdir -p build/poppler/libopenjp2")
    c.run("mkdir -p build/poppler/libopenjp2/build")    
    with c.cd("build/poppler/libopenjp2/build"):
        print(ROOT_PATH)        
        c.run(f"cmake -S {LIBOPENJP2_PATH} -B . -DBUILD_SHARED_LIBS=OFF")
        c.run("make")
        c.run("cmake --install . --prefix ..")
    with c.cd("build/poppler"):

        print(APP_PATH)
        print(POPPLER_PATH)        
        os.environ["CMAKE_PREFIX_PATH"] = "libopenjp2/lib/cmake/openjpeg-2.5"
        c.run(f"cmake -DBUILD_GTK_TESTS=OFF -DBUILD_QT5_TESTS=OFF -DBUILD_QT6_TESTS=OFF -DBUILD_CPP_TESTS=OFF -DBUILD_MANUAL_TESTS=OFF -DENABLE_BOOST=OFF -DENABLE_UTILS=OFF -DENABLE_CPP=OFF -DENABLE_GLIB=OFF -DENABLE_GOBJECT_INTROSPECTION=OFF -DENABLE_GTK_DOC=OFF -DENABLE_QT5=OFF -DENABLE_QT6=OFF -DENABLE_LCMS=OFF -DENABLE_LIBCURL=OFF -DENABLE_LIBTIFF=OFF -DBUILD_SHARED_LIBS=OFF -DRUN_GPERF_IF_PRESENT=OFF -DENABLE_NSS3=OFF -DENABLE_GPGME=OFF {POPPLER_PATH}")
        c.run("make -j 8")
        
        if output:
            c.run(f"cp libpoppler.a {output}")
