import os
import sys

# -- Path setup (not strictly needed for just C++/Breathe) --------------------
# sys.path.insert(0, os.path.abspath('..'))

# -- Project information -------------------------------------------------------
project = 'STM32MP135F eBook Reader'
author = 'Jakub Buczynski <KubaTaba1uga>'

# -- General configuration -----------------------------------------------------
extensions = [
    'breathe',
]

# Tell Breathe where the Doxygen XML is
# This path is relative to this conf.py file.
breathe_projects = {
    "STM32MP135F eBook Reader": os.path.abspath(os.path.join('..', 'build', 'doxygen', 'xml')),
}
breathe_default_project = "STM32MP135F eBook Reader"

templates_path = ['_templates']
exclude_patterns = []

# -- Options for HTML output ---------------------------------------------------
html_theme = "furo"
html_theme_options = {
    "light_css_variables": {
        "color-brand-content": "#CC3333",
        "color-admonition-background": "orange",
    },
}

html_short_title = "eBook Reader"
