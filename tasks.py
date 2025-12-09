import os
import glob

from invoke import task

ROOT_PATH = os.path.dirname(os.path.abspath(__file__))
BUILD_PATH = os.path.join(ROOT_PATH, "build")
DOCS_PATH = os.path.join(ROOT_PATH, "docs")
env = None

@task
def add_repo(c, name, tag, url):
    _run(c, "mkdir -p third_party")
    _run(c, "git status")
    _run(c, "git add . || true")
    _run(c, "git commit -m \"WIP\" || true")

    if _run(c, f"git remote get-url \"{name}\"", warn=True, echo=False) == 0:
        _pr_error(f"{name} already exists")
        return 1

    _run(c, f"git remote add \"{name}\" \"{url}\"")
    _run(c, f"git subtree add --prefix=\"third_party/{name}\" \"{name}\" \"{tag}\" --squash")


@task
def install(c):
    _pr_info(f"Installing dependencies...")

    try: 
       c.run("sudo apt-get install doxygen")
       c.run("virtualenv .venv")
       os.environ["PATH"] = f'{os.path.join(ROOT_PATH, ".venv", "bin")}:{os.environ["PATH"]}'
       c.run("pip install sphinx==8.2.3 breathe==4.36.0 furo==2025.9.25 sphinx-autobuild==2025.08.25")
    except Exception:
        _pr_error("Installing failed")
        raise

    _pr_info(f"Installing dependencies completed")
    

@task
def build_docs(c):
    _pr_info(f"Building docs...")

    os.environ["PATH"] = f'{os.path.join(ROOT_PATH, ".venv", "bin")}:{os.environ["PATH"]}'
    docs_path = os.path.join(BUILD_PATH, "docs", "html")
    
    try:
        c.run("doxygen Doxyfile")
        with c.cd(DOCS_PATH):
            c.run(f"sphinx-build -b html . {docs_path}")
    except Exception:
        _pr_error(f"Building docs failed")
        raise

    _pr_info(f"Building docs completed")


@task
def serve_docs(c, port=8000):
    os.environ["PATH"] = f'{os.path.join(ROOT_PATH, ".venv", "bin")}:{os.environ["PATH"]}'

    c.run(f"sphinx-autobuild --port {port} docs build/docs/html", pty=True)

    
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
        

