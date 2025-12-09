import os
import sys

# -- Path setup (not strictly needed for just C++/Breathe) --------------------
# sys.path.insert(0, os.path.abspath('..'))

# -- Project information -------------------------------------------------------
project = 'stm32mp135f_ebook_reader'
author = 'Jakub Buczynski <KubaTaba1uga>'

# -- General configuration -----------------------------------------------------
extensions = [
    'breathe',
]

# Tell Breathe where the Doxygen XML is
# This path is relative to this conf.py file.
breathe_projects = {
    "stm32mp135f_ebook_reader": os.path.abspath(os.path.join('..', 'build', 'doxygen', 'xml')),
}
breathe_default_project = "stm32mp135f_ebook_reader"

templates_path = ['_templates']
exclude_patterns = []

# -- Options for HTML output ---------------------------------------------------
html_theme = "furo"
