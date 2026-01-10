from pathlib import Path
import sys
from jinja2 import Environment, FileSystemLoader, Undefined

TEMPLATE_PATH = Path("subprojects/packagefiles/lvgl/lv_conf.h.jinja")
OUT_PATH      = Path("subprojects/lvgl-9.4.0/src/lv_conf.h")

def parse_kv(args):
    ctx = {}
    for a in args:
        if "=" not in a:
            raise SystemExit(f"Bad arg '{a}'. Use KEY=VALUE (example: EBK_DISPLAY_MODEL=2)")
        k, v = a.split("=", 1)
        k = k.strip()
        v = v.strip()

        # auto-cast ints
        if v.isdigit() or (v.startswith("-") and v[1:].isdigit()):
            v = int(v)

        # allow true/false
        lv = str(v).lower()
        if lv == "true":
            v = True
        elif lv == "false":
            v = False

        print(k, "=", v)
        ctx[k] = v
    return ctx

def main():
    if len(sys.argv) < 2:
        raise SystemExit("Usage: render_lv_conf.py KEY=VALUE [KEY=VALUE ...]")

    ctx = parse_kv(sys.argv[1:])

    env = Environment(
        loader=FileSystemLoader(str(TEMPLATE_PATH.parent)),
        undefined=Undefined,
        autoescape=False
    )

    tpl = env.get_template(TEMPLATE_PATH.name)
    rendered = tpl.render(**ctx)

    OUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUT_PATH.write_text(rendered, encoding="utf-8")

if __name__ == "__main__":
    main()
