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

