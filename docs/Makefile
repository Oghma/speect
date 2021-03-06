# Makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
SPHINXEXTDIR  = sphinxext
PAPER         =
BUILDDIR      = build
DOXYDIR       = doxygen
DOXYGEN       = doxygen
DOXYFILE      = Doxyfile

# Internal variables.
PAPEROPT_a4     = -D latex_paper_size=a4
PAPEROPT_letter = -D latex_paper_size=letter
ALLSPHINXOPTS   = -d $(BUILDDIR)/doctrees $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) source
# the i18n builder cannot share the environment and doctrees with the others
I18NSPHINXOPTS  = $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) source

.PHONY: help clean html dirhtml singlehtml pickle json htmlhelp qthelp devhelp epub latex latexpdf text man changes linkcheck doctest gettext sphinxext_clean doxygen_clean

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  html                to make standalone HTML files"
	@echo "  html_c_engine       to make standalone HTML files, Speect Engine C API only"
	@echo "  html_python_engine  to make standalone HTML files, Speect Engine C/Python API only"
	@echo "  dirhtml             to make HTML files named index.html in directories"
	@echo "  singlehtml          to make a single large HTML file"
	@echo "  pickle              to make pickle files"
	@echo "  json                to make JSON files"
	@echo "  htmlhelp            to make HTML files and a HTML help project"
	@echo "  qthelp              to make HTML files and a qthelp project"
	@echo "  devhelp             to make HTML files and a Devhelp project"
	@echo "  epub                to make an epub"
	@echo "  latex               to make LaTeX files, you can set PAPER=a4 or PAPER=letter"
	@echo "  latexpdf            to make LaTeX files and run them through pdflatex"
	@echo "  text                to make text files"
	@echo "  man                 to make manual pages"
	@echo "  texinfo             to make Texinfo files"
	@echo "  info                to make Texinfo files and run them through makeinfo"
	@echo "  gettext             to make PO message catalogs"
	@echo "  changes             to make an overview of all changed/added/deprecated items"
	@echo "  linkcheck           to check all external links for integrity"
	@echo "  doctest             to run all doctests embedded in the documentation (if enabled)"

doxygen_clean:
	-rm -rf $(DOXYDIR)/*

sphinxext_clean:
	-rm -f $(SPHINXEXTDIR)/doxybridge/*.pyc       \
               $(SPHINXEXTDIR)/doxybridge/*/*.pyc     \
               $(SPHINXEXTDIR)/doxybridge/*/*/*.pyc   \
               $(SPHINXEXTDIR)/doxybridge/*/*/*/*.pyc

doxygen_make:
	-mkdir -p $(DOXYDIR)
	$(DOXYGEN) $(DOXYFILE)

clean: doxygen_clean sphinxext_clean
	-rm -rf $(BUILDDIR)/*

html:
	make doxygen_make
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) $(BUILDDIR)/html
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html."

dirhtml:
	make doxygen_make
	$(SPHINXBUILD) -b dirhtml $(ALLSPHINXOPTS) $(BUILDDIR)/dirhtml
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/dirhtml."

singlehtml:
	make doxygen_make
	$(SPHINXBUILD) -b singlehtml $(ALLSPHINXOPTS) $(BUILDDIR)/singlehtml
	@echo
	@echo "Build finished. The HTML page is in $(BUILDDIR)/singlehtml."

pickle:
	make doxygen_make
	$(SPHINXBUILD) -b pickle $(ALLSPHINXOPTS) $(BUILDDIR)/pickle
	@echo
	@echo "Build finished; now you can process the pickle files."

json:
	make doxygen_make
	$(SPHINXBUILD) -b json $(ALLSPHINXOPTS) $(BUILDDIR)/json
	@echo
	@echo "Build finished; now you can process the JSON files."

htmlhelp:
	make doxygen_make
	$(SPHINXBUILD) -b htmlhelp $(ALLSPHINXOPTS) $(BUILDDIR)/htmlhelp
	@echo
	@echo "Build finished; now you can run HTML Help Workshop with the" \
	      ".hhp project file in $(BUILDDIR)/htmlhelp."

qthelp:
	make doxygen_make
	$(SPHINXBUILD) -b qthelp $(ALLSPHINXOPTS) $(BUILDDIR)/qthelp
	@echo
	@echo "Build finished; now you can run "qcollectiongenerator" with the" \
	      ".qhcp project file in $(BUILDDIR)/qthelp, like this:"
	@echo "# qcollectiongenerator $(BUILDDIR)/qthelp/Speect.qhcp"
	@echo "To view the help file:"
	@echo "# assistant -collectionFile $(BUILDDIR)/qthelp/Speect.qhc"

devhelp:
	make doxygen_make
	$(SPHINXBUILD) -b devhelp $(ALLSPHINXOPTS) $(BUILDDIR)/devhelp
	@echo
	@echo "Build finished."
	@echo "To view the help file:"
	@echo "# mkdir -p $$HOME/.local/share/devhelp/Speect"
	@echo "# ln -s $(BUILDDIR)/devhelp $$HOME/.local/share/devhelp/Speect"
	@echo "# devhelp"

epub:
	make doxygen_make
	$(SPHINXBUILD) -b epub $(ALLSPHINXOPTS) $(BUILDDIR)/epub
	@echo
	@echo "Build finished. The epub file is in $(BUILDDIR)/epub."

latex:
	make doxygen_make
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo
	@echo "Build finished; the LaTeX files are in $(BUILDDIR)/latex."
	@echo "Run \`make' in that directory to run these through (pdf)latex" \
	      "(use \`make latexpdf' here to do that automatically)."

latexpdf:
	make doxygen_make
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo "Running LaTeX files through pdflatex..."
	make -C $(BUILDDIR)/latex all-pdf
	@echo "pdflatex finished; the PDF files are in $(BUILDDIR)/latex."

text:
	make doxygen_make
	$(SPHINXBUILD) -b text $(ALLSPHINXOPTS) $(BUILDDIR)/text
	@echo
	@echo "Build finished. The text files are in $(BUILDDIR)/text."

man:
	make doxygen_make
	$(SPHINXBUILD) -b man $(ALLSPHINXOPTS) $(BUILDDIR)/man
	@echo
	@echo "Build finished. The manual pages are in $(BUILDDIR)/man."

changes:
	make doxygen_make
	$(SPHINXBUILD) -b changes $(ALLSPHINXOPTS) $(BUILDDIR)/changes
	@echo
	@echo "The overview file is in $(BUILDDIR)/changes."

linkcheck:
	make doxygen_make
	$(SPHINXBUILD) -b linkcheck $(ALLSPHINXOPTS) $(BUILDDIR)/linkcheck
	@echo
	@echo "Link check complete; look for any errors in the above output " \
	      "or in $(BUILDDIR)/linkcheck/output.txt."

doctest:
	make doxygen_make
	$(SPHINXBUILD) -b doctest $(ALLSPHINXOPTS) $(BUILDDIR)/doctest
	@echo "Testing of doctests in the sources finished, look at the " \
	      "results in $(BUILDDIR)/doctest/output.txt."

