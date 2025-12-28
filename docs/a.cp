import os
import sys

# -- Project information -------------------------------------------------------
project = 'STM32MP135F eBook Reader'
author = 'Jakub Buczynski <KubaTaba1uga>'

# -- General configuration -----------------------------------------------------
extensions = [
    'breathe',
    "sphinx.ext.autodoc",
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
    "sphinx.ext.mathjax",
    "sphinx.ext.todo",
    "sphinx.ext.viewcode",
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
# html_theme = "furo"
html_theme = "sphinx_rtd_theme"


html_theme_options = {
    # Optional: let the user toggle light/dark
    # Sidebar project name visible
    "sidebar_hide_name": False,

    # Keyboard navigation
    "navigation_with_keys": True,

    # --- COLORS ------------------------------------------------------------
    "light_css_variables": {
        # Main brand color (links, accents) – purple
        "color-brand-primary": "#7C3AED",   # vivid purple
        "color-brand-content": "#7C3AED",

        # Accent / highlight – orange
        "color-accent": "#F97316",          # warm orange

        # Optional tweaks (light mode)
        "color-background-primary": "#ffffff",
        "color-background-secondary": "#faf5ff",  # very light lavender
        "color-sidebar-background": "#f5f3ff",
        "color-link": "#7C3AED",
        "color-link--hover": "#A855F7",
    },
    "dark_css_variables": {
        # Dark mode brand colors (slightly lighter purples/oranges)
        "color-brand-primary": "#A855F7",
        "color-brand-content": "#A855F7",
        "color-accent": "#FDBA74",

        # Optional tweaks (dark mode)
        "color-background-primary": "#050816",
        "color-background-secondary": "#111827",
        "color-sidebar-background": "#020617",
        "color-link": "#C4B5FD",
        "color-link--hover": "#E9D5FF",
    },
    "source_repository": "https://github.com/KubaTaba1uga/stm32mp135f_ebook_reader",
    "source_branch": "main",
    "source_directory": "docs/",
}

html_short_title = "eBook Reader"
pygments_style = "monokai"
pygments_dark_style = "monokai"

copyright = "2025, Jakub Buczynski <KubaTaba1uga>"
