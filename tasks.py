import glob
import os

from invoke import task

ROOT_PATH = os.path.dirname(os.path.abspath(__file__))
BUILD_PATH = os.path.join(ROOT_PATH, "build")
DOCS_PATH = os.path.join(ROOT_PATH, "docs")

os.environ["PATH"] = f"{os.path.join(ROOT_PATH, '.venv', 'bin')}:{os.environ['PATH']}"

@task
def add_repo(c, name, tag, url):
    c.run("mkdir -p third_party")
    c.run("git status")
    c.run("git add . || true")
    c.run('git commit -m "WIP" || true')

    if c.run(f'git remote get-url "{name}"', warn=True, echo=False) == 0:
        _pr_error(f"{name} already exists")
        return 1

    c.run(f'git remote add "{name}" "{url}"')
    c.run(f'git subtree add --prefix="third_party/{name}" "{name}" "{tag}" --squash')

@task
def install(c):
    _pr_info(f"Installing dependencies...")

    try: 
       c.run("sudo apt-get install -y doxygen virtualenv \
              which sed make binutils build-essential diffutils \
              gcc g++ bash patch gzip bzip2 perl tar cpio \
              unzip rsync file bc findutils gawk wget \
              git libncurses5-dev python3")
       c.run("virtualenv .venv")
       c.run("pip install sphinx==8.2.3 breathe==4.36.0 furo==2025.9.25 sphinx-autobuild==2025.08.25")
    except Exception:
        _pr_error("Installing failed")
        raise

    _pr_info(f"Installing dependencies completed")

@task
def configure(c, config="ebook_reader_dev_defconfig"):
    _pr_info(f"Configuring buildroot...")    

    with c.cd("third_party/buildroot"):
        flags = ["O=../../build/buildroot", "BR2_EXTERNAL=../../buildroot_external_tree", config]
        
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
def build_bsp(c, config="ebook_reader_dev_defconfig"):
    _pr_info(f"Building BSP...")    

    if config:
       configure(c, config)
           
    with c.cd("build/buildroot"):
        c.run("make BR2_DL_DIR=../../build/third_party")

    _pr_info(f"Building BSP completed")

    
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

