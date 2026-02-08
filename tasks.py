import os
import glob

from invoke import task

ROOT_PATH = os.path.dirname(os.path.abspath(__file__))
BUILD_PATH = os.path.join(ROOT_PATH, "build")
DOCS_PATH = os.path.join(ROOT_PATH, "docs")
EBOOK_READER_PATH = os.path.join(ROOT_PATH, "ebook_reader")
DISPLAY_DRIVER_PATH = os.path.join(ROOT_PATH, "display_driver")

C_FORMATER = "clang-format-19"
C_LINTER = "clang-tidy-19"

os.environ["PATH"] = f"{os.path.join(ROOT_PATH, '.venv', 'bin')}:{os.environ['PATH']}"
os.chdir(ROOT_PATH)


@task
def build_bsp(c, config="ebook_reader_dev_defconfig"):
    """
    @todo change config="ebook_reader_dev_defconfig" to config="ebook_reader_defconfig"
          dev build should not be default.
    """
    repos = {
        "buildroot": {
            "tag": "st/2024.02.9",
            "url": "https://github.com/bootlin/buildroot.git",
        },
        "linux": {
            "tag": "v6.6-stm32mp-r2",
            "url": "https://github.com/STMicroelectronics/linux.git",
        },
        "uboot": {
            "tag": "v2023.10-stm32mp-r2",
            "url": "https://github.com/STMicroelectronics/u-boot.git",
        },
        "optee": {
            "tag": "4.0.0-stm32mp-r2",
            "url": "https://github.com/STMicroelectronics/optee_os.git",
        },
        "tf-a": {
            "tag": "v2.10-stm32mp-r2",
            "url": "https://github.com/STMicroelectronics/arm-trusted-firmware.git",
        },
    }
    _pr_info(f"Building BSP...")

    if "dev" in config:
        c.run("mkdir -p third_party")
        with c.cd("third_party"):
            for repo, rdata in repos.items():
                c.run(f"git clone {rdata['url']} {repo} || true")
                with c.cd(repo):
                    c.run(f"git checkout {rdata['tag']}")

    if config:
        configure(c, config)

    with c.cd("build/buildroot"):
        c.run("make BR2_DL_DIR=../../build/third_party")

    _pr_info(f"Building BSP completed")


@task
def add_repo(c, name, tag, url):
    _pr_info("Adding repo...")

    c.run("mkdir -p third_party")
    c.run("git status")
    c.run("git add . || true")
    c.run('git commit -m "WIP" || true')

    if c.run(f'git remote get-url "{name}"', warn=True, echo=False) == 0:
        _pr_error(f"{name} already exists")
        return 1

    c.run(f'git remote add "{name}" "{url}"')
    c.run(f'git subtree add --prefix="third_party/{name}" "{name}" "{tag}" --squash')

    _pr_info("Adding repo completed")


@task
def install(c):
    _pr_info(f"Installing dependencies...")

    try:
        c.run(
            "sudo apt-get install -y doxygen virtualenv \
              which sed make binutils build-essential diffutils \
              gcc g++ bash patch gzip bzip2 perl tar cpio \
              unzip rsync file bc findutils gawk curl \
              git libncurses5-dev python3 libpoppler-glib-dev"
            f" {C_FORMATER} {C_LINTER} "
        )

        c.run("virtualenv .venv")
        c.run(
            "pip install invoke sphinx==8.2.3 breathe==4.36.0 sphinx_rtd_theme==3.0.2 sphinx-autobuild==2025.08.25"
        )
        install_libgpiod(c)

    except Exception:
        _pr_error("Installing failed")
        raise

    _pr_info(f"Installing dependencies completed")


@task
def install_libgpiod(c):
    r = c.run("pkg-config --modversion libgpiod", warn=True)

    if r.ok and "1.6.5" in r.stdout.strip():
        return

    _pr_info(f"Installing libgpiod dependency...")

    try:
        c.run(
            "curl https://mirrors.edge.kernel.org/pub/software/libs/libgpiod/libgpiod-1.6.5.tar.xz -o /tmp/libpiod.tar.xz"
        )
        c.run("tar -xf /tmp/libpiod.tar.xz -C /tmp/")
        with c.cd("/tmp/libgpiod-1.6.5"):
            c.run("./configure --prefix=/usr/")
            c.run("make")
            c.run("sudo make install")

    except Exception:
        _pr_error("Installing failed")
        raise

    _pr_info(f"Installing libgpiod completed")


@task
def build_docs(c):
    _pr_info("Building docs...")

    docs_path = os.path.join(BUILD_PATH, "docs", "html")

    try:
        c.run("doxygen docs/Doxyfile")
        with c.cd(DOCS_PATH):
            c.run(f"sphinx-build -b html . {docs_path}")
    except Exception:
        _pr_error(f"Building docs failed")
        raise

    _pr_info("Building docs completed")


@task
def serve_docs(c, port=8000):
    _pr_info("Serving docs...")

    build_docs(c)

    c.run(
        " ".join(
            [
                f"sphinx-autobuild",
                f"--port {port}",
                f"--watch display_driver/include",
                f"docs build/docs/html",
            ]
        ),
        pty=True,
    )

    _pr_info("Serving docs completed")


@task
def configure(c, config="ebook_reader_dev_defconfig"):
    _pr_info(f"Configuring buildroot...")

    with c.cd("third_party/buildroot"):
        flags = [
            "O=../../build/buildroot",
            "BR2_EXTERNAL=../../br2_external_tree",
            config,
        ]

        c.run(f"make " + " ".join(flags))

    _pr_info(f"Configuring buildroot completed")


@task
def download(c, config="ebook_reader_dev_defconfig"):
    _pr_info(f"Downloading dependencies...")

    if config:
        configure(c, config)

    with c.cd("build/buildroot"):
        c.run("make BR2_DL_DIR=../../build/third_party source")

    _pr_info(f"Downloading dependencies completed")


@task
def build_linux(c, config="ebook_reader_dev_defconfig", target=None):
    _pr_info(f"Building linux...")

    if config:
        configure(c, config)

    with c.cd("build/buildroot"):
        cmd = "linux"
        if target:
            cmd = f"{cmd}-{target}"
        c.run(f"make BR2_DL_DIR=../../build/third_party {cmd}")
    with c.cd("build/buildroot/build/linux-custom"):
        c.run(
            "python scripts/clang-tools/gen_compile_commands.py && cp compile_commands.json ../../../../third_party/linux"
        )

    _pr_info(f"Building linux completed")


@task
def build_uboot(c, config="ebook_reader_dev_defconfig", target=None):
    _pr_info(f"Building u-boot...")

    if config:
        configure(c, config)

    with c.cd("build/buildroot"):
        cmd = "uboot"
        if target:
            cmd = f"{cmd}-{target}"
        c.run(f"make BR2_DL_DIR=../../build/third_party {cmd}")

    _pr_info(f"Building u-boot completed")


@task
def build_tfa(c, config="ebook_reader_dev_defconfig", target=None):
    _pr_info(f"Building tf-a...")

    if config:
        configure(c, config)

    with c.cd("build/buildroot"):
        cmd = "arm-trusted-firmware"
        if target:
            cmd = f"{cmd}-{target}"
        c.run(f"make BR2_DL_DIR=../../build/third_party {cmd}")

    _pr_info(f"Building tf-a completed")


@task
def build_optee(c, config="ebook_reader_dev_defconfig", target=None):
    _pr_info(f"Building optee...")

    if config:
        configure(c, config)

    with c.cd("build/buildroot"):
        cmd = "optee-os"
        if target:
            cmd = f"{cmd}-{target}"
        c.run(f"make BR2_DL_DIR=../../build/third_party {cmd}")

    _pr_info(f"Building optee completed")


@task
def fbuild_linux_kernel(c):
    _pr_info("Fast building linux kernel...")

    cmd = _br2_create_linux_env("zImage")
    with c.cd("build/buildroot/build/linux-custom"):
        c.run(cmd)
        c.run(f"cp arch/arm/boot/zImage ../../images/")

    _pr_info("Linux kernel fast build completed")


@task
def fbuild_linux_dt(c):
    _pr_info("Fast building linux device tree...")

    dtb = None
    with open("br2_external_tree/configs/ebook_reader_dev_defconfig", "r") as fp:
        for line in fp.readlines():
            if line.startswith("BR2_LINUX_KERNEL_INTREE_DTS_NAME="):
                dtb = line.removeprefix("BR2_LINUX_KERNEL_INTREE_DTS_NAME=").rstrip(
                    "\n"
                )
                dtb = dtb.replace('"', "").replace("'", "")
            if line.startswith("BR2_LINUX_KERNEL_CUSTOM_DTS_DIR="):
                dts_dirs = line.removeprefix("BR2_LINUX_KERNEL_CUSTOM_DTS_DIR=")
                sanitized_dts_dirs = dts_dirs.replace(
                    "$(BR2_EXTERNAL_ST_PATH)", "br2_external_tree"
                )
                for dts in sanitized_dts_dirs.split(" "):
                    c.run(
                        f"cp -r {dts.rstrip('\n')}/* build/buildroot/build/linux-custom/arch/arm/boot/dts"
                    )

    if not dtb:
        _pr_error(
            "No dtb file found in br2_external_tree/configs/ebook_reader_dev_defconfig"
        )
        return 1

    cmd = _br2_create_linux_env(f"{dtb}.dtb")
    with c.cd("build/buildroot/build/linux-custom"):
        c.run(cmd)
        c.run(f"cp arch/arm/boot/dts/{dtb}.dtb ../../images/")

    _pr_info("Linux device tree fast build completed")


@task
def fbuild_ebook_reader(c, recompile=False, local=False, display="wvs7in5v2"):
    ereader_path = EBOOK_READER_PATH
    if not os.path.exists(ereader_path):
        return

    _pr_info("Fast building ebook reader...")

    cross_tpl_path = os.path.join(
        "br2_external_tree", "board", "ebook_reader", "meson-cross-compile.txt"
    )

    with c.cd(ereader_path):
        build_dir = os.path.join(BUILD_PATH, os.path.basename(ereader_path))
        if recompile:
            c.run(f"rm -rf {build_dir}")

        c.run(f"mkdir -p {build_dir}")
        root = os.path.abspath(ROOT_PATH)
        with open(cross_tpl_path, "r", encoding="utf-8") as f:
            cross_txt = f.read()
            cross_txt = cross_txt.replace("PLACEHOLDER", root)

        cross_out_path = os.path.join(BUILD_PATH, "cross-file.txt")
        with open(cross_out_path, "w", encoding="utf-8") as f:
            f.write(cross_txt)

        c.run(
            f"rm -rf subprojects/display_driver && "
            f"ln -s {DISPLAY_DRIVER_PATH} "
            f"{os.path.join(ereader_path, 'subprojects', 'display_driver')}"
        )

        c.run(
            f"meson setup -Dbuildtype=debug {build_dir} "
            + (" --wipe " if recompile else " ")
            + (
                f" --cross-file {cross_out_path} -Db_sanitize=address,undefined -Db_lundef=false"
                f" -Ddisplay={display} "
                if not local
                else "  -Db_sanitize=address,undefined -Db_lundef=false -Ddisplay=x11 "
                # else " -Ddisplay=x11 "                
            )
        )

        c.run(
            f"rm -f compile_commands.json && ln -s {os.path.join(build_dir, 'compile_commands.json')} compile_commands.json"
        )

        c.run(f"meson compile -v -C {build_dir}")

    _pr_info("Fast building ebook reader completed")


@task
def fbuild_ebook_reader_test(c):
    tests_path = EBOOK_READER_PATH
    if not os.path.exists(tests_path):
        return

    _pr_info("Fast building ebook reader tests...")

    with c.cd(tests_path):
        build_dir = os.path.join(BUILD_PATH, "test_ebook_reader")
        c.run(
            f"meson setup -Dbuildtype=debug -Dtests=true -Db_sanitize=address,undefined -Db_lundef=false {build_dir}"
            
        )
        c.run(
            f"rm -f compile_commands.json && ln -s {os.path.join(build_dir, 'compile_commands.json')} compile_commands.json"
        )

        c.run(
            f"LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH meson compile -v -C {build_dir}"
        )

    _pr_info("Fast building ebook reader tests completed")


@task
def fbuild_display_driver(c):
    driver_path = DISPLAY_DRIVER_PATH
    if not os.path.exists(driver_path):
        return

    _pr_info("Fast building display driver...")

    cross_tpl_path = os.path.join(
        "br2_external_tree", "board", "ebook_reader", "meson-cross-compile.txt"
    )

    build_dir = os.path.join(BUILD_PATH, os.path.basename(driver_path))
    c.run(f"mkdir -p {build_dir}")
    
    with c.cd(driver_path):
        root = os.path.abspath(ROOT_PATH)
        with open(cross_tpl_path, "r", encoding="utf-8") as f:
            cross_txt = f.read()
            cross_txt = cross_txt.replace("PLACEHOLDER", root)

        cross_out_path = os.path.join(BUILD_PATH, "cross-file.txt")
        with open(cross_out_path, "w", encoding="utf-8") as f:
            f.write(cross_txt)

        c.run(
            f"meson setup --cross-file {cross_out_path} -Dexamples=stm -Dbuildtype=debug  -Db_sanitize=address,undefined -Db_lundef=false {build_dir}"
        )
        c.run(
            f"rm -f compile_commands.json && ln -s {os.path.join(build_dir, 'compile_commands.json')} compile_commands.json"
        )

        c.run(f"meson compile -v -C {build_dir}")

    _pr_info("Fast building display driver completed")


@task
def test_ebook_reader(c, asan_options=None):
    tests_path = EBOOK_READER_PATH
    if not os.path.exists(tests_path):
        return

    _pr_info("Testing display driver...")

    build_dir = os.path.join(BUILD_PATH, "test_ebook_reader")

    c.run(
        (f"ASAN_OPTIONS={asan_options} " if asan_options else "")
        + f"meson test -v -C {build_dir}"
    )

    _pr_info("Testing display driver completed")


@task
def fbuild_display_driver_test(c):
    tests_path = DISPLAY_DRIVER_PATH
    if not os.path.exists(tests_path):
        return

    _pr_info("Fast building display driver tests...")

    with c.cd(tests_path):
        build_dir = os.path.join(BUILD_PATH, "test_display_driver")
        c.run(
            f"meson setup -Dbuildtype=debug -Dtests=true -Db_sanitize=address,undefined -Db_lundef=false {build_dir}"
        )
        c.run(
            f"rm -f compile_commands.json && ln -s {os.path.join(build_dir, 'compile_commands.json')} compile_commands.json"
        )

        c.run(f"meson compile -v -C {build_dir}")

    _pr_info("Fast building display driver tests completed")


@task
def test_display_driver(c, asan_options=None):
    tests_path = DISPLAY_DRIVER_PATH
    if not os.path.exists(tests_path):
        return

    _pr_info("Testing display driver...")

    build_dir = os.path.join(BUILD_PATH, "test_display_driver")

    c.run(
        f"ASAN_OPTIONS={asan_options} "
        if asan_options
        else "" + f"meson test --print-errorlogs -v -C {build_dir}"
    )

    _pr_info("Testing display driver completed")


@task
def deploy_tftp(c, directory="/srv/tftp"):
    _pr_info(f"Deploying to TFTP...")

    if not os.path.exists(directory):
        raise ValueError(f"{directory} does not exists")

    with c.cd("build/buildroot/images"):
        c.run(f"sudo -u dnsmasq cp zImage stm32mp135f-dk-ebook_reader.dtb {directory}")

    _pr_info(f"Deploy to TFTP completed")


@task
def deploy_nfs(c, directory="/srv/nfs", rootfs=True, sanitizers=False):
    _pr_info(f"Deploying to NFS...")

    if not os.path.exists(directory):
        raise ValueError(f"{directory} does not exists")

    if rootfs:
        with c.cd("build/buildroot/images"):
            c.run(f"sudo tar xvf rootfs.tar -C {directory}")

    if sanitizers:
        with c.cd(
            "build/buildroot/build/toolchain-external-bootlin-2024.05-1/arm-buildroot-linux-gnueabihf/lib"
        ):
            c.run(f"sudo cp lib*san* {directory}/lib")

    with c.cd("build/display_driver"):
        c.run(f"sudo cp *example {directory}/root/")
    with c.cd("build/ebook_reader"):
        c.run(f"sudo cp ebook_reader {directory}/root/")
        c.run(f"sudo cp -r ../../ebook_reader/data {directory}/root/")

    _pr_info(f"Deploy to NFS completed")


@task
def deploy_sdcard(c, dev="sda"):
    _pr_info(f"Deploying to sdcard...")

    if not os.path.exists("/dev/disk/by-partlabel/fsbl1"):
        raise ValueError("No /dev/disk/by-partlabel/fsbl1")
    if not os.path.exists("/dev/disk/by-partlabel/fsbl2"):
        raise ValueError("No /dev/disk/by-partlabel/fsbl2")
    if not os.path.exists("/dev/disk/by-partlabel/fip"):
        raise ValueError("No /dev/disk/by-partlabel/fip")

    with c.cd("build/buildroot/images"):
        c.run(
            "sudo dd if=tf-a-stm32mp135f-dk-ebook_reader.stm32 of=/dev/disk/by-partlabel/fsbl1 bs=1K conv=fsync"
        )
        c.run(
            "sudo dd if=tf-a-stm32mp135f-dk-ebook_reader.stm32 of=/dev/disk/by-partlabel/fsbl2 bs=1K conv=fsync"
        )
        c.run("sudo dd if=fip.bin of=/dev/disk/by-partlabel/fip bs=1K conv=fsync")

    c.run("sudo sync")

    _pr_info(f"Deploy to sdcard completed")


@task
def lint(c, project=""):
    patterns = [
        "src/**/*.c",
        "src/**/*.h",
        "include/*.h",
    ]

    _pr_info("Linting...")

    projects = ["display_driver", "ebook_reader"]
    if project:
        projects = [project]

    for proj in projects:
        proj_patterns = [f"{proj}/{pattern}" for pattern in patterns]
        for pattern in proj_patterns:
            _pr_info(f"Linting files matching pattern '{pattern}'")
            for path in glob.glob(pattern, recursive=True):
                if os.path.isfile(path):
                    c.run(f"{C_LINTER} -p build/{proj} {path}")
                    _pr_info(f"{path} linted")

    _pr_info("Linting done")


@task
def format(c, project=""):
    patterns = [
        "src/**/*.c",
        "src/**/*.h",
        "include/*.h",
    ]

    _pr_info("Formating...")

    projects = ["display_driver", "ebook_reader"]
    if project:
        projects = [project]

    for proj in projects:
        proj_patterns = [f"{proj}/{pattern}" for pattern in patterns]
        for pattern in proj_patterns:
            _pr_info(f"Formating files matching pattern '{pattern}'")

            for path in glob.glob(pattern, recursive=True):
                if os.path.isfile(path):
                    c.run(f"{C_FORMATER} -i {path}")
                    _pr_info(f"{path} formated")

    _pr_info("Formating done")


###############################################
#                Private API                  #
###############################################
def _pr_info(message: str):
    print(f"\033[94m[INFO] {message}\033[0m")


def _pr_warn(message: str):
    print(f"\033[93m[WARN] {message}\033[0m")


def _pr_debug(message: str):
    print(f"\033[96m[DEBUG] {message}\033[0m")


def _pr_error(message: str):
    print(f"\033[91m[ERROR] {message}\033[0m")


def _br2_create_linux_env(targets):
    br = os.path.join(ROOT_PATH, "build", "buildroot")

    host_bin = os.path.join(br, "host", "bin")
    host_sbin = os.path.join(br, "host", "sbin")
    host_lib = os.path.join(br, "host", "lib")
    host_inc = os.path.join(br, "host", "include")
    images = os.path.join(br, "images")
    target = os.path.join(br, "target")
    linux_src = os.path.join(br, "build", "linux-custom")

    os.environ.update(
        {
            "ARCH": "arm",
            "CROSS_COMPILE": os.path.join(host_bin, "arm-linux-"),
            "PATH": ":".join(
                [
                    host_bin,
                    host_sbin,
                    os.environ["PATH"],
                ]
            ),
            "PKG_CONFIG": os.path.join(host_bin, "pkg-config"),
            "PKG_CONFIG_SYSROOT_DIR": "/",
            "PKG_CONFIG_ALLOW_SYSTEM_CFLAGS": "1",
            "PKG_CONFIG_ALLOW_SYSTEM_LIBS": "1",
            "PKG_CONFIG_LIBDIR": ":".join(
                [
                    os.path.join(host_lib, "pkgconfig"),
                    os.path.join(br, "host", "share", "pkgconfig"),
                ]
            ),
            "BR_BINARIES_DIR": images,
        }
    )

    hostcc = (
        f"{os.path.join(host_bin, 'ccache')} /usr/bin/gcc -O2 "
        f"-isystem {host_inc} "
        f"-L{host_lib} "
        f"-Wl,-rpath,{host_lib}"
    )

    cmd = (
        f"/usr/bin/make -j5 "
        f'HOSTCC="{hostcc}" '
        f"ARCH=arm "
        f'KCFLAGS="-Wno-attribute-alias" '
        f"INSTALL_MOD_PATH={target} "
        f'CROSS_COMPILE="{os.path.join(host_bin, "arm-linux-")}" '
        f"WERROR=0 "
        f"REGENERATE_PARSERS=1 "
        f"DEPMOD={os.path.join(host_sbin, 'depmod')} "
        f"INSTALL_MOD_STRIP=1 "
        f"-C {linux_src} {targets}"
    )

    return cmd
