# Configuration for Sphinx;
# see http://www.sphinx-doc.org/en/master/usage/configuration.html

# Project Information

project = 'POLO'
author = 'Arda Aytekin, Martin Biel, Mikael Johansson'
copyright = '2018, Arda Aytekin, Martin Biel, Mikael Johansson'
version = ''
release = ''

# General Configuration

extensions = ['sphinx.ext.mathjax']
templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
language = None
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
pygments_style = None
numfig = True

# HTML Output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_css_files = ['css/custom.css']

# HTMLHelp Output

htmlhelp_basename = 'POLOdoc'

# LaTeX Output

latex_documents = [
  (master_doc, 'POLO.tex', 'POLO Documentation',
    'Arda Aytekin \\and Martin Biel \\and Mikael Johansson', 'manual', False),
]

# Manual Page Output

man_pages = [
  (master_doc, 'polo', 'POLO Documentation', [author], 1),
]

# Texinfo Output

texinfo_documents = [
  (master_doc, 'POLO', 'POLO Documentation',
    'Arda Aytekin@*Martin Biel@*Mikael Johansson', 'POLO',
    'POLO: a POLicy-based Optimization library', 'Miscellaneous', False),
]

# Epub Output

epub_title = project
epub_exclude_files = ['search.html']
