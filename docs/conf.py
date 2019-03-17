# Project Information

project = 'POLO'
author = 'Arda Aytekin, Martin Biel, Mikael Johansson'
copyright = '2018, Arda Aytekin, Martin Biel, Mikael Johansson'
version = ''
release = ''

# General Configuration

extensions = ['sphinx.ext.mathjax', 'sphinx.ext.todo']
master_doc = 'index'
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
numfig = True
todo_include_todos = True

# HTML Output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_css_files = ['css/custom.css']

# LaTeX Output

latex_documents = [
  (master_doc, 'POLO.tex', 'POLO Documentation',
    'Arda Aytekin \\and Martin Biel \\and Mikael Johansson', 'manual', False),
]
